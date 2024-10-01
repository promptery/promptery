#include <ui/settings_widget.h>

#include <QVBoxLayout>

SettingsWidget::SettingsWidget(QWidget *parent)
    : TileChildInterface{ parent }
    , m_layout(new QVBoxLayout(this))
{
    m_layout->addStretch();
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
