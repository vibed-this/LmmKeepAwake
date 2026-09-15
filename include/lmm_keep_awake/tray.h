#ifndef lmm_keep_awake_TRAY_H
#define lmm_keep_awake_TRAY_H

#include <stdbool.h>
#include <windows.h>

bool tray_initialize(HWND window, HINSTANCE instance, bool keep_awake);
void tray_set_keep_awake(bool keep_awake);
void tray_show_startup_notification(void);
void tray_remove(void);
LRESULT tray_handle_message(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

#endif

