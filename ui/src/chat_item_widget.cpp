#include <ui/chat_item_widget.h>

#include <ui/chat_input_widget.h>
#include <ui/chat_output_widget.h>
#include <ui/pushbutton.h>

#include <QHBoxLayout>
#include <QRegularExpression>
#include <QToolBar>
#include <QVBoxLayout>

#include <tools/finally.h>

ChatItemWidget::ChatItemWidget(ContentModel *contentModel, QWidget *parent)
    : QWidget(parent)
    , m_input(new InputWidget(contentModel, this))
    , m_inputBar(new QToolBar(this))
    , m_mainLayout(new QVBoxLayout(this))
{
    auto *header       = new QWidget(this);
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->addStretch();
    headerLayout->addWidget(m_inputBar);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    m_inputBar->setLayoutDirection(Qt::RightToLeft);
    m_inputBar->layout()->setContentsMargins(0, 0, 0, 0);
    m_inputBar->setStyleSheet(QString(R"(
        QToolBar {
            background-color: rgba(127, 127, 127, 20%);
            border-top-right-radius: 3px;
            border-top-left-radius: 3px;
            border-bottom-left-radius: 3px;
            padding: 0px;
        }
    )"));

    m_activeIndicator = new QWidget(m_inputBar);
    m_activeIndicator->setFixedWidth(0);
    m_inputBar->addWidget(m_activeIndicator);

    // ToDo add folding

    m_inputBar->addAction(tr("remove"), [this]() { Q_EMIT remove(); });

    m_disableAction =
        m_inputBar->addAction(tr("disable"), this, &ChatItemWidget::disableActionTriggered);
    m_disableAction->setCheckable(true);
    m_disableAction->setChecked(!m_enabled);

    m_inputBar->addAction(tr("repeat"), [this]() { Q_EMIT repeat(); });

    m_inputBar->addAction(tr("clone"), [this]() { Q_EMIT clone(); });
    m_inputBar->addAction(tr("edit"), [this]() { Q_EMIT edit(); });

    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(2);
    m_mainLayout->addWidget(header);
    m_mainLayout->addWidget(m_input);
    m_mainLayout->addSpacing(2);
    createAdditionalOutputSection();
}

ChatData::Interaction ChatItemWidget::itemData() const
{
    std::vector<QString> outputs;
    outputs.reserve(m_outputs.size());
    for (const auto &o : m_outputs) {
        outputs.push_back(o->toPlainText());
    }

    return ChatData::Interaction{ m_input->text(),          m_input->contextFiles(),
                                  m_input->contextPages(),  std::move(outputs),
                                  m_current->toPlainText(), itemEnabled() };
}

void ChatItemWidget::setItemData(const ChatData::Interaction &data)
{
    setInput(data.input);
    setInputContext(data.contextFiles, data.pages);
    insertOutput(data.outputs.at(0));
    m_outputs.reserve(data.outputs.size());
    for (std::size_t i = 1; i < data.outputs.size(); ++i) {
        createAdditionalOutputSection();
        insertOutput(data.outputs.at(i));
    }
    m_disableAction->setChecked(!data.enabled);
    disableActionTriggered();
}

void ChatItemWidget::setInput(const QString &input)
{
    m_input->setText(input);
}

void ChatItemWidget::setInputContext(ContextFiles files, ContextPages pages)
{
    m_input->setContext(std::move(files), std::move(pages));
}

void ChatItemWidget::createAdditionalOutputSection()
{
    if (m_outputs.empty()) {
        auto *ptr = new OutputWidget(this);
        m_outputs.push_back(ptr);
        m_mainLayout->addWidget(ptr);
    }
    m_current = m_outputs.back();
    m_current->show();

    // need to create the next output, even if it is not yet shown, to have it in the layout, to
    // resize properly when the first text is inserted.
    // Alternative is processEvents, which I try not to use
    auto *next = new OutputWidget(this);
    m_outputs.push_back(next);
    m_mainLayout->addWidget(next);
    next->hide();
}

constexpr QChar nl('\n');

int ChatItemWidget::insertOutput(const QString &output)
{
    // [from, to)
    auto insert = [this](const QString &text, int from, int to) {
        finally f(nullptr);

        auto slice = text.sliced(from, to - from);
        m_currentLine.append(slice);
        if (m_currentLine.endsWith(nl)) {
            switch (m_state) {
            case OutputModeState::normal:
                if (m_currentLine == "<think>\n") {
                    m_stateNesting = 1;
                    m_state        = OutputModeState::thinking;
                    m_current->setThinking(true);
                }
                break;
            case OutputModeState::thinking:
                if (m_currentLine == "<think>\n") {
                    ++m_stateNesting;
                }
                if (m_currentLine == "</think>\n") {
                    --m_stateNesting;
                    if (m_stateNesting == 0) {
                        m_state = OutputModeState::normal;
                        f.reset([this]() { m_current->setThinking(false); });
                    }
                }
                break;
            }
            m_currentLine.clear();
        }

        static const QRegularExpression notWS("\\S");
        if (m_outputEmpty) {
            const auto idx = slice.indexOf(notWS);
            if (idx == -1) {
                return 0;
            } else if (idx == 0) {
                m_outputEmpty = false;
                return insertAndResizeOutput(slice);
            } else {
                m_outputEmpty = false;
                return insertAndResizeOutput(slice.sliced(idx));
            }
        } else {
            return insertAndResizeOutput(slice);
        }
    };
    auto ret  = 0;
    auto last = 0;
    auto idx  = output.indexOf(nl, last) + 1;
    while (idx > 0) {
        ret += insert(output, last, idx);
        last = idx;
        idx  = output.indexOf(nl, last) + 1;
    }
    ret += insert(output, last, output.size());
    return ret;
}

void ChatItemWidget::clearOutput()
{
    for (std::size_t i = 2; i < m_outputs.size(); ++i) {
        m_outputs.at(i)->deleteLater();
    }
    m_outputs.resize(2);
    auto *last = m_outputs.back();
    last->clear();
    last->hide();
    m_current = m_outputs.front();
    m_current->clear();
    m_current->show();

    m_requestedOutputs = 0;
    m_outputEmpty      = true;
    m_finalOutput.clear();
    m_currentLine.clear();
}

int ChatItemWidget::startOutputSection(const QString &outputTitle)
{
    ++m_requestedOutputs;
    if (m_requestedOutputs > 1) {
        createAdditionalOutputSection();
    }
    m_current->setTitle(outputTitle);
    m_finalOutput.clear();
    return resize();
}

void ChatItemWidget::endOutputSection() {}

bool ChatItemWidget::itemEnabled() const
{
    return m_enabled;
}

void ChatItemWidget::setHighlighted(bool highlighted)
{
    if (highlighted) {
        m_activeIndicator->setFixedWidth(10);
        m_activeIndicator->setStyleSheet("border: 5px solid orange; border-top-right-radius: 3px;");
    } else {
        m_activeIndicator->setFixedWidth(0);
        m_activeIndicator->setStyleSheet("");
    }
}

void ChatItemWidget::resizeEvent(QResizeEvent *event)
{
    resize();
}

int ChatItemWidget::resize()
{
    QSize size           = m_current->document()->size().toSize();
    const auto newHeight = size.height() + 2;
    const auto diff      = newHeight - m_current->height();
    if (diff != 0) {
        m_current->setFixedHeight(newHeight);
    }
    return diff;
}

void ChatItemWidget::disableActionTriggered()
{
    m_enabled = !m_disableAction->isChecked();
    m_disableAction->setText(!m_enabled ? tr("enable") : tr("disable"));
    m_input->setGreyedOut(!m_enabled);
    m_current->setGreyedOut(!m_enabled);
}

int ChatItemWidget::insertAndResizeOutput(const QString &output)
{
    m_current->insertPlainText(output);
    m_finalOutput.append(output);
    return resize();
}
