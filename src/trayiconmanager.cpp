#include "trayiconmanager.h"
#include <QApplication>
#include <QStyle>
#include <QMessageBox>

TrayIconManager::TrayIconManager(QObject *parent) : QObject(parent)
{
    initTray();
}

void TrayIconManager::initTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::warning(nullptr, tr("Error"), tr("System tray is not available on this system."));
        return;
    }

    m_trayIcon = new QSystemTrayIcon(this);
    
    // Load icon, check if valid, fallback if needed
    // Prioritize the new SVG icon
    QIcon icon(":/resources/images/vdi_input.svg");
    if (icon.isNull()) {
        // Fallback to old png if svg fails
        icon = QIcon(":/resources/images/target.png");
    }
    if (icon.isNull()) {
        icon = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    }
    m_trayIcon->setIcon(icon);
    
    m_trayIcon->setToolTip(tr("VDI Chinese Input"));

    createActions();
    createMenu();

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &TrayIconManager::onActivated);
}

void TrayIconManager::createActions()
{
    m_showAction = new QAction(tr("打开"), this);
    // 当点击“打开”时，发送信号，由外部决定如何“打开”
    connect(m_showAction, &QAction::triggered, this, &TrayIconManager::showWindowRequested);

    // Auto Boot Action
    m_autoBootAction = new QAction(tr("开机自启"), this);
    m_autoBootAction->setCheckable(true);
    // Initialize state from BootManager
    m_autoBootAction->setChecked(BootManager::instance().isAutoBoot());
    connect(m_autoBootAction, &QAction::toggled, this, [](bool checked){
        BootManager::instance().setAutoBoot(checked);
    });

    // Hyper Mode Action
    m_hyperModeAction = new QAction(tr("Hyper Mode"), this);
    m_hyperModeAction->setCheckable(true);
    m_hyperModeAction->setChecked(ConfigManager::instance().isHyperModeEnabled());
    m_hyperModeAction->setToolTip(tr("开启后，按 Enter 键自动将内容粘贴到目标窗口"));
    connect(m_hyperModeAction, &QAction::toggled, this, [](bool checked){
        ConfigManager::instance().setHyperModeEnabled(checked);
    });

    m_quitAction = new QAction(tr("退出"), this);
    // 当点击“退出”时，发送信号
    connect(m_quitAction, &QAction::triggered, this, &TrayIconManager::quitRequested);
}

void TrayIconManager::createMenu()
{
    m_trayMenu = new QMenu();
    m_trayMenu->addAction(m_showAction);
    m_trayMenu->addAction(m_autoBootAction);
    m_trayMenu->addAction(m_hyperModeAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);
}

void TrayIconManager::show()
{
    m_trayIcon->show();
}

void TrayIconManager::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    emit trayIconActivated(reason);
}