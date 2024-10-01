#include <ui/chat_output_widget.h>

#include <ui/defines.h>

OutputWidget::OutputWidget(QWidget *parent)
    : QWidget(parent)
    , m_textEdit(new QTextEdit(this))
{
    setFixedHeight(m_textEdit->fontMetrics().height() + 11);
    m_textEdit->setReadOnly(true);
    m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textEdit->setFrameStyle(QFrame::NoFrame);
    m_textEdit->setBackgroundRole(QPalette::Window);
    setStyleSheet(false);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, cChatIndent, 0);
    layout->addWidget(m_textEdit);
    setLayout(layout);
}

void OutputWidget::insertPlainText(const QString &text)
{
    auto cursor = m_textEdit->textCursor();
    if (cursor.atEnd()) {
        m_textEdit->insertPlainText(text);
        return;
    }
    const auto currentPos     = cursor.position();
    const auto selection      = cursor.hasSelection();
    const auto selectionStart = cursor.selectionStart();
    const auto selectionEnd   = cursor.selectionEnd();

    cursor.movePosition(QTextCursor::End);
    m_textEdit->setTextCursor(cursor);
    m_textEdit->insertPlainText(text);

    if (selection) {
        if (currentPos == selectionStart) {
            cursor.setPosition(selectionEnd, QTextCursor::MoveAnchor);
            cursor.setPosition(selectionStart, QTextCursor::KeepAnchor);
        } else {
            cursor.setPosition(selectionStart, QTextCursor::MoveAnchor);
            cursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
        }
    } else {
        cursor.setPosition(currentPos);
    }
    m_textEdit->setTextCursor(cursor);
}

void OutputWidget::clear()
{
    m_textEdit->clear();
    setFixedHeight(m_textEdit->fontMetrics().height() + 11);
}

void OutputWidget::setGreyedOut(bool greyedOut)
{
    setStyleSheet(greyedOut);
}

void OutputWidget::setStyleSheet(bool greyedOut)
{
    m_textEdit->setStyleSheet(QString(R"(
        QTextEdit {
            background-color: rgba(150, 200, 240, 50%);
            border-top-right-radius: 4px;
            border-bottom-right-radius: 4px;
            %1
        }
    )")
                                  .arg(greyedOut ? "color: rgb(150, 150, 150);" : ""));
}
