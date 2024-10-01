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

    void insertPlainText(const QString &text);
    QString toPlainText() const { return m_textEdit->toPlainText(); }

    QTextDocument *document() const { return m_textEdit->document(); }

    void clear();

    void setGreyedOut(bool greyedOut);

private:
    void setStyleSheet(bool greyedOut);

    QTextEdit *m_textEdit;
};
