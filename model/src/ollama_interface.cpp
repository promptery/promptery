#include <model/ollama_interface.h>

#include <model/ollama_config.h>

#include <common/downloader.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>


OllamaInterface::OllamaInterface(QObject *parent)
    : LlmInterface(parent)
{
}

QString OllamaInterface::id() const
{
    return OllamaConfig::backendId();
}

QString OllamaInterface::address() const
{
    return OllamaConfig::global().serverAddr("").toString();
}

QNetworkReply *OllamaInterface::asyncChat(QString &&model, QJsonArray &&messages)
{
    QJsonObject json{ { "model", std::move(model) }, { "messages", std::move(messages) } };
    return Downloader::global().postAsync(OllamaConfig::global().serverAddr("chat"),
                                          QJsonDocument(json));
}

QNetworkReply *OllamaInterface::asyncEmbed(QString &&model, QString &&text)
{
    QJsonObject json{ { "model", std::move(model) }, { "prompt", std::move(text) } };
    return Downloader::global().postAsync(OllamaConfig::global().serverAddr("embeddings"),
                                          QJsonDocument(json));
}

void OllamaInterface::fetchModels()
{
    if (m_modelsReply) {
        m_modelsReply->deleteLater();
    }
    if (!models().empty()) {
        setModels({});
        Q_EMIT modelsAvailable(id());
    }
    Q_EMIT startingConnection(id());
    m_modelsReply = Downloader::global().getAsync(OllamaConfig::global().serverAddr("tags"));
    connect(m_modelsReply, &QNetworkReply::readyRead, this, [this]() {
        std::vector<ModelInformation> models;

        const auto text = m_modelsReply->readAll();

        const auto jsonModels = QJsonDocument::fromJson(text);
        const auto &array     = jsonModels.object()["models"].toArray();
        for (const auto &m : array) {
            const auto &obj = m.toObject();
            const auto size =
                std::round(obj["size"].toInteger() / (1024. * 1024. * 1024.) * 100.) / 100;
            const auto decimals = size > 10 ? 0 : size > 1 ? 1 : 2;

            const auto baseName = obj["name"].toString().split(":")[0];
            const auto &details = obj["details"].toObject();
            const auto params   = details["parameter_size"].toString();
            const auto family   = details["family"].toString();
            const auto quant    = details["quantization_level"].toString();

            models.push_back(
                { obj["model"].toString(),
                  baseName.split("/").back() +
                      QString(" %1 (%2, %3 GB)").arg(params, quant).arg(size, 0, 'f', decimals),
                  family.contains("bert") ? ModelType::embedding : ModelType::chat });
        }

        std::sort(models.begin(), models.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.name < rhs.name;
        });
        setModels(std::move(models));
        Q_EMIT modelsAvailable(id());
    });
}
