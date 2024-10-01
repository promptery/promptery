#include <ui/file_system_model.h>

#include <ui/defines.h>

#include <QApplication>
#include <QDirIterator>
#include <QFileDialog>
#include <QPushButton>
#include <QStandardItemModel>

#include <common/settings.h>

FileSystemModel::FileSystemModel(QObject *parent)
    : EnhancedTreeModel(parent)
    , m_model(new QStandardItemModel(this))
    , m_rootPath(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).at(0))
    , m_actOpen(new QAction(QIcon(":/icons/open.svg"), tr("Open"), this))
{
    connect(m_model, &QStandardItemModel::itemChanged, this, &CheckableModel::onItemChanged);
    connect(m_actOpen, &QAction::triggered, this, &FileSystemModel::onActOpen);
}

QAbstractItemModel *FileSystemModel::itemModel()
{
    return m_model;
}

const QAbstractItemModel *FileSystemModel::itemModel() const
{
    return m_model;
}

ContextFiles FileSystemModel::contextFiles() const
{
    const auto ids = CheckableModel::getChecked(m_model->invisibleRootItem());
    std::vector<QString> res;
    res.reserve(ids.size());
    std::transform(ids.begin(), ids.end(), std::back_inserter(res), [](const auto &val) {
        return val.toString();
    });
    std::sort(res.begin(), res.end());
    return { rootPath(), std::move(res) };
}

void FileSystemModel::setRootPath(const QString &path)
{
    m_model->clear();
    if (path.isEmpty()) {
        return;
    }

    QStandardItem *rootItem = m_model->invisibleRootItem();

    m_rootPath = path;
    addFilesRecursively(path, rootItem);
    Q_EMIT displayNameChanged();
}

TileChildData FileSystemModel::tileData() const
{
    if (!m_rootPath.isEmpty()) {
        QFileInfo info(m_rootPath);
        return TileChildData{ tr("Folder: %1").arg(info.baseName()), info.canonicalFilePath() };
    }
    return { tr("External folder") };
}

std::vector<QAction *> FileSystemModel::actions() const
{
    return { m_actOpen };
}

static constexpr const char *cFolder = "folder";

void FileSystemModel::readSettings(const QString &prefix)
{
    auto &s = Settings::global();
    s.sync();
    setRootPath(s.value(prefix + "/" + cFolder, m_rootPath).toString());
}

void FileSystemModel::storeSettings(const QString &prefix) const
{
    auto &s = Settings::global();
    s.setValue(prefix + "/" + cFolder, m_rootPath);
    s.sync();
}

void FileSystemModel::onActOpen()
{
    QString dir = QFileDialog::getExistingDirectory(qApp->activeWindow(),
                                                    tr("Open Directory"),
                                                    m_rootPath,
                                                    QFileDialog::ShowDirsOnly |
                                                        QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        setRootPath(dir);
    }
}

void FileSystemModel::addFilesRecursively(const QString &path, QStandardItem *parent)
{
    QDirIterator it(path, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    while (it.hasNext()) {
        it.next();
        const QString filePath = it.filePath();
        const QString fileName = it.fileName();

        auto relPath = filePath;
        relPath.replace(m_rootPath + "/", "");

        QStandardItem *item = new QStandardItem(fileName);
        item->setCheckable(true);
        item->setData(relPath, cUuidRole);
        parent->appendRow(item);

        if (it.fileInfo().isDir()) {
            addFilesRecursively(filePath, item);
        }
    }
    parent->sortChildren(0);
}
