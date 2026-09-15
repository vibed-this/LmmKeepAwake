#include "lmm_keep_awake/power.h"

#include <windows.h>

bool power_set_keep_awake(bool enabled)
{
    if (enabled) {
        return SetThreadExecutionState(
            ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED) != 0;
    }

    return SetThreadExecutionState(ES_CONTINUOUS) != 0;
}

