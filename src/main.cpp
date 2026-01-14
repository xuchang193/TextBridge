#include "inputwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include "bootmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QApplication::setOrganizationName("VdiChineseInput");
    QApplication::setApplicationName("VdiChineseInput");
    if (!BootManager::instance().isAutoBoot()) {
        BootManager::instance().setAutoBoot(true);
    }

    QApplication::setQuitOnLastWindowClosed(false);
    a.setStyle(QStyleFactory::create("Fusion"));
    InputWindow w;
    w.show();
    return QApplication::exec();
}
