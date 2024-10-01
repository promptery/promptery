#pragma once

#include <model/llm_interface.h>

class OllamaInterface : public LlmInterface
{
public:
    explicit OllamaInterface(QObject *parent = nullptr);

    QString id() const override { return "ollama"; }
    QString name() const override { return "Ollama backend"; }

    bool isDefaultLlm() const override { return true; }

    QIODevice *asyncChat(QString &&model, QJsonArray &&messages) override;
    QIODevice *asyncEmbed(QString &&model, QString &&text) override;

protected:
    void fetchModels() override;

private:
    QNetworkReply *m_modelsReply{ nullptr };
};
