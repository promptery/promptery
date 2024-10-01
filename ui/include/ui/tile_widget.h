#pragma once

#include <ui/page_interface.h>
#include <ui/tile_child_interface.h>

#include <QWidget>

class QLabel;
class QHBoxLayout;
class QVBoxLayout;

class TileWidget : public QWidget
{
    Q_OBJECT
public:
    TileWidget(TileChildInterface *child, QWidget *parent = nullptr);

protected:
    TileChildInterface *m_child;

    QVBoxLayout *m_tileLayout;

    QWidget *m_header;
    QHBoxLayout *m_headerLayout;

    QLabel *m_title;
};
