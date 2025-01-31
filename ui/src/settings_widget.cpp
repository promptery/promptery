#include <ui/settings_widget.h>

#include <ui/label.h>
#include <ui/request_config_widget.h>

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
    , m_optionsCtx(newCtxSb(this))
    , m_optionsSeed(newSeedSb(this))
    , m_optionsTemp(newTemperatureSb(this))
{
    m_ollamaStatus->setIndent(2);

    auto grid = std::make_unique<QGridLayout>();
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setVerticalSpacing(6);
    grid->setHorizontalSpacing(6);

    int row = 0;
    grid->addItem(new QSpacerItem(0, 6, QSizePolicy::Expanding), row, 2);

    ++row;
    grid->addWidget(newTitleLabel(tr("Ollama Config"), this), row, 0);
    grid->addWidget(m_ollamaStatus, row, 1);

    ++row;
    grid->addWidget(newLabel(tr("IP"), this), row, 0);
    grid->addWidget(m_ollamaIp, row, 1);

    ++row;
    grid->addWidget(newLabel(tr("Port"), this), row, 0);
    grid->addWidget(m_ollamaPort, row, 1);

    ++row;
    grid->addWidget(newTitleLabel(tr("Default Ollama Options"), this), row, 0);

    ++row;
    grid->addWidget(newLabel(tr("Context length (*1024)"), this), row, 0);
    grid->addWidget(m_optionsCtx, row, 1);

    ++row;
    grid->addWidget(newLabel(tr("Seed (-1 = random)"), this), row, 0);
    grid->addWidget(m_optionsSeed, row, 1);

    ++row;
    grid->addWidget(newLabel(tr("Temperature (0.8 = default)"), this), row, 0);
    grid->addWidget(m_optionsTemp, row, 1);

    m_layout->addItem(grid.release());
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

    connect(m_optionsCtx, &QSpinBox::valueChanged, this, [this](int value) {
        OllamaConfig::global().setCtx(value);
    });
    connect(m_optionsSeed, &QSpinBox::valueChanged, this, [this](int value) {
        OllamaConfig::global().setSeed(value);
    });
    connect(m_optionsTemp, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        OllamaConfig::global().setTemp(value);
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

    m_optionsCtx->setValue(ollama.ctx());
    m_optionsSeed->setValue(ollama.seed());
    m_optionsTemp->setValue(ollama.temp());
}
