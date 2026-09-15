cmake_minimum_required(VERSION 3.25)

project(LmmKeepAwake VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(LmmKeepAwake WIN32
    src/main.c
    src/app.c
    src/power.c
    src/startup.c
    src/tray.c
    src/window.c
    resources/LmmKeepAwake.rc
)

target_include_directories(LmmKeepAwake PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
)

if(MSVC)
    target_compile_options(LmmKeepAwake PRIVATE /W4 /utf-8)
    target_compile_definitions(LmmKeepAwake PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
else()
    target_compile_options(LmmKeepAwake PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(LmmKeepAwake PROPERTIES
    OUTPUT_NAME "LmmKeepAwake"
    LINK_FLAGS "/MANIFEST:NO"
)

install(TARGETS LmmKeepAwake DESTINATION ".")

# Optional Inno Setup installer target (needs ISCC from Inno Setup 6).
# The project version is passed explicitly so the installer never
# carries a stale hardcoded version.
# "ProgramFiles(x86)" cannot be named in an $ENV{...} reference
# (parentheses are not valid in a CMake variable name),
# so probe the default install locations literally.
# Override with -DLMM_INNO_ISCC=<path-to-ISCC.exe> when needed.
find_program(LMM_INNO_ISCC NAMES ISCC
    PATHS "C:/Program Files (x86)/Inno Setup 6" "C:/Program Files/Inno Setup 6" "$ENV{LOCALAPPDATA}/Programs/Inno Setup 6")
if(LMM_INNO_ISCC)
    add_custom_target(installer
        COMMAND "${LMM_INNO_ISCC}" "/DMyAppVersion=${PROJECT_VERSION}" "${CMAKE_CURRENT_SOURCE_DIR}/installer/LmmKeepAwake.iss"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Building Inno Setup installer with ${LMM_INNO_ISCC}"
        VERBATIM)
else()
    message(STATUS "ISCC not found: 'installer' target unavailable (install Inno Setup 6 to enable it)")
endif()

