/**
 * PureEngine - Step 319: the shared window/GL bootstrap body (Core
 * Foundation slice 5). See window.h for the contract. glad owns the GL
 * declarations (the console.h:46-49 rule - window.h guards GLFW_INCLUDE_NONE
 * before glfw3.h, so no GL headers arrive from GLFW).
 */
#include "window.h"

#include <glad/gl.h>
#include <cstdio>

namespace pe {

WindowBoot bootWindow(int width, int height, const char* title, const char* logTag) {
    WindowBoot result;
    if (!glfwInit()) {
        std::fprintf(stderr, "%s: glfwInit failed\n", logTag);
        return result;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    GLFWwindow* w = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!w) {
        std::fprintf(stderr, "%s: window failed\n", logTag);
        glfwTerminate();
        return result;
    }
    glfwMakeContextCurrent(w);
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::fprintf(stderr, "%s: glad failed\n", logTag);
        glfwDestroyWindow(w);
        glfwTerminate();
        return result;
    }
    result.window = w;
    result.ok = true;
    return result;
}

} // namespace pe
