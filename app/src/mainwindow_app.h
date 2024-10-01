#pragma once

#include <QMainWindow>

#include <common/settings.h>

class ChatMainWidget;

namespace Ui
{
class MainWindowApp;
}

class MainWindowApp : public QMainWindow, WithSettings
{
    Q_OBJECT

public:
    explicit MainWindowApp(QWidget *parent = nullptr);
    ~MainWindowApp();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void readSettings();
    Q_SLOT void storeSettings();
    Q_SLOT void storeWindowSettings();

    QToolBar *m_toolBar;

    ChatMainWidget *m_chatMainWidget;

    Ui::MainWindowApp *ui;
};
