#pragma once

#include <ui/page_interface.h>
#include <ui/tile_child_interface.h>

#include <QWidget>

class NamedObjectView;
class DecoratorPromptModel;

namespace Ui
{
class DecoratorPromptWidget;
}

class DecoratorPromptWidget : public TileChildInterface, public PageInterface
{
    Q_OBJECT

public:
    DecoratorPromptWidget(DecoratorPromptModel *decoratorPromptModel, QWidget *parent = nullptr);
    ~DecoratorPromptWidget();

    DecoratorPromptModel *model() const { return m_model; }

    PageData pageData() const override;
    TileChildInterface *mainView() override { return this; }
    QWidget *sideView() override;

    TileChildData data() const override;

private:
    Q_SLOT void currentIndexChanged(const QModelIndex &idx);

    Q_SLOT void writeTextToModel();

    Ui::DecoratorPromptWidget *ui;

    DecoratorPromptModel *m_model;

    NamedObjectView *m_tree;
};
