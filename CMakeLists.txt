cmake_minimum_required(VERSION 3.10)
project(PureEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Game Build Phase 6: fully standalone executable ---
# MSVC defaults to /MD (dynamic C runtime): the built exe would depend on
# VCRUNTIME140.dll / MSVCP140.dll, which a CLEAN machine without Visual
# Studio does not have. Everything else was already static (GLFW,
# miniaudio, stb, GLAD), so statically linking the runtime too leaves
# the shipped exe needing only Windows' own system DLLs.
#
# Mechanism, chosen after an honest first attempt failed: the modern
# CMAKE_MSVC_RUNTIME_LIBRARY property did NOT take effect here — the
# fetched dependencies (GLFW, miniaudio) declare their own
# cmake_minimum_required, which resets the CMP0091 policy scope in their
# directories (the generated project files proved they still carried
# /MD). The old-school method below is generator- and scope-proof: it
# rewrites the flag VARIABLES every target inherits, before any
# dependency is configured.
foreach(flagVar
        CMAKE_C_FLAGS                CMAKE_C_FLAGS_DEBUG    CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL     CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS              CMAKE_CXX_FLAGS_DEBUG  CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL   CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if(${flagVar} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flagVar} "${${flagVar}}")
    endif()
endforeach()

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

include(FetchContent)

# GLFW - Windowing and Context creation
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# miniaudio - Audio playback/capture, single-file library.
# Its own CMakeLists builds a static library target named "miniaudio";
# examples/tests/tools are OFF by default. Same FetchContent pattern as GLFW.
FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.25
)

FetchContent_MakeAvailable(miniaudio)

# stb - single-header utility libraries (we use stb_image for PNG loading).
# The repo has NO CMakeLists.txt at its root, so MakeAvailable only
# populates the source; we consume it as headers. Pinned to an exact
# commit because stb publishes no version tags.
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG 2c980bb59875b0d32144a71867fbdebb2f77cd20
)

FetchContent_MakeAvailable(stb)

# ---------------------------------------------------------------------------
# Project Source
# ---------------------------------------------------------------------------

# We use the GLAD loader provided within the GLFW dependency folder
# to keep the build minimal and avoid extra external downloads.
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/stb_impl.cpp
    "${glfw_SOURCE_DIR}/deps/glad_gl.c"
)

target_include_directories(${PROJECT_NAME} PRIVATE "${glfw_SOURCE_DIR}/deps")
target_include_directories(${PROJECT_NAME} PRIVATE "${stb_SOURCE_DIR}")

# Link GLFW and miniaudio
target_link_libraries(${PROJECT_NAME} glfw miniaudio)

# Copy the runtime asset bundle next to the executable so the game can
# still find its data when launched from build/Release/ without relying
# on the repo root working directory. This includes both the parser data
# and the audio assets used at runtime.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/assets/beep.wav"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/beep.wav"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/assets/GAMEOVER.wav"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/GAMEOVER.wav"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/assets/win_sound.wav"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/win_sound.wav"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/assets/hostile_default.txt"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/hostile_default.txt"
)

# Minimal parser validation target: exercises the real hostile-data loading
# logic without changing PureEngine.exe's source, flags, or dependency set.
add_executable(hostile_data_test
    tests/hostile_data_test.cpp
)

# Keep the test translation unit scoped to the exact header it needs.
# This avoids exposing the whole src/ directory, which is what makes
# src/time.h shadow the CRT's time.h during compilation.

# On Linux, we might need extra libs
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} GL X11 lpthread ldl lrt)
endif()
