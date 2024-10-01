#pragma once

#include <QObject>

#include <optional>
#include <unordered_map>

class LlmInterface;

class BackendManager : public QObject
{
    Q_OBJECT
public:
    explicit BackendManager(QObject *parent = nullptr);

    void initialize();

    const std::unordered_map<QString, LlmInterface *> &llmBackends() const { return m_llms; }

    std::optional<LlmInterface *> llmBackend(const QString &backendId) const;

    Q_SIGNAL void modelsAvailable(const QString &backendId) const;

private:
    std::unordered_map<QString, LlmInterface *> m_llms;
};
