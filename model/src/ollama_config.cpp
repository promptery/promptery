#include <model/ollama_config.h>

#include <common/settings.h>

OllamaConfig &OllamaConfig::global()
{
    static OllamaConfig config;
    return config;
}

static constexpr const char *cPrefix = "OllamaConfig";
static constexpr const char *cIp     = "ip";
static constexpr const char *cPort   = "port";
static constexpr const char *cCtx    = "ctx";
static constexpr const char *cSeed   = "seed";
static constexpr const char *cTemp   = "temp";

void OllamaConfig::readSettings()
{
    auto &s = Settings::global();
    s.sync();
    setIp(s.value(QString(cPrefix) + "/" + cIp, "localhost").toString());
    setPort(s.value(QString(cPrefix) + "/" + cPort, 11434).toUInt());
    setCtx(s.value(QString(cPrefix) + "/" + cCtx, 4).toInt());
    setSeed(s.value(QString(cPrefix) + "/" + cSeed, -1).toInt());
    setTemp(s.value(QString(cPrefix) + "/" + cTemp, 0.8).toDouble());
}

void OllamaConfig::storeSettings() const
{
    auto &s = Settings::global();
    s.setValue(QString(cPrefix) + "/" + cIp, m_ip);
    s.setValue(QString(cPrefix) + "/" + cPort, m_port);
    s.setValue(QString(cPrefix) + "/" + cCtx, m_ctx);
    s.setValue(QString(cPrefix) + "/" + cSeed, m_seed);
    s.setValue(QString(cPrefix) + "/" + cTemp, m_temp);
    s.sync();
}

QString OllamaConfig::serverApi() const
{
    return QString("http://%1:%2/api").arg(ip()).arg(port());
}
