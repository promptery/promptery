#pragma once

#include <QObject>

class ComboBox;
class ChatRequestConfigModel;

class QGridLayout;

class ChatRequestConfigWidget : public QObject
{
    Q_OBJECT
public:
    explicit ChatRequestConfigWidget(ChatRequestConfigModel *model, QWidget *parent = nullptr);

    void setupUi(QGridLayout *parentGrid, int row);

    void readSettings();

private:
    Q_SLOT void onSelectedBackendChanged();
    Q_SLOT void onSelectedModelChanged();
    Q_SLOT void onSelectedSystemPromptChanged();

    Q_SLOT void modelsAvailable(const QString &backendId);

    ChatRequestConfigModel *m_model;

    ComboBox *m_cmbBackend;
    ComboBox *m_cmbModel;
    ComboBox *m_cmbSystemPrompt;

    QWidget *m_parent;
};
