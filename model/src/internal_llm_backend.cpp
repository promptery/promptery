#include <model/internal_llm_backend.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QTimer>

constexpr auto cMirror    = "internal_mirror";
constexpr auto cTimeStamp = "internal_timestamp";

class AsyncString : public QNetworkReply
{
public:
    explicit AsyncString(QString model, QString str, QObject *parent = nullptr)
        : QNetworkReply(parent)
        , m_timer(new QTimer(this))
        , m_model(std::move(model))
        , m_data(std::move(str))
    {
        open(QIODevice::ReadOnly);
        connect(m_timer, &QTimer::timeout, this, [this]() {
            m_block = false;
            m_timer->stop();
            Q_EMIT readyRead();
        });
        m_timer->setSingleShot(true);
        m_timer->start(m_interval);
    }
    bool isSequential() const override { return true; }
    qint64 bytesAvailable() const override
    {
        if (m_block) {
            return 0;
        }

        prepareNext();
        return m_next.size() + QNetworkReply::bytesAvailable();
    }

    Q_SLOT void abort() override
    {
        m_timer->stop();
        Q_EMIT finished();
    }

protected:
    void prepareNext() const
    {
        if (!m_next.isEmpty()) {
            return;
        }

        if (m_atEnd) {
            return;
        }

        if (m_finish) {
            static const auto finalRresponse = QString(R"({
  "model": "%1",
  "created_at": "%2",
  "done": true,
  "total_duration": 42,
  "load_duration": 42,
  "prompt_eval_count": %3,
  "prompt_eval_duration": 42,
  "eval_count": %3,
  "eval_duration": 42
})");
            m_next                           = finalRresponse
                         .arg(m_model, QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
                         .arg(m_tokenCount)
                         .toUtf8();
            m_atEnd = true;
            return;
        }

        static const auto response = QString(R"({
  "model": "%1",
  "created_at": "%2",
  "message": %3,
  "done": false
})");

        bool end{ false };
        const int pos = m_data.indexOf(' ', m_index);

        std::size_t count{ 0 };
        if (pos == -1) {
            count = m_data.size() - m_index;
            end   = true;
        } else if (pos == m_data.size() - 1) {
            count = 1;
            end   = true;
        } else {
            count = pos + 1 - m_index;
        }
        m_next = response
                     .arg(m_model,
                          QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs),
                          QString::fromLocal8Bit(
                              QJsonDocument({ { "role", "assistant" },
                                              { "content", m_data.mid(m_index, count) },
                                              { "images", QJsonValue::Null } })
                                  .toJson()))
                     .toUtf8();
        ++m_tokenCount;
        if (end) {
            m_finish = true;
        }
        m_index += count;
        return;
    }

    qint64 readData(char *data, qint64 maxSize) override
    {
        if (m_block) {
            return 0;
        }
        prepareNext();

        const auto size = m_next.size();
        if (size == 0) {
            return -1;
        }

        if (size > maxSize) {
            return -1;
        }

        memcpy(data, m_next.constData(), size);

        m_next.clear();
        m_timer->start(m_interval);
        m_block = true;
        return size;
    }

    qint64 writeData(const char *data, qint64 maxSize) override
    {
        Q_UNUSED(data);
        Q_UNUSED(maxSize);
        return -1; // read-only device
    }

private:
    QTimer *m_timer;
    QString m_model;
    QString m_data;
    int m_interval{ 25 };
    bool m_block{ false };
    mutable bool m_atEnd{ false };
    mutable bool m_finish{ false };
    mutable int m_index{ 0 };
    mutable int m_tokenCount{ 0 };
    mutable QByteArray m_next;
};

//------------------------------------------------

InternalLlmBackend::InternalLlmBackend(QObject *parent)
    : LlmInterface(parent)
{
}

QNetworkReply *InternalLlmBackend::asyncChat(QString &&model,
                                             QJsonArray &&messages,
                                             const RequestOptions & /*options*/)
{
    if ((model == cMirror) && !messages.isEmpty()) {
        auto str = messages.at(messages.count() - 1).toObject()["content"].toString();
        return new AsyncString(std::move(model), std::move(str), this);
    } else if ((model == cTimeStamp) && !messages.isEmpty()) {
        return new AsyncString(
            std::move(model), QDateTime::currentDateTime().toString(Qt::TextDate), this);
    }

    return nullptr;
}

void InternalLlmBackend::fetchModels()
{
    setModels(
        { { cMirror, "Mirror", ModelType::chat }, { cTimeStamp, "Time stamp", ModelType::chat } });
    Q_EMIT modelsAvailable(id());
}
