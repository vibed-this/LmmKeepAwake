# MiuKeepAwake

[English](./README.en.md)

一个 Windows 原生的小型常驻托盘工具，用于阻止系统因空闲而关闭显示器或进入睡眠状态。启动后默认开启常亮，常驻通知区域，不创建普通任务栏窗口。

## 功能

- 托盘图标显示当前状态：点亮 / 熄灭灯泡。
- 左键单击托盘图标切换「屏幕常亮」。
- 右键菜单：
  - 屏幕常亮（带勾选状态，与实际状态同步）
  - 开机启动（带勾选状态）
  - 关于
  - 退出
- 中英文界面：跟随系统语言自动切换（英文 / 简体中文）。
- 「退出」时自动移除托盘图标并释放常亮请求。

## 使用方法

1. 运行 `MiuKeepAwake.exe`，托盘区出现灯泡图标即表示已启动（默认常亮开启）。
2. 需要临时允许息屏时，左键单击图标或在右键菜单中取消勾选「屏幕常亮」。
3. 需要开机自启时，在右键菜单中勾选「开机启动」，无需管理员权限。
4. 退出请使用右键菜单中的「退出」，不要直接结束进程，以确保正确清理托盘图标和电源请求。

## 开机启动

使用当前用户的注册表项实现，不需要管理员权限：

```text
HKCU\Software\Microsoft\Windows\CurrentVersion\Run
```

键名为 `MiuKeepAwake`，值为带引号的程序完整路径。取消勾选即删除该键值（若本来就不存在则视为成功）。

## 实现原理

通过 Win32 `SetThreadExecutionState` 实现，不修改用户的电源计划：

开启时：

```c
SetThreadExecutionState(
    ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
```

关闭或退出时：

```c
SetThreadExecutionState(ES_CONTINUOUS);
```

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

输出位置：

```text
build/windows-msvc-debug/Debug/MiuKeepAwake.exe
build/windows-msvc-release/Release/MiuKeepAwake.exe
```

## 技术栈

- C17（`CMAKE_C_EXTENSIONS OFF`）
- MSVC（`/W4 /utf-8`）
- CMake
- Win32 API（仅显式 Unicode `*W` API）
- Windows SDK

程序使用一个隐藏的 message-only window 接收托盘通知，因此不会创建普通的任务栏窗口。各模块按职责划分（`app` / `power` / `startup` / `tray` / `window`），详见 [`AGENTS.md`](./AGENTS.md)。

## 托盘图标与本地化

- 图标的视觉方向参考了 Icons8 的 Windows 11 Color 风格 Light On / Light Off 图标，并针对 Windows notification area 的 16×16 / 32×32 尺寸重新绘制，参考来源见 [`resources/icon-source.md`](resources/icon-source.md)。
- 界面字符串存放在 `resources/MiuKeepAwake.rc` 的英文（`LANG_ENGLISH`）和简体中文（`LANG_CHINESE`）两个 `STRINGTABLE` 中。

## 项目结构

```text
MiuKeepAwake/
├── CMakeLists.txt
├── CMakePresets.json
├── AGENTS.md
├── README.md
├── README.en.md
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
    ├── MiuKeepAwake.manifest
    ├── resource.h
    ├── light-on.ico
    ├── light-off.ico
    ├── light-on.svg
    ├── light-off.svg
    └── icon-source.md
```
