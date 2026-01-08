# TextBridge (原 VDI 中文输入助手)

**TextBridge** 是一款专为 Windows 设计的轻量级、无侵入式文本输入桥接工具。它作为本地环境与 VDI（虚拟桌面基础架构）、远程桌面场景或全屏应用程序之间的无缝连接器，解决了在这些环境中切换系统输入法（IME）可能带来的卡顿、兼容性差或操作繁琐的问题。

该工具提供一个按需显示的悬浮输入窗口，允许您在本地环境中流畅输入文本，并通过剪贴板瞬间将内容发送到目标应用程序中。

## 主要功能

*   **全局快捷激活**：双击 `Ctrl` 键即可瞬间显示或隐藏输入窗口，无需鼠标点击。
*   **智能剪贴板同步**：
    *   输入文本后按下 `Enter` 键，内容将自动复制到剪贴板并隐藏窗口。
    *   按下 `Shift + Enter` 可在输入框内换行。
*   **历史记录回溯**：
    *   使用键盘 `↑` / `↓` 键（或界面上的箭头按钮）快速查看和复用之前的输入记录。
    *   自动保存最近的输入内容。
*   **现代化无侵入 UI**：
    *   无边框设计，带有优雅的阴影效果。
    *   支持拖拽：按住左侧图标区域即可随意移动窗口位置。
    *   支持自定义 QSS 样式表。
*   **系统托盘集成**：程序启动后最小化至系统托盘，静默运行，支持右键菜单控制。
*   **绿色便携**：无需安装，解压即用，不修改系统敏感配置。

## 编译环境要求

*   **操作系统**: Windows 10/11
*   **编译器**: MinGW-w64 (推荐 GCC 7.3.0 或更高版本)
*   **Qt 框架**: Qt 5.14.2 (MinGW 64-bit 版本)
*   **构建系统**: CMake 3.28+

## 如何构建

1.  **克隆仓库**:
    ```bash
    git clone https://github.com/your-repo/TextBridge.git
    cd TextBridge
    ```

2.  **配置与编译**:
    建议使用项目提供的打包脚本，它会自动处理编译和依赖复制：
    ```bash
    # 在项目根目录下打开 Git Bash
    ./scripts/release.sh
    ```

    或者手动使用 CMake 编译：
    ```bash
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .
    ```

## 部署与发布

要创建一个独立的绿色发布包：
1.  确保您的环境已安装 Qt 5.14.2 MinGW 64-bit。
2.  运行发布脚本：
    ```bash
    ./scripts/release.sh
    ```
3.  脚本运行完成后，`release_package` 目录下即为最终产物。该文件夹包含了可执行文件及所有必要的 DLL 依赖库和插件。您可以直接将此文件夹复制到任何 Windows 机器上运行，无需安装 Qt 环境。

## 使用指南

1.  运行 `TextBridge.exe`。
2.  程序将在后台启动，您可以在系统托盘区看到图标。
3.  **快速连按两次 `Ctrl` 键** 呼出输入窗口。
4.  输入您想要发送的中文或英文内容。
5.  按下 **Enter** 键，内容自动复制到剪贴板，窗口自动消失。
6.  在您的目标应用（如 VDI 桌面）中，按下 `Ctrl+V` 粘贴内容。

## 项目结构

```
TextBridge/
├── src/
│   ├── resources/          # 图标、字体和 QSS 样式表资源
│   ├── bootmanager.cpp     # 开机自启逻辑
│   ├── historymanager.cpp  # 输入历史记录管理
│   ├── inputwindow.cpp     # 主界面 UI 逻辑
│   ├── keyboardhook.cpp    # 全局键盘钩子实现
│   ├── trayiconmanager.cpp # 系统托盘管理
│   └── ...
├── scripts/
│   └── release.sh          # 自动化打包脚本 (适配 MinGW 64-bit)
└── README.md
```

## 许可证

[MIT License](LICENSE)
