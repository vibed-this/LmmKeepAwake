# MiuKeepAwake

一个 Windows 原生的小型常驻托盘工具，用于阻止系统因空闲而关闭显示器或进入睡眠状态。

## 功能

- 托盘图标显示当前状态：点亮 / 熄灭灯泡。
- 左键点击托盘图标切换「屏幕常亮」。
- 右键菜单：
  - 屏幕常亮
  - 开机启动
  - 关于
  - 退出
- 「开机启动」使用当前用户的 `HKCU\Software\Microsoft\Windows\CurrentVersion\Run`。
- 使用 Win32 `SetThreadExecutionState` 实现 keep-awake，不修改系统电源计划。

## 构建

要求：

- Windows
- Visual Studio 2022 / MSVC
- Windows SDK
- CMake 3.25+

Debug：

```powershell
cmake --preset windows-msvc-debug
cmake --build --preset windows-msvc-debug
```

Release：

```powershell
cmake --preset windows-msvc-release
cmake --build --preset windows-msvc-release
```

Debug 可执行文件位于：

```text
build/windows-msvc-debug/Debug/MiuKeepAwake.exe
```

## 技术栈

- C17
- MSVC
- CMake
- Win32 API
- Windows SDK
- Unicode / UTF-16 Win32 API

程序使用一个隐藏的 message-only window 接收托盘通知，因此不会创建普通的任务栏窗口。

## 托盘图标

图标的视觉方向参考了 Icons8 的 Windows 11 Color 风格 Light On / Light Off 图标，并针对 Windows notification area 的 16×16 / 32×32 尺寸重新绘制。参考来源见 [`resources/icon-source.md`](resources/icon-source.md)。

## 项目结构

```text
MiuKeepAwake/
├── CMakeLists.txt
├── CMakePresets.json
├── AGENTS.md
├── README.md
├── include/
│   └── miu_keep_awake/
│       ├── app.h
│       ├── power.h
│       ├── startup.h
│       ├── tray.h
│       └── window.h
├── src/
│   ├── app.c
│   ├── main.c
│   ├── power.c
│   ├── startup.c
│   ├── tray.c
│   └── window.c
└── resources/
    ├── MiuKeepAwake.rc
    ├── resource.h
    ├── light-on.ico
    ├── light-off.ico
    ├── light-on.svg
    ├── light-off.svg
    └── icon-source.md
```
