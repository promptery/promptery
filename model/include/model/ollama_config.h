#pragma once

#include <QString>
#include <QUrl>

#include <cstdint>

class OllamaConfig
{
public:
    static const char *backendId() { return "ollama"; }

    static OllamaConfig &global();

    QUrl serverAddr(const QString &path) const { return serverApi() + "/" + path; }

    QString ip() const { return m_ip; }
    void setIp(QString ip) { m_ip = std::move(ip); }

    uint32_t port() const { return m_port; }
    void setPort(uint32_t port) { m_port = port; }

    int ctx() const { return m_ctx; }
    void setCtx(int ctx) { m_ctx = ctx; }

    int seed() const { return m_seed; }
    void setSeed(int seed) { m_seed = seed; }

    double temp() const { return m_temp; }
    void setTemp(double temp) { m_temp = temp; }

    void readSettings();
    void storeSettings() const;

private:
    QString serverApi() const;

    QString m_ip{ "localhost" };
    uint32_t m_port{ 11434 };

    int m_ctx{ 4 };
    int m_seed{ -1 };
    double m_temp{ 0.8 };
};
