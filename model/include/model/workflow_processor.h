#pragma once

#include <model/chat_types.h>

#include <QNetworkReply>
#include <QObject>

#include <memory>

class WorkflowProcessor : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    bool start(std::unique_ptr<WorkflowInterface> workflow);

    void stop();

    bool isActive() const;

    Q_SIGNAL void beginBlock(int index, const QString &title);
    Q_SIGNAL void endBlock(int index);
    Q_SIGNAL void newContent(const QString &content);
    Q_SIGNAL void finished();

private:
    bool beginNextStep();
    void cleanUpReply();
    void finish();

    Q_SLOT void errorOccurred(QNetworkReply::NetworkError error);
    Q_SLOT void finishedReply();
    Q_SLOT void readyRead();

    std::unique_ptr<WorkflowInterface> m_workflow;

    QNetworkReply *m_reply{ nullptr };

    int m_blockIndex{ -1 };
    std::vector<QString> m_responses;
};
