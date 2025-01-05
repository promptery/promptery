#include <ui/workflow_model.h>

#include <ui/decorator_prompt_model.h>
#include <ui/system_prompt_model.h>

#include <model/backend_manager.h>
#include <model/llm_interface.h>

#include <common/settings.h>
#include <common/tree_to_list_model_adapter.h>

#include <QStandardItemModel>


constexpr const char *cBackend = "backend";
constexpr const char *cModel   = "model/";


class WorkflowModelNEW : public NamedObjectModelWithData<WorkflowData>, public ModelInterface
{
    using Base = NamedObjectModelWithData<WorkflowData>;

public:
    using Base::Base;

    TileChildData tileData() const override { return { tr("Workflows") }; }

    void readSettings() override { Base::readSettings(cId); }
    void storeSettings() const override { Base::storeSettings(cId); }

protected:
    QString newObjectName() const override { return tr("new workflow"); }

private:
    static constexpr const char *cId = "workflows";
};


class ComboboxListAdapter : public TreeToListModelAdapter
{
public:
    using TreeToListModelAdapter::TreeToListModelAdapter;

    QVariant data(const QModelIndex &index, int role) const override
    {
        switch (role) {
        case Qt::DecorationRole: return {};
        case Qt::DisplayRole: {
            QString name = TreeToListModelAdapter::data(index, role).toString();
            auto current = mapToSource(index).parent();
            while (current.isValid()) {
                name    = m_sourceModel->data(current, role).toString() + "/" + name;
                current = current.parent();
            }
            return name;
        }
        default: return TreeToListModelAdapter::data(index, role);
        }
    }
};

template <typename T>
class WorkflowAdapter : public AdapterBase
{
public:
    WorkflowAdapter(T *model, QString settingsKey, QObject *parent = nullptr)
        : AdapterBase(parent)
        , m_settingsKey(std::move(settingsKey))
        , m_model(model)
        , m_adapter(new ComboboxListAdapter(m_model->itemModel(), this))
    {
        // Workaround since the TreeToListModelAdapter doesn't support fine grained updates
        connect(m_adapter, &ComboboxListAdapter::beginModelChange, this, [this]() {
            m_peristentId = idxToUuid(m_idx);
        });
        connect(m_adapter, &ComboboxListAdapter::endModelChange, this, [this]() {
            if (m_peristentId.isValid()) {
                m_idx = idxFromUuid(m_peristentId);
            }
        });
    }

    QAbstractItemModel *model() const { return m_adapter; }

    void setSelectedIdx(int index)
    {
        m_idx = index;
        storeSettings();
        Q_EMIT selectedChanged();
    }
    int selectedIdx() const { return m_idx; }
    T::DataType selected() const
    {
        const auto idx = m_adapter->mapToSource(m_adapter->index(m_idx, 0));
        return idx.isValid() ? m_model->userData(idx) : typename T::DataType{};
    }

    void readSettings()
    {
        auto &s = Settings::global();
        s.sync();
        if (s.contains(m_settingsKey)) {
            m_idx = idxFromUuid(QUuid(s.value(m_settingsKey).toString()));
        }
    }
    void storeSettings() const
    {
        auto &s = Settings::global();
        s.setValue(m_settingsKey, idxToUuid(m_idx).toString());
        s.sync();
    }

private:
    QVariant idxToUuid(int idx) const
    {
        return m_adapter->index(idx, 0).data(NamedObjectModel::idRole());
    }
    int idxFromUuid(const QVariant &uuid) const
    {
        bool found = false;
        auto i     = 0;
        while (!found && i < m_adapter->rowCount()) {
            found = idxToUuid(i) == uuid;
            ++i;
        }
        return found ? i - 1 : -1;
    }

    QString m_settingsKey;
    T *m_model;
    ComboboxListAdapter *m_adapter;
    QVariant m_peristentId;
    int m_idx{ -1 };
};


WorkflowModel::WorkflowModel(BackendManager *backends,
                             DecoratorPromptModel *decoratorPromptModel,
                             SystemPromptModel *systemPromptModel,
                             QObject *parent)
    : QObject(parent)
    , m_backends(backends)
    , m_backendModel(new QStandardItemModel(this))
    , m_modelsModel(new QStandardItemModel(this))
    , m_decoratorPrompt(
          new WorkflowAdapter<DecoratorPromptModel>(decoratorPromptModel, "decoratorPrompt", this))
    , m_systemPrompt(
          new WorkflowAdapter<SystemPromptModel>(systemPromptModel, "systemPrompt", this))
    , m_workflowRaw(new WorkflowModelNEW(this))
    , m_workflows(new WorkflowAdapter<WorkflowModelNEW>(m_workflowRaw, "workflow", this))
{
    {
        auto *item = m_workflowRaw->createObject();
        m_workflowRaw->setUserData(item->index(), WorkflowData("simple", ""));
        m_workflowRaw->renameItem(item->index(), tr("Simple query"));
    }
    {
        auto *item = m_workflowRaw->createObject();
        m_workflowRaw->setUserData(item->index(), WorkflowData("basic_cot", ""));
        m_workflowRaw->renameItem(item->index(), tr("Basic CoT query"));
    }

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

    connect(m_backends, &BackendManager::modelsAvailable, this, &WorkflowModel::onModelsAvailable);

    connect(m_decoratorPrompt,
            &WorkflowAdapter<DecoratorPromptModel>::selectedChanged,
            this,
            &WorkflowModel::selectedDecoratorPromptChanged);

    connect(m_systemPrompt,
            &WorkflowAdapter<SystemPromptModel>::selectedChanged,
            this,
            &WorkflowModel::selectedSystemPromptChanged);
}

QAbstractItemModel *WorkflowModel::backendModel() const
{
    return m_backendModel;
}

QAbstractItemModel *WorkflowModel::modelsModel() const
{
    return m_modelsModel;
}

QAbstractItemModel *WorkflowModel::decoratorPromptModel() const
{
    return m_decoratorPrompt->model();
}

QAbstractItemModel *WorkflowModel::systemPromptModel() const
{
    return m_systemPrompt->model();
}

QAbstractItemModel *WorkflowModel::workflowModel() const
{
    return m_workflows->model();
}

void WorkflowModel::setSelectedBackend(int index)
{
    m_backend = index;

    auto &s = Settings::global();
    s.setValue(QString(cModel) + cBackend, backendId(m_backend));
    s.sync();

    Q_EMIT selectedBackendChanged();

    if (selectedBackendIdx() == -1) {
        m_modelsModel->clear();
        return;
    }
    onModelsAvailable(backendId(selectedBackendIdx()));
}

QString WorkflowModel::backendId(int idx) const
{
    return idx == -1 ? "" : m_backendModel->item(idx)->data().toString();
}

std::optional<LlmInterface *> WorkflowModel::selectedBackend()
{
    return m_backends->llmBackend(backendId(m_backend));
}

void WorkflowModel::setSelectedModelIdx(int index)
{
    m_modelId = index == -1 ? "" : m_modelsModel->item(index)->data().toString();

    auto &s = Settings::global();
    s.setValue(cModel + backendId(m_backend), m_modelId);
    s.sync();

    Q_EMIT selectedModelChanged();
}

int WorkflowModel::selectedModelIdx() const
{
    bool found = false;
    auto i     = 0;
    while (!found && i < m_modelsModel->rowCount()) {
        found = modelId(i) == m_modelId;
        ++i;
    }
    return found ? i - 1 : -1;
}

QString WorkflowModel::modelId(int idx) const
{
    return m_modelsModel->item(idx)->data().toString();
}

void WorkflowModel::setSelectedDecoratorPromptIdx(int index)
{
    m_decoratorPrompt->setSelectedIdx(index);
}

int WorkflowModel::selectedDecoratorPromptIdx() const
{
    return m_decoratorPrompt->selectedIdx();
}

DecoratorPromptData WorkflowModel::selectedDecoratorPrompt() const
{
    return m_decoratorPrompt->selected();
}

void WorkflowModel::setSelectedSystemPromptIdx(int index)
{
    m_systemPrompt->setSelectedIdx(index);
}

int WorkflowModel::selectedSystemPromptIdx() const
{
    return m_systemPrompt->selectedIdx();
}

SystemPromptData WorkflowModel::selectedSystemPrompt() const
{
    return m_systemPrompt->selected();
}

void WorkflowModel::setSelectedWorkflowIdx(int index)
{
    m_workflows->setSelectedIdx(index);
}

int WorkflowModel::selectedWorkflowIdx() const
{
    return m_workflows->selectedIdx();
}

WorkflowData WorkflowModel::selectedWorkflow() const
{
    return m_workflows->selected();
}

void WorkflowModel::readSettings()
{
    auto &s = Settings::global();
    s.sync();
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
    m_decoratorPrompt->readSettings();
    m_systemPrompt->readSettings();
}

void WorkflowModel::storeSettings() const
{
    auto &s = Settings::global();
    s.setValue(QString(cModel) + cBackend, backendId(m_backend));
    s.setValue(cModel + backendId(m_backend), m_modelId);
    s.sync();
    m_decoratorPrompt->storeSettings();
    m_systemPrompt->storeSettings();
}

void WorkflowModel::onModelsAvailable(const QString &id)
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
    m_modelId =
        Settings::global().value(cModel + backendId(selectedBackendIdx()), m_modelId).toString();

    Q_EMIT modelsAvailable(id);
}
