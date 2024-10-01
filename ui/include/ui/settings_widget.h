#pragma once

#include <ui/page_interface.h>
#include <ui/tile_child_interface.h>

class QVBoxLayout;

class SettingsWidget : public TileChildInterface, public PageInterface
{
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget *parent = nullptr);

    PageData pageData() const override;
    TileChildInterface *mainView() override { return this; }
    QWidget *sideView() override;

    TileChildData data() const override;

private:
    QVBoxLayout *m_layout;
};
