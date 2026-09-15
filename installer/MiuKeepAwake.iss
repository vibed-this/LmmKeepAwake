; MiuKeepAwake Inno Setup script (requires Inno Setup 6, Unicode).
;
; Build the Release exe first, then compile this script:
;
;   cmake --preset windows-msvc-release
;   cmake --build --preset windows-msvc-release
;   powershell -File installer/build-installer.ps1
;
; Or via the CMake "installer" target (needs ISCC on PATH / default location):
;
;   cmake --build --preset windows-msvc-release --target installer
;
; Version source of truth:
;   - automated builds pass /DMyAppVersion=<PROJECT_VERSION> explicitly;
;   - local builds without that define fall back to the VERSIONINFO
;     embedded in the Release exe, so the installer never carries a
;     stale hardcoded version.
; Keep CMakeLists.txt (project VERSION), resources/MiuKeepAwake.rc
; (VERSIONINFO), resources/MiuKeepAwake.manifest and this script in sync.

#define MyAppName "MiuKeepAwake"
#define MyAppExeName "MiuKeepAwake.exe"
#define MyAppPublisher "vibed-this"
#define MyAppURL "https://github.com/vibed-this/LmmKeepAwake"
#define RepoRoot SourcePath + "/.."
#define MySourceExe RepoRoot + "/build/windows-msvc-release/Release/MiuKeepAwake.exe"
#ifndef MyAppVersion
  #define MyAppVersion GetVersionNumbersString(MySourceExe)
#endif
#define MyAppId "{{C5293542-D79A-4D84-BF7C-24C7CC9F7F42}}"

[Setup]
AppId={#MyAppId}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}/releases
AppCopyright=(c) vibed-this
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
; Per-user install: matches the app's HKCU Run startup toggle,
; which must keep working without administrator privileges.
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=6.1sp1
WizardStyle=modern
Compression=lzma2/max
SolidCompression=yes
CloseApplications=yes
RestartApplications=no
UninstallDisplayIcon={app}\{#MyAppExeName}
SetupIconFile={#RepoRoot}/resources/light-on.ico
OutputDir={#RepoRoot}/dist
OutputBaseFilename=MiuKeepAwake-{#MyAppVersion}-x64-Setup
VersionInfoVersion={#MyAppVersion}
VersionInfoProductName={#MyAppName}
VersionInfoDescription={#MyAppName} Setup
VersionInfoCopyright=(c) vibed-this

; English ships with the compiler (Default.isl). Chinese Simplified is
; vendored under installer/languages so builds do not depend on which
; translations the local Inno Setup install happens to include.
; Upstream: https://github.com/jrsoftware/issrc/tree/main/Files/Languages
; (Inno Setup 6.5.0+ messages, maintained by Zhenghan Yang / Kira).
[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinesesimplified"; MessagesFile: "{#RepoRoot}/installer/languages/ChineseSimplified.isl"

[CustomMessages]
english.StartupTask=Start with Windows
english.StartupTaskTip=Launch MiuKeepAwake automatically when you sign in (current user only, no admin rights needed)
english.DesktopIconTask=Create a &desktop icon
english.LaunchProgram=Launch {#MyAppName}
chinesesimplified.StartupTask=开机启动
chinesesimplified.StartupTaskTip=登录后自动启动 MiuKeepAwake（仅当前用户，无需管理员权限）
chinesesimplified.DesktopIconTask=创建桌面图标(&D)
chinesesimplified.LaunchProgram=启动 {#MyAppName}

[Tasks]
Name: "startup"; Description: "{cm:StartupTask}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "desktopicon"; Description: "{cm:DesktopIconTask}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#MySourceExe}"; DestDir: "{app}"; Flags: ignoreversion

[Registry]
; Same HKCU Run value the app itself manages, so the tray menu checkmark
; stays in sync. uninsdeletevalue removes it on uninstall.
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "MiuKeepAwake"; ValueData: """{app}\{#MyAppExeName}"""; Tasks: startup; Flags: uninsdeletevalue

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram}"; Flags: nowait postinstall skipifsilent

[Code]
{ The tray app owns no top-level window, so the Restart Manager cannot
  reliably close it. Kill the running instance before (un)installing so
  the exe can be overwritten/removed. A non-zero taskkill exit code only
  means "no such process", so it is intentionally ignored. }
procedure KillRunningApp();
var
  ResultCode: Integer;
begin
  Exec('taskkill.exe', '/F /IM {#MyAppExeName}', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssInstall then
    KillRunningApp();
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
    KillRunningApp();
end;
