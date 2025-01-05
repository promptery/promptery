#include "mainwindow_app.h"
#include "ui_mainwindow_app.h"

#include <ui/chat_main_widget.h>
#include <ui/defines.h>

#include <common/settings.h>

#include <QActionGroup>
#include <QApplication>
#include <QComboBox>
#include <QDesktopServices>
#include <QMessageBox>
#include <QScreen>
#include <QToolBar>
#include <QUrl>

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

    connect(ui->actAbout, &QAction::triggered, this, [this]() {
        QString version(VERSION);
        QMessageBox::about(this, tr("About Promptery"), tr("Promptery version %1").arg(version));
    });
    connect(ui->actAboutQt, &QAction::triggered, this, [this]() { QMessageBox::aboutQt(this); });

    connect(ui->actSave, &QAction::triggered, this, &MainWindowApp::storeSettings);

    connect(
        ui->actStoreWindowPosition, &QAction::triggered, this, &MainWindowApp::storeWindowSettings);
    connect(ui->actOpenSettingsDirectory, &QAction::triggered, this, []() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(Settings::global().settingsPath()));
    });

    readSettings();
    m_chatMainWidget->initialize();
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
        setGeometry(
            QRect(0, 0, screen->geometry().height() * 0.9, screen->geometry().height() * 0.75));
        move((screen->geometry().width() - geometry().width()) / 2,
             (screen->geometry().height() - geometry().height()) / 2);
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
