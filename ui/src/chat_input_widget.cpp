#include <ui/chat_input_widget.h>

#include <ui/content_model.h>
#include <ui/defines.h>

#include <QApplication>
#include <QLabel>
#include <QResizeEvent>
#include <QTextEdit>
#include <QVBoxLayout>


class TextView : public QTextEdit
{
public:
    using QTextEdit::QTextEdit;
};

InputWidget::InputWidget(ContentModel *contentModel, QWidget *parent)
    : QWidget(parent)
    , m_contentModel(contentModel)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(1);

    // setAutoFillBackground(true);
    // setBackgroundRole(QPalette::Window);
}

QString InputWidget::text() const
{
    return m_textEdit ? m_textEdit->toPlainText() : QString();
}

void InputWidget::setText(const QString &text)
{
    if (m_textEdit) {
        m_textEdit->deleteLater();
        m_textEdit = nullptr;
    }
    m_textEdit = createTextView(text, false);
}

void InputWidget::setContext(ContextFiles contextFiles, ContextPages contextPages)
{
    if (m_contextView) {
        m_contextView->deleteLater();
        m_contextView = nullptr;
    }

    QString text;
    m_contextFiles = std::move(contextFiles);
    if (!m_contextFiles.empty()) {
        for (const auto &file : m_contextFiles.files) {
            if (!text.isEmpty()) {
                text.append("\n");
            }
            text.append(file);
        }
    }

    m_contextPages = std::move(contextPages);
    if (!m_contextPages.empty()) {
        for (const auto &i : m_contextPages.ids) {
            if (!text.isEmpty()) {
                text.append("\n");
            }
            const auto name = m_contentModel->data(i, Qt::DisplayRole);
            text.append(name.isValid() ? name.toString() : tr("<item not found>"));
        }
    }
    if (!text.isEmpty()) {
        m_contextView = createTextView(text, true);
    }
}

void InputWidget::setGreyedOut(bool greyedOut)
{
    setStyleSheet(m_textEdit, greyedOut);
}

void InputWidget::resizeEvent(QResizeEvent *event)
{
    const auto newWidth = event->size().width();
    const auto oldWidth = event->oldSize().width();

    if (oldWidth != newWidth) {
        resize(m_textEdit, newWidth);
        resize(m_contextView, newWidth);
    }
}

TextView *InputWidget::createTextView(const QString &text, bool appendix)
{
    auto *ptr = new TextView(this);
    ptr->setReadOnly(true);
    ptr->setFixedHeight(0);
    ptr->setFrameStyle(QFrame::NoFrame);
    ptr->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ptr->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if (appendix) {
        ptr->setAlignment(Qt::AlignRight);
    }
    setStyleSheet(ptr, appendix);

    m_layout->addWidget(ptr);
    m_layout->setAlignment(ptr, Qt::AlignRight);

    ptr->insertPlainText(text);

    const bool visible = isVisible();
    const auto pWidth  = parentWidget() ? parentWidget()->width() : 0;
    resize(ptr, visible ? width() : pWidth);

    return ptr;
}

void InputWidget::resize(TextView *view, int width)
{
    if (!view) {
        return;
    }
    const auto max = std::max(width - cChatIndent, cChatIndent);
    const auto bb =
        view->fontMetrics().boundingRect(QRect(0, 0, max - 2 * cTextIndent - 2, QWIDGETSIZE_MAX),
                                         Qt::TextWordWrap,
                                         view->toPlainText());

    view->document()->setDocumentMargin(cTextIndent);
    const auto newWidth = bb.width() + 2 * cTextIndent + 2;
    if ((double)newWidth / max > 0.88) {
        view->setFixedWidth(max);
    } else {
        view->setFixedWidth(newWidth);
    }
    view->setFixedHeight(bb.height() + 2 * cTextIndent);
}

void InputWidget::setStyleSheet(TextView *view, bool greyedOut)
{
    if (!view) {
        return;
    }

    view->setStyleSheet(QString(R"(
        QTextEdit {
            background-color: rgba(170, 240, 150, 30%);
            border-top-left-radius: 4px;
            border-bottom-left-radius: 4px;
            padding-left: 2px;
            %1
        }
    )")
                            .arg(greyedOut ? "color: rgb(150, 150, 150);" : ""));
}
