#include "inputwindow.h"
#include <QApplication>
#include <QClipboard>
#include <QScreen>
#include <QDebug>
#include <QFontDatabase>
#include <QPixmap>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QIcon>

static void ForceActivateWindow(HWND hwnd) {
    if (!hwnd) return;

    DWORD currentThreadId = GetCurrentThreadId();
    DWORD foregroundThreadId = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

    if (currentThreadId != foregroundThreadId) {
        AttachThreadInput(foregroundThreadId, currentThreadId, TRUE);
        
        int retry = 3;
        while (retry-- > 0) {
            if (SetForegroundWindow(hwnd)) break;
            Sleep(10);
        }
        
        SetFocus(hwnd);
        AttachThreadInput(foregroundThreadId, currentThreadId, FALSE);
    } else {
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
    }
    
    // 确保窗口不在最小化状态
    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }
}

static bool loadQssFile(const QString& qssFilePath, QWidget* targetWidget = nullptr)
{
    QFile qssFile(qssFilePath);
    if (!qssFile.open(QFile::ReadOnly | QFile::Text))
    {
        QString errorMsg = QString("无法打开QSS文件：%1\n错误信息：%2").arg(qssFilePath, qssFile.errorString());
        QMessageBox::warning(nullptr, "QSS加载失败", errorMsg);
        return false;
    }

    QTextStream textStream(&qssFile);
    textStream.setCodec("UTF-8");
    QString qssContent = textStream.readAll();

    qssFile.close();

    if (nullptr != targetWidget)
    {
        targetWidget->setStyleSheet(qssContent);
    }

    return true;
}

void InputWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    // 窗口显示后自动聚焦到输入框，确保可以直接输入
    ForceActivateWindow((HWND)this->winId());
    m_inputEdit->setFocus();
}

void InputWindow::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
}

bool InputWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    return QWidget::nativeEvent(eventType, message, result);
}



void InputWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // Since we are using a translucent background with a child widget for shadow,
    // we don't need to manually paint the background here anymore.
    // The child widget 'm_centerWidget' handles the white background and border via stylesheet.
}


InputWindow::InputWindow(QWidget *parent)
    : QWidget(parent)
{
    // m_instance = this; // Removed static instance

    initFont();
    initUI();

    // 双击Ctrl定时器 - Removed, moved to KeyboardHook
    // m_ctrlTimer = new QTimer(this);
    // ...
    
    // 安装事件过滤器以处理 Enter 和 Shift+Enter
    m_inputEdit->installEventFilter(this);
    
    // Set Window Icon (Taskbar Icon)
    // Prioritize SVG, fallback to PNG, then system default
    QIcon appIcon(":/resources/images/vdi_input.svg");
    if (appIcon.isNull()) {
        appIcon = QIcon(":/resources/images/target.png");
    }
    if (!appIcon.isNull()) {
        this->setWindowIcon(appIcon);
    }

    // Initialize Tray Icon Manager
    m_trayManager = new TrayIconManager(this);
    connect(m_trayManager, &TrayIconManager::showWindowRequested, this, &InputWindow::showNormal);
    connect(m_trayManager, &TrayIconManager::quitRequested, qApp, &QApplication::quit);
    connect(m_trayManager, &TrayIconManager::trayIconActivated, this, &InputWindow::onTrayIconActivated);
    m_trayManager->show();

    // KeyboardHook is now a singleton managed internally
    // Connect signals from the hook to this window's slots
    connect(&KeyboardHook::instance(), &KeyboardHook::activationRequested, this, &InputWindow::toggleVisibility);
    connect(&KeyboardHook::instance(), &KeyboardHook::hideRequested, this, &InputWindow::hideWindow);
    
    // Install the hook immediately
    KeyboardHook::instance().installHook();
    qDebug() << "Keyboard hook connected.";
}

InputWindow::~InputWindow()
{
    KeyboardHook::instance().uninstallHook();
}

void InputWindow::initUI()
{
    // 为主窗口设置透明背景，以便显示阴影
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    
    // 初始化大尺寸以容纳阴影
    setFixedSize(INIT_WIDTH + 20, INIT_HEIGHT + 20); 

    this->setObjectName("MainWindow");
//    loadQssFile(":/resources/qss/inputwindow_style.qss", this);

    // 创建一个中心部件来承载内容，并应用阴影
    m_centerWidget = new QWidget(this);
    m_centerWidget->setObjectName("m_centerWidget");
    m_centerWidget->setGeometry(10, 10, INIT_WIDTH, INIT_HEIGHT); // 留出阴影边距
    loadQssFile(":/resources/qss/center_widget_style.qss", m_centerWidget);
    
    // 添加阴影效果
    auto shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 2);
    m_centerWidget->setGraphicsEffect(shadow);

    m_main_h_layout = new QHBoxLayout(m_centerWidget);
    m_main_h_layout->setSpacing(0);
    m_main_h_layout->setContentsMargins(2, 2, 2, 2); // 内部留白
    
    m_content_widget = new QWidget(m_centerWidget);
    m_buttons_widget = new QWidget(m_centerWidget);

    m_inputEdit = new QPlainTextEdit();
    m_inputEdit->setFont(QFont(m_font_family, 14));
    m_inputEdit->setPlaceholderText("输入文字后按 Enter 复制到剪贴板，Shift+Enter 换行...");
    m_inputEdit->setFocusPolicy(Qt::StrongFocus);
    m_inputEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_inputEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_inputEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_inputEdit->setMinimumHeight(60);
    loadQssFile(":/resources/qss/plain_text_edit_style.qss", m_inputEdit);

    m_historyManager = new HistoryManager(this);

    m_content_widget_v_layout = new QVBoxLayout(m_content_widget);
    m_content_widget_v_layout->addWidget(m_inputEdit);
    m_content_widget_v_layout->setSpacing(0);
    m_content_widget_v_layout->setContentsMargins(0, 0, 0, 0);

    m_draft_label = new QLabel();
    m_draft_label->setFixedSize(m_DRAFT_LABEL_WIDTH, m_DRAFT_LABEL_RIGHT);
    QPixmap draft_label_pixmap(":/resources/images/target.png");
    QPixmap scale_draft_label_pixmap = draft_label_pixmap.scaled(m_draft_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_draft_label->setPixmap(scale_draft_label_pixmap);
    m_draft_label->setAlignment(Qt::AlignCenter);
    m_draft_label->installEventFilter(this); // 给标签安装事件过滤器
    m_draft_label->setCursor(Qt::PointingHandCursor); // 可选：鼠标悬浮时显示手型，提示可拖动

    m_up_arrow = new QPushButton();
    m_up_arrow->setFixedSize(m_DRAFT_LABEL_WIDTH, m_DRAFT_LABEL_RIGHT);
    QIcon up_arrow_icon;
    up_arrow_icon.addFile(":/resources/images/up_arrow.png", QSize(), QIcon::Normal);
    up_arrow_icon.addFile(":/resources/images/up_arrow_disabled.png", QSize(), QIcon::Disabled);
    m_up_arrow->setIcon(up_arrow_icon);
    m_up_arrow->setIconSize(QSize(m_DRAFT_LABEL_WIDTH, m_DRAFT_LABEL_RIGHT));
    loadQssFile(":/resources/qss/up_pushbtn_style.qss", m_up_arrow);
    m_down_arrow = new QPushButton();
    m_down_arrow->setFixedSize(m_DRAFT_LABEL_WIDTH, m_DRAFT_LABEL_RIGHT);
    QIcon down_arrow_icon;
    down_arrow_icon.addFile(":/resources/images/down_arrow.png", QSize(), QIcon::Normal);
    down_arrow_icon.addFile(":/resources/images/down_arrow_disabled.png", QSize(), QIcon::Disabled);
    m_down_arrow->setIcon(down_arrow_icon);
    m_down_arrow->setIconSize(QSize(m_DRAFT_LABEL_WIDTH, m_DRAFT_LABEL_RIGHT));
    loadQssFile(":/resources/qss/down_pushbtn_style.qss", m_down_arrow);
    m_buttons_widget_v_layout = new QVBoxLayout(m_buttons_widget);
    m_buttons_widget_v_layout->addWidget(m_draft_label);
    m_buttons_widget_v_layout->addWidget(m_up_arrow);
    m_buttons_widget_v_layout->addWidget(m_down_arrow);
//    m_buttons_widget_v_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    m_buttons_widget_v_layout->setSpacing(0);
    m_buttons_widget_v_layout->setContentsMargins(0, 0, 0, 0);

    m_main_h_layout->addWidget(m_content_widget);
    m_main_h_layout->addWidget(m_buttons_widget);
    m_main_h_layout->setStretch(0, 1);
    m_main_h_layout->setContentsMargins(1, 1, 1, 1);
    m_main_h_layout->setSpacing (1);

    connect(m_up_arrow, &QPushButton::clicked, this, &InputWindow::showPreviousHistory);
    connect(m_down_arrow, &QPushButton::clicked, this, &InputWindow::showNextHistory);
    updateButtonState();

    this->hide();
}

void InputWindow::initFont() {
    int fontId = QFontDatabase::addApplicationFont(":/resources/fonts/NotoSansSC-Regular.ttf");
    if (-1 == fontId)
    {
        qDebug() << "字体加载失败！请检查资源路径是否正确";
    }
    else
    {
        m_font_family = QFontDatabase::applicationFontFamilies(fontId).at(0);
        qDebug() << "字体加载成功，字体族名：" << m_font_family;
    }
}

void InputWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if (this->isVisible()) {
            this->hide();
        } else {
            this->showNormal();
            ForceActivateWindow((HWND)this->winId());
        }
        break;
    default:
        break;
    }
}

void InputWindow::hideWindow()
{
    // ESC handling: Only hide if visible
    if (isVisible()) {
        hide();
    }
}

void InputWindow::toggleVisibility()
{
    if (isVisible())
    {
        // If visible, hide
        qDebug() << "Double Ctrl -> Hide";
        hide();
    }
    else
    {
        // If hidden, show
        qDebug() << "Double Ctrl -> Show";
        // Ensure centered if needed (logic can be refined)
        if (m_stay_in_center) {
            QScreen *screen = QApplication::primaryScreen();
            QRect screenRect = screen->availableGeometry();
            move((screenRect.width() - width()) / 2, (screenRect.height() - height()) / 2);               
        }
        show();
        ForceActivateWindow((HWND)this->winId());
        m_inputEdit->setFocus();
    }
}

bool InputWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_inputEdit && event->type() == QEvent::KeyPress)
    {
        auto *keyEvent = dynamic_cast<QKeyEvent *>(event);
        if (nullptr == keyEvent) {
            qDebug() << "keyEvent is nullptr!";
        }
        // 处理 Enter 键
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            // 如果按下了 Shift，则插入换行符
            if (keyEvent->modifiers() & Qt::ShiftModifier)
            {
                // Shift+Enter：插入换行，不处理事件（让默认行为处理）
                return false;
            }
                // Enter（无 Shift）：复制到剪贴板并隐藏窗口
                onEnterPressed();
                return true; // 已处理，阻止默认行为
        }
    }

    // ========== 新增逻辑：处理m_draft_label拖动窗口 ==========
    if (obj == m_draft_label)
    {
        // 1. 鼠标左键按下：标记开始拖动，记录起始坐标
        if (event->type() == QEvent::MouseButtonPress)
        {
            auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
            if (nullptr == mouseEvent) {
                qDebug() << "mouseEvent is nullptr!";
            }
            if (mouseEvent->button() == Qt::LeftButton)
            {
                m_isDragging = true;
                m_dragStartPos = mouseEvent->globalPos(); // 记录鼠标按下时的全局坐标
            }
        }
        // 2. 鼠标移动：如果正在拖动，更新窗口位置
        else if (event->type() == QEvent::MouseMove && m_isDragging)
        {
            m_stay_in_center = false;
            auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
            if (nullptr == mouseEvent) {
                qDebug() << "mouseEvent is nullptr!";
            }
            QPoint offset = mouseEvent->globalPos() - m_dragStartPos; // 计算鼠标偏移量
            this->move(this->pos() + offset); // 移动窗口
            m_dragStartPos = mouseEvent->globalPos(); // 更新起始坐标，避免窗口跳动
        }
        // 3. 鼠标左键释放：停止拖动
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
            if (nullptr == mouseEvent) {
                qDebug() << "mouseEvent is nullptr!";
            }
            if (mouseEvent->button() == Qt::LeftButton)
            {
                m_isDragging = false;
            }
        }
    }

    // 其他事件交给默认处理
    return QWidget::eventFilter(obj, event);
}

void InputWindow::onEnterPressed()
{
    QString inputText = m_inputEdit->toPlainText().trimmed();
    if (!inputText.isEmpty())
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(inputText);
        qDebug() << "复制到剪贴板：" << inputText;

        m_historyManager->addHistory(inputText);
        updateButtonState();
    }
    m_inputEdit->clear();
    hide();
}

void InputWindow::showPreviousHistory()
{
    if (m_historyManager->isAtNewest()) {
        m_historyManager->setDraft(m_inputEdit->toPlainText());
    }

    if (!m_historyManager->hasPrevious()) return;

    QString historyText = m_historyManager->getPrevious();
    m_inputEdit->setPlainText(historyText);
    
    // Move cursor to end
    QTextCursor cursor = m_inputEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_inputEdit->setTextCursor(cursor);
    
    updateButtonState();
}

void InputWindow::showNextHistory()
{
    if (!m_historyManager->hasNext()) return;

    QString historyText = m_historyManager->getNext();
    m_inputEdit->setPlainText(historyText);
    
    // Move cursor to end
    QTextCursor cursor = m_inputEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_inputEdit->setTextCursor(cursor);

    updateButtonState();
}

void InputWindow::updateButtonState()
{
    m_up_arrow->setEnabled(m_historyManager->hasPrevious());
    m_down_arrow->setEnabled(m_historyManager->hasNext());
}
