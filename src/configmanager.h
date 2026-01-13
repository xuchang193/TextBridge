#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    static ConfigManager& instance();

    bool isHyperModeEnabled() const;
    void setHyperModeEnabled(bool enabled);

signals:
    void hyperModeChanged(bool enabled);

private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager() = default;
    
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    const QString KEY_HYPER_MODE = "HyperMode";
};

#endif // CONFIGMANAGER_H
