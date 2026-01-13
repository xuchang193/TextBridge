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

bool ConfigManager::isHyperModeEnabled() const
{
    QSettings settings;
    bool value = settings.value(KEY_HYPER_MODE, false).toBool();
    // qDebug() << "ConfigManager: Reading HyperMode =" << value << " from " << settings.fileName();
    return value;
}

void ConfigManager::setHyperModeEnabled(bool enabled)
{
    QSettings settings;
    settings.setValue(KEY_HYPER_MODE, enabled);
    settings.sync(); // Ensure written to disk
    // qDebug() << "ConfigManager: Writing HyperMode =" << enabled << " to " << settings.fileName();
    emit hyperModeChanged(enabled);
}
