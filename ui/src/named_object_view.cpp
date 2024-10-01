#include <ui/named_object_view.h>

#include <ui/named_object_model.h>
#include <ui/treeview.h>

NamedObjectView::NamedObjectView(NamedObjectModel *model, QWidget *parent)
    : EnhancedTreeView(model, parent)
    , m_model(model)
{
    auto *addObject = new QAction(QIcon(":/icons/page-add.svg"), tr("Add object"), this);
    connect(addObject, &QAction::triggered, this, &NamedObjectView::addObject);
    addButton(addObject);

    auto *addFolder = new QAction(QIcon(":/icons/folder-add.svg"), tr("Add folder"), this);
    connect(addFolder, &QAction::triggered, this, &NamedObjectView::addFolder);
    addButton(addFolder);

    auto *deleteCurrent = new QAction(QIcon(":/icons/cancel"), tr("Delete current item"), this);
    connect(deleteCurrent, &QAction::triggered, this, &NamedObjectView::deleteCurrent);
    addButton(deleteCurrent);

    connect(m_tree, &TreeView::currentIndexChanged, this, &NamedObjectView::currentIndexChanged);
}

void NamedObjectView::addFolder()
{
    createItem([this](const auto &parent) { return m_model->createFolder(parent); });
}

void NamedObjectView::addObject()
{
    auto *item = createItem([this](const auto &parent) { return m_model->createObject(parent); });
    if (item) {
        item->setData(QString());
    }
}

void NamedObjectView::deleteCurrent()
{
    auto current = currentIndex();
    if (current.isValid()) {
        m_model->itemModel()->removeRow(current.row(), current.parent());
    }
}

template <typename Fct>
QStandardItem *NamedObjectView::createItem(const Fct &fct)
{
    auto current = currentIndex();
    auto *item   = fct(current);
    if (!item && current.isValid()) {
        current = current.parent();
        item    = fct(current);
    }

    if (item) {
        m_tree->setExpanded(current, true);
    }
    return item;
}
