# MiuKeepAwake

[中文](./README.md)

A small native Windows utility that prevents the display from turning off and the system from sleeping due to inactivity. Keep-awake is enabled by default on launch. It lives in the notification area and does not create a regular taskbar window.

## Features

- Tray icon reflects the current state: lit / unlit bulb.
- Left-click the tray icon to toggle keep-awake.
- Right-click context menu:
  - Keep screen awake (checkmarked, synced with the actual state)
  - Start with Windows (checkmarked)
  - About
  - Exit
- Bilingual UI: English / Simplified Chinese, selected automatically from the system language.
- On `Exit`, the tray icon is removed and the execution-state request is released.

## Usage

1. Run `MiuKeepAwake.exe`. A bulb icon in the tray means it is running (keep-awake is on by default).
2. To allow the screen to sleep temporarily, left-click the icon or uncheck "Keep screen awake" in the context menu.
3. To launch at startup, check "Start with Windows" in the context menu. No administrator privileges required.
4. Always quit via `Exit` in the context menu rather than killing the process, so the tray icon and power request are cleaned up correctly.

## Startup

Startup registration uses the current user's registry key, so no elevation is needed:

```text
HKCU\Software\Microsoft\Windows\CurrentVersion\Run
```

The value name is `MiuKeepAwake` and the value is the quoted full path of the executable. Unchecking the option deletes the value (treating "not present" as success).

## How It Works

Implemented with Win32 `SetThreadExecutionState`. The user's power plan is never modified.

When enabled:

```c
SetThreadExecutionState(
    ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
```

When disabled or exiting:

```c
SetThreadExecutionState(ES_CONTINUOUS);
```

## Build

Requirements:

- Windows
- Visual Studio 2022 / MSVC
- Windows SDK
- CMake 3.25+

Debug:

```powershell
cmake --preset windows-msvc-debug
cmake --build --preset windows-msvc-debug
```

Release:

```powershell
cmake --preset windows-msvc-release
cmake --build --preset windows-msvc-release
```

Output binaries:

```text
build/windows-msvc-debug/Debug/MiuKeepAwake.exe
build/windows-msvc-release/Release/MiuKeepAwake.exe
```

### Installer (Inno Setup)

Prerequisite: [Inno Setup 6](https://jrsoftware.org/isinfo.php).

```powershell
cmake --preset windows-msvc-release
cmake --build --preset windows-msvc-release
powershell -File installer/build-installer.ps1
```

Or use the CMake `installer` target (needs ISCC; the version comes from `project VERSION`):

```powershell
cmake --build --preset windows-msvc-release --target installer
```

Output:

```text
dist/MiuKeepAwake-<version>-x64-Setup.exe (+ .sha256)
```

Installer behavior:

- Per-user install (`PrivilegesRequired=lowest`), no admin rights needed; defaults to `%LOCALAPPDATA%\Programs\MiuKeepAwake`.
- 64-bit Windows only.
- Optional tasks: start with Windows (writes `HKCU\...\Run`, kept in sync with the tray menu checkmark) and a desktop icon.
- A running instance is terminated before install/uninstall; uninstall removes the startup entry and shortcuts.
- The setup wizard supports English / Simplified Chinese (`installer/languages/ChineseSimplified.isl` is a vendored copy of the official translation, so builds never depend on which translations the local Inno Setup happens to ship).

## Tech Stack

- C17 (`CMAKE_C_EXTENSIONS OFF`)
- MSVC (`/W4 /utf-8`)
- CMake
- Win32 API (explicit Unicode `*W` APIs only)
- Windows SDK

The program uses a hidden message-only window to receive tray notifications, so no regular taskbar window is created. Code is organized by responsibility (`app` / `power` / `startup` / `tray` / `window`); see [`AGENTS.md`](./AGENTS.md).

## Tray Icons and Localization

- The icon design follows the visual language of the Icons8 Windows 11 Color style Light On / Light Off icons, redrawn for Windows notification-area sizes (16×16 / 32×32). See [`resources/icon-source.md`](resources/icon-source.md) for the reference.
- UI strings live in the English (`LANG_ENGLISH`) and Simplified Chinese (`LANG_CHINESE`) `STRINGTABLE`s in `resources/MiuKeepAwake.rc`.

## Project Layout

```text
MiuKeepAwake/
├── CMakeLists.txt
├── CMakePresets.json
├── AGENTS.md
├── README.md
├── README.en.md
├── installer/
│   ├── MiuKeepAwake.iss
│   ├── build-installer.ps1
│   └── languages/
│       └── ChineseSimplified.isl
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
