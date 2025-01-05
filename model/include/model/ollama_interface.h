#pragma once

#include <model/llm_interface.h>

class OllamaInterface : public LlmInterface
{
public:
    explicit OllamaInterface(QObject *parent = nullptr);

    QString id() const override;
    QString name() const override { return "Ollama backend"; }
    QString address() const override;

    bool isDefaultLlm() const override { return true; }

    QNetworkReply *asyncChat(QString &&model, QJsonArray &&messages) override;
    QNetworkReply *asyncEmbed(QString &&model, QString &&text) override;

protected:
    void fetchModels() override;

private:
    QNetworkReply *m_modelsReply{ nullptr };
};
