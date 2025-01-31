#pragma once

#include <ui/page_interface.h>
#include <ui/tile_child_interface.h>

class BackendManager;

class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class QVBoxLayout;

class SettingsWidget : public TileChildInterface, public PageInterface
{
    Q_OBJECT
public:
    SettingsWidget(BackendManager *backends, QWidget *parent = nullptr);

    PageData pageData() const override;
    TileChildInterface *mainView() override { return this; }
    QWidget *sideView() override;

    TileChildData data() const override;

    void readSettings() override;
    void storeSettings() const override;

protected:
    void showEvent(QShowEvent *event) override;

private:
    BackendManager *m_backends;
    QVBoxLayout *m_layout;

    QLineEdit *m_ollamaIp;
    QSpinBox *m_ollamaPort;
    QLabel *m_ollamaStatus;

    QSpinBox *m_optionsCtx;
    QSpinBox *m_optionsSeed;
    QDoubleSpinBox *m_optionsTemp;
};
