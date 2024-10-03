#pragma once

#include <QObject>
#include <QString>

#include <cstdint>
#include <mutex>
#include <vector>

class QNetworkReply;

enum class ModelType : uint8_t { unknown, chat, embedding };

struct ModelInformation {
    QString id;
    QString name;
    ModelType type{ ModelType::unknown };
};

class LlmInterface : public QObject
{
    Q_OBJECT
public:
    LlmInterface(QObject *parent)
        : QObject(parent)
    {
    }
    virtual ~LlmInterface() = default;

    virtual QString id() const   = 0;
    virtual QString name() const = 0;

    virtual bool isDefaultLlm() const { return false; }

    void updateModels() { fetchModels(); }

    Q_SIGNAL void startingConnection(const QString &backendId) const;
    Q_SIGNAL void modelsAvailable(const QString &backendId) const;

    const std::vector<ModelInformation> models() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_models;
    }

    virtual QIODevice *asyncChat(QString &&model, QJsonArray &&messages) = 0;
    virtual QIODevice *asyncEmbed(QString &&model, QString &&text)       = 0;

protected:
    virtual void fetchModels() = 0;

    void setModels(std::vector<ModelInformation> models)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_models = models;
    }

private:
    mutable std::mutex m_mutex;
    std::vector<ModelInformation> m_models;
};
