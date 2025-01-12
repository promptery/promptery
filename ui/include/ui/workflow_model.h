#pragma once

#include <model/chat_types.h>

#include <QObject>
#include <QVariant>

class BackendManager;
class SystemPromptModel;
class ChatRequestConfigModel;
class DecoratorPromptModel;
class WorkflowModelNEW;

class QAbstractItemModel;
class QStandardItemModel;

template <typename T>
class WorkflowAdapter;

class WorkflowModel : public QObject
{
    Q_OBJECT
public:
    WorkflowModel(BackendManager *backends,
                  SystemPromptModel *systemPromptModel,
                  DecoratorPromptModel *decoratorPromptModel,
                  QObject *parent = nullptr);

    QAbstractItemModel *decoratorPromptModel() const;
    QAbstractItemModel *workflowModel() const;

    ChatRequestConfigModel *baseModel() const { return m_baseModel; }
    ChatRequestConfigModel *refineModel() const { return m_refineModel; }

    Q_SIGNAL void workflowReady(bool isReady);
    bool isReady() const;

    Q_SIGNAL void selectedDecoratorPromptChanged();
    void setSelectedDecoratorPromptIdx(int index);
    int selectedDecoratorPromptIdx() const;
    DecoratorPromptData selectedDecoratorPrompt() const;

    Q_SIGNAL void selectedWorkflowChanged();
    void setSelectedWorkflowIdx(int index);
    int selectedWorkflowIdx() const;
    WorkflowData selectedWorkflow() const;

    void readSettings();
    void storeSettings() const;

private:
    WorkflowAdapter<DecoratorPromptModel> *m_decoratorPrompt;

    ChatRequestConfigModel *m_baseModel;
    ChatRequestConfigModel *m_refineModel;

    WorkflowModelNEW *m_workflowRaw;
    WorkflowAdapter<WorkflowModelNEW> *m_workflows;
};
