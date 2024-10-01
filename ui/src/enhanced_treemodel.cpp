#include <ui/enhanced_treemodel.h>

#include <ui/defines.h>

#include <QStandardItem>

QFileIconProvider EnhancedTreeModel::m_fileIconProvider{};

EnhancedTreeModel::EnhancedTreeModel(QObject *parent)
    : QObject(parent)
{
}

namespace CheckableModel
{

// The (recursive) checking should also work with models, which are used in checkable AND
// non-checkable views, see ContentModel

namespace
{
Qt::CheckState checkState(QStandardItem *item)
{
    if (item->isCheckable()) {
        return item->checkState();
    }
    return Qt::CheckState(qvariant_cast<int>(item->data(cCheckRole)));
}

void setCheckState(QStandardItem *item, Qt::CheckState state)
{
    if (item->isCheckable()) {
        item->setCheckState(state);
        return;
    }
    item->setData(state, cCheckRole);
}
} // namespace

void onItemChanged(QStandardItem *item)
{
    Qt::CheckState state = checkState(item);

    if (state != Qt::PartiallyChecked) {
        for (int i = 0; i < item->rowCount(); ++i) {
            QStandardItem *childItem = item->child(i);
            setCheckStateRecursively(childItem, state);
        }
    }

    QStandardItem *parent = item->parent();
    while (parent != nullptr) {
        Qt::CheckState state = checkStateForChildren(parent);
        setCheckState(parent, state);
        parent = parent->parent();
    }
}

std::vector<QVariant> getChecked(QStandardItem *root)
{
    std::vector<QVariant> res;
    std::function<void(QStandardItem *, std::vector<QVariant> &)> get =
        [&get](QStandardItem *item, std::vector<QVariant> &res) {
            if (item->rowCount()) { // is folder
                for (int i = 0; i < item->rowCount(); ++i) {
                    QStandardItem *childItem = item->child(i);
                    if (checkState(childItem) != Qt::Unchecked) {
                        get(childItem, res);
                    }
                }
            } else { // is file
                if (checkState(item) != Qt::Unchecked) {
                    res.push_back(item->data(cUuidRole));
                }
            }
        };

    get(root, res);
    return res;
}

void setCheckStateRecursively(QStandardItem *item, Qt::CheckState state)
{
    setCheckState(item, state); // Set check state of current item

    for (int i = 0; i < item->rowCount(); ++i) {
        QStandardItem *childItem = item->child(i);
        setCheckStateRecursively(childItem, state); // Recursively set check state of children
    }
}

Qt::CheckState checkStateForChildren(const QStandardItem *item)
{
    Qt::CheckState state = checkState(item->child(0));

    for (int i = 1; i < item->rowCount(); ++i) {
        QStandardItem *childItem = item->child(i);
        if (checkState(childItem) != state) {
            return Qt::PartiallyChecked;
        }
    }

    return state;
}

} // namespace CheckableModel
