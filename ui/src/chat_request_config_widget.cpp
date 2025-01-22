#include <ui/chat_request_config_widget.h>

#include <ui/chat_request_config_model.h>
#include <ui/combobox.h>

#include <QFont>
#include <QGridLayout>
#include <QLabel>

ChatRequestConfigWidget::ChatRequestConfigWidget(ChatRequestConfigModel *model, QWidget *parent)
    : QObject{ parent }
    , m_model(model)
    , m_cmbBackend(new ComboBox(parent))
    , m_cmbModel(new ComboBox(parent))
    , m_cmbSystemPrompt(new ComboBox(parent))
    , m_parent(parent)
{
    connect(m_model,
            &ChatRequestConfigModel::modelsAvailable,
            this,
            &ChatRequestConfigWidget::modelsAvailable);

    connect(m_model,
            &ChatRequestConfigModel::selectedBackendChanged,
            this,
            &ChatRequestConfigWidget::onSelectedBackendChanged);
    m_cmbBackend->setModel(m_model->backendModel());

    connect(m_model,
            &ChatRequestConfigModel::selectedModelChanged,
            this,
            &ChatRequestConfigWidget::onSelectedModelChanged);
    m_cmbModel->setModel(m_model->modelsModel());

    connect(m_model,
            &ChatRequestConfigModel::selectedSystemPromptChanged,
            this,
            &ChatRequestConfigWidget::onSelectedSystemPromptChanged);
    m_cmbSystemPrompt->setModel(m_model->systemPromptModel());
}

void ChatRequestConfigWidget::setupUi(QGridLayout *parentGrid, int row)
{
    QFont font              = m_parent->font();
    const auto origFontSize = font.pointSize();
    font.setPointSize(origFontSize - 2);
    auto newLabel = [&font](QString &&text, QWidget *parent) {
        auto *lbl = new QLabel(std::move(text), parent);
        lbl->setFont(font);
        lbl->setIndent(8);
        // lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        return lbl;
    };
    auto newTitle = [&font, &origFontSize](QString &&text, QWidget *parent) {
        auto *lbl = new QLabel(std::move(text), parent);
        font.setBold(true);
        font.setPointSize(origFontSize - 1);
        lbl->setFont(font);
        return lbl;
    };

    parentGrid->addWidget(newLabel(tr("Backend"), m_parent), row, 1);
    parentGrid->addWidget(newLabel(tr("Model"), m_parent), row, 3);
    parentGrid->addWidget(newLabel(tr("System"), m_parent), row, 5);

    parentGrid->addWidget(m_cmbBackend, row, 2);
    parentGrid->addWidget(m_cmbModel, row, 4);
    m_cmbModel->setSizePolicy(QSizePolicy::Expanding, m_cmbModel->sizePolicy().verticalPolicy());
    parentGrid->addWidget(m_cmbSystemPrompt, row, 6);
    m_cmbSystemPrompt->setSizePolicy(QSizePolicy::Expanding,
                                     m_cmbSystemPrompt->sizePolicy().verticalPolicy());
}

void ChatRequestConfigWidget::readSettings()
{
    // we preselect the first entry of each combobox if needed and possible

    m_cmbBackend->setCurrentIndex(m_model->selectedBackendIdx());
    connect(m_cmbBackend, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_model->setSelectedBackend(index);
    });
    if (m_cmbBackend->currentIndex() == -1 && m_cmbBackend->count() > 0) {
        m_cmbBackend->setCurrentIndex(0);
    }

    m_cmbModel->setCurrentIndex(m_model->selectedModelIdx());
    connect(m_cmbModel, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_model->setSelectedModelIdx(index);
    });
    if (m_cmbModel->currentIndex() == -1 && m_cmbModel->count() > 0) {
        m_cmbModel->setCurrentIndex(0);
    }

    m_cmbSystemPrompt->setCurrentIndex(m_model->selectedSystemPromptIdx());
    connect(m_cmbSystemPrompt, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_model->setSelectedSystemPromptIdx(index);
    });
    if (m_cmbSystemPrompt->currentIndex() == -1 && m_cmbSystemPrompt->count() > 0) {
        m_cmbSystemPrompt->setCurrentIndex(0);
    }
}

void ChatRequestConfigWidget::onSelectedBackendChanged()
{
    QSignalBlocker b(m_cmbBackend);
    m_cmbBackend->setCurrentIndex(m_model->selectedBackendIdx());
}

void ChatRequestConfigWidget::onSelectedModelChanged()
{
    QSignalBlocker b(m_cmbModel);
    m_cmbModel->setCurrentIndex(m_model->selectedModelIdx());
}

void ChatRequestConfigWidget::onSelectedSystemPromptChanged()
{
    QSignalBlocker b(m_cmbSystemPrompt);
    m_cmbSystemPrompt->setCurrentIndex(m_model->selectedSystemPromptIdx());
}

void ChatRequestConfigWidget::modelsAvailable(const QString &backendId)
{
    {
        QSignalBlocker b(m_cmbModel);
        m_cmbModel->setCurrentIndex(m_model->selectedModelIdx());
    }

    // signals need to be active, if we need to set to the first model programmatically
    if (m_cmbModel->currentIndex() == -1 && m_cmbModel->count() > 0) {
        m_cmbModel->setCurrentIndex(0);
    }

    m_cmbModel->update();
}
