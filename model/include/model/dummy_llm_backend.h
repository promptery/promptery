#pragma once

#include <model/llm_interface.h>

class DummyLlmBackend : public LlmInterface
{
public:
    explicit DummyLlmBackend(QObject *parent = nullptr);

    QString id() const override { return "dummy"; }
    QString name() const override { return "Dummy backend"; }

    QIODevice *asyncChat(QString &&model, QJsonArray &&messages) override;
    QIODevice *asyncEmbed(QString && /*model*/, QString && /*text*/) override { return nullptr; }

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

    QIODevice *asyncChat(QString &&, QJsonArray &&) override { return nullptr; }
    QIODevice *asyncEmbed(QString &&, QString &&) override { return nullptr; }

protected:
    void fetchModels() override {}
};
