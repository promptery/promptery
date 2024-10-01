#pragma once

#include <ui/page_interface.h>
#include <ui/tile_child_interface.h>

class NamedObjectView;
class ContentModel;

namespace Ui
{
class ContentWidget;
}

class ContentWidget : public TileChildInterface, public PageInterface
{
    Q_OBJECT

public:
    ContentWidget(ContentModel *contentModel, QWidget *parent = nullptr);
    ~ContentWidget();

    ContentModel *model() const { return m_model; }

    PageData pageData() const override;
    TileChildInterface *mainView() override { return this; }
    QWidget *sideView() override;

    TileChildData data() const override;

private:
    Q_SLOT void currentIndexChanged(const QModelIndex &idx);

    Q_SLOT void writeTextToModel();

    Ui::ContentWidget *ui;

    ContentModel *m_model;

    NamedObjectView *m_tree;
};
