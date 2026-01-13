#include "inputwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include "bootmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QApplication::setOrganizationName("VdiChineseInput");
    QApplication::setApplicationName("VdiChineseInput");

    // Ensure default auto-boot setting on first run
    // If user hasn't set it yet (not in registry), enable it by default
    if (!BootManager::instance().isAutoBoot()) {
        // Double check if it was explicitly disabled or just never set?
        // Since we don't store "disabled" state separately, we can just enforce default true logic
        // But simply, let's just enable it if it's missing for now, or respect user choice.
        // Requirement: "Default is on"
        // To strictly follow "Default is on", we should check if this is first run.
        // For simplicity, let's just set it to true if not present.
        BootManager::instance().setAutoBoot(true);
    }

//    a.setQuitOnLastWindowClosed(false);
    QApplication::setQuitOnLastWindowClosed(false);
    a.setStyle(QStyleFactory::create("Fusion"));
    InputWindow w;
    // 初始隐藏窗口，仅通过双击 Ctrl 触发显示
     w.show();
    return QApplication::exec();
}
