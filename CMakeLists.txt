cmake_minimum_required(VERSION 3.25)

project(MiuKeepAwake VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(MiuKeepAwake WIN32
    src/main.c
    src/app.c
    src/power.c
    src/startup.c
    src/tray.c
    src/window.c
    resources/MiuKeepAwake.rc
)

target_include_directories(MiuKeepAwake PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
)

if(MSVC)
    target_compile_options(MiuKeepAwake PRIVATE /W4 /utf-8)
    target_compile_definitions(MiuKeepAwake PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
else()
    target_compile_options(MiuKeepAwake PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(MiuKeepAwake PROPERTIES
    OUTPUT_NAME "MiuKeepAwake"
    LINK_FLAGS "/MANIFEST:NO"
)
