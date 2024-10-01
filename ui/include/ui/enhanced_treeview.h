#pragma once

#include <ui/tile_child_interface.h>

#include <QWidget>

class EnhancedTreeModel;
class TreeView;

class QHBoxLayout;
class QPushButton;

class EnhancedTreeView : public TileChildInterface
{
    Q_OBJECT
public:
    EnhancedTreeView(EnhancedTreeModel *model, QWidget *parent = nullptr);

    void readSettings(const QString &prefix);
    void storeSettings(const QString &prefix);

    QModelIndex currentIndex() const;

    // TileChildInterface interface
    TileChildData data() const override;
    std::vector<QWidget *> actionWidgetsRight() override;

    QWidget *asTile();

protected:
    void addButton(QAction *action);

    EnhancedTreeModel *m_model;

    TreeView *m_tree;

    std::vector<QPushButton *> m_buttons;
    QWidget *m_tile{ nullptr };
};
