#pragma once

#include <ui/enhanced_treemodel.h>

#include <model/chat_types.h>

class QAction;
class QStandardItemModel;

class FileSystemModel : public EnhancedTreeModel
{
    Q_OBJECT
public:
    explicit FileSystemModel(QObject *parent = nullptr);

    QAbstractItemModel *itemModel() override;
    const QAbstractItemModel *itemModel() const override;

    ContextFiles contextFiles() const;

    void setRootPath(const QString &path);
    const QString &rootPath() const { return m_rootPath; }

    TileChildData tileData() const override;

    std::vector<QAction *> actions() const override;

    void readSettings(const QString &prefix) override;
    void storeSettings(const QString &prefix) const override;

private:
    Q_SLOT void onActOpen();

    void addFilesRecursively(const QString &path, QStandardItem *parent);

    QStandardItemModel *m_model;

    QString m_rootPath;

    QAction *m_actOpen;
    QString m_displayName;

    // EnhancedTreeModel interface
public:
};
