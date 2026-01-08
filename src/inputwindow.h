#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QTimer>
#include <windows.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>
#include <QApplication>
#include "historymanager.h"
#include "trayiconmanager.h"
#include "keyboardhook.h"

#include <QGraphicsDropShadowEffect>

class InputWindow : public QWidget
{
    Q_OBJECT

public:
    InputWindow(QWidget *parent = nullptr);
    ~InputWindow();

    // Removed static instance as it's no longer needed for global hook callback

public slots:
    void toggleVisibility(); // Renamed from onCtrlPressed
    void hideWindow();       // For ESC handling

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void initUI();
    void initFont();
    
    void updateButtonState();

private slots:
    void onEnterPressed();
    void showPreviousHistory();
    void showNextHistory();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    QPlainTextEdit *m_inputEdit = nullptr;
    // Removed m_ctrlTimer, m_isFirstCtrl, constants, hook methods
    
    bool m_stay_in_center = true;
    const int INIT_WIDTH = 500;
    const int INIT_HEIGHT = 105;
    const int m_DRAFT_LABEL_WIDTH = 32;
    const int m_DRAFT_LABEL_RIGHT = 32;

    QString m_font_family;
    HistoryManager *m_historyManager = nullptr;
    TrayIconManager *m_trayManager = nullptr;
    // KeyboardHook is a singleton, accessed via instance()

    QHBoxLayout *m_main_h_layout = nullptr;
    QVBoxLayout *m_buttons_widget_v_layout = nullptr;
    QVBoxLayout *m_content_widget_v_layout = nullptr;
    QWidget *m_centerWidget = nullptr;
    QWidget *m_content_widget = nullptr;
    QWidget *m_buttons_widget = nullptr;
    QLabel *m_draft_label = nullptr;
    QPushButton *m_up_arrow = nullptr;
    QPushButton *m_down_arrow = nullptr;

    bool m_isDragging = false;       
    QPoint m_dragStartPos;
};

#endif // INPUTWINDOW_H
