#include <model/dummy_llm_backend.h>

#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

constexpr auto cMirror = "dummy_mirror";

class AsyncString : public QIODevice
{
public:
    explicit AsyncString(QString str, QObject *parent = nullptr)
        : QIODevice(parent)
        , m_timer(new QTimer(this))
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
        return m_next.size() + QIODevice::bytesAvailable();
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
  "model": "dummy_mirror",
  "created_at": "%1",
  "done": true,
  "total_duration": 42,
  "load_duration": 42,
  "prompt_eval_count": %2,
  "prompt_eval_duration": 42,
  "eval_count": %2,
  "eval_duration": 42
})");
            m_next = finalRresponse.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
                         .arg(m_tokenCount)
                         .toUtf8();
            m_atEnd = true;
            return;
        }

        static const auto response = QString(R"({
  "model": "dummy_mirror",
  "created_at": "%1",
  "message": %2,
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
                     .arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs),
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

DummyLlmBackend::DummyLlmBackend(QObject *parent)
    : LlmInterface(parent)
{
}

QIODevice *DummyLlmBackend::asyncChat(QString &&model, QJsonArray &&messages)
{
    if ((model == cMirror) && !messages.isEmpty()) {
        auto str = messages.at(messages.count() - 1).toObject()["content"].toString();
        return new AsyncString(std::move(str), this);
    }

    return nullptr;
}

void DummyLlmBackend::fetchModels()
{
    setModels({ { cMirror, "Mirror", ModelType::chat } });
    Q_EMIT modelsAvailable(id());
}
