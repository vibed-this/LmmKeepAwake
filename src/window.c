#include "miu_keep_awake/window.h"

#include <windows.h>

#include "miu_keep_awake/tray.h"

static const wchar_t WINDOW_CLASS_NAME[] = L"MiuKeepAwakeMessageWindow";

static LRESULT CALLBACK window_proc(
    HWND window,
    UINT message,
    WPARAM wparam,
    LPARAM lparam)
{
    if (message == WM_APP + 1) {
        return tray_handle_message(window, message, wparam, lparam);
    }

    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(window, message, wparam, lparam);
    }
}

HWND app_window_create(HINSTANCE instance)
{
    const WNDCLASSEXW window_class = {
        .cbSize = sizeof(window_class),
        .lpfnWndProc = window_proc,
        .hInstance = instance,
        .lpszClassName = WINDOW_CLASS_NAME,
    };

    if (!RegisterClassExW(&window_class)) {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return NULL;
        }
    }

    return CreateWindowExW(
        0,
        WINDOW_CLASS_NAME,
        L"MiuKeepAwake",
        0,
        0,
        0,
        0,
        0,
        HWND_MESSAGE,
        NULL,
        instance,
        NULL);
}
