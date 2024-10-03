#include <ui/settings_widget.h>

#include <model/backend_manager.h>
#include <model/llm_interface.h>
#include <model/ollama_config.h>

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

SettingsWidget::SettingsWidget(BackendManager *backends, QWidget *parent)
    : TileChildInterface{ parent }
    , m_backends(backends)
    , m_layout(new QVBoxLayout(this))
    , m_ollamaIp(new QLineEdit(this))
    , m_ollamaPort(new QSpinBox(this))
    , m_ollamaStatus(new QLabel(this))
{
    m_ollamaStatus->setIndent(2);

    auto ollamaGrid = std::make_unique<QGridLayout>();
    ollamaGrid->setContentsMargins(0, 0, 0, 0);
    ollamaGrid->setVerticalSpacing(6);
    ollamaGrid->setHorizontalSpacing(6);

    ollamaGrid->addWidget(new QLabel(tr("Ollama Config"), this), 0, 0);
    ollamaGrid->addWidget(m_ollamaStatus, 0, 1);
    ollamaGrid->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 2);

    ollamaGrid->addWidget(new QLabel(tr("IP"), this), 1, 0);
    ollamaGrid->addWidget(m_ollamaIp, 1, 1);

    ollamaGrid->addWidget(new QLabel(tr("Port"), this), 2, 0);
    ollamaGrid->addWidget(m_ollamaPort, 2, 1);

    m_layout->addItem(ollamaGrid.release());
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addStretch();

    connect(m_ollamaIp, &QLineEdit::editingFinished, this, [this]() {
        OllamaConfig::global().setIp(m_ollamaIp->text());
        m_backends->updateModels(OllamaConfig::backendId());
    });
    m_ollamaPort->setMinimum(0);
    m_ollamaPort->setMaximum(65535);
    connect(m_ollamaPort, &QSpinBox::valueChanged, this, [this](int value) {
        OllamaConfig::global().setPort(static_cast<uint32_t>(value));
        m_backends->updateModels(OllamaConfig::backendId());
    });

    connect(
        m_backends, &BackendManager::startingConnection, this, [this](const QString &backendId) {
            if (backendId == OllamaConfig::backendId()) {
                m_ollamaStatus->setText(tr("Not connected"));
            }
        });

    connect(m_backends, &BackendManager::modelsAvailable, this, [this](const QString &backendId) {
        if (backendId == OllamaConfig::backendId()) {
            const auto &models = m_backends->llmBackend(backendId).value()->models();
            m_ollamaStatus->setText(
                tr("Models available: %1")
                    .arg(models.empty() ? tr("none") : QString::number(models.size())));
        }
    });
}

PageData SettingsWidget::pageData() const
{
    return PageData{ tr("Settings"), QIcon(":/icons/settings.svg"), tr("Settings") };
}

QWidget *SettingsWidget::sideView()
{
    return nullptr;
}

TileChildData SettingsWidget::data() const
{
    return { tr("Settings") };
}

void SettingsWidget::readSettings()
{
    OllamaConfig::global().readSettings();
}

void SettingsWidget::storeSettings() const
{
    OllamaConfig::global().storeSettings();
}

void SettingsWidget::showEvent(QShowEvent *event)
{
    auto &ollama = OllamaConfig::global();
    m_ollamaIp->setText(ollama.ip());
    m_ollamaPort->setValue(ollama.port());
}
