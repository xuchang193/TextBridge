#include "keyboardhook.h"
#include <QDebug>
#include <QApplication>

// Global static variables for the hook procedure
static HHOOK g_hHook = NULL;
static bool g_isCtrlDown = false;
static bool g_isCleanCtrl = true;

KeyboardHook& KeyboardHook::instance()
{
    static KeyboardHook instance;
    return instance;
}

KeyboardHook::KeyboardHook(QObject *parent) : QObject(parent)
{
    m_ctrlTimer = new QTimer(this);
    m_ctrlTimer->setSingleShot(true);
    connect(m_ctrlTimer, &QTimer::timeout, this, [this]() {
        m_isFirstCtrl = false;
    });
}

KeyboardHook::~KeyboardHook()
{
    uninstallHook();
}

void KeyboardHook::installHook()
{
    if (!g_hHook) {
        g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
        if (!g_hHook) {
            qCritical() << "Failed to install keyboard hook! Error:" << GetLastError();
        } else {
            qDebug() << "Keyboard hook installed successfully.";
        }
    }
}

void KeyboardHook::uninstallHook()
{
    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
        qDebug() << "Keyboard hook uninstalled.";
    }
}

LRESULT CALLBACK KeyboardHook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT *pkb = (KBDLLHOOKSTRUCT *)lParam;
        
        // Detect Ctrl (Left or Right)
        bool isCtrl = (pkb->vkCode == VK_LCONTROL || pkb->vkCode == VK_RCONTROL || pkb->vkCode == VK_CONTROL);

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            if (isCtrl)
            {
                // Only mark as pressed if it wasn't already (to avoid auto-repeat)
                if (!g_isCtrlDown) {
                    g_isCtrlDown = true;
                    g_isCleanCtrl = true; // Start potential clean press
                }
            }
            else
            {
                // ESC Detection
                if (pkb->vkCode == VK_ESCAPE)
                {
                    // Emit signal asynchronously
                    QMetaObject::invokeMethod(&KeyboardHook::instance(), "hideRequested", Qt::QueuedConnection);
                }

                g_isCleanCtrl = false;
                
                // Reset double-click state on interference
                QMetaObject::invokeMethod(&KeyboardHook::instance(), "handleOtherKey", Qt::QueuedConnection);
            }
        }
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
        {
            if (isCtrl)
            {
                // Only process if it was marked as down (prevents processing unrelated keyups if logic state is off)
                if (g_isCtrlDown) {
                    g_isCtrlDown = false;
                    if (g_isCleanCtrl)
                    {
                        // Valid clean Ctrl release
                        QMetaObject::invokeMethod(&KeyboardHook::instance(), "handleCtrlRelease", Qt::QueuedConnection);
                    }
                }
            }
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void KeyboardHook::handleCtrlRelease()
{
    if (!m_isFirstCtrl)
    {
        // First Ctrl press
        m_isFirstCtrl = true;
        m_ctrlTimer->start(CTRL_INTERVAL);
    }
    else
    {
        // Second Ctrl press within interval
        m_ctrlTimer->stop();
        m_isFirstCtrl = false;
        emit activationRequested();
    }
}

void KeyboardHook::handleOtherKey()
{
    if (m_isFirstCtrl) {
        m_isFirstCtrl = false;
        m_ctrlTimer->stop();
    }
    emit otherKeyDetected();
}
