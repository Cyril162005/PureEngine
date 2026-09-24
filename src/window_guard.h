/**
 * PureEngine — Step 190: ordered init/shutdown failure-path guard
 * File: window_guard.h
 *
 * The engine-init order (Step 171 contract, documented from the real
 * call sites) repeats ONE failure pattern in every game: on any failed
 * step after the window exists, the early return must destroy the
 * window AND terminate GLFW. Before this header that was copy-pasted
 * per failure path (Arcade main.cpp carried four copies).
 *
 * What this header is (and nothing more):
 *   - one tiny RAII guard that owns the GLFWwindow* for the duration
 *     of the INIT SECTION only: on destruction (any early return) it
 *     destroys the window and terminates GLFW — a forgotten cleanup
 *     still happens (the same teardown insurance as pe::Audio's
 *     destructor, Step 9/20);
 *   - release() for the SUCCESS path: the game keeps the window and
 *     the guard becomes a no-op. Nulling before destroying keeps a
 *     release()+destructor sequence safe.
 *
 * What it deliberately is NOT:
 *   - not a replacement for the call-site teardown order and not a
 *     forced adoption: games keep their existing paths unchanged —
 *     this is opt-in, additive, engine-pure;
 *   - not a full init orchestrator (no Engine class): the init order
 *     and each subsystem's init stay exactly where Step 171/174 put
 *     them;
 *   - no GL calls here at all: glfwDestroyWindow/glfwTerminate only.
 *     A null-window guard is a no-op with zero GLFW calls, so the
 *     headless test needs no context for that case.
 *
 * Call-site contract for GL (documented, not testable headless): the
 * guard covers the window's lifetime; the GL context dies with
 * glfwDestroyWindow, so renderer.destroyAll() must run BEFORE the
 * guard's destruction on any success-path exit that later tears down.
 *
 * Header-only, like every project module: no window_guard.cpp, no
 * CMakeLists.txt change (the test includes headers by relative path).
 */
#ifndef PUREENGINE_WINDOW_GUARD_H
#define PUREENGINE_WINDOW_GUARD_H

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>  // glfwDestroyWindow / glfwTerminate only

namespace pe {

struct WindowGuard {
    GLFWwindow* window = nullptr;

    explicit WindowGuard(GLFWwindow* w) : window(w) {}

    // Success path: the caller keeps the window; the destructor below
    // becomes a no-op. Idempotent — calling twice is safe.
    void release() { window = nullptr; }

    // Failure path (or scope exit without release): destroy the window
    // and terminate GLFW. Nulling first keeps a release()+destructor
    // sequence safe; a never-released guard cleans up on its own.
    ~WindowGuard() {
        if (window) {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
        window = nullptr;
    }

    WindowGuard(const WindowGuard&) = delete;
    WindowGuard& operator=(const WindowGuard&) = delete;
};

} // namespace pe

#endif // PUREENGINE_WINDOW_GUARD_H
