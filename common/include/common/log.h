#pragma once

#include <QCoreApplication>
#include <QVariant>

class GlobalLogger : public QObject
{
    Q_OBJECT

public:
    static void installGlobalLogger(QCoreApplication &app)
    {
        static auto *globalLogger = new GlobalLogger(&app);
        app.setProperty("globalLogger", QVariant::fromValue(static_cast<QObject *>(globalLogger)));
    }

    Q_SIGNAL void logMessage(const QString &msg);

private:
    explicit GlobalLogger(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};

inline GlobalLogger *globalLogger()
{
    return qApp->property("globalLogger").value<GlobalLogger *>();
}

inline void log(const QString &msg)
{
    GlobalLogger *logger = globalLogger();
    if (logger) {
        Q_EMIT logger->logMessage(msg);
    }
}
