#pragma once

#include <model/chat_types.h>

class QAbstractItemModel;
class ContentModel;


// Hier geht es weiter.
// diese Klasse ausimplementierne und im ChatWidget den WorkflowProcessor einbauen
// * OutputWidget anschließen
// * ToDos im Processor
// danach weitere Workflows definieren


class WorkflowBasic : public WorkflowInterface
{
public:
    WorkflowBasic(LlmInterface *backend,
                  QAbstractItemModel *contentModel,
                  QString model,
                  QString query,
                  ContextFiles contextFiles,
                  ContextPages contextPages,
                  ChatData chat,
                  SystemPromptData systemPrompt,
                  DecoratorPromptData decoratorPrompt);

    bool hasNext() const override { return !m_started; }
    ChatRequest nextRequest() override;
    void finishRequest(ChatResponse /*response*/) override { /*nothing to do*/ }

protected:
    QJsonArray chatAsJson() const;

    LlmInterface *m_backend;
    QAbstractItemModel *m_contentModel;

    QString m_title{ QObject::tr("response") };
    QString m_model;
    QString m_query;
    ContextFiles m_contextFiles;
    ContextPages m_contextPages;
    ChatData m_chat;
    SystemPromptData m_systemPrompt;
    DecoratorPromptData m_decoratorPrompt;

    bool m_started{ false };
};

class WorkflowBasicWithSteps : public WorkflowBasic
{
public:
    using WorkflowBasic::WorkflowBasic;

    bool hasNext() const override { return m_counter < 3; }
    ChatRequest nextRequest() override
    {
        if (m_counter == 0) {
            m_title = QObject::tr("Response only thinking");
            return WorkflowBasic::nextRequest();
        } else if (m_counter == 1) {
            m_title       = QObject::tr("Thinking step.");
            m_queryStored = m_query;
            m_query = "Think about how you would solve the following question. What steps are "
                      "needed to solve the question? Be very precise. Only output the steps needed "
                      "not the solution.\n\nQuestion:\n" +
                      m_queryStored;
            return WorkflowBasic::nextRequest();
        } else if (m_counter == 2) {
            m_title = QObject::tr("Response with thinking steps as input.");
            m_query = "Question: \n" + m_queryStored +
                      "\n\nAdhere to the following steps to solve the above question:\n" + m_steps;
            return WorkflowBasic::nextRequest();
        }
        return ChatRequest{};
    }
    void finishRequest(ChatResponse response) override
    {
        if (m_counter == 1) {
            m_steps = response.response;
        }
        ++m_counter;
    }

private:
    int m_counter{ 0 };
    QString m_steps;
    QString m_queryStored;
};
