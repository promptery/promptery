#include <common/downloader.h>

#include <QNetworkReply>

#include <QCoreApplication>
#include <QJsonDocument>
#include <QNetworkAccessManager>

Downloader &Downloader::global()
{
    static Downloader instance;
    return instance;
}

Downloader::Downloader(QObject *parent)
    : QObject{ parent }
    , m_webCtrl(new QNetworkAccessManager(this))
{
    connect(m_webCtrl, &QNetworkAccessManager::finished, this, &Downloader::finished);
    connect(m_webCtrl, &QNetworkAccessManager::sslErrors, this, &Downloader::sslErrors);
}

QNetworkReply *
Downloader::getAsync(const QUrl &url, const std::function<void(QNetworkRequest &)> &enrich)
{
    QNetworkRequest request = createRequest(url, enrich);
    return m_webCtrl->get(request);
}

QNetworkReply *Downloader::postAsync(const QUrl &url, const QJsonDocument &json)
{
    const auto byteArray = json.toJson(QJsonDocument::Compact);

    QNetworkRequest request =
        createRequest(url, [size = QByteArray::number(byteArray.size())](auto &request) {
            request.setRawHeader("Content-Type", "application/json");
            request.setRawHeader("Content-Length", size);
        });
    return m_webCtrl->post(request, byteArray);
}

QNetworkReply *Downloader::postAsync(const QUrl &url,
                                     QIODevice *data,
                                     const std::function<void(QNetworkRequest &)> &enrich)
{
    QNetworkRequest request = createRequest(url, enrich);
    return m_webCtrl->post(request, data);
}

void Downloader::sslErrors(QNetworkReply *reply, const QList<QSslError> & /*errors*/)
{
    reply->ignoreSslErrors();
}

QNetworkRequest
Downloader::createRequest(const QUrl &url, const std::function<void(QNetworkRequest &)> &enrich)
{
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, false);
    request.setRawHeader("User-Agent",
                         "Mozilla/5.0 (X11; Linux x86_64) "
                         "AppleWebKit/537.36 (KHTML, like Gecko) "
                         "Chrome/86.0.4240.75 Safari/537.36");
    if (enrich) {
        enrich(request);
    }

    return request;
}
