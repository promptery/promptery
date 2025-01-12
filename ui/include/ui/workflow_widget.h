#pragma once

#include <ui/tile_child_interface.h>

#include <QWidget>

class ComboBox;
class WorkflowModel;
class ChatRequestConfigWidget;

class QGridLayout;

class WorkflowWidget : public TileChildInterface
{
    Q_OBJECT
public:
    WorkflowWidget(WorkflowModel *workflowModel, QWidget *parent = nullptr);

    TileChildData data() const override { return { "Process Config" }; }

protected:
    void showEvent(QShowEvent *event) override;

private:
    Q_SLOT void onSelectedDecoratorChanged();

    Q_SLOT void onSelectedWorkflowChanged();

    WorkflowModel *m_workflowModel;

    ChatRequestConfigWidget *m_baseModelConfig;
    ChatRequestConfigWidget *m_refineModelConfig;

    QGridLayout *m_layout;

    ComboBox *m_cmbDecorator;

    ComboBox *m_cmbWorkflow;
};
