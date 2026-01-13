# TextBridge

<div align=center>
<img src="src/resources/images/vdi_input.svg" height="160" width="160" />
</div>

<br>

## Introduction

<p align="right"><br><a href="README_CN.md">简体中文</a> | <a href="README.md">English</a></p>

![License](https://img.shields.io/badge/license-MIT-green) ![Qt](https://img.shields.io/badge/Qt-5.14.2-green) ![Platform](https://img.shields.io/badge/platform-Windows-blue) ![Language](https://img.shields.io/badge/language-C++-orange)

> **TextBridge** is a lightweight, non-intrusive text input bridge designed for Windows. It acts as a seamless connector between your local environment and VDI (Virtual Desktop Infrastructure), remote desktops, or full-screen applications, solving the compatibility and latency issues of switching system IMEs.

<br>

The tool provides a floating input window that appears on demand, allowing you to type text locally and instantly send it to the active application via the clipboard.

## Features

*   **Global Activation**: Double-tap the `Ctrl` key to instantly show or hide the input window.
*   **Smart Clipboard Sync**:
    *   Type your text and press `Enter` to automatically copy it to the clipboard and hide the window.
    *   Press `Shift + Enter` to insert a new line without closing the window.
*   **Input History**:
    *   Navigate through your input history using the `Up` and `Down` arrow keys (or the on-screen buttons).
    *   Automatically saves your recent inputs.
*   **Non-Intrusive UI**:
    *   Frameless, modern design with shadow effects.
    *   Drag anywhere on the draft icon to move the window.
    *   Supports custom QSS styling.
*   **System Tray Integration**: Runs silently in the background with a system tray icon for quick access and control.
*   **Portable**: Designed to be deployed as a standalone, "green" application without installation.

## Build Requirements

*   **OS**: Windows 10/11
*   **Compiler**: MinGW-w64 (GCC 7.3.0 or later recommended)
*   **Framework**: Qt 5.14.2 (MinGW 64-bit edition)
*   **Build System**: CMake 3.28+

## How to Build

1.  **Clone the repository**:
    ```bash
    git clone https://github.com/your-repo/TextBridge.git
    cd TextBridge
    ```

2.  **Configure & Build**:
    You can use the provided packaging script which handles the build process and dependency deployment:
    ```bash
    # Open Git Bash in the project root
    ./scripts/release.sh
    ```

    Or manually using CMake:
    ```bash
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .
    ```

## Deployment

To create a standalone distribution package:
1.  Ensure you have a valid Qt 5.14.2 MinGW 64-bit environment.
2.  Run the release script:
    ```bash
    ./scripts/release.sh
    ```
3.  The output will be generated in the `release_package` directory. This folder contains the executable and all necessary DLLs/plugins. You can copy this folder to any Windows machine to run the application.

## Usage

1.  Run `TextBridge.exe`.
2.  The application will start in the background (check the system tray).
3.  **Double-press `Ctrl`** to toggle the input window.
4.  Type your text.
5.  Press **Enter** to copy the text to your clipboard and close the window.
6.  Paste (`Ctrl+V`) the text into your target application (e.g., a VDI session).

## Project Structure

```
TextBridge/
├── src/
│   ├── resources/          # Icons, fonts, and QSS stylesheets
│   ├── bootmanager.cpp     # Auto-start logic
│   ├── historymanager.cpp  # Input history management
│   ├── inputwindow.cpp     # Main UI logic
│   ├── keyboardhook.cpp    # Global keyboard hook implementation
│   ├── trayiconmanager.cpp # System tray handling
│   └── ...
├── scripts/
│   └── release.sh          # Auto-packaging script (MinGW 64-bit)
└── README.md
```

## License

[MIT License](LICENSE)
