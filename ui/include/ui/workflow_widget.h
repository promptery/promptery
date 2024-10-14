#pragma once

#include <ui/tile_child_interface.h>

#include <QWidget>

class ComboBox;
class WorkflowModel;

class WorkflowWidget : public TileChildInterface
{
    Q_OBJECT
public:
    WorkflowWidget(WorkflowModel *workflowModel, QWidget *parent = nullptr);

    TileChildData data() const override { return { "Process" }; }

protected:
    void showEvent(QShowEvent *event) override;

private:
    Q_SLOT void onSelectedBackendChanged();
    Q_SLOT void onSelectedModelChanged();
    Q_SLOT void onSelectedDecoratorChanged();
    Q_SLOT void onSelectedSystemPromptChanged();

    Q_SLOT void onSelectedWorkflowChanged();

    Q_SLOT void modelsAvailable(const QString &backendId);

    WorkflowModel *m_workflowModel;

    ComboBox *m_cmbBackend;
    ComboBox *m_cmbModel;
    ComboBox *m_cmbDecorator;
    ComboBox *m_cmbSystemPrompt;

    ComboBox *m_cmbWorkflow;
};
