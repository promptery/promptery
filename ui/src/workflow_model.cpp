#include <ui/workflow_model.h>

#include <ui/chat_request_config_model.h>
#include <ui/decorator_prompt_model.h>
#include <ui/workflow_adapter.h>

#include <common/settings.h>

#include <QStandardItemModel>


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


WorkflowModel::WorkflowModel(BackendManager *backends,
                             SystemPromptModel *systemPromptModel,
                             DecoratorPromptModel *decoratorPromptModel,
                             QObject *parent)
    : QObject(parent)
    , m_decoratorPrompt(
          new WorkflowAdapter<DecoratorPromptModel>(decoratorPromptModel, "decoratorPrompt", this))
    , m_baseModel(new ChatRequestConfigModel(backends, systemPromptModel))
    , m_refineModel(new ChatRequestConfigModel(backends, systemPromptModel))
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

    connect(m_decoratorPrompt,
            &WorkflowAdapter<DecoratorPromptModel>::selectedChanged,
            this,
            &WorkflowModel::selectedDecoratorPromptChanged);

    connect(
        m_baseModel, &ChatRequestConfigModel::modelsAvailable, this, [this](const auto &backendId) {
            // ToDo
            Q_EMIT workflowReady(true);
        });
}

QAbstractItemModel *WorkflowModel::decoratorPromptModel() const
{
    return m_decoratorPrompt->model();
}

QAbstractItemModel *WorkflowModel::workflowModel() const
{
    return m_workflows->model();
}

bool WorkflowModel::isReady() const
{
    // ToDo, fill in all aspects

    if (m_baseModel->selectedModelIdx() == -1) {
        return false;
    }

    return true;
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
    m_decoratorPrompt->readSettings();
}

void WorkflowModel::storeSettings() const
{
    m_decoratorPrompt->storeSettings();
}
