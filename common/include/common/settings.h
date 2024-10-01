#pragma once

#include <QSettings>
#include <QStandardPaths>

class Settings : public QSettings
{
public:
    static void initGlobal(const QString &projectName) { global(projectName); }

    static Settings &global(const QString &projectName = "")
    {
        static Settings settings = [projectName]() {
            if (projectName.isEmpty()) {
                throw std::runtime_error("Settings need to be initialized before use.");
            }
            return Settings(projectName, QSettings::IniFormat);
        }();
        return settings;
    }

    QString additionalFile(const QString &fileName) const
    {
        return settingsPath(m_projectName) + fileName;
    }

private:
    Settings(const QString &projectName, Format format)
        : QSettings(settingsPath(projectName) + "settings.ini", format)
        , m_projectName(projectName)
    {
    }

    static QString settingsPath(const QString &projectName)
    {
        const auto configs = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
        const auto config  = configs[0];
        return config + "/" + projectName + "/";
    }

    Settings()                                 = default;
    Settings(const Settings &other)            = delete;
    Settings &operator=(const Settings &other) = delete;

    QString m_projectName;
};

class WithSettings
{
public:
    explicit WithSettings(const QString &projectName) { Settings::initGlobal(projectName); }
};
