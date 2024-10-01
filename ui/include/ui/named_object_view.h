#pragma once

#include <ui/enhanced_treeview.h>

class NamedObjectModel;
class QStandardItem;

class NamedObjectView : public EnhancedTreeView
{
    Q_OBJECT
public:
    NamedObjectView(NamedObjectModel *model, QWidget *parent = nullptr);

    Q_SIGNAL void currentIndexChanged(const QModelIndex &current);

private:
    Q_SLOT void addFolder();
    Q_SLOT void addObject();
    Q_SLOT void deleteCurrent();

    template <typename Fct>
    QStandardItem *createItem(const Fct &fct);

    NamedObjectModel *m_model;
};
