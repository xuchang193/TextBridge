#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

#include <QObject>
#include <QTimer>
#include <windows.h>

class KeyboardHook : public QObject
{
    Q_OBJECT

public:
    static KeyboardHook& instance();

    void installHook();
    void uninstallHook();

signals:
    void activationRequested(); // Double Ctrl detected
    void hideRequested();       // ESC detected
    void otherKeyDetected();    // Used internally or for other resets

private:
    explicit KeyboardHook(QObject *parent = nullptr);
    ~KeyboardHook();

    KeyboardHook(const KeyboardHook&) = delete;
    KeyboardHook& operator=(const KeyboardHook&) = delete;

    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

private slots:
    void handleCtrlRelease();
    void handleOtherKey();

private:
    HHOOK m_hHook = NULL;
    QTimer *m_ctrlTimer;
    bool m_isFirstCtrl = false;
    const int CTRL_INTERVAL = 700;
};

#endif // KEYBOARDHOOK_H
