#pragma once

#include <ui/tile_child_interface.h>

#include <QWidget>

class ComboBox;
class WorkflowModel;
class ChatRequestConfigWidget;

class QDoubleSpinBox;
class QGridLayout;
class QSpinBox;

class WorkflowWidget : public TileChildInterface
{
    Q_OBJECT
public:
    WorkflowWidget(WorkflowModel *workflowModel, QWidget *parent = nullptr);

    TileChildData data() const override { return { "Process Config" }; }

    void updateUi();

private:
    Q_SLOT void onSelectedDecoratorChanged();

    Q_SLOT void onSelectedWorkflowChanged();

    Q_SLOT void onIntOptionsChanged(int);
    Q_SLOT void onDoubleOptionsChanged(double);

    WorkflowModel *m_workflowModel;

    ChatRequestConfigWidget *m_baseModelConfig;
    ChatRequestConfigWidget *m_refineModelConfig;

    QGridLayout *m_layout;

    ComboBox *m_cmbDecorator;

    ComboBox *m_cmbWorkflow;

    QSpinBox *m_ctx;
    QSpinBox *m_seed;
    QDoubleSpinBox *m_temp;
};
