#pragma once

#include <ui/tile_child_interface.h>

#include <QWidget>

namespace Ui
{
class LogWidget;
}

class LogWidget : public TileChildInterface
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = nullptr);
    ~LogWidget();

    Q_SLOT void append(const QString &text);
    Q_SLOT void clear();

    TileChildData data() const override { return { "Log" }; }
    std::vector<QWidget *> actionWidgetsRight() override;

private:
    Ui::LogWidget *ui;
};
