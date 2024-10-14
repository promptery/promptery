#include <ui/workflow_widget.h>

#include <ui/combobox.h>
#include <ui/workflow_model.h>

#include <QGridLayout>
#include <QLabel>

WorkflowWidget::WorkflowWidget(WorkflowModel *workflowModel, QWidget *parent)
    : TileChildInterface(parent)
    , m_workflowModel(workflowModel)
    , m_cmbBackend(new ComboBox(this))
    , m_cmbModel(new ComboBox(this))
    , m_cmbDecorator(new ComboBox(this))
    , m_cmbSystemPrompt(new ComboBox(this))
    , m_cmbWorkflow(new ComboBox(this))
{
    auto *layout = new QGridLayout(this);
    auto margins = layout->contentsMargins();
    margins.setTop(0);
    layout->setContentsMargins(margins);

    layout->addWidget(new QLabel(tr("Model"), this), 0, 0);
    layout->addWidget(new QLabel(tr("System"), this), 1, 0);
    layout->addWidget(new QLabel(tr("Decorator"), this), 1, 2);
    layout->addWidget(new QLabel(tr("Workflow"), this), 2, 0);
    layout->addWidget(m_cmbBackend, 0, 1);
    layout->addWidget(m_cmbModel, 0, 2, 1, 2);
    m_cmbModel->setSizePolicy(QSizePolicy::Expanding, m_cmbModel->sizePolicy().verticalPolicy());
    layout->addWidget(m_cmbSystemPrompt, 1, 1);
    m_cmbSystemPrompt->setSizePolicy(QSizePolicy::Expanding,
                                     m_cmbSystemPrompt->sizePolicy().verticalPolicy());
    layout->addWidget(m_cmbDecorator, 1, 3);
    m_cmbDecorator->setSizePolicy(QSizePolicy::Expanding,
                                  m_cmbDecorator->sizePolicy().verticalPolicy());
    layout->addWidget(m_cmbWorkflow, 2, 1);
    m_cmbWorkflow->setSizePolicy(QSizePolicy::Expanding,
                                 m_cmbWorkflow->sizePolicy().verticalPolicy());

    connect(
        m_workflowModel, &WorkflowModel::modelsAvailable, this, &WorkflowWidget::modelsAvailable);

    connect(m_workflowModel,
            &WorkflowModel::selectedBackendChanged,
            this,
            &WorkflowWidget::onSelectedBackendChanged);
    m_cmbBackend->setModel(m_workflowModel->backendModel());

    connect(m_workflowModel,
            &WorkflowModel::selectedModelChanged,
            this,
            &WorkflowWidget::onSelectedModelChanged);
    m_cmbModel->setModel(m_workflowModel->modelsModel());

    connect(m_workflowModel,
            &WorkflowModel::selectedDecoratorPromptChanged,
            this,
            &WorkflowWidget::onSelectedDecoratorChanged);
    m_cmbDecorator->setModel(m_workflowModel->decoratorPromptModel());

    connect(m_workflowModel,
            &WorkflowModel::selectedSystemPromptChanged,
            this,
            &WorkflowWidget::onSelectedSystemPromptChanged);
    m_cmbSystemPrompt->setModel(m_workflowModel->systemPromptModel());

    connect(m_workflowModel,
            &WorkflowModel::selectedWorkflowChanged,
            this,
            &WorkflowWidget::onSelectedWorkflowChanged);
    m_cmbWorkflow->setModel(m_workflowModel->workflowModel());
}

void WorkflowWidget::showEvent(QShowEvent *event)
{
    // we preselect the first entry of each combobox if needed and possible

    m_cmbBackend->setCurrentIndex(m_workflowModel->selectedBackendIdx());
    connect(m_cmbBackend, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_workflowModel->setSelectedBackend(index);
    });
    if (m_cmbBackend->currentIndex() == -1 && m_cmbBackend->count() > 0) {
        m_cmbBackend->setCurrentIndex(0);
    }

    m_cmbModel->setCurrentIndex(m_workflowModel->selectedModelIdx());
    connect(m_cmbModel, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_workflowModel->setSelectedModelIdx(index);
    });
    if (m_cmbModel->currentIndex() == -1 && m_cmbModel->count() > 0) {
        m_cmbModel->setCurrentIndex(0);
    }

    m_cmbDecorator->setCurrentIndex(m_workflowModel->selectedDecoratorPromptIdx());
    connect(m_cmbDecorator, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_workflowModel->setSelectedDecoratorPromptIdx(index);
    });
    if (m_cmbDecorator->currentIndex() == -1 && m_cmbDecorator->count() > 0) {
        m_cmbDecorator->setCurrentIndex(0);
    }

    m_cmbSystemPrompt->setCurrentIndex(m_workflowModel->selectedSystemPromptIdx());
    connect(m_cmbSystemPrompt, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_workflowModel->setSelectedSystemPromptIdx(index);
    });
    if (m_cmbSystemPrompt->currentIndex() == -1 && m_cmbSystemPrompt->count() > 0) {
        m_cmbSystemPrompt->setCurrentIndex(0);
    }

    m_cmbWorkflow->setCurrentIndex(m_workflowModel->selectedWorkflowIdx());
    connect(m_cmbWorkflow, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_workflowModel->setSelectedWorkflowIdx(index);
    });
    if (m_cmbWorkflow->currentIndex() == -1 && m_cmbWorkflow->count() > 0) {
        m_cmbWorkflow->setCurrentIndex(0);
    }
}

void WorkflowWidget::onSelectedBackendChanged()
{
    {
        QSignalBlocker b(m_cmbBackend);
        m_cmbBackend->setCurrentIndex(m_workflowModel->selectedBackendIdx());
    }
}

void WorkflowWidget::onSelectedModelChanged()
{
    {
        QSignalBlocker b(m_cmbModel);
        m_cmbModel->setCurrentIndex(m_workflowModel->selectedModelIdx());
    }
}

void WorkflowWidget::onSelectedDecoratorChanged()
{
    QSignalBlocker b(m_cmbDecorator);
    m_cmbDecorator->setCurrentIndex(m_workflowModel->selectedDecoratorPromptIdx());
}

void WorkflowWidget::onSelectedSystemPromptChanged()
{
    QSignalBlocker b(m_cmbSystemPrompt);
    m_cmbSystemPrompt->setCurrentIndex(m_workflowModel->selectedSystemPromptIdx());
}

void WorkflowWidget::onSelectedWorkflowChanged()
{
    QSignalBlocker b(m_cmbWorkflow);
    m_cmbWorkflow->setCurrentIndex(m_workflowModel->selectedWorkflowIdx());
}

void WorkflowWidget::modelsAvailable(const QString &backendId)
{
    {
        QSignalBlocker b(m_cmbModel);
        m_cmbModel->setCurrentIndex(m_workflowModel->selectedModelIdx());
    }

    // signals need to be active, if we need to set to the first model programmatically
    if (m_cmbModel->currentIndex() == -1 && m_cmbModel->count() > 0) {
        m_cmbModel->setCurrentIndex(0);
    }

    m_cmbModel->update();
}
