#include <ui/chat_request_config_model.h>

#include <ui/system_prompt_model.h>
#include <ui/workflow_adapter.h>

#include <model/backend_manager.h>
#include <model/llm_interface.h>

#include <QStandardItemModel>

constexpr const char *cBackend = "backend";
constexpr const char *cModel   = "model/";

struct SettingsRAII {
    SettingsRAII(QString settingsKey)
        : m_settings(Settings::global())
        , m_settingsKey(std::move(settingsKey))
    {
        m_settings.sync();
        if (!m_settingsKey.isEmpty()) {
            m_settings.beginGroup(m_settingsKey);
        }
    }
    ~SettingsRAII()
    {
        if (!m_settingsKey.isEmpty()) {
            m_settings.endGroup();
        }
        m_settings.sync();
    }
    void setValue(QAnyStringView key, const QVariant &value)
    {
        m_settings.setValue(std::move(key), value);
    }
    QVariant value(QAnyStringView key, const QVariant &defaultValue) const
    {
        return m_settings.value(std::move(key), defaultValue);
    }

private:
    Settings &m_settings;
    QString m_settingsKey;
};


ChatRequestConfigModel::ChatRequestConfigModel(BackendManager *backends,
                                               SystemPromptModel *systemPromptModel,
                                               QString settingsKey,
                                               QObject *parent)
    : QObject(parent)
    , m_backends(backends)
    , m_backendModel(new QStandardItemModel(this))
    , m_modelsModel(new QStandardItemModel(this))
    , m_systemPrompt(
          new WorkflowAdapter<SystemPromptModel>(systemPromptModel, "systemPrompt", this))
    , m_settingsKey(std::move(settingsKey))
{
    auto count = 0;
    for (const auto &llmEntry : m_backends->llmBackends()) {
        auto &llm = llmEntry.second;
        auto item = std::make_unique<QStandardItem>(llm->name());
        item->setData(llm->id());
        const auto address = llm->address();
        if (!address.isEmpty()) {
            item->setData(address, Qt::ToolTipRole);
        }
        m_backendModel->appendRow(item.release());

        if (llm->isDefaultLlm()) {
            m_backend = count;
        }
        ++count;
    }

    connect(m_backends,
            &BackendManager::modelsAvailable,
            this,
            &ChatRequestConfigModel::onModelsAvailable);

    connect(m_systemPrompt,
            &WorkflowAdapter<SystemPromptModel>::selectedChanged,
            this,
            &ChatRequestConfigModel::selectedSystemPromptChanged);
}

QAbstractItemModel *ChatRequestConfigModel::backendModel() const
{
    return m_backendModel;
}

QAbstractItemModel *ChatRequestConfigModel::modelsModel() const
{
    return m_modelsModel;
}

QAbstractItemModel *ChatRequestConfigModel::systemPromptModel() const
{
    return m_systemPrompt->model();
}

void ChatRequestConfigModel::setSelectedBackend(int index)
{
    m_backend = index;

    {
        auto s = SettingsRAII(m_settingsKey);
        s.setValue(QString(cModel) + cBackend, backendId(m_backend));
    }

    Q_EMIT selectedBackendChanged();

    if (selectedBackendIdx() == -1) {
        m_modelsModel->clear();
        return;
    }
    onModelsAvailable(backendId(selectedBackendIdx()));
}

QString ChatRequestConfigModel::backendId(int idx) const
{
    return idx == -1 ? "" : m_backendModel->item(idx)->data().toString();
}

std::optional<LlmInterface *> ChatRequestConfigModel::selectedBackend()
{
    return m_backends->llmBackend(backendId(m_backend));
}

void ChatRequestConfigModel::setSelectedModelIdx(int index)
{
    m_modelId = index == -1 ? "" : m_modelsModel->item(index)->data().toString();

    {
        auto s = SettingsRAII(m_settingsKey);
        s.setValue(cModel + backendId(m_backend), m_modelId);
    }

    Q_EMIT selectedModelChanged();
}

int ChatRequestConfigModel::selectedModelIdx() const
{
    bool found = false;
    auto i     = 0;
    while (!found && i < m_modelsModel->rowCount()) {
        found = modelId(i) == m_modelId;
        ++i;
    }
    return found ? i - 1 : -1;
}

QString ChatRequestConfigModel::modelId(int idx) const
{
    return m_modelsModel->item(idx)->data().toString();
}

void ChatRequestConfigModel::setSelectedSystemPromptIdx(int index)
{
    auto s = SettingsRAII(m_settingsKey);
    m_systemPrompt->setSelectedIdx(index);
}

int ChatRequestConfigModel::selectedSystemPromptIdx() const
{
    return m_systemPrompt->selectedIdx();
}

SystemPromptData ChatRequestConfigModel::selectedSystemPrompt() const
{
    return m_systemPrompt->selected();
}

void ChatRequestConfigModel::readSettings()
{
    auto s = SettingsRAII(m_settingsKey);
    {
        const auto currentId = backendId(m_backend);
        const auto readId    = s.value(QString(cModel) + cBackend, currentId).toString();
        bool found           = false;
        auto i               = 0;
        while (!found && i < m_backendModel->rowCount()) {
            found = backendId(i) == readId;
            ++i;
        }
        m_backend = found ? i - 1 : -1;
        onModelsAvailable(backendId(m_backend));
    }
    m_modelId = s.value(cModel + backendId(m_backend), m_modelId).toString();
    m_systemPrompt->readSettings();
}

void ChatRequestConfigModel::storeSettings() const
{
    auto s = SettingsRAII(m_settingsKey);
    s.setValue(QString(cModel) + cBackend, backendId(m_backend));
    s.setValue(cModel + backendId(m_backend), m_modelId);
    m_systemPrompt->storeSettings();
}

void ChatRequestConfigModel::onModelsAvailable(const QString &id)
{
    if (backendId(m_backend) != id) {
        return;
    }
    {
        QSignalBlocker b(m_modelsModel);
        m_modelsModel->clear();
        if (const auto &backend = selectedBackend()) {
            for (const auto &model : backend.value()->models()) {
                if (model.type == ModelType::chat) {
                    auto item = std::make_unique<QStandardItem>(model.name);
                    item->setData(model.id);
                    item->setData(model.name + " - " + model.id, Qt::ToolTipRole);
                    m_modelsModel->appendRow(item.release());
                }
            }
        }
    }
    {
        auto s    = SettingsRAII(m_settingsKey);
        m_modelId = s.value(cModel + backendId(selectedBackendIdx()), m_modelId).toString();
    }

    Q_EMIT modelsAvailable(id);
}
