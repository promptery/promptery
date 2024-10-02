#include <model/types.h>

#include <QWidget>

#pragma once

class ContentModel;
class TextView;

class QVBoxLayout;

class InputWidget : public QWidget
{
    Q_OBJECT
public:
    InputWidget(ContentModel *contentModel, QWidget *parent = nullptr);

    QString text() const;
    void setText(const QString &text);

    void setContext(ContextFiles contextFiles, ContextPages contextPages);

    const ContextFiles &contextFiles() const { return m_contextFiles; }
    const ContextPages &contextPages() const { return m_contextPages; }

    void setGreyedOut(bool greyedOut);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    TextView *createTextView(const QString &text, bool appendix);

    void resize(TextView *view, int width);
    void setStyleSheet(TextView *view, bool greyedOut);

    ContentModel *m_contentModel;

    TextView *m_textEdit{ nullptr };
    TextView *m_contextView{ nullptr };
    QVBoxLayout *m_layout;
    ContextFiles m_contextFiles;
    ContextPages m_contextPages;
};
