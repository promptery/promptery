#include <model/ollama_config.h>

OllamaConfig &OllamaConfig::global()
{
    static OllamaConfig config;
    return config;
}
