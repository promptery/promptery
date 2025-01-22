#pragma once

#include <model/chat_types.h>

#include <QObject>

class BackendManager;
class ComboboxListAdapter;
class LlmInterface;

class SystemPromptModel;

class QAbstractItemModel;
class QStandardItemModel;

template <typename T>
class WorkflowAdapter;

class ChatRequestConfigModel : public QObject
{
    Q_OBJECT
public:
    ChatRequestConfigModel(BackendManager *backends,
                           SystemPromptModel *systemPromptModel,
                           QString settingsKey, // empty means base model, backwards compatibility
                           QObject *parent = nullptr);

    QAbstractItemModel *backendModel() const;
    QAbstractItemModel *modelsModel() const;
    QAbstractItemModel *systemPromptModel() const;

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

    Q_SIGNAL void selectedSystemPromptChanged();
    void setSelectedSystemPromptIdx(int index);
    int selectedSystemPromptIdx() const;
    SystemPromptData selectedSystemPrompt() const;

    void readSettings();
    void storeSettings() const;

private:
    void onModelsAvailable(const QString &id);

    BackendManager *m_backends;
    QStandardItemModel *m_backendModel;

    QStandardItemModel *m_modelsModel;

    WorkflowAdapter<SystemPromptModel> *m_systemPrompt;

    QString m_settingsKey;

    // currently selected
    int m_backend;
    QString m_modelId;
};
