#include "ui/chat_request_config_model.h"
#include "ui_chat_widget.h"
#include <ui/chat_widget.h>

#include <ui/chat_item_widget.h>
#include <ui/chat_model.h>
#include <ui/content_model.h>
#include <ui/defines.h>
#include <ui/enhanced_treeview.h>
#include <ui/file_system_model.h>
#include <ui/log_widget.h>
#include <ui/named_object_model.h>
#include <ui/named_object_view.h>
#include <ui/pushbutton.h>
#include <ui/system_prompt_widget.h>
#include <ui/tab_view.h>
#include <ui/tile_widget.h>
#include <ui/workflow_basic.h>
#include <ui/workflow_model.h>
#include <ui/workflow_widget.h>

#include <model/backend_manager.h>
#include <model/llm_interface.h>
#include <model/workflow_processor.h>

#include <common/log.h>
#include <common/settings.h>

#include <tools/is_one_of.h>

#include <QActionGroup>
#include <QBuffer>
#include <QFileDialog>
#include <QIdentityProxyModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QScrollBar>
#include <QStandardPaths>
#include <QStyleHints>
#include <QTextEdit>
#include <QToolBar>

constexpr int cScrollBuffer = 1000;


// This extension adds checking via a proxy model.
// A separate role is used to not interfere with the regular QStandardItem mechanics.
class ExtIdentityProxyModel : public QIdentityProxyModel
{
public:
    using QIdentityProxyModel::QIdentityProxyModel;

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        return QIdentityProxyModel::flags(index) | Qt::ItemIsUserCheckable;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == Qt::CheckStateRole) {
            const auto val = QIdentityProxyModel::data(index, cCheckRole);
            return val.isValid() ? val : 0;
        }
        return QIdentityProxyModel::data(index, role);
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) override
    {
        if (role == Qt::CheckStateRole) {
            return QIdentityProxyModel::setData(index, value, cCheckRole);
        }
        return QIdentityProxyModel::setData(index, value, role);
    }
};

// this class is a bit hacky to reuse other code...
class AdditionalContentModel : public EnhancedTreeModel
{
public:
    AdditionalContentModel(ContentModel *contentModel, QObject *parent = nullptr)
        : EnhancedTreeModel(parent)
        , m_source(contentModel)
        , m_model(new ExtIdentityProxyModel(m_source->itemModel()))
    {
        m_model->setSourceModel(contentModel->itemModel());

        QStandardItemModel *ptr = dynamic_cast<QStandardItemModel *>(m_source->itemModel());
        assert(ptr);
        connect(ptr, &QStandardItemModel::itemChanged, this, &CheckableModel::onItemChanged);
    }

    ContextPages contextPages() const
    {
        QStandardItemModel *ptr = dynamic_cast<QStandardItemModel *>(m_source->itemModel());
        assert(ptr);
        return { CheckableModel::getChecked(ptr->invisibleRootItem()) };
    }

    QAbstractItemModel *itemModel() override { return m_model; }
    const QAbstractItemModel *itemModel() const override { return m_model; }
    TileChildData tileData() const override { return { tr("Content") }; }
    void readSettings(const QString & /*prefix*/) override {}
    void storeSettings(const QString & /*prefix*/) const override {}

private:
    ContentModel *m_source;
    ExtIdentityProxyModel *m_model;
};


ChatWidget::ChatWidget(ChatModel *chatModel,
                       ContentModel *contentModel,
                       WorkflowModel *workflowModel,
                       QWidget *parent)
    : TileChildInterface(parent)
    , ui(new Ui::ChatWidget)
    , m_btnGo(new PushButton(QIcon(":/icons/play"), "", this))
    , m_btnClearInput(new PushButton(QIcon(":/icons/cancel"), "", this))
    , m_toolWidgets(new TabView("chat/tools", true, this))
    , m_workflowModel(workflowModel)
    , m_workflowWidget(new WorkflowWidget(m_workflowModel, this))
    , m_logWidget(new LogWidget(this))
    , m_model(chatModel)
    , m_tree(new NamedObjectView(m_model, this))
    , m_contentModel(contentModel)
    , m_additionalContentModel(new AdditionalContentModel(contentModel, this))
    , m_contentTree(new EnhancedTreeView(m_additionalContentModel, this))
    , m_fileModel(new FileSystemModel(this))
    , m_fileTree(new EnhancedTreeView(m_fileModel, this))
    , m_processor(new WorkflowProcessor(this))
{
    ui->setupUi(this);
    ui->frmInput->setBackgroundRole(QPalette::Base);
    ui->buttonLayout->insertWidget(0, m_btnClearInput);
    ui->buttonLayout->addWidget(m_btnGo);

    // sideview
    m_sideView = new QWidget(this);
    auto *l    = new QVBoxLayout(m_sideView);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    m_sideViewWidgets = new TabView("chat/sideView", false, m_sideView);
    l->addWidget(m_sideViewWidgets);
    l->addWidget(m_sideViewWidgets->toolBarWidget());

    m_sideViewWidgets->addPage(tr("Chats"), m_tree->asTile(), true);
    m_sideViewWidgets->addPage(tr("Content"), m_contentTree->asTile());
    m_sideViewWidgets->addPage(tr("Files"), m_fileTree->asTile());
    // ~sideView

    ui->splitInput->insertWidget(1, m_toolWidgets);

    m_toolWidgets->addPage(tr("Workflow"), new TileWidget(m_workflowWidget, this), true);
    m_toolWidgets->addPage(tr("Log"), new TileWidget(m_logWidget, this));
    ui->bottomBorderLayout->addWidget(m_toolWidgets->toolBarWidget());
    connect(m_toolWidgets, &TabView::tabVisible, this, [this](bool anyVisible) {
        ui->splitInput->setHandleWidth(anyVisible ? 4 : 0);
    });

    connect(globalLogger(), &GlobalLogger::logMessage, this, &ChatWidget::logMessage);
    connect(this, &ChatWidget::logMessage, m_logWidget, &LogWidget::append);

    connect(m_btnGo, &QPushButton::clicked, this, &ChatWidget::btnGoClicked);
    connect(m_btnClearInput, &QPushButton::clicked, this, &ChatWidget::btnClearInputClicked);

    connect(&m_scrollTimer, &QTimer::timeout, this, &ChatWidget::updateScroll);

    connect(m_workflowModel, &WorkflowModel::workflowReady, this, &ChatWidget::workflowReady);

    connect(m_processor, &WorkflowProcessor::beginBlock, this, &ChatWidget::procBeginBlock);
    connect(m_processor, &WorkflowProcessor::endBlock, this, &ChatWidget::procEndBlock);
    connect(m_processor, &WorkflowProcessor::newContent, this, &ChatWidget::procNewContent);
    connect(m_processor, &WorkflowProcessor::finished, this, &ChatWidget::procFinished);

    ui->edtInput->installEventFilter(this);
    ui->outputArea->installEventFilter(this);

    btnGoShowPlay();
    m_btnGo->setToolTip(tr("Start / Stop generation"));
    m_btnGo->setEnabled(false);
    m_btnClearInput->setToolTip(tr("Clear input field"));

    ui->scrollContent->setBackgroundRole(QPalette::Base);
    ui->splitInput->setCollapsible(0, false);
    ui->splitInput->setCollapsible(1, false);

    connect(m_tree, &NamedObjectView::currentIndexChanged, this, &ChatWidget::currentIndexChanged);

    currentIndexChanged(QModelIndex());
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

PageData ChatWidget::pageData() const
{
    return PageData{ tr("Chat"), QIcon(":/icons/chat.svg"), tr("Chat") };
}

QWidget *ChatWidget::sideView()
{
    return m_sideView;
}

constexpr const char *cBackend    = "backend";
constexpr const char *cChat       = "chat";
constexpr const char *cModel      = "model/";
constexpr const char *cSplitChat  = "splitChat";
constexpr const char *cSplitInput = "splitInput";
constexpr const char *cStack      = "stack";

void ChatWidget::readSettings()
{
    auto &s = Settings::global();
    s.sync();
    ui->splitChat->restoreState(s.value(sName(cSplitChat)).toByteArray());
    ui->splitInput->restoreState(s.value(sName(cSplitInput)).toByteArray());

    m_toolWidgets->readSettings();
    m_sideViewWidgets->readSettings();
    m_contentTree->readSettings(cChat);
    m_fileTree->readSettings(cChat);
}

void ChatWidget::updateUi()
{
    m_workflowWidget->updateUi();
}

void ChatWidget::prepareStoreSettings()
{
    writeTextToModel();
}

void ChatWidget::storeSettings() const
{
    m_contentTree->storeSettings(cChat);
    m_fileTree->storeSettings(cChat);
    m_toolWidgets->storeSettings();
    m_sideViewWidgets->storeSettings();
}

void ChatWidget::saveState() const
{
    auto &s = Settings::global();
    s.setValue(sName(cSplitChat), ui->splitChat->saveState());
    s.setValue(sName(cSplitInput), ui->splitInput->saveState());
    s.sync();
}

TileChildData ChatWidget::data() const
{
    return { tr("Chat") };
}

std::vector<QWidget *> ChatWidget::actionWidgetsRight()
{
    auto *btnClear = new PushButton(QIcon(":/icons/cancel"), "", this);
    btnClear->setToolTip(tr("Clear complete chat"));
    connect(btnClear, &QPushButton::clicked, this, &ChatWidget::clear);
    return { btnClear };
}

void ChatWidget::exportChat()
{
    static const auto filters = tr("Chat files (*.json);;All files (*.*)");

    const auto fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save chat"),
        QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).constFirst(),
        filters);
    if (fileName.isEmpty()) {
        return;
    }

    auto json = chatAsJson(true);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(json).toJson());
    }
}

QString ChatWidget::sName(const char *prop)
{
    return QString(cChat) + prop;
}

bool ChatWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->edtInput && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (is_one_of(keyEvent->key(), Qt::Key_Enter, Qt::Key_Return) &&
            keyEvent->modifiers().testFlag(Qt::NoModifier)) {
            if (m_btnGo->isEnabled()) {
                btnGoClicked();
            }
            return true;
        } else if (keyEvent->modifiers().testFlag(Qt::ControlModifier) ||
                   keyEvent->modifiers().testFlag(Qt::MetaModifier) ||
                   keyEvent->modifiers().testFlag(Qt::ShiftModifier)) {
            if (keyEvent->key() == Qt::Key_Up) {
                if (m_currentInputIdx > 0) {
                    --m_currentInputIdx;
                    ui->edtInput->setPlainText(m_inputHistory[m_currentInputIdx]);
                    return true;
                }
            } else if (keyEvent->key() == Qt::Key_Down) {
                if (m_currentInputIdx < static_cast<int64_t>(m_inputHistory.size()) - 1) {
                    ++m_currentInputIdx;
                    ui->edtInput->setPlainText(m_inputHistory[m_currentInputIdx]);
                    return true;
                } else if (m_currentInputIdx == m_inputHistory.size() - 1) {
                    ++m_currentInputIdx;
                    ui->edtInput->clear();
                    return true;
                }
            }
        }
    } else if (object == ui->outputArea && event->type() == QEvent::Resize) {
        setScrollSpacerToIdealheight();
    }
    return false;
}

void ChatWidget::showEvent(QShowEvent *event)
{
    ui->splitInput->setHandleWidth(m_toolWidgets->anyTabVisible() ? 4 : 0);
    ui->edtInput->setFocus();
}

void ChatWidget::clear()
{
    abortReply();
    for (auto &query : m_queries) {
        query->deleteLater();
    }
    m_queries.clear();
}

void ChatWidget::btnGoClicked()
{
    if (!m_workflowModel->isReady()) {
        return;
    }

    if (m_processor->isActive()) {
        m_processor->stop();
        return;
    }

    auto text = ui->edtInput->toPlainText().trimmed();
    if (text.trimmed().isEmpty()) {
        ui->edtInput->clear();
        btnGoShowPlay();
        return;
    }

    m_inputHistory.push_back(text);
    m_currentInputIdx = m_inputHistory.size();

    if (!m_current) {
        setCurrent(createNewItem(-1));
    } else {
        m_current->clearOutput();
    }

    auto files = m_fileModel->contextFiles();
    auto pages = m_additionalContentModel->contextPages();

    ui->edtInput->clear();
    m_current->setInput(text);
    m_current->setInputContext(files, pages);
    writeTextToModel();

    QMetaObject::invokeMethod(
        this,
        [this]() {
            m_scrollTarget = std::max(0, m_current->geometry().y() - 50);
            m_scrollTimer.start(10);
        },
        Qt::QueuedConnection);

    if (startQuery(text, std::move(files), std::move(pages))) {
        btnGoShowStop();
    }
}

void ChatWidget::btnClearInputClicked()
{
    ui->edtInput->clear();
    if (!m_processor->isActive()) {
        setCurrent(nullptr);
    }
}

void ChatWidget::abortReply()
{
    if (m_processor->isActive()) {
        cleanReply();
    }
}

void ChatWidget::workflowReady(bool isReady)
{
    // ToDo: should be only enabled if model is selected
    m_btnGo->setEnabled(isReady);
}

void ChatWidget::currentIndexChanged(const QModelIndex &idx)
{
    if (m_model->uuid(idx) == m_currentChatId && idx.isValid()) {
        return;
    }

    // currently running generations need to be stopped, also saves current chat
    abortReply();

    // clean current chat
    clear();

    m_currentChatId = m_model->uuid(idx);

    if (!idx.isValid() || m_model->isFolder(idx)) {
        clear();
        ui->frmInput->setDisabled(true);
        return;
    }

    const auto &data = m_model->userData(idx);
    for (const auto &i : data.interactions()) {
        setCurrent(createNewItem(-1));
        m_current->setItemData(i);
    }
    cleanReply();

    ui->frmInput->setDisabled(false);
    ui->scrollArea->verticalScrollBar()->setValue(0);
}

void ChatWidget::writeTextToModel()
{
    ChatData data;
    data.setScroll(ui->scrollArea->verticalScrollBar()->value());
    for (const auto &i : m_queries) {
        data.addInteraction(i->itemData());
    }

    m_model->setUserData(m_tree->currentIndex(), std::move(data));
}

ChatItemWidget *ChatWidget::createNewItem(int index)
{
    auto *ptr = new ChatItemWidget(m_contentModel, ui->cntOutput);

    connect(ptr, &ChatItemWidget::remove, this, [this, instance = ptr]() {
        if (m_current == instance) {
            abortReply();
            m_current = nullptr;
        }
        const auto it = std::find(m_queries.begin(), m_queries.end(), instance);
        if (it != m_queries.end()) {
            (*it)->deleteLater();
            m_queries.erase(it);
            writeTextToModel();
        }
    });

    connect(ptr, &ChatItemWidget::repeat, this, [this, instance = ptr]() {
        abortReply();
        const auto it = std::find(m_queries.begin(), m_queries.end(), instance);
        if (it != m_queries.end()) {
            setCurrent(*it);
            const auto data = (*it)->itemData();
            startQuery(data.input, data.contextFiles, data.pages);
        }
    });

    connect(ptr, &ChatItemWidget::clone, this, [this, instance = ptr]() {
        const auto it = std::find(m_queries.begin(), m_queries.end(), instance);
        if (it != m_queries.end()) {
            const auto data = (*it)->itemData();
            auto *clone     = createNewItem(it - m_queries.begin());
            clone->setItemData(std::move(data));
        }
    });

    connect(ptr, &ChatItemWidget::edit, this, [this, instance = ptr]() {
        abortReply();
        const auto it = std::find(m_queries.begin(), m_queries.end(), instance);
        if (it != m_queries.end()) {
            setCurrent(*it);
            const auto data = (*it)->itemData();
            ui->edtInput->setPlainText(data.input);
        }
    });

    auto *layout = dynamic_cast<QVBoxLayout *>(ui->cntOutput->layout());

    if (index == -1) {
        m_queries.push_back(ptr);
        layout->insertWidget(layout->count(), ptr);
    } else {
        assert(m_queries.size() == layout->count());
        assert(index <= m_queries.size());
        m_queries.insert(m_queries.begin() + index, ptr);
        layout->insertWidget(index, ptr);
    }

    return ptr;
}

void ChatWidget::setCurrent(ChatItemWidget *current)
{
    if (m_current) {
        m_current->setHighlighted(false);
    }
    m_current = current;
    if (m_current) {
        m_current->setHighlighted(true);
    }
}

void ChatWidget::cleanReply()
{
    writeTextToModel();
    m_processor->stop();
    btnGoShowPlay();
    // the current is stored in m_queries, we just reset the current pointer
    setCurrent(nullptr);
    setScrollSpacerToIdealheight();
}

void ChatWidget::btnGoShowPlay()
{
    m_btnGo->setIcon(QIcon(":/icons/play"));
}

void ChatWidget::btnGoShowStop()
{
    m_btnGo->setIcon(QIcon(":/icons/stop"));
}

void ChatWidget::updateScroll()
{
    const int currentPos = ui->scrollArea->verticalScrollBar()->value();
    const int diff       = m_scrollTarget - currentPos;
    int step             = diff / 8;

    if (step != 0) {
        ui->scrollArea->verticalScrollBar()->setValue(currentPos + step);
    } else if (step == 0 && diff != 0) {
        ui->scrollArea->verticalScrollBar()->setValue(currentPos + ((diff > 0) ? 1 : -1));
    } else {
        m_scrollTimer.stop();
    }
}

void ChatWidget::setScrollSpacerToIdealheight()
{
    QMetaObject::invokeMethod(
        this,
        [this]() {
            ui->scrollSpacer->changeSize(
                0, ui->outputArea->height() - 34, QSizePolicy::Minimum, QSizePolicy::Fixed);
            ui->scrollContent->layout()->invalidate();
        },
        Qt::QueuedConnection);
}

bool ChatWidget::startQuery(QString query, ContextFiles contextFiles, ContextPages contextPages)
{
    ChatData chat{};
    for (auto it = m_queries.begin(); it != m_queries.end(); ++it) {
        // only take all entries until current, needed fro "repeat"
        if (*it == m_current) {
            break;
        }
        auto data = (*it)->itemData();
        if (!data.enabled) {
            continue;
        }
        chat.addInteraction(std::move(data));
    }

    const auto workflow = m_workflowModel->selectedWorkflow().name();
    if (workflow == "simple") {
        return m_processor->start(
            std::make_unique<WorkflowBasic>(m_workflowModel->baseModel(),
                                            m_contentModel->itemModel(),
                                            std::move(query),
                                            std::move(contextFiles),
                                            std::move(contextPages),
                                            std::move(chat),
                                            m_workflowModel->selectedDecoratorPrompt(),
                                            m_workflowModel->options()));
    } else if (workflow == "basic_cot") {
        return m_processor->start(
            std::make_unique<WorkflowBasicCoT>(m_workflowModel->baseModel(),
                                               m_workflowModel->refineModel(),
                                               m_contentModel->itemModel(),
                                               std::move(query),
                                               std::move(contextFiles),
                                               std::move(contextPages),
                                               std::move(chat),
                                               m_workflowModel->selectedDecoratorPrompt(),
                                               m_workflowModel->options()));
    }
    return false;
}

QJsonArray ChatWidget::chatAsJson(bool forSaving) const
{
    assert(forSaving); // ToDo

    QJsonArray json;

    auto systemPrompt = m_workflowModel->baseModel()->selectedSystemPrompt();
    auto system       = systemPrompt.systemPrompt();
    if (!system.isEmpty()) {
        json.append(QJsonObject{ { "role", "system" }, { "content", std::move(system) } });
    }

    for (auto it = m_queries.begin(); it != m_queries.end(); ++it) {
        // only take all entries until current, needed fro "repeat"
        if (!forSaving && (*it == m_current)) {
            break;
        }
        auto data = (*it)->itemData();
        if (!forSaving && !data.enabled) {
            continue;
        }
        json.append(QJsonObject{ { "role", "user" }, { "content", data.input } });
        if (forSaving) {
            json.append(
                QJsonObject{ { "role", "assistant" }, { "content", data.outputWithSteps } });
            json.append(QJsonObject{ { "role", "enabled" }, { "content", data.enabled } });
        } else {
            json.append(QJsonObject{ { "role", "assistant" }, { "content", data.finalOutput } });
        }
    }
    return json;
}

void ChatWidget::procBeginBlock(int index, const QString &title)
{
    newOutput(m_current->startOutputSection(tr("==== %1 : %2 ====\n").arg(index).arg(title)));
}

void ChatWidget::procEndBlock(int index)
{
    newOutput(m_current->endOutputSection(tr("\n==== ~%1 ====\n\n").arg(index)));
}

void ChatWidget::procNewContent(const QString &content)
{
    newOutput(m_current->insertOutput(content));
}

void ChatWidget::procFinished()
{
    cleanReply();

    setScrollSpacerToIdealheight();
}

void ChatWidget::newOutput(int diff)
{
    const auto newHeight = ui->scrollSpacer->geometry().height() - diff;
    if (newHeight < 50) { // magic number 50
        setScrollSpacerToIdealheight();
    } else {
        ui->scrollSpacer->changeSize(0, newHeight, QSizePolicy::Minimum, QSizePolicy::Fixed);
        ui->scrollContent->layout()->invalidate();
    }
}
