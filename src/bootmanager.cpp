#include "bootmanager.h"

BootManager::BootManager(QObject *parent) : QObject(parent)
{
}

BootManager& BootManager::instance()
{
    static BootManager instance;
    return instance;
}

bool BootManager::isAutoBoot() const
{
    QSettings settings(REG_PATH, QSettings::NativeFormat);
    return settings.contains(APP_NAME);
}

void BootManager::setAutoBoot(bool enable)
{
    QSettings settings(REG_PATH, QSettings::NativeFormat);
    if (enable) {
        QString appPath = QDir::toNativeSeparators(QApplication::applicationFilePath());
        settings.setValue(APP_NAME, appPath);
    } else {
        settings.remove(APP_NAME);
    }
}
