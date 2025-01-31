#pragma once

#include <model/llm_interface.h>

class InternalLlmBackend : public LlmInterface
{
public:
    explicit InternalLlmBackend(QObject *parent = nullptr);

    QString id() const override { return "internal"; }
    QString name() const override { return "Debug backend"; }

    QNetworkReply *
    asyncChat(QString &&model, QJsonArray &&messages, const RequestOptions &options) override;
    QNetworkReply *asyncEmbed(QString && /*model*/, QString && /*text*/) override
    {
        return nullptr;
    }

protected:
    void fetchModels() override;
};


class EmptyLlmBackend : public LlmInterface
{
public:
    explicit EmptyLlmBackend(QObject *parent = nullptr)
        : LlmInterface(parent)
    {
    }

    QString id() const override { return "empty"; }
    QString name() const override { return "Empty backend"; }

    QNetworkReply *asyncChat(QString &&, QJsonArray &&, const RequestOptions &) override
    {
        return nullptr;
    }
    QNetworkReply *asyncEmbed(QString &&, QString &&) override { return nullptr; }

protected:
    void fetchModels() override {}
};
