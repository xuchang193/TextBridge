#ifndef BOOTMANAGER_H
#define BOOTMANAGER_H

#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QDir>

class BootManager : public QObject
{
    Q_OBJECT
public:
    static BootManager& instance();

    bool isAutoBoot() const;
    void setAutoBoot(bool enable);

private:
    explicit BootManager(QObject *parent = nullptr);
    ~BootManager() = default;
    
    // Disable copy and assignment
    BootManager(const BootManager&) = delete;
    BootManager& operator=(const BootManager&) = delete;

    const QString REG_PATH = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const QString APP_NAME = "TextBridge";
};

#endif // BOOTMANAGER_H
