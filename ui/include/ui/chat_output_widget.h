#include <QFrame>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QWidget>

#pragma once

class OutputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OutputWidget(QWidget *parent = nullptr);

    void setTitle(const QString &title);

    void insertPlainText(const QString &text);
    QString toPlainText() const;

    QTextDocument *document() const { return m_textEdit->document(); }

    void clear();

    void setGreyedOut(bool greyedOut);

    void setThinking(bool thinking);

private:
    void setStyleSheet(bool greyedOut);

    QVBoxLayout *m_layout;
    QTextEdit *m_textEdit;
    QString m_title;
};
