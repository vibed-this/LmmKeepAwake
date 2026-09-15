#include <windows.h>

#include "lmm_keep_awake/app.h"

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE previous_instance,
    PWSTR command_line,
    int show_command)
{
    (void)previous_instance;
    (void)command_line;
    (void)show_command;

    return app_run(instance);
}
