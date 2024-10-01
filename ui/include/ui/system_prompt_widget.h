#pragma once

#include <ui/page_interface.h>
#include <ui/tile_child_interface.h>

#include <QWidget>

class NamedObjectView;
class SystemPromptModel;

namespace Ui
{
class SystemPromptWidget;
}

class SystemPromptWidget : public TileChildInterface, public PageInterface
{
    Q_OBJECT

public:
    SystemPromptWidget(SystemPromptModel *systemPromptModel, QWidget *parent = nullptr);
    ~SystemPromptWidget();

    SystemPromptModel *model() const { return m_model; }

    PageData pageData() const override;
    TileChildInterface *mainView() override { return this; }
    QWidget *sideView() override;

    TileChildData data() const override;

private:
    Q_SLOT void currentIndexChanged(const QModelIndex &idx);

    Q_SLOT void writeTextToModel();

    Ui::SystemPromptWidget *ui;

    SystemPromptModel *m_model;

    NamedObjectView *m_tree;
};
