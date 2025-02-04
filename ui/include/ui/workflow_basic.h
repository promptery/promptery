#pragma once

#include <model/chat_types.h>

class QAbstractItemModel;
class ContentModel;
class ChatRequestConfigModel;


class WorkflowBasic : public WorkflowInterface
{
public:
    WorkflowBasic(ChatRequestConfigModel *config,
                  QAbstractItemModel *contentModel,
                  QString query,
                  ContextFiles contextFiles,
                  ContextPages contextPages,
                  ChatData chat,
                  DecoratorPromptData decoratorPrompt,
                  RequestOptions options);

    bool hasNext() const override { return !m_started; }
    void finishRequest(ChatResponse /*response*/) override { /*nothing to do*/ }

    bool isComplexWorkflow() const override { return false; }

protected:
    void prepareNextRequest() override;

    QJsonArray chatAsJson() const;

    ChatRequestConfigModel *m_currentConfig;

    QAbstractItemModel *m_contentModel;

    QString m_title{ QObject::tr("response") };
    QString m_query;
    ContextFiles m_contextFiles;
    ContextPages m_contextPages;
    ChatData m_chat;
    DecoratorPromptData m_decoratorPrompt;
    RequestOptions m_options;

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
                     DecoratorPromptData decoratorPrompt,
                     RequestOptions options);

    bool hasNext() const override { return m_counter < 2; }
    void finishRequest(ChatResponse response) override;

    bool isComplexWorkflow() const override { return true; }

protected:
    void prepareNextRequest() override;

private:
    ChatRequestConfigModel *m_refineConfig;

    int m_counter{ 0 };
    QString m_baseResponse;
    QString m_queryStored;
};
