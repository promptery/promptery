#include <ui/tab_view.h>

#include <common/settings.h>

#include <QActionGroup>
#include <QStackedWidget>

// ToDo: Splitter samt Speichern dafür fehlen noch

TabView::TabView(const QString &settingsId, bool exclusiveMode, QWidget *parent)
    : QWidget(parent)
    , m_settingsId(settingsId)
    , m_layout(new QVBoxLayout(this))
    , m_stack(exclusiveMode ? new QStackedWidget(this) : nullptr)
    , m_toolBar(new QToolBar(this))
    , m_actGroup(new QActionGroup(m_toolBar))
    , m_exclusiveMode(exclusiveMode)
{
    setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);

    if (m_stack) {
        m_layout->addWidget(m_stack);
    }

    m_layout->addWidget(m_toolBar);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    m_toolBar->layout()->setContentsMargins(0, 0, 0, 0);

    m_actGroup->setExclusive(m_exclusiveMode);
    m_actGroup->setExclusionPolicy(m_exclusiveMode ?
                                       QActionGroup::ExclusionPolicy::ExclusiveOptional :
                                       QActionGroup::ExclusionPolicy::None);
}

void TabView::addPage(const QString &title, QWidget *page, bool enable)
{
    if (m_stack) {
        auto *w = new QWidget(m_stack);
        auto *l = new QVBoxLayout(w);
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(0);
        w->setLayout(l);
        l->addWidget(page);
        l->addStretch();
        m_stack->addWidget(w);
        if (enable) {
            m_stack->setCurrentWidget(w);
        }
    } else {
        m_layout->insertWidget(m_pages.size(), page);
        page->setVisible(enable);
    }

    m_pages.push_back(page);
    const auto index = m_pages.size();
    auto action      = new QAction(title.isEmpty() ? QString::number(index) : title, this);
    action->setCheckable(true);
    action->setChecked(enable);
    m_actGroup->addAction(action);
    connect(action, &QAction::triggered, this, [this, action, index]() {
        if (m_stack) {
            m_stack->setCurrentIndex(index - 1);
            setVisibleInternal(action->isChecked());
        } else {
            m_pages[index - 1]->setVisible(action->isChecked());
        }
        sendTabVisible();
    });
    m_toolBar->addAction(action);
}

void TabView::readSettings()
{
    auto &s = Settings::global();
    s.sync();

    if (!s.contains(m_settingsId)) {
        sendTabVisible();
        return;
    }

    setVisibleInternal(false);
    if (!m_stack) {
        for (auto &p : m_pages) {
            p->hide();
        }
    }
    for (auto &a : m_actGroup->actions()) {
        a->setChecked(false);
    }

    QStringList visibleList = s.value(m_settingsId).toString().split(",");
    for (const auto &i : visibleList) {
        bool ok    = false;
        auto index = i.toInt(&ok);
        if (ok && index < m_pages.size()) {
            if (m_stack) {
                m_stack->setCurrentIndex(index);
                setVisibleInternal(true);
            } else {
                m_pages[index]->show();
            }
            m_actGroup->actions().at(index)->setChecked(true);
        }
    }
    sendTabVisible();
}

void TabView::storeSettings() const
{
    auto &s = Settings::global();

    QString activeList;
    const auto &actions = m_actGroup->actions();
    for (std::size_t i = 0; i < actions.size(); ++i) {
        if (actions[i]->isChecked()) {
            if (!activeList.isEmpty()) {
                activeList += ",";
            }
            activeList += QString::number(i);
        }
    }
    s.setValue(m_settingsId, activeList);

    s.sync();
}

QWidget *TabView::toolBarWidget()
{
    return m_toolBar;
}

bool TabView::anyTabVisible() const
{
    bool anyVisible = false;
    if (m_stack) {
        anyVisible = m_stack->isVisible();
    } else {
        for (const auto &page : m_pages) {
            anyVisible = anyVisible || page->isVisible();
        }
    }
    return anyVisible;
}

void TabView::setVisibleInternal(bool visible)
{
    if (m_stack) {
        if (m_toolBar->parent() == this) {
            m_stack->setVisible(visible);
        } else {
            setVisible(visible);
        }
    }
}

void TabView::sendTabVisible()
{
    Q_EMIT tabVisible(anyTabVisible());
}
