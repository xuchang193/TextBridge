#ifndef TRAYICONMANAGER_H
#define TRAYICONMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include "bootmanager.h"

class TrayIconManager : public QObject
{
    Q_OBJECT
public:
    explicit TrayIconManager(QObject *parent = nullptr);
    void show();

signals:
    // 转发用户的操作意图，而不是具体的 UI 逻辑
    void showWindowRequested();
    void quitRequested();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void initTray();
    void createActions();
    void createMenu();

    QSystemTrayIcon *m_trayIcon = nullptr;
    QMenu *m_trayMenu = nullptr;
    QAction *m_showAction = nullptr;
    QAction *m_autoBootAction = nullptr;
    QAction *m_quitAction = nullptr;
};

#endif // TRAYICONMANAGER_H
