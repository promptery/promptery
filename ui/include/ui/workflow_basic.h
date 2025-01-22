#pragma once

#include <model/chat_types.h>

class QAbstractItemModel;
class ContentModel;
class ChatRequestConfigModel;


// Hier geht es weiter.
// diese Klasse ausimplementierne und im ChatWidget den WorkflowProcessor einbauen
// * OutputWidget anschließen
// * ToDos im Processor
// danach weitere Workflows definieren


class WorkflowBasic : public WorkflowInterface
{
public:
    WorkflowBasic(ChatRequestConfigModel *config,
                  QAbstractItemModel *contentModel,
                  QString query,
                  ContextFiles contextFiles,
                  ContextPages contextPages,
                  ChatData chat,
                  DecoratorPromptData decoratorPrompt);

    bool hasNext() const override { return !m_started; }
    ChatRequest nextRequest() override;
    void finishRequest(ChatResponse /*response*/) override { /*nothing to do*/ }

    bool isComplexWorkflow() const override { return false; }

protected:
    QJsonArray chatAsJson() const;

    ChatRequestConfigModel *m_currentConfig;

    QAbstractItemModel *m_contentModel;

    QString m_title{ QObject::tr("response") };
    QString m_query;
    ContextFiles m_contextFiles;
    ContextPages m_contextPages;
    ChatData m_chat;
    DecoratorPromptData m_decoratorPrompt;

    bool m_started{ false };
};

class WorkflowBasicCoT : public WorkflowBasic
{
public:
    WorkflowBasicCoT(ChatRequestConfigModel *baseConfig,
                     ChatRequestConfigModel *refineConfig,
                     QAbstractItemModel *contentModel,
                     QString query,
                     ContextFiles contextFiles,
                     ContextPages contextPages,
                     ChatData chat,
                     DecoratorPromptData decoratorPrompt)
        : WorkflowBasic(baseConfig,
                        contentModel,
                        std::move(query),
                        std::move(contextFiles),
                        std::move(contextPages),
                        std::move(chat),
                        std::move(decoratorPrompt))
        , m_refineConfig(refineConfig)
    {
    }

    bool hasNext() const override { return m_counter < 2; }
    ChatRequest nextRequest() override
    {
        if (m_counter == 0) {
            m_title       = QObject::tr("Base step.");
            m_queryStored = m_query;
            m_query = "List the steps necessary for solving questions like the following. Only "
                      "output the steps needed not the solution.\n\nQuestion:\n" +
                      m_queryStored;
            return WorkflowBasic::nextRequest();
        } else if (m_counter == 1) {
            m_title = QObject::tr("Refinement step.");
            m_query = "Context:\n" + QString("\n````\n") + m_baseResponse + "\n````\n\n";
            m_query +=
                "Adhere to the given context to solve the following question:\n" + m_queryStored;
            m_currentConfig = m_refineConfig;
            return WorkflowBasic::nextRequest();
        }
        return ChatRequest{};
    }
    void finishRequest(ChatResponse response) override
    {
        if (m_counter == 0) {
            m_baseResponse = response.response;
        }
        ++m_counter;
    }

    bool isComplexWorkflow() const override { return true; }

private:
    ChatRequestConfigModel *m_refineConfig;

    int m_counter{ 0 };
    QString m_baseResponse;
    QString m_queryStored;
};
