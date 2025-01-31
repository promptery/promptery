#include <ui/tile_widget.h>

#include <ui/defines.h>
#include <ui/label.h>
#include <ui/tile_child_interface.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

TileWidget::TileWidget(TileChildInterface *child, QWidget *parent)
    : QWidget(parent)
    , m_child(child)
    , m_tileLayout(new QVBoxLayout(this))
    , m_header(new QWidget(this))
    , m_headerLayout(new QHBoxLayout(m_header))
    , m_title(newSectionLabel("", this))
{
    m_tileLayout->setContentsMargins(0, 0, 0, 0);
    m_tileLayout->setSpacing(0);
    m_tileLayout->addWidget(m_header);
    m_tileLayout->addWidget(child);

    m_title->setMinimumWidth(100);
    m_title->setMinimumHeight(cHeaderHeight);
    m_title->setText(child->data().name);

    m_headerLayout->setContentsMargins(0, 0, 0, 0);
    m_headerLayout->setSpacing(0);
    m_headerLayout->addWidget(m_title);

    for (auto *i : child->actionWidgetsLeft()) {
        m_headerLayout->addWidget(i);
    }

    m_headerLayout->addStretch();

    for (auto *i : child->actionWidgetsRight()) {
        m_headerLayout->addWidget(i);
    }

    connect(child, &TileChildInterface::tileChildDataChanged, this, [this]() {
        m_title->setText(m_child->data().name);
        m_title->setToolTip(m_child->data().tooltip);
    });
}
