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
    void setIp(QString ip);

    uint32_t port() const { return m_port; }
    void setPort(uint32_t port);

    void readSettings();
    void storeSettings() const;

private:
    QString serverApi() const;

    QString m_ip{ "localhost" };
    uint32_t m_port{ 11434 };
};
