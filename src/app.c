#include "miu_keep_awake/app.h"

#include <windows.h>

#include "miu_keep_awake/power.h"
#include "miu_keep_awake/startup.h"
#include "miu_keep_awake/tray.h"
#include "miu_keep_awake/window.h"

int app_run(HINSTANCE instance)
{
    const bool initial_keep_awake = true;
    if (!power_set_keep_awake(initial_keep_awake)) {
        return (int)GetLastError();
    }

    HWND window = app_window_create(instance);
    if (!window) {
        const DWORD error = GetLastError();
        (void)power_set_keep_awake(false);
        return (int)error;
    }

    if (!tray_initialize(window, instance, initial_keep_awake)) {
        const DWORD error = GetLastError();
        DestroyWindow(window);
        (void)power_set_keep_awake(false);
        return error != ERROR_SUCCESS ? (int)error : 1;
    }

    MSG message;
    int result = 0;
    while (GetMessageW(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (message.message == WM_QUIT) {
        result = (int)message.wParam;
    }

    tray_remove();
    (void)power_set_keep_awake(false);
    DestroyWindow(window);
    return result;
}
