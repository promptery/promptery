#pragma once

#include <QTreeView>

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    TreeView(QWidget *parent = nullptr)
        : QTreeView(parent)
    {
    }

    Q_SIGNAL void currentIndexChanged(const QModelIndex &current);

    static TreeView *newListView(QWidget *parent = nullptr)
    {
        auto view = std::make_unique<TreeView>(parent);
        view->setRootIsDecorated(false);
        return view.release();
    }

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override
    {
        // ToDo: this has a problem, when an item gets removed
        QTreeView::currentChanged(current, previous);
        Q_EMIT currentIndexChanged(current);
    }
};
