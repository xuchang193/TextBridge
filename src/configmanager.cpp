#include "configmanager.h"
#include <QCoreApplication>

ConfigManager::ConfigManager(QObject *parent) : QObject(parent)
{
}

ConfigManager& ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

QString ConfigManager::configFilePath() const
{
    QString appDir = QCoreApplication::applicationDirPath();
    return QDir(appDir).filePath(CONFIG_FILE_NAME);
}

QSettings* ConfigManager::getSettings() const
{
    // 使用可执行文件所在目录的 config.ini 文件
    return new QSettings(configFilePath(), QSettings::IniFormat);
}

bool ConfigManager::isHyperModeEnabled() const
{
    QSettings* settings = getSettings();
    bool value = settings->value(KEY_HYPER_MODE, false).toBool();
    delete settings;
    return value;
}

void ConfigManager::setHyperModeEnabled(bool enabled)
{
    QSettings* settings = getSettings();
    settings->setValue(KEY_HYPER_MODE, enabled);
    settings->sync();
    delete settings;
    emit hyperModeChanged(enabled);
}

bool ConfigManager::isAutoBootEnabled() const
{
    QSettings* settings = getSettings();
    // 默认值为 false，不自动开启开机自启
    bool value = settings->value(KEY_AUTO_BOOT, true).toBool();
    delete settings;
    return value;
}

void ConfigManager::setAutoBootEnabled(bool enabled)
{
    QSettings* settings = getSettings();
    settings->setValue(KEY_AUTO_BOOT, enabled);
    settings->sync();
    delete settings;
    emit autoBootChanged(enabled);
}
