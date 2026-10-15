/**
 * PureEngine - Step 319: the shared window/GL bootstrap (Core Foundation
 * slice 5). File: window.h
 *
 * What this owns: the ONE sequence every game repeated (main.cpp, pong,
 * platformer) - glfwInit, the 3.3 Core window hints (the Step 171 order, the
 * APPLE forward-compat hint included), glfwCreateWindow, context
 * current-ness, gladLoadGL. On any failure everything created so far is
 * cleaned up INSIDE (window destroyed + glfwTerminate) and ok=false comes
 * back with a "<logTag>: <step> failed" line on stderr (the one deliberate
 * wording unification of the three sites' messages - the exit codes stay
 * caller-side).
 *
 * What this deliberately is NOT:
 *  - not a replacement for WindowGuard (window_guard.h): the SUCCESS path
 *    hands the window AND the live GLFW session to the caller - the caller
 *    still owns the teardown order (the WindowGuard contract, the
 *    renderer.destroyAll()-before-guard-destruction rule, unchanged);
 *  - not an init orchestrator: nothing after glad load (renderer, audio,
 *    input) moves here;
 *  - no viewport logic: each game keeps its own defaults as today.
 *
 * Paired .h/.cpp - the first Core Foundation boundary of the migration to
 * split (the callers of bootWindow link src/window.cpp: PureEngine, Pong,
 * Platformer, hostile_data_test). GLFW_INCLUDE_NONE discipline kept.
 */
#ifndef PUREENGINE_WINDOW_H
#define PUREENGINE_WINDOW_H

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace pe {

struct WindowBoot {
    GLFWwindow* window = nullptr;  // non-null iff ok
    bool ok = false;
};

WindowBoot bootWindow(int width, int height, const char* title, const char* logTag);

} // namespace pe

#endif // PUREENGINE_WINDOW_H
