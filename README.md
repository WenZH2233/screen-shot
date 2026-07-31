# Screen Shot — C++ 截图工具

基于 [Snow Shot](https://github.com/mg-chao/snow-shot) 的 **C++ 复刻版**，使用 Win32 API + Skia 图形库构建。

## 功能特性

- 📸 **屏幕截图** — 全屏、当前显示器、聚焦窗口、自定义区域截图
- ✏️ **标注工具** — 矩形、椭圆、箭头、线条、自由笔、文字、马赛克、编号
- 🎯 **智能窗口识别** — 自动识别窗口元素边界
- ⌨️ **全局快捷键** — 支持自定义快捷键 (默认 F1 / Ctrl+F1 / Shift+F1 / Alt+F1)
- 💾 **多种保存方式** — 保存到本地 / 复制到剪贴板
- 🔄 **撤销/重做** — 标注过程中支持撤销和重做
- 🎨 **Skia 渲染** — 高性能 2D 图形渲染
- 🖥️ **多显示器支持** — 跨多显示器截图
- 🧩 **系统托盘** — 最小化到系统托盘运行

## 项目结构

```
src/
├── main.cpp                     # 入口点
├── app.h / app.cpp              # 应用主类
├── screenshot/
│   ├── screen_capture.h/.cpp    # 屏幕捕获 (GDI/DXGI)
├── annotation/
│   ├── overlay_window.h/.cpp    # 截图标注覆盖窗口
├── core/
│   ├── hotkey_manager.h/.cpp    # 全局快捷键管理
│   ├── tray_manager.h/.cpp      # 系统托盘管理
│   ├── settings.h/.cpp          # 设置管理 (JSON)
│   ├── clipboard_helper.h/.cpp  # 剪贴板操作
├── utils/
│   ├── types.h                  # 公共类型定义
│   ├── window_utils.h/.cpp      # Windows 窗口工具
```

## 构建要求

- **CMake** ≥ 3.15
- **Vcpkg** (包管理器)
- **C++20** 编译器 (MSVC)
- **依赖**: Skia, GLFW3, nlohmann-json, clip, utf8cpp

## 构建步骤

```bash
# 1. 安装依赖 (vcpkg 会自动处理)
cd screen-shot
cmake --preset=default
cmake --build build

# 或者手动:
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build
```

## 快捷键

| 快捷键 | 功能 |
|--------|------|
| `F1` | 截图 (当前显示器) |
| `Ctrl+F1` | 全屏截图 |
| `Shift+F1` | 窗口截图 |
| `Alt+F1` | 区域截图 |
| `Enter` | 完成标注 |
| `Esc` / 右键 | 取消 |
| `Ctrl+Z` | 撤销 |
| `Ctrl+Y` | 重做 |
| `1-8` | 切换标注工具 |

## 与 Snow Shot 对比（现在还在基础建设）

| 特性 | Snow Shot (原版) | Screen Shot (C++ 版) |
|------|-----------------|---------------------|
| 技术栈 | Tauri (Rust + React) | Win32 + Skia (C++) |
| 截图 | ✅ | ✅ |
| 标注工具 | ✅ (Excalidraw) | ❌ (Skia) |
| 全局快捷键 | ✅ | ❌ |
| 系统托盘 | ✅ | ❌ |
| 剪贴板 | ✅ | ❌ |
| 滚动截图 | ✅ | ❌ (规划中) |
| 视频录制 | ✅ (需插件) | ❌ (规划中) |
| OCR | ✅ (需插件) | ❌ (规划中) |
| 翻译 | ✅ (需插件) | ❌ (规划中) |
| 插件系统 | ✅ | ❌ (规划中)