#include "inputwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include "bootmanager.h"
#include "configmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QApplication::setOrganizationName("VdiChineseInput");
    QApplication::setApplicationName("VdiChineseInput");
    
    // 根据配置文件中的设置同步注册表状态
    bool autoBootConfig = ConfigManager::instance().isAutoBootEnabled();
    bool autoBootRegistry = BootManager::instance().isAutoBoot();
    if (autoBootConfig != autoBootRegistry) {
        BootManager::instance().setAutoBoot(autoBootConfig);
    }

    QApplication::setQuitOnLastWindowClosed(false);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    InputWindow w;
    w.show();
    return QApplication::exec();
}
