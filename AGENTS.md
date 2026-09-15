# AGENTS.md

## Project

LmmKeepAwake is a Windows-only native desktop utility written in C17.

## Toolchain

- MSVC / Visual Studio 2022
- Windows SDK
- CMake 3.25+
- Win32 API

Use `CMakePresets.json` for normal builds.

## Architecture

Keep the project Windows-native. Do not introduce a cross-platform `platform/` abstraction unless the product requirements actually become cross-platform.

Organize code by responsibility:

- `src/app.c`: application lifecycle and message loop.
- `src/power.c`: keep-awake implementation through `SetThreadExecutionState`.
- `src/startup.c`: per-user startup registration.
- `src/tray.c`: notification-area icon and context menu behavior.
- `src/window.c`: hidden message-only Win32 window.
- `src/main.c`: process entry point only.

Keep Win32-specific code in the modules that own the corresponding responsibility rather than adding an artificial platform layer.

## C conventions

- C17, with `CMAKE_C_EXTENSIONS OFF`.
- Prefer explicit Unicode Win32 APIs (`*W`).
- Do not use the ANSI (`*A`) Win32 APIs.
- Keep ownership and lifetime explicit.
- Use a single cleanup path with `goto cleanup` when a function owns multiple resources.
- Check Win32 API return values.
- Preserve the original Win32 error where it is useful to callers.
- Avoid compiler-specific extensions unless there is a strong Windows-specific reason.

## UI / tray behavior

- The program is tray-first and must not create a normal taskbar window.
- The message-only window exists only to receive Win32 messages and tray callbacks.
- Left-clicking the tray icon toggles keep-awake.
- The context menu must keep its checkmarks synchronized with the actual state.
- `Exit` must remove the tray icon and release the execution-state request.

## Power behavior

When enabled, keep-awake uses:

```c
SetThreadExecutionState(
    ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
```

When disabled or exiting, clear the request with:

```c
SetThreadExecutionState(ES_CONTINUOUS);
```

Do not modify the user's Windows power plan for this feature.

## Startup behavior

Use the current user's:

```text
HKCU\Software\Microsoft\Windows\CurrentVersion\Run
```

Do not require administrator privileges for the startup toggle.

## Resources

Tray icons should remain small and readable at notification-area sizes. The current bulb artwork is custom-rendered with Windows 11-style visual references; see `resources/icon-source.md` before replacing it.

## Validation

After source changes:

```powershell
cmake --preset windows-msvc-debug
cmake --build --preset windows-msvc-debug
```

Warnings should be treated seriously. Do not hide warnings merely to make a build pass.

## Packaging

The Inno Setup installer (`installer/LmmKeepAwake.iss`) is per-user (`PrivilegesRequired=lowest`, x64 only) to match the no-admin HKCU startup toggle. Version source of truth: `project VERSION` in `CMakeLists.txt`, passed as `/DMyAppVersion` (local fallback: the Release exe's VERSIONINFO).

```powershell
powershell -File installer/build-installer.ps1
# or: cmake --build --preset windows-msvc-release --target installer
```

