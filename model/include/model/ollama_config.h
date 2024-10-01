#pragma once

#include <QString>
#include <QUrl>

class OllamaConfig
{
public:
    static OllamaConfig &global();

    QUrl serverAddr(const QString &path) const { return serverApi() + "/" + path; }

private:
    QString serverApi() const { return "http://localhost:11434/api"; }
};
