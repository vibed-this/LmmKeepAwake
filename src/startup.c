#include "lmm_keep_awake/startup.h"

#include <stdio.h>
#include <wchar.h>
#include <windows.h>

static const wchar_t RUN_KEY[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const wchar_t VALUE_NAME[] = L"LmmKeepAwake";

bool startup_is_enabled(void)
{
    HKEY key = NULL;
    wchar_t value[32768];
    DWORD size = sizeof(value);
    DWORD type = 0;

    if (RegOpenKeyExW(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        return false;
    }

    const LONG result = RegQueryValueExW(
        key,
        VALUE_NAME,
        NULL,
        &type,
        (LPBYTE)value,
        &size);

    RegCloseKey(key);
    return result == ERROR_SUCCESS && (type == REG_SZ || type == REG_EXPAND_SZ);
}

bool startup_set_enabled(bool enabled)
{
    HKEY key = NULL;
    const LONG open_result = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        RUN_KEY,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        NULL,
        &key,
        NULL);

    if (open_result != ERROR_SUCCESS) {
        return false;
    }

    LONG result;
    if (!enabled) {
        result = RegDeleteValueW(key, VALUE_NAME);
        if (result == ERROR_FILE_NOT_FOUND) {
            result = ERROR_SUCCESS;
        }
    } else {
        wchar_t path[32768];
        const DWORD length = GetModuleFileNameW(NULL, path, ARRAYSIZE(path));
        if (length == 0 || length >= ARRAYSIZE(path)) {
            RegCloseKey(key);
            return false;
        }

        wchar_t command[32768];
        const int written = swprintf_s(command, ARRAYSIZE(command), L"\"%ls\"", path);
        if (written < 0) {
            RegCloseKey(key);
            return false;
        }

        const DWORD bytes = (DWORD)((wcslen(command) + 1) * sizeof(wchar_t));
        result = RegSetValueExW(
            key,
            VALUE_NAME,
            0,
            REG_SZ,
            (const BYTE *)command,
            bytes);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

