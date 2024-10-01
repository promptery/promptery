#pragma once

#include <QString>
#include <QWidget>

#include <vector>

class QWidget;

struct TileChildData {
    TileChildData(QString name)
        : name(std::move(name))
    {
    }
    TileChildData(QString name, QString tooltip)
        : name(std::move(name))
        , tooltip(std::move(tooltip))
    {
    }

    QString name;
    QString tooltip;
};

class TileChildInterface : public QWidget
{
    Q_OBJECT
public:
    using QWidget::QWidget;

    virtual TileChildData data() const = 0;

    virtual std::vector<QWidget *> actionWidgetsLeft() { return {}; }
    virtual std::vector<QWidget *> actionWidgetsRight() { return {}; }

    Q_SIGNAL void tileChildDataChanged();
};
