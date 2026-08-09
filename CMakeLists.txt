cmake_minimum_required(VERSION 3.10)
project(PureEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# On Linux, we might need extra libs
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} GL X11 lpthread ldl lrt)
endif()
