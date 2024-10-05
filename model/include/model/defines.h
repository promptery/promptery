#pragma once

#include <QAbstractItemModel>
#include <QVariant>
#include <Qt>

#include <limits>

constexpr auto cTypeRole  = std::numeric_limits<std::underlying_type_t<Qt::ItemDataRole>>::max();
constexpr auto cUuidRole  = cTypeRole - 1;
constexpr auto cDataRole  = cTypeRole - 2;
constexpr auto cCheckRole = cTypeRole - 3;

// this function needs a better place...
// used, to allow the workflows in the package "model" while most data models are in "ui" due to
// QStandardItemModel which is in Qt::Gui
inline QVariant dataAdapter(QAbstractItemModel *model, const QVariant &uuid, int role)
{
    const auto ids =
        model->match(model->index(0, 0), cUuidRole, uuid, Qt::MatchExactly | Qt::MatchRecursive);
    if (ids.empty()) {
        return QVariant();
    }
    return ids[0].data(role);
}
