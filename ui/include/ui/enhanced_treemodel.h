#pragma once

#include <ui/tile_child_interface.h>

#include <QFileIconProvider>
#include <QObject>

#include <vector>

class QAction;
class QAbstractItemModel;
class QStandardItem;

class EnhancedTreeModel : public QObject
{
    Q_OBJECT
public:
    explicit EnhancedTreeModel(QObject *parent = nullptr);

    virtual QAbstractItemModel *itemModel()             = 0;
    virtual const QAbstractItemModel *itemModel() const = 0;

    virtual TileChildData tileData() const = 0;

    virtual std::vector<QAction *> actions() const { return {}; }

    virtual void readSettings(const QString &prefix)        = 0;
    virtual void storeSettings(const QString &prefix) const = 0;

    virtual bool supportsDragDrop() const { return false; };

    Q_SIGNAL void displayNameChanged();

protected:
    static QFileIconProvider m_fileIconProvider;
};

namespace CheckableModel
{

void onItemChanged(QStandardItem *item);

std::vector<QVariant> getChecked(QStandardItem *root);

void setCheckStateRecursively(QStandardItem *item, Qt::CheckState state);

Qt::CheckState checkStateForChildren(const QStandardItem *item);

} // namespace CheckableModel
