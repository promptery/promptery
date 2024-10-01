#pragma once

#include <QAction>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

class QActionGroup;
class QStackedWidget;

class TabView : public QWidget
{
    Q_OBJECT

public:
    TabView(const QString &settingsId, bool exclusiveMode, QWidget *parent = nullptr);

    void addPage(const QString &title, QWidget *page, bool enable = false);

    void readSettings();
    void storeSettings() const;

    QWidget *toolBarWidget();

    bool anyTabVisible() const;
    Q_SIGNAL void tabVisible(bool anyVisible);

private:
    void setVisibleInternal(bool visible);
    void sendTabVisible();

    QString m_settingsId;

    QVBoxLayout *m_layout;
    QStackedWidget *m_stack;
    QToolBar *m_toolBar;
    QActionGroup *m_actGroup;
    std::vector<QWidget *> m_pages;
    bool m_exclusiveMode;
};
