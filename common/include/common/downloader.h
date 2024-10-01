#pragma once

#include <QObject>
#include <QSslError>
#include <QUrl>

#include <functional>

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

class Downloader : public QObject
{
    Q_OBJECT
public:
    static Downloader &global();

    explicit Downloader(QObject *parent = nullptr);

    QNetworkReply *
    getAsync(const QUrl &url, const std::function<void(QNetworkRequest &)> &enrich = nullptr);

    QNetworkReply *postAsync(const QUrl &url, const QJsonDocument &json);

    QNetworkReply *postAsync(const QUrl &url, QIODevice *data,
                             const std::function<void(QNetworkRequest &)> &enrich = nullptr);

    // Signal reporting *every* finished request of this downloader
    Q_SIGNAL void finished(QNetworkReply *reply);

private:
    Q_SLOT void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

    QNetworkRequest
    createRequest(const QUrl &url, const std::function<void(QNetworkRequest &)> &enrich);

    QNetworkAccessManager *m_webCtrl;
};
