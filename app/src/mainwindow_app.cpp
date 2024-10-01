#include "mainwindow_app.h"
#include "ui_mainwindow_app.h"

#include <ui/chat_main_widget.h>
#include <ui/defines.h>

#include <common/settings.h>

#include <QActionGroup>
#include <QApplication>
#include <QComboBox>
#include <QDesktopServices>
#include <QScreen>
#include <QToolBar>

QString screenId()
{
    const auto &geo = qApp->primaryScreen()->geometry();
    return QString("%1x%2x%3x%4").arg(geo.x()).arg(geo.y()).arg(geo.width()).arg(geo.height());
}

//------------------------------------------------

MainWindowApp::MainWindowApp(QWidget *parent)
    : QMainWindow(parent)
    , WithSettings("promptery")
    , ui(new Ui::MainWindowApp)
    , m_toolBar(new QToolBar("Main", this))
    , m_chatMainWidget(new ChatMainWidget(this))
{
    ui->setupUi(this);

    ui->page1Chat->layout()->addWidget(m_chatMainWidget);

    m_toolBar->setObjectName("toolBarMain");
    m_toolBar->setMovable(false);
    m_toolBar->setAllowedAreas(Qt::LeftToolBarArea);
    m_toolBar->setIconSize(QSize(cHeaderHeight, cHeaderHeight));
    QMetaObject::invokeMethod(
        this, [this]() { addToolBar(Qt::LeftToolBarArea, m_toolBar); }, Qt::QueuedConnection);

    QActionGroup *btnGroup = new QActionGroup(this);
    btnGroup->setExclusive(true);

    const auto chatPages = m_chatMainWidget->pages();
    int count{ 0 };
    int chatCount{ 0 };

    ui->stackedWidget->setCurrentIndex(count);
    m_chatMainWidget->setCurrentIndex(chatCount);

    for (const auto &page : chatPages) {
        auto *action = m_toolBar->addAction(page.icon, page.tooltip, [this, count, chatCount] {
            ui->stackedWidget->setCurrentIndex(count);
            m_chatMainWidget->setCurrentIndex(chatCount);
        });
        action->setCheckable(true);
        btnGroup->addAction(action);
        if (chatCount == 0) {
            action->setChecked(true);
        }
        ++chatCount;
    }


    connect(ui->actSave, &QAction::triggered, this, &MainWindowApp::storeSettings);

    connect(
        ui->actStoreWindowPosition, &QAction::triggered, this, &MainWindowApp::storeWindowSettings);

    readSettings();
}

MainWindowApp::~MainWindowApp()
{
    delete ui;
}

void MainWindowApp::closeEvent(QCloseEvent *event)
{
    storeSettings();
    QMainWindow::closeEvent(event);
}

void MainWindowApp::readSettings()
{
    auto &s = Settings::global();

    s.sync();
    const auto screen = screenId();
    if (s.contains(screen + "/geometry")) {
        restoreGeometry(s.value(screen + "/geometry").toByteArray());
        restoreState(s.value(screen + "/windowState").toByteArray());
    } else {
        const auto *const screen = QApplication::screens().at(0);
        setGeometry(screen->geometry().adjusted(screen->geometry().width() / 8,
                                                screen->geometry().height() / 8,
                                                -screen->geometry().width() / 8,
                                                -screen->geometry().height() / 8));
        move(screen->geometry().width() / 8, screen->geometry().height() / 8);
    }
    m_chatMainWidget->readSettings();
}

void MainWindowApp::storeSettings()
{
    auto &s = Settings::global();
    m_chatMainWidget->storeSettings();
    s.sync();
}

void MainWindowApp::storeWindowSettings()
{
    const auto screen = screenId();
    auto &s           = Settings::global();
    s.setValue(screen + "/geometry", saveGeometry());
    s.setValue(screen + "/windowState", saveState());
    s.sync();
    m_chatMainWidget->saveState();
}