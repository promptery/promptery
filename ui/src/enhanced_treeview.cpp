#include <ui/enhanced_treeview.h>

#include <common/settings.h>

#include <QFileDialog>
#include <QHBoxLayout>

#include <ui/file_system_model.h>
#include <ui/pushbutton.h>
#include <ui/tile_widget.h>
#include <ui/treeview.h>

EnhancedTreeView::EnhancedTreeView(EnhancedTreeModel *model, QWidget *parent)
    : TileChildInterface{ parent }
    , m_model(model)
    , m_tree(new TreeView(this))
{
    Q_ASSERT(m_model);

    m_tree->setModel(m_model->itemModel());
    m_tree->setHeaderHidden(true);
    m_tree->setAlternatingRowColors(true);

    if (m_model->supportsDragDrop()) {
        m_tree->setDragEnabled(true);
        m_tree->setDropIndicatorShown(true);
        m_tree->setDefaultDropAction(Qt::MoveAction);
        m_tree->setDragDropMode(QAbstractItemView::InternalMove);
    }

    for (auto &action : m_model->actions()) {
        addButton(action);
    }

    // main layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_tree);

    setLayout(layout);

    connect(m_model,
            &EnhancedTreeModel::displayNameChanged,
            this,
            &TileChildInterface::tileChildDataChanged);
}

void EnhancedTreeView::readSettings(const QString &prefix)
{
    m_model->readSettings(prefix);
}

void EnhancedTreeView::storeSettings(const QString &prefix)
{
    m_model->storeSettings(prefix);
}

QModelIndex EnhancedTreeView::currentIndex() const
{
    return m_tree->currentIndex();
}

TileChildData EnhancedTreeView::data() const
{
    return m_model->tileData();
}

std::vector<QWidget *> EnhancedTreeView::actionWidgetsRight()
{
    // return {};
    std::vector<QWidget *> res(m_buttons.size());
    std::copy(m_buttons.begin(), m_buttons.end(), res.begin());
    return res;
}

QWidget *EnhancedTreeView::asTile()
{
    if (!m_tile) {
        m_tile = new TileWidget(this, parentWidget());
    }
    return m_tile;
}

void EnhancedTreeView::addButton(QAction *action)
{
    QPushButton *btn = nullptr;
    if (action->icon().isNull()) {
        btn = new PushButton(action->text(), this);
    } else {
        btn = new PushButton(action->icon(), "", this);
    }

    btn->setToolTip(action->toolTip());
    connect(btn, &QPushButton::clicked, action, &QAction::trigger);
    m_buttons.push_back(btn);
}
