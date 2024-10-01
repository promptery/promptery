#include <QAbstractItemModel>
#include <QModelIndex>

#include <vector>

#pragma once

// Note: this class does not support fine grained updates, only model reset
class TreeToListModelAdapter : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TreeToListModelAdapter(QAbstractItemModel *sourceModel, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (!index.isValid()) {
            return {};
        }
        return m_sourceModel->data(mapToSource(index), role);
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override
    {
        if (hasIndex(row, column, parent)) {
            return createIndex(row, column);
        }
        return {};
    }

    QModelIndex parent(const QModelIndex &child) const override { return QModelIndex(); }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        if (parent.isValid()) {
            return 0;
        }
        return m_unfoldedData.size();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return m_sourceModel->columnCount(parent);
    }

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const
    {
        if (!proxyIndex.isValid()) {
            return {};
        }
        return m_unfoldedData[proxyIndex.row()];
    }

    Q_SIGNAL void beginModelChange();
    Q_SIGNAL void endModelChange();

protected:
    void updateModel()
    {
        if (m_modelResetOngoning == 0) {
            Q_EMIT beginModelChange();
            beginResetModel();
            m_unfoldedData.clear();
            unfoldTree(QModelIndex());
            endResetModel();
            Q_EMIT endModelChange();
        }
    }

    void unfoldTree(const QModelIndex &parent)
    {
        int rows = m_sourceModel->rowCount(parent);
        for (int row = 0; row < rows; ++row) {
            auto child = m_sourceModel->index(row, 0, parent);
            if (m_sourceModel->hasChildren(child)) {
                unfoldTree(child);
            } else {
                m_unfoldedData.push_back(child);
            }
        }
    }

    QAbstractItemModel *m_sourceModel;
    std::vector<QModelIndex> m_unfoldedData;
    int m_modelResetOngoning{ 0 };
};
