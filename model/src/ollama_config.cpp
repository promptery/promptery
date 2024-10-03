#include <model/ollama_config.h>

#include <common/settings.h>

OllamaConfig &OllamaConfig::global()
{
    static OllamaConfig config;
    return config;
}

void OllamaConfig::setIp(QString ip)
{
    m_ip = std::move(ip);
}

void OllamaConfig::setPort(uint32_t port)
{
    m_port = port;
}

static constexpr const char *cPrefix = "OllamaConfig";
static constexpr const char *cIp     = "ip";
static constexpr const char *cPort   = "port";

void OllamaConfig::readSettings()
{
    auto &s = Settings::global();
    s.sync();
    setIp(s.value(QString(cPrefix) + "/" + cIp, "localhost").toString());
    setPort(s.value(QString(cPrefix) + "/" + cPort, 11434).toUInt());
}

void OllamaConfig::storeSettings() const
{
    auto &s = Settings::global();
    s.setValue(QString(cPrefix) + "/" + cIp, m_ip);
    s.setValue(QString(cPrefix) + "/" + cPort, m_port);
    s.sync();
}

QString OllamaConfig::serverApi() const
{
    return QString("http://%1:%2/api").arg(ip()).arg(port());
}
