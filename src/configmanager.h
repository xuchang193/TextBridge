#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    static ConfigManager& instance();

    bool isHyperModeEnabled() const;
    void setHyperModeEnabled(bool enabled);

    bool isAutoBootEnabled() const;
    void setAutoBootEnabled(bool enabled);

    QString configFilePath() const;

signals:
    void hyperModeChanged(bool enabled);
    void autoBootChanged(bool enabled);

private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager() = default;
    
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    QSettings* getSettings() const;

    const QString CONFIG_FILE_NAME = "config.ini";
    const QString KEY_HYPER_MODE = "General/HyperMode";
    const QString KEY_AUTO_BOOT = "General/AutoBoot";
};

#endif // CONFIGMANAGER_H
