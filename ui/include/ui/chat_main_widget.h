#pragma once

#include <ui/page_interface.h>

#include <QWidget>

#include <vector>

class BackendManager;
class ChatModel;
class ContentModel;
class DecoratorPromptModel;
class SystemPromptModel;
class WorkflowModel;

namespace Ui
{
class ChatMainWidget;
}

class ChatMainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatMainWidget(QWidget *parent = nullptr);
    ~ChatMainWidget();

    std::vector<PageData> pages() const;

    void setCurrentIndex(int pageIdx);

    void readSettings();
    void storeSettings();
    void saveState();

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::ChatMainWidget *ui;

    BackendManager *m_backends;
    ChatModel *m_chatModel;
    ContentModel *m_contentModel;
    DecoratorPromptModel *m_decoratorPromptModel;
    SystemPromptModel *m_systemPromptModel;
    WorkflowModel *m_workflowModel;

    struct PageWrapper {
        PageInterface *page{ nullptr };
        QWidget *tile{ nullptr };
    };

    std::vector<PageWrapper> m_pages;

    bool m_settingsAvailable{ true };
};
