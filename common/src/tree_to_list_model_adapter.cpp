#include <common/tree_to_list_model_adapter.h>

TreeToListModelAdapter::TreeToListModelAdapter(QAbstractItemModel *sourceModel, QObject *parent)
    : QAbstractItemModel(parent)
    , m_sourceModel(sourceModel)
{
    connect(
        sourceModel, &QAbstractItemModel::rowsInserted, this, &TreeToListModelAdapter::updateModel);
    connect(
        sourceModel, &QAbstractItemModel::rowsRemoved, this, &TreeToListModelAdapter::updateModel);
    connect(
        sourceModel, &QAbstractItemModel::dataChanged, this, &TreeToListModelAdapter::updateModel);

    connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, [this]() {
        ++m_modelResetOngoning;
        Q_EMIT beginModelChange();
        beginResetModel();
    });
    connect(sourceModel, &QAbstractItemModel::modelReset, this, [this]() {
        m_unfoldedData.clear();
        unfoldTree(QModelIndex());
        endResetModel();
        Q_EMIT endModelChange();
        --m_modelResetOngoning;
    });
}
