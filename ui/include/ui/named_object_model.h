#pragma once

#include <ui/enhanced_treemodel.h>

#include <QModelIndex>
#include <QStandardItem>

class QAction;
class WrappedStandardItemModel;

class NamedObjectModel : public EnhancedTreeModel
{
    Q_OBJECT
public:
    enum ItemType : int { Folder = 0, Object };

    explicit NamedObjectModel(QObject *parent = nullptr);

    QAbstractItemModel *itemModel() override;
    const QAbstractItemModel *itemModel() const override;

    std::vector<QAction *> actions() const override { return {}; }

    QVariant uuid(const QModelIndex &index) const;

    QVariant data(const QVariant &uuid, int role) const;

    // ToDo: need to handle the hierarchy, but for the data, there needs to be a callback from above
    void readSettings(const QString &prefix) override;
    void storeSettings(const QString &prefix) const override;

    bool supportsDragDrop() const override { return true; }

    QStandardItem *createFolder();
    QStandardItem *createFolder(const QModelIndex &parent, const QString &uuid = "");
    QStandardItem *createObject(const QString &uuid = "");
    QStandardItem *createObject(const QModelIndex &parent, const QString &uuid = "");

    void renameItem(const QModelIndex &index, const QString &newName);

    bool isFolder(const QModelIndex &idx) const;

protected:
    virtual void readItem(const QJsonValue &value, QStandardItem *item);
    virtual QJsonValue storeItem(QStandardItem *item) const;

    QStandardItem *itemFromIndex(const QModelIndex &idx) const;

    virtual bool emptyModelOk() const { return false; }
    virtual QString newObjectName() const;

private:
    WrappedStandardItemModel *m_model;
};

template <typename T>
class NamedObjectModelWithData : public NamedObjectModel
{
public:
    using DataType = T;

    using NamedObjectModel::NamedObjectModel;

    T userData(const QModelIndex &idx) const { return T{ storeItem(itemFromIndex(idx)) }; }
    void setUserData(const QModelIndex &idx, const T &data)
    {
        readItem(data.toJson(), itemFromIndex(idx));
    }
};
