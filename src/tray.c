#include "miu_keep_awake/tray.h"

#include <shellapi.h>
#include <windows.h>

#include "miu_keep_awake/power.h"
#include "miu_keep_awake/startup.h"
#include "resource.h"

#define WM_APP_TRAY (WM_APP + 1)
#define IDM_KEEP_AWAKE 1001
#define IDM_STARTUP 1002
#define IDM_ABOUT 1003
#define IDM_EXIT 1004

static NOTIFYICONDATAW tray_icon;
static HICON icon_on;
static HICON icon_off;
static bool keep_awake;

static void tray_update_icon(void)
{
    tray_icon.hIcon = keep_awake ? icon_on : icon_off;
    Shell_NotifyIconW(NIM_MODIFY, &tray_icon);
}

static HMENU create_context_menu(void)
{
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return NULL;
    }

    AppendMenuW(menu, MF_STRING, IDM_KEEP_AWAKE, L"屏幕常亮");
    AppendMenuW(menu, MF_STRING, IDM_STARTUP, L"开机启动");
    AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(menu, MF_STRING, IDM_ABOUT, L"关于");
    AppendMenuW(menu, MF_STRING, IDM_EXIT, L"退出");

    CheckMenuItem(menu, IDM_KEEP_AWAKE,
        MF_BYCOMMAND | (keep_awake ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(menu, IDM_STARTUP,
        MF_BYCOMMAND | (startup_is_enabled() ? MF_CHECKED : MF_UNCHECKED));

    return menu;
}

static void show_context_menu(HWND window)
{
    HMENU menu = create_context_menu();
    if (!menu) {
        return;
    }

    POINT point;
    if (!GetCursorPos(&point)) {
        DestroyMenu(menu);
        return;
    }

    SetForegroundWindow(window);

    const UINT command = TrackPopupMenu(
        menu,
        TPM_RIGHTBUTTON | TPM_RETURNCMD,
        point.x,
        point.y,
        0,
        window,
        NULL);

    DestroyMenu(menu);
    PostMessageW(window, WM_NULL, 0, 0);

    switch (command) {
    case IDM_KEEP_AWAKE: {
        const bool new_state = !keep_awake;
        if (power_set_keep_awake(new_state)) {
            tray_set_keep_awake(new_state);
        }
        break;
    }

    case IDM_STARTUP:
        (void)startup_set_enabled(!startup_is_enabled());
        break;

    case IDM_ABOUT:
        MessageBoxW(
            window,
            L"MiuKeepAwake 0.1.0\n\n保持 Windows 屏幕常亮的小型原生工具。",
            L"关于 MiuKeepAwake",
            MB_OK | MB_ICONINFORMATION);
        break;

    case IDM_EXIT:
        PostQuitMessage(0);
        break;

    default:
        break;
    }
}

void tray_show_startup_notification(void)
{
    wcscpy_s(tray_icon.szInfoTitle, ARRAYSIZE(tray_icon.szInfoTitle), L"MiuKeepAwake");
    wcscpy_s(tray_icon.szInfo, ARRAYSIZE(tray_icon.szInfo), L"屏幕常亮已开启");
    tray_icon.dwInfoFlags = NIIF_INFO;
    tray_icon.uFlags = NIF_INFO;
    (void)Shell_NotifyIconW(NIM_MODIFY, &tray_icon);

    tray_icon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
}

bool tray_initialize(HWND window, HINSTANCE instance, bool initial_keep_awake)
{
    icon_on = LoadIconW(instance, MAKEINTRESOURCEW(IDI_APP_ON));
    icon_off = LoadIconW(instance, MAKEINTRESOURCEW(IDI_APP_OFF));
    if (!icon_on || !icon_off) {
        return false;
    }

    keep_awake = initial_keep_awake;
    ZeroMemory(&tray_icon, sizeof(tray_icon));
    tray_icon.cbSize = sizeof(tray_icon);
    tray_icon.hWnd = window;
    tray_icon.uID = 1;
    tray_icon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tray_icon.uCallbackMessage = WM_APP_TRAY;
    tray_icon.hIcon = keep_awake ? icon_on : icon_off;
    wcscpy_s(tray_icon.szTip, ARRAYSIZE(tray_icon.szTip), L"MiuKeepAwake");

    if (!Shell_NotifyIconW(NIM_ADD, &tray_icon)) {
        return false;
    }

    /* Keep the classic callback contract: lParam contains the mouse event. */
    return true;
}

void tray_set_keep_awake(bool enabled)
{
    keep_awake = enabled;
    tray_update_icon();
}

void tray_remove(void)
{
    if (tray_icon.hWnd != NULL) {
        Shell_NotifyIconW(NIM_DELETE, &tray_icon);
        ZeroMemory(&tray_icon, sizeof(tray_icon));
    }

    if (icon_on) {
        DestroyIcon(icon_on);
        icon_on = NULL;
    }
    if (icon_off) {
        DestroyIcon(icon_off);
        icon_off = NULL;
    }
}

LRESULT tray_handle_message(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    (void)wparam;

    if (message != WM_APP_TRAY) {
        return 0;
    }

    switch ((UINT)lparam) {
    case WM_LBUTTONUP:
        if (power_set_keep_awake(!keep_awake)) {
            tray_set_keep_awake(!keep_awake);
        }
        return 0;

    case WM_RBUTTONUP:
        show_context_menu(window);
        return 0;

    default:
        return 0;
    }
}
