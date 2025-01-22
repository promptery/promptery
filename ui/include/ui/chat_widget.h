#pragma once

#include <model/chat_types.h>

#include <ui/page_interface.h>
#include <ui/system_prompt_model.h>
#include <ui/tile_child_interface.h>

#include <QTimer>

#include <vector>

class AdditionalContentModel;
class BackendManager;
class ChatItemWidget;
class ChatModel;
class ContentModel;
class EnhancedTreeView;
class FileSystemModel;
class LogWidget;
class NamedObjectView;
class TabView;
class WorkflowModel;
class WorkflowWidget;

class QNetworkReply;
class QPushButton;
class QToolBar;


class WorkflowProcessor;
namespace Ui
{
class ChatWidget;
}

// ToDo:
// Andere Email wenn es auf Github soll, also im neuen Repo
// Antworten
// * einfalten
// * löschen
// * wiederholen
// * temporär aus dem Kontext entfernen
// ChatModell
// Scrollbar in den Chats
// Aufgeschlagenen Modelleintrag merken, oder erste Seite aufschlagen

class ChatWidget : public TileChildInterface, public PageInterface
{
    Q_OBJECT

public:
    explicit ChatWidget(ChatModel *chatModel,
                        ContentModel *contentModel,
                        WorkflowModel *workflowModel,
                        QWidget *parent = nullptr);
    ~ChatWidget();

    PageData pageData() const override;
    TileChildInterface *mainView() override { return this; }
    QWidget *sideView() override;

    void readSettings() override;
    void prepareStoreSettings() override;
    void storeSettings() const override;
    void saveState() const override;

    TileChildData data() const override;
    std::vector<QWidget *> actionWidgetsRight() override;

    Q_SLOT void exportChat();

    static QString sName(const char *prop);

    Q_SIGNAL void logMessage(const QString &msg);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Q_SLOT void clear();
    Q_SLOT void btnGoClicked();
    Q_SLOT void btnClearInputClicked();
    Q_SLOT void abortReply();
    Q_SLOT void workflowReady(bool isReady);

    // model connection
    Q_SLOT void currentIndexChanged(const QModelIndex &idx);
    Q_SLOT void writeTextToModel();

    ChatItemWidget *createNewItem(int index);
    void setCurrent(ChatItemWidget *current);

    void cleanReply();

    void btnGoShowPlay();
    void btnGoShowStop();

    Q_SLOT void updateScroll();
    void setScrollSpacerToIdealheight();

    bool startQuery(QString query, ContextFiles contextFiles, ContextPages contextPages);

    QJsonArray chatAsJson(bool forSaving) const;


    Q_SLOT void procBeginBlock(int index, const QString &title);
    Q_SLOT void procEndBlock(int index);
    Q_SLOT void procNewContent(const QString &content);
    Q_SLOT void procFinished();
    void newOutput(int diff);

    Ui::ChatWidget *ui;
    QPushButton *m_btnGo;
    QPushButton *m_btnClearInput;

    QWidget *m_sideView;
    TabView *m_sideViewWidgets;

    TabView *m_toolWidgets;
    WorkflowModel *m_workflowModel;
    WorkflowWidget *m_workflowWidget;
    LogWidget *m_logWidget;

    ChatModel *m_model;
    NamedObjectView *m_tree;

    ContentModel *m_contentModel;
    AdditionalContentModel *m_additionalContentModel;
    EnhancedTreeView *m_contentTree;

    FileSystemModel *m_fileModel;
    EnhancedTreeView *m_fileTree;

    WorkflowProcessor *m_processor;

    std::vector<ChatItemWidget *> m_queries;
    ChatItemWidget *m_current{ nullptr };

    QTimer m_scrollTimer;
    int m_scrollTarget = 0;

    std::vector<QString> m_inputHistory;
    int64_t m_currentInputIdx{ 0 };

    QVariant m_currentChatId;
};
