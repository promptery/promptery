#pragma once

#include <model/chat_types.h>

#include <QObject>
#include <QVariant>

#include <optional>

class BackendManager;
class ComboboxListAdapter;
class LlmInterface;

class DecoratorPromptModel;
class SystemPromptModel;
class WorkflowModelNEW;

class QAbstractItemModel;
class QStandardItemModel;

template <typename T>
class WorkflowAdapter;

class AdapterBase : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    Q_SIGNAL void selectedChanged();
};

class WorkflowModel : public QObject
{
    Q_OBJECT
public:
    WorkflowModel(BackendManager *backends,
                  DecoratorPromptModel *decoratorPromptModel,
                  SystemPromptModel *systemPromptModel,
                  QObject *parent = nullptr);

    QAbstractItemModel *backendModel() const;
    QAbstractItemModel *modelsModel() const;
    QAbstractItemModel *decoratorPromptModel() const;
    QAbstractItemModel *systemPromptModel() const;
    QAbstractItemModel *workflowModel() const;

    Q_SIGNAL void selectedBackendChanged();
    void setSelectedBackend(int index);
    int selectedBackendIdx() const { return m_backend; }
    QString backendId(int idx) const;
    std::optional<LlmInterface *> selectedBackend();

    Q_SIGNAL void modelsAvailable(const QString &backendId) const;

    Q_SIGNAL void selectedModelChanged();
    void setSelectedModelIdx(int index);
    int selectedModelIdx() const;
    QString modelId(int idx) const;

    Q_SIGNAL void selectedDecoratorPromptChanged();
    void setSelectedDecoratorPromptIdx(int index);
    int selectedDecoratorPromptIdx() const;
    DecoratorPromptData selectedDecoratorPrompt() const;

    Q_SIGNAL void selectedSystemPromptChanged();
    void setSelectedSystemPromptIdx(int index);
    int selectedSystemPromptIdx() const;
    SystemPromptData selectedSystemPrompt() const;

    Q_SIGNAL void selectedWorkflowChanged();
    void setSelectedWorkflowIdx(int index);
    int selectedWorkflowIdx() const;
    WorkflowData selectedWorkflow() const;

    void readSettings();
    void storeSettings() const;

private:
    void onModelsAvailable(const QString &id);

    BackendManager *m_backends;
    QStandardItemModel *m_backendModel;

    QStandardItemModel *m_modelsModel;

    WorkflowAdapter<DecoratorPromptModel> *m_decoratorPrompt;
    WorkflowAdapter<SystemPromptModel> *m_systemPrompt;

    WorkflowModelNEW *m_workflowRaw;
    WorkflowAdapter<WorkflowModelNEW> *m_workflows;

    // currently selected
    int m_backend;
    QString m_modelId;
};
