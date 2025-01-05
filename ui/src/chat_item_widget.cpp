#include <ui/chat_item_widget.h>

#include <ui/chat_input_widget.h>
#include <ui/chat_output_widget.h>
#include <ui/pushbutton.h>

#include <QHBoxLayout>
#include <QToolBar>
#include <QVBoxLayout>


ChatItemWidget::ChatItemWidget(ContentModel *contentModel, QWidget *parent)
    : QWidget(parent)
    , m_input(new InputWidget(contentModel, this))
    , m_output(new OutputWidget(this))
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

    m_inputBar->addAction(tr("repeat"), [this]() {
        clearOutput();
        Q_EMIT repeat();
    });

    m_inputBar->addAction(tr("clone"), [this]() { Q_EMIT clone(); });
    m_inputBar->addAction(tr("edit"), [this]() { Q_EMIT edit(); });

    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(header);
    m_mainLayout->addWidget(m_input);
    m_mainLayout->addSpacing(4);
    m_mainLayout->addWidget(m_output);
}

ChatData::Interaction ChatItemWidget::itemData() const
{
    return ChatData::Interaction{ m_input->text(),         m_input->contextFiles(),
                                  m_input->contextPages(), m_output->toPlainText(),
                                  m_finalOutput,           itemEnabled() };
}

void ChatItemWidget::setItemData(const ChatData::Interaction &data)
{
    setInput(data.input);
    setInputContext(data.contextFiles, data.pages);
    insertOutput(data.outputWithSteps);
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

int ChatItemWidget::insertOutput(const QString &output)
{
    static const QRegularExpression notWS("\\S");
    if (m_outputEmpty) {
        const auto idx = output.indexOf(notWS);
        if (idx == -1) {
            return 0;
        } else if (idx == 0) {
            m_outputEmpty = false;
            return insertAndResizeOutput(output);
        } else {
            m_outputEmpty = false;
            return insertAndResizeOutput(output.mid(idx));
        }
    } else {
        return insertAndResizeOutput(output);
    }
}

void ChatItemWidget::clearOutput()
{
    m_output->clear();
    m_outputEmpty = true;
    m_finalOutput.clear();
}

int ChatItemWidget::startOutputSection(const QString &output)
{
    const auto ret = insertOutput(output);
    m_finalOutput.clear();
    return ret;
}

int ChatItemWidget::endOutputSection(const QString &output)
{
    auto temp      = std::move(m_finalOutput);
    const auto ret = insertOutput(output);
    m_finalOutput  = std::move(temp);
    return ret;
}

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
    QSize size           = m_output->document()->size().toSize();
    const auto newHeight = size.height() + 2;
    const auto diff      = newHeight - m_output->height();
    if (diff != 0) {
        m_output->setFixedHeight(newHeight);
    }
    return diff;
}

void ChatItemWidget::disableActionTriggered()
{
    m_enabled = !m_disableAction->isChecked();
    m_disableAction->setText(!m_enabled ? tr("enable") : tr("disable"));
    m_input->setGreyedOut(!m_enabled);
    m_output->setGreyedOut(!m_enabled);
}

int ChatItemWidget::insertAndResizeOutput(const QString &output)
{
    m_output->insertPlainText(output);
    m_finalOutput.append(output);
    return resize();
}
