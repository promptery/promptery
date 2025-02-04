#include <ui/chat_output_widget.h>

#include <ui/defines.h>

#include <QLabel>
#include <QRegularExpression>
#include <QTextBlock>

OutputWidget::OutputWidget(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout(this))
    , m_textEdit(new QTextEdit(this))
{
    setFixedHeight(m_textEdit->fontMetrics().height() + 11);
    m_textEdit->setReadOnly(true);
    m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textEdit->setFrameStyle(QFrame::NoFrame);
    m_textEdit->setBackgroundRole(QPalette::Window);
    setStyleSheet(false);

    m_layout->setContentsMargins(0, 0, cChatIndent, 0);
    m_layout->addWidget(m_textEdit);
}

void OutputWidget::setTitle(const QString &title)
{
    static const QRegularExpression ws("\\s+");
    m_title = title.trimmed();
    m_title.replace(ws, " ");
    auto cursor = m_textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);

    // Apply formatting
    auto format       = cursor.charFormat();
    const auto weight = format.fontWeight();
    format.setFontWeight(weight + 200);
    cursor.setCharFormat(format);
    cursor.setBlockCharFormat(format);
    cursor.insertText(m_title);
    cursor.insertText("\n");
    cursor.movePosition(QTextCursor::NextBlock);
    format.setFontWeight(weight);
    cursor.setCharFormat(format);
    cursor.setBlockCharFormat(format);
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

QString OutputWidget::toPlainText() const
{
    auto text = m_textEdit->toPlainText();
    if (m_title.isEmpty()) {
        return text;
    }

    auto idx = text.indexOf("\n");
    assert(idx != -1);
    return text.sliced(idx);
}

void OutputWidget::clear()
{
    m_textEdit->clear();
    m_title.clear();
    setFixedHeight(m_textEdit->fontMetrics().height() + 11);
}

void OutputWidget::setGreyedOut(bool greyedOut)
{
    setStyleSheet(greyedOut);
}

void OutputWidget::setThinking(bool thinking)
{
    auto cursor         = m_textEdit->textCursor();
    int currentPosition = cursor.position();

    QTextBlock block = cursor.block();
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);

    // Apply formatting
    auto format = cursor.charFormat();
    format.setFontItalic(thinking);
    cursor.setCharFormat(format);
    cursor.setBlockCharFormat(format);

    // Restore the cursor position
    cursor.setPosition(currentPosition);
    m_textEdit->setTextCursor(cursor);
    m_textEdit->setFontItalic(thinking);
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
