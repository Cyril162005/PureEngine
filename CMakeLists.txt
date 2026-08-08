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

# ---------------------------------------------------------------------------
# Project Source
# ---------------------------------------------------------------------------

# We use the GLAD loader provided within the GLFW dependency folder
# to keep the build minimal and avoid extra external downloads.
add_executable(${PROJECT_NAME} 
    src/main.cpp
    "${glfw_SOURCE_DIR}/deps/glad_gl.c"
)

target_include_directories(${PROJECT_NAME} PRIVATE "${glfw_SOURCE_DIR}/deps")

# Link GLFW
target_link_libraries(${PROJECT_NAME} glfw)

# On Linux, we might need extra libs
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} GL X11 lpthread ldl lrt)
endif()
