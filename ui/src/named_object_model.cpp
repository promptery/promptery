#include <ui/named_object_model.h>

#include <ui/defines.h>

#include <tools/tools.h>

#include <QAction>
#include <QFileIconProvider>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardItemModel>
#include <QUuid>

#include <common/log.h>
#include <common/settings.h>

class WrappedStandardItemModel : public QStandardItemModel
{
public:
    using QStandardItemModel::QStandardItemModel;

    using QStandardItemModel::beginResetModel;
    using QStandardItemModel::endResetModel;
};


NamedObjectModel::NamedObjectModel(QObject *parent)
    : EnhancedTreeModel(parent)
    , m_model(new WrappedStandardItemModel(this))
{
}

QAbstractItemModel *NamedObjectModel::itemModel()
{
    return m_model;
}

const QAbstractItemModel *NamedObjectModel::itemModel() const
{
    return m_model;
}

QVariant NamedObjectModel::uuid(const QModelIndex &index) const
{
    return index.data(cUuidRole);
}

QVariant NamedObjectModel::data(const QVariant &uuid, int role) const
{
    return dataAdapter(m_model, uuid, role);
}

std::unique_ptr<QFile> settingsFile(const QString &prefix)
{
    auto &s             = Settings::global();
    const auto fileName = s.additionalFile(prefix + ".json");
    return std::make_unique<QFile>(fileName);
}

void NamedObjectModel::readSettings(const QString &prefix)
{
    finally emptyGuard([this] {
        // we always need at least one object
        if (m_model->invisibleRootItem()->rowCount() == 0 && !emptyModelOk()) {
            createObject();
        }
    });

    auto file = settingsFile(prefix);
    if (!file->exists()) {
        return;
    }
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        log(tr("Could not open file '%1' to read settings").arg(file->fileName()));
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(file->readAll());
    if (document.isNull()) {
        log(tr("Failed to parse JSON data for '%1'").arg(file->fileName()));
        return;
    }

    QJsonObject jsonObject = document.object();
    if ((jsonObject["prefix"].toString() != prefix) || (jsonObject["version"].toInt() != 1)) {
        log(tr("Wrong version for file '%1'").arg(file->fileName()));
        return;
    }

    std::function<void(const QJsonArray &, QStandardItem *)> parseChildren =
        [&parseChildren, &file, this](const QJsonArray &children, QStandardItem *parent) {
            for (int i = 0; i < children.size(); ++i) {
                const QJsonObject child = children[i].toObject();
                if (child["type"] == to_underlying(ItemType::Folder)) {
                    auto *item = createFolder(parent->index(), child["uuid"].toString());
                    item->setText(child["name"].toString());
                    parseChildren(child["children"].toArray(), item);
                } else if (child["type"] == to_underlying(ItemType::Object)) {
                    auto *item = createObject(parent->index(), child["uuid"].toString());
                    item->setText(child["name"].toString());
                    readItem(child["data"], item);
                } else {
                    log(tr("Unexpected type '%1' in json '%2'")
                            .arg(child["type"].toInt())
                            .arg(file->fileName()));
                }
            }
        };

    m_model->beginResetModel();
    finally f([this]() { m_model->endResetModel(); });
    m_model->clear();
    parseChildren(jsonObject["tree"].toArray(), m_model->invisibleRootItem());
}

void NamedObjectModel::storeSettings(const QString &prefix) const
{
    std::function<QJsonArray(QStandardItem * item)> fct =
        [this, &fct](QStandardItem *item) -> QJsonArray {
        auto json = QJsonArray();
        for (int i = 0; i < item->rowCount(); ++i) {
            QStandardItem *childItem = item->child(i);
            if (isFolder(childItem->index())) {
                json.append(QJsonObject(
                    { { "type", to_underlying(ItemType::Folder) },
                      { "name", childItem->text() },
                      { "uuid", childItem->data(cUuidRole).toUuid().toString(QUuid::WithoutBraces) },
                      { "children", fct(childItem) } }));
            } else {
                json.append(QJsonObject(
                    { { "type", to_underlying(ItemType::Object) },
                      { "name", childItem->text() },
                      { "uuid", childItem->data(cUuidRole).toUuid().toString(QUuid::WithoutBraces) },
                      { "data", storeItem(childItem) } }));
            }
        }
        return json;
    };

    auto json = QJsonObject(
        { { "prefix", prefix }, { "version", 1 }, { "tree", fct(m_model->invisibleRootItem()) } });
    auto file = settingsFile(prefix);
    if (file->open(QIODevice::WriteOnly)) {
        file->write(QJsonDocument(json).toJson());
    } else {
        log(tr("Could not open file '%1' to store settings").arg(file->fileName()));
    }
}

QStandardItem *NamedObjectModel::createFolder()
{
    return createFolder(m_model->invisibleRootItem()->index());
}

QStandardItem *NamedObjectModel::createFolder(const QModelIndex &parent, const QString &uuid)
{
    if (parent.isValid() && !isFolder(parent)) {
        return nullptr;
    }

    auto item = std::make_unique<QStandardItem>(tr("new folder"));
    item->setIcon(m_fileIconProvider.icon(QFileIconProvider::Folder));
    item->setEditable(true);
    item->setDragEnabled(true);
    item->setDropEnabled(true);
    item->setData(uuid.isEmpty() ? QUuid::createUuid() : QUuid::fromString(uuid), cUuidRole);
    item->setData(Folder, cTypeRole);

    if (parent.isValid()) {
        m_model->itemFromIndex(parent)->appendRow(item.get());
    } else {
        m_model->appendRow(item.get());
    }
    return item.release();
}

QStandardItem *NamedObjectModel::createObject(const QString &uuid)
{
    return createObject(m_model->invisibleRootItem()->index(), uuid);
}

QStandardItem *NamedObjectModel::createObject(const QModelIndex &parent, const QString &uuid)
{
    if (parent.isValid() && !isFolder(parent)) {
        return nullptr;
    }

    auto item = std::make_unique<QStandardItem>(newObjectName());
    item->setIcon(m_fileIconProvider.icon(QFileIconProvider::File));
    item->setEditable(true);
    item->setDragEnabled(true);
    item->setDropEnabled(false);
    item->setData(uuid.isEmpty() ? QUuid::createUuid() : QUuid::fromString(uuid), cUuidRole);
    item->setData(Object, cTypeRole);

    if (parent.isValid()) {
        m_model->itemFromIndex(parent)->appendRow(item.get());
    } else {
        m_model->appendRow(item.get());
    }
    return item.release();
}

void NamedObjectModel::renameItem(const QModelIndex &index, const QString &newName)
{
    if (index.isValid()) {
        m_model->itemFromIndex(index)->setText(newName);
    }
}

bool NamedObjectModel::isFolder(const QModelIndex &idx) const
{
    return m_model->data(idx, cTypeRole).value<ItemType>() == Folder;
}

void NamedObjectModel::readItem(const QJsonValue &value, QStandardItem *item)
{
    if (item) {
        item->setData(value, cDataRole);
    }
}

QJsonValue NamedObjectModel::storeItem(QStandardItem *item) const
{
    return item ? item->data(cDataRole).toJsonValue() : QJsonValue();
}

QStandardItem *NamedObjectModel::itemFromIndex(const QModelIndex &idx) const
{
    return m_model->itemFromIndex(idx);
}

QString NamedObjectModel::newObjectName() const
{
    return tr("new object");
}
