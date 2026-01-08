#!/bin/bash

# ==============================================================================
# VdiChineseInput Release Packaging Script (手动部署依赖版)
# Environment: Windows (Git Bash) + MinGW + Qt 5.14.2
# ==============================================================================

# --- CONFIGURATION START ---
QT_DIR="/d/Qt/Qt5.14.2/5.14.2/mingw73_64"
QT_BIN_DIR="${QT_DIR}/bin"
MINGW_BIN_DIR="/d/Qt/Qt5.14.2/Tools/mingw730_64/bin"

# 优先使用指定版本的MinGW和Qt
export PATH="${MINGW_BIN_DIR}:${QT_BIN_DIR}:$PATH"
# --- CONFIGURATION END ---

# Project Settings
PROJECT_NAME="TextBridge"
SOURCE_DIR=$(pwd)
BUILD_DIR="${SOURCE_DIR}/build_release"
RELEASE_DIR="${SOURCE_DIR}/release_package"

# 定义需要手动复制的Qt核心DLL（Release版本）
QT_CORE_DLLS=(
    "${QT_BIN_DIR}/Qt5Core.dll"
    "${QT_BIN_DIR}/Qt5Gui.dll"
    "${QT_BIN_DIR}/Qt5Widgets.dll"
    "${QT_BIN_DIR}/libgcc_s_seh-1.dll"
    "${QT_BIN_DIR}/libstdc++-6.dll"
    "${QT_BIN_DIR}/libwinpthread-1.dll"
    "${QT_BIN_DIR}/Qt5Svg.dll"
)

# 定义需要复制的平台插件
# qminimal.dll 很多时候不是必须的，为了减小体积可以去掉
QT_PLATFORM_PLUGINS=(
    "${QT_BIN_DIR}/../plugins/platforms/qwindows.dll"
)

# 定义需要复制的图片格式插件
QT_IMAGE_PLUGINS=(
    "${QT_BIN_DIR}/../plugins/imageformats/qsvg.dll" # 增加SVG图片格式插件
)

echo "========================================================"
echo "   Starting Packaging for ${PROJECT_NAME} (手动部署)"
echo "========================================================"

# 验证工具和文件
echo "Checking tools and dependencies..."
if ! command -v cmake &> /dev/null; then
    echo "Error: 'cmake' not found."
    exit 1
fi
if ! command -v mingw32-make &> /dev/null; then
    echo "Error: 'mingw32-make' not found."
    exit 1
fi
# 检查核心DLL是否存在
for dll in "${QT_CORE_DLLS[@]}"; do
    if [ ! -f "$dll" ]; then
        echo "Error: Missing Qt DLL: $dll"
        exit 1
    fi
done
# 检查平台插件是否存在
for plugin in "${QT_PLATFORM_PLUGINS[@]}"; do
    if [ ! -f "$plugin" ]; then
        echo "Error: Missing Qt plugin: $plugin"
        exit 1
    fi
done
# 检查图片插件是否存在
for plugin in "${QT_IMAGE_PLUGINS[@]}"; do
    if [ ! -f "$plugin" ]; then
        echo "Error: Missing Qt image plugin: $plugin"
        exit 1
    fi
done
echo "Check passed."
echo "--------------------------------------------------------"

# 1. 清理旧构建
echo "[1/4] Cleaning previous build/release directories..."
rm -rf "$BUILD_DIR"
rm -rf "$RELEASE_DIR"
mkdir -p "$BUILD_DIR"
mkdir -p "$RELEASE_DIR"

# 2. CMake配置
echo "[2/4] Configuring project (Release)..."
cd "$BUILD_DIR"
cmake -G "MinGW Makefiles" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="$QT_DIR" \
      "$SOURCE_DIR"
if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed."
    exit 1
fi

# 3. 编译
echo "[3/4] Compiling..."
cmake --build . --config Release --clean-first
if [ $? -ne 0 ]; then
    echo "Error: Compilation failed."
    exit 1
fi

# 4. 手动部署所有依赖
echo "[4/4] Deploying dependencies (manual mode)..."
cd "$SOURCE_DIR"

# 复制可执行文件
# 注意：因为用了 add_subdirectory(src)，可执行文件会在 src 子目录下
EXE_PATH="$BUILD_DIR/src/${PROJECT_NAME}.exe"
if [ ! -f "$EXE_PATH" ]; then
    # 尝试查找旧位置（兼容性）
    EXE_PATH="$BUILD_DIR/${PROJECT_NAME}.exe"
fi

if [ ! -f "$EXE_PATH" ]; then
    echo "Error: Executable not found: $EXE_PATH"
    exit 1
fi
cp "$EXE_PATH" "$RELEASE_DIR/"
echo "✅ Copied executable: ${PROJECT_NAME}.exe"

# 复制Qt核心DLL
for dll in "${QT_CORE_DLLS[@]}"; do
    cp "$dll" "$RELEASE_DIR/"
    echo "✅ Copied DLL: $(basename $dll)"
done

# 复制平台插件（创建platforms目录）
PLATFORM_DST_DIR="${RELEASE_DIR}/platforms"
mkdir -p "$PLATFORM_DST_DIR"
for plugin in "${QT_PLATFORM_PLUGINS[@]}"; do
    cp "$plugin" "$PLATFORM_DST_DIR/"
    echo "✅ Copied plugin: platforms/$(basename $plugin)"
done

# 复制图片格式插件（创建imageformats目录）
IMAGE_DST_DIR="${RELEASE_DIR}/imageformats"
mkdir -p "$IMAGE_DST_DIR"
for plugin in "${QT_IMAGE_PLUGINS[@]}"; do
    cp "$plugin" "$IMAGE_DST_DIR/"
    echo "✅ Copied plugin: imageformats/$(basename $plugin)"
done

# 5. 创建 qt.conf 以确保独立运行
echo "[5/4] Creating qt.conf..."
QT_CONF="${RELEASE_DIR}/qt.conf"
echo "[Paths]" > "$QT_CONF"
echo "Plugins=." >> "$QT_CONF"
echo "✅ Created qt.conf"

echo "========================================================"
echo "   SUCCESS! Packaging Complete (手动部署)"
echo "   Release folder: $RELEASE_DIR"
echo "   验证方式：双击 $RELEASE_DIR/${PROJECT_NAME}.exe 测试"
echo "========================================================"