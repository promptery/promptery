#include "ui_chat_main_widget.h"
#include <ui/chat_main_widget.h>

#include <ui/chat_model.h>
#include <ui/chat_widget.h>
#include <ui/content_model.h>
#include <ui/content_widget.h>
#include <ui/decorator_prompt_model.h>
#include <ui/decorator_prompt_widget.h>
#include <ui/settings_widget.h>
#include <ui/system_prompt_model.h>
#include <ui/system_prompt_widget.h>
#include <ui/tile_widget.h>
#include <ui/workflow_model.h>
#include <ui/workflow_widget.h>

#include <model/backend_manager.h>

#include <common/settings.h>

ChatMainWidget::ChatMainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatMainWidget)
    , m_backends(new BackendManager(this))
    , m_chatModel(new ChatModel(this))
    , m_contentModel(new ContentModel(this))
    , m_decoratorPromptModel(new DecoratorPromptModel(this))
    , m_systemPromptModel(new SystemPromptModel(this))
    , m_workflowModel(
          new WorkflowModel(m_backends, m_systemPromptModel, m_decoratorPromptModel, this))
    , m_settingWidget(new SettingsWidget(m_backends, this))
    , m_pages({ { new ChatWidget(m_chatModel, m_contentModel, m_workflowModel, this), nullptr },
                { new ContentWidget(m_contentModel, this), nullptr },
                { new SystemPromptWidget(m_systemPromptModel, this), nullptr },
                { new DecoratorPromptWidget(m_decoratorPromptModel, this), nullptr },
                { m_settingWidget, nullptr } })
{
    ui->setupUi(this);

    for (auto &page : m_pages) {
        if (page.page->sideView()) {
            ui->leftSide->layout()->addWidget(page.page->sideView());
        }
        page.tile = new TileWidget(page.page->mainView(), ui->rightSide);
        ui->rightSide->layout()->addWidget(page.tile);
    }

    setCurrentIndex(0);
}

ChatMainWidget::~ChatMainWidget()
{
    delete ui;
}

std::vector<PageData> ChatMainWidget::pages() const
{
    std::vector<PageData> res;
    res.reserve(m_pages.size());
    for (const auto &page : m_pages) {
        res.push_back(page.page->pageData());
    }
    return res;
}

void ChatMainWidget::setCurrentIndex(int pageIdx)
{
    if (pageIdx >= m_pages.size()) {
        return;
    }

    for (auto &page : m_pages) {
        if (page.page->sideView()) {
            page.page->sideView()->hide();
        }
        page.tile->hide();
    }

    auto &current = m_pages[pageIdx];
    if (current.page->sideView()) {
        current.page->sideView()->show();
    }
    current.tile->show();
}

constexpr const char *cSplitMain = "splitMain";

void ChatMainWidget::readSettings()
{
    auto &s = Settings::global();
    s.sync();

    if (s.contains(ChatWidget::sName(cSplitMain))) {
        ui->splitMain->restoreState(s.value(ChatWidget::sName(cSplitMain)).toByteArray());
    } else {
        m_settingsAvailable = false;
    }

    m_decoratorPromptModel->readSettings();
    m_systemPromptModel->readSettings();
    m_workflowModel->readSettings();
    m_contentModel->readSettings();
    m_chatModel->readSettings();

    m_settingWidget->readSettings();
    for (auto &page : m_pages) {
        page.page->readSettings();
    }

    m_settingWidget->updateUi();
    for (auto &page : m_pages) {
        page.page->updateUi();
    }
}

void ChatMainWidget::initialize()
{
    m_backends->initialize();
}

void ChatMainWidget::storeSettings()
{
    for (const auto &page : m_pages) {
        page.page->prepareStoreSettings();
    }

    m_decoratorPromptModel->storeSettings();
    m_systemPromptModel->storeSettings();
    m_workflowModel->storeSettings();
    m_contentModel->storeSettings();
    m_chatModel->storeSettings();

    m_settingWidget->storeSettings();

    for (const auto &page : m_pages) {
        page.page->storeSettings();
    }
}

void ChatMainWidget::saveState()
{
    auto &s = Settings::global();
    s.setValue(ChatWidget::sName(cSplitMain), ui->splitMain->saveState());

    for (const auto &page : m_pages) {
        page.page->saveState();
    }
    s.sync();
}

void ChatMainWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    if (!m_settingsAvailable) {
        const auto w = ui->splitMain->width();
        const auto l = w * 2 / 9;
        const auto r = w - l;
        ui->splitMain->setSizes({ l, r });
    }
}
