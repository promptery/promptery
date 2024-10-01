#pragma once

#include <ui/chat_model.h>

#include <QWidget>

class ChatData;
class ContentModel;
class InputWidget;
class OutputWidget;
class QToolBar;

class QHBoxLayout;
class QVBoxLayout;

class ChatItemWidget : public QWidget
{
    Q_OBJECT
public:
    ChatItemWidget(ContentModel *contentModel, QWidget *parent = nullptr);

    ChatData::Interaction itemData() const;

    void setItemData(const ChatData::Interaction &data);

    void setInput(const QString &input);
    void setInputContext(ContextFiles files, ContextPages pages);

    // returns the hieght diff of the output widget
    int insertOutput(const QString &output);
    void clearOutput();

    bool itemEnabled() const;

    void setHighlighted(bool highlighted);

    Q_SIGNAL void clone();
    Q_SIGNAL void edit();
    Q_SIGNAL void repeat();
    Q_SIGNAL void remove();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Q_SLOT int resize();

    Q_SLOT void disableActionTriggered();

    int insertAndResizeOutput(const QString &output);

    InputWidget *m_input;
    OutputWidget *m_output;

    QToolBar *m_inputBar;
    QWidget *m_activeIndicator;

    QAction *m_disableAction;

    QVBoxLayout *m_mainLayout;

    bool m_enabled{ true };
    bool m_outputEmpty{ true };
};
