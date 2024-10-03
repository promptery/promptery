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

    /**
     * @return if a backend for the given id was found
     */
    bool updateModels(const QString &backendId);

    const std::unordered_map<QString, LlmInterface *> &llmBackends() const { return m_llms; }

    std::optional<LlmInterface *> llmBackend(const QString &backendId) const;

    Q_SIGNAL void startingConnection(const QString &backendId) const;
    Q_SIGNAL void modelsAvailable(const QString &backendId) const;

private:
    std::unordered_map<QString, LlmInterface *> m_llms;
};
