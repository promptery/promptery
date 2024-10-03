#include <model/backend_manager.h>

#include <model/internal_llm_backend.h>
#include <model/llm_interface.h>
#include <model/ollama_interface.h>

template <typename T>
std::pair<QString, LlmInterface *> create(QObject *parent)
{
    auto ptr = new T(parent);
    return { ptr->id(), ptr };
}

BackendManager::BackendManager(QObject *parent)
    : QObject{ parent }
    , m_llms({ create<OllamaInterface>(this),
               create<InternalLlmBackend>(this)
#ifndef NDEBUG
                   ,
               create<EmptyLlmBackend>(this)
#endif
      })
{
    for (auto &llm : m_llms) {
        connect(llm.second,
                &LlmInterface::startingConnection,
                this,
                &BackendManager::startingConnection);
        connect(llm.second, &LlmInterface::modelsAvailable, this, &BackendManager::modelsAvailable);
    }
}

void BackendManager::initialize()
{
    for (auto &llm : m_llms) {
        llm.second->updateModels();
    }
}

bool BackendManager::updateModels(const QString &backendId)
{
    for (auto &llm : m_llms) {
        if (llm.second->id() == backendId) {
            llm.second->updateModels();
            return true;
        }
    }
    return false;
}

std::optional<LlmInterface *> BackendManager::llmBackend(const QString &backendId) const
{
    const auto it = m_llms.find(backendId);
    if (it != m_llms.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}
