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
{
    auto *layout = new QGridLayout(this);
    auto margins = layout->contentsMargins();
    margins.setTop(0);
    layout->setContentsMargins(margins);

    layout->addWidget(new QLabel(tr("Model"), this), 0, 0);
    layout->addWidget(new QLabel(tr("System"), this), 1, 0);
    layout->addWidget(new QLabel(tr("Decorator"), this), 1, 2);
    layout->addWidget(m_cmbBackend, 0, 1);
    layout->addWidget(m_cmbModel, 0, 2, 1, 2);
    m_cmbModel->setSizePolicy(QSizePolicy::Expanding, m_cmbModel->sizePolicy().verticalPolicy());
    layout->addWidget(m_cmbSystemPrompt, 1, 1);
    m_cmbSystemPrompt->setSizePolicy(QSizePolicy::Expanding,
                                     m_cmbSystemPrompt->sizePolicy().verticalPolicy());
    layout->addWidget(m_cmbDecorator, 1, 3);
    m_cmbDecorator->setSizePolicy(QSizePolicy::Expanding,
                                  m_cmbDecorator->sizePolicy().verticalPolicy());

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
}

void WorkflowWidget::showEvent(QShowEvent *event)
{
    m_cmbBackend->setCurrentIndex(m_workflowModel->selectedBackendIdx());
    connect(m_cmbBackend, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_workflowModel->setSelectedBackend(index);
    });

    m_cmbModel->setCurrentIndex(m_workflowModel->selectedModelIdx());
    connect(m_cmbModel, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_workflowModel->setSelectedModelIdx(index);
    });

    m_cmbDecorator->setCurrentIndex(m_workflowModel->selectedDecoratorPromptIdx());
    connect(m_cmbDecorator, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_workflowModel->setSelectedDecoratorPromptIdx(index);
    });

    m_cmbSystemPrompt->setCurrentIndex(m_workflowModel->selectedSystemPromptIdx());
    connect(m_cmbSystemPrompt, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_workflowModel->setSelectedSystemPromptIdx(index);
    });
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

void WorkflowWidget::modelsAvailable(const QString &backendId)
{
    {
        QSignalBlocker b(m_cmbModel);
        m_cmbModel->setCurrentIndex(m_workflowModel->selectedModelIdx());
    }
    m_cmbModel->update();
}
