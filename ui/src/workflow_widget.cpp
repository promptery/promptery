#include <ui/workflow_widget.h>

#include <ui/chat_request_config_model.h>
#include <ui/chat_request_config_widget.h>
#include <ui/combobox.h>
#include <ui/label.h>
#include <ui/request_config_widget.h>
#include <ui/workflow_model.h>

#include <model/ollama_config.h>

#include <QGridLayout>
#include <QLabel>

WorkflowWidget::WorkflowWidget(WorkflowModel *workflowModel, QWidget *parent)
    : TileChildInterface(parent)
    , m_workflowModel(workflowModel)
    , m_baseModelConfig(new ChatRequestConfigWidget(m_workflowModel->baseModel(), this))
    , m_refineModelConfig(new ChatRequestConfigWidget(m_workflowModel->refineModel(), this))
    , m_layout(new QGridLayout(this))
    , m_cmbDecorator(new ComboBox(this))
    , m_cmbWorkflow(new ComboBox(this))
    , m_ctx(newCtxSb(this))
    , m_seed(newSeedSb(this))
    , m_temp(newTemperatureSb(this))
{
    m_layout->setVerticalSpacing(1);
    m_layout->setHorizontalSpacing(1);
    m_layout->setContentsMargins(6, 0, 0, 0);

    m_layout->addWidget(newTitleLabel(tr("Process"), this), 0, 0);

    m_layout->addWidget(newLabel(tr("Workflow"), this, 8), 0, 1);
    m_layout->addWidget(m_cmbWorkflow, 0, 2);

    m_layout->addWidget(newLabel(tr("Decorator"), this, 8), 0, 3);
    m_layout->addWidget(m_cmbDecorator, 0, 4);
    m_cmbDecorator->setSizePolicy(QSizePolicy::Expanding,
                                  m_cmbDecorator->sizePolicy().verticalPolicy());

    m_layout->addWidget(newLabel(tr("Ctx T S"), this, 8), 0, 5);
    auto options = std::make_unique<QHBoxLayout>();
    options->addWidget(m_ctx);
    options->addWidget(m_temp);
    options->addWidget(m_seed);
    m_layout->addLayout(options.release(), 0, 6);

    m_layout->addWidget(newTitleLabel(tr("Base model"), this), 1, 0);
    m_baseModelConfig->setupUi(m_layout, 1);

    m_layout->addWidget(newTitleLabel(tr("Refiner model"), this), 2, 0);
    m_refineModelConfig->setupUi(m_layout, 2);

    connect(m_workflowModel,
            &WorkflowModel::selectedDecoratorPromptChanged,
            this,
            &WorkflowWidget::onSelectedDecoratorChanged);
    m_cmbDecorator->setModel(m_workflowModel->decoratorPromptModel());

    connect(m_workflowModel,
            &WorkflowModel::selectedWorkflowChanged,
            this,
            &WorkflowWidget::onSelectedWorkflowChanged);
    m_cmbWorkflow->setModel(m_workflowModel->workflowModel());

    connect(m_ctx, &QSpinBox::valueChanged, this, &WorkflowWidget::onIntOptionsChanged);
    connect(m_seed, &QSpinBox::valueChanged, this, &WorkflowWidget::onIntOptionsChanged);
    connect(m_temp, &QDoubleSpinBox::valueChanged, this, &WorkflowWidget::onDoubleOptionsChanged);
}

void WorkflowWidget::updateUi()
{
    // we preselect the first entry of each combobox if needed and possible
    m_cmbDecorator->setCurrentIndex(m_workflowModel->selectedDecoratorPromptIdx());
    connect(m_cmbDecorator, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_workflowModel->setSelectedDecoratorPromptIdx(index);
    });
    if (m_cmbDecorator->currentIndex() == -1 && m_cmbDecorator->count() > 0) {
        m_cmbDecorator->setCurrentIndex(0);
    }

    auto setRefinerState = [this](int index) {
        m_workflowModel->setSelectedWorkflowIdx(index);
        for (auto i = 0; i < m_layout->columnCount(); i++) {
            if (auto *item = m_layout->itemAtPosition(2, i)) {
                item->widget()->setEnabled(index == 1);
            }
        }
    };

    const auto index = m_workflowModel->selectedWorkflowIdx();
    m_cmbWorkflow->setCurrentIndex(index);
    // needs to be done manually, slot not called during show
    setRefinerState(index);

    connect(m_cmbWorkflow, &QComboBox::currentIndexChanged, this, setRefinerState);
    if (m_cmbWorkflow->currentIndex() == -1 && m_cmbWorkflow->count() > 0) {
        m_cmbWorkflow->setCurrentIndex(0);
    }

    m_baseModelConfig->readSettings();
    m_refineModelConfig->readSettings();

    auto const &config = OllamaConfig::global();
    m_ctx->setValue(config.ctx());
    m_seed->setValue(config.seed());
    m_temp->setValue(config.temp());
}

void WorkflowWidget::onSelectedDecoratorChanged()
{
    QSignalBlocker b(m_cmbDecorator);
    m_cmbDecorator->setCurrentIndex(m_workflowModel->selectedDecoratorPromptIdx());
}

void WorkflowWidget::onSelectedWorkflowChanged()
{
    QSignalBlocker b(m_cmbWorkflow);
    m_cmbWorkflow->setCurrentIndex(m_workflowModel->selectedWorkflowIdx());
}

void WorkflowWidget::onIntOptionsChanged(int)
{
    m_workflowModel->setOptions(RequestOptions{ m_ctx->value(), m_seed->value(), m_temp->value() });
}

void WorkflowWidget::onDoubleOptionsChanged(double)
{
    onIntOptionsChanged(-1);
}
