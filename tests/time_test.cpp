/**
 * PureEngine - Step 318: the FrameTime contract test (Core Foundation slice 4).
 * Deterministic asserts ONLY: the dt clamp, the scale/pause state machine, and
 * the clamp invariant. The only real-clock use is a forced 150 ms stall, which
 * is load-robust by construction: sleep overshoots, never undershoots, so the
 * raw delta is ALWAYS >= 0.15 s and tick() returns the clamped constant
 * EXACTLY (a longer stall under load changes nothing). glfwInit is required
 * because FrameTime reads glfwGetTime() - legal headless (no window needed).
 * Slice-4 decision recorded in V3: engine_time STAYS header-inline (every
 * body is 1-5 lines; a .cpp would fan out to 5+ consumer targets for no gain;
 * the GLFW include is already guarded NONE since slice 1).
 */
#include <cstdio>
#include <thread>
#include <chrono>
#include "engine_time.h"

static int g_fail = 0;
static void expect(bool ok, const char* name) {
    if (ok) { std::printf("PASS: %s\n", name); }
    else { std::printf("FAIL: %s\n", name); ++g_fail; }
}

int main() {
    expect(glfwInit() == GLFW_TRUE, "time: glfwInit (the FrameTime clock precondition)");

    // --- the normal dt path + the clamp invariant ---
    pe::FrameTime ft;
    ft.start();
    const float dt1 = ft.tick();
    expect(dt1 >= 0.0f && dt1 <= 0.1f, "time: the first tick is within [0, 0.1] (the clamp invariant)");

    // --- THE CLAMP: a forced 150 ms stall -> tick() returns EXACTLY 0.1f ---
    ft.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    const float dt2 = ft.tick();
    expect(dt2 == 0.1f, "time: a 150ms stall clamps tick() to EXACTLY 0.1f (bit-exact)");

    // --- scale: the pure state + the guards ---
    ft.setTimeScale(2.0f);
    expect(ft.getTimeScale() == 2.0f, "time: setTimeScale(2) is stored");
    ft.setTimeScale(-5.0f);
    expect(ft.getTimeScale() == 0.0f, "time: a negative scale clamps to 0 (the setTimeScale guard)");
    ft.setTimeScale(0.0f);
    expect(ft.scaledTick() == 0.0f, "time: scaledTick with scale 0 returns exactly 0");
    ft.setTimeScale(1.0f);

    // --- pause: the paused path ignores the clock entirely ---
    ft.setPaused(true);
    expect(ft.isPaused(), "time: setPaused(true) is stored");
    ft.setTimeScale(2.0f);
    expect(ft.scaledTick() == 0.0f, "time: scaledTick while paused returns EXACTLY 0 regardless of scale");
    ft.setPaused(false);
    expect(!ft.isPaused(), "time: setPaused(false) is stored");

    // --- unpaused scaledTick at scale 2: the clamp times the scale, bit-exact ---
    ft.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    const float st = ft.scaledTick();
    expect(st == 0.2f, "time: a 150ms stall at scale 2 -> scaledTick EXACTLY 0.2f (the clamp x scale)");
    expect(st >= 0.0f && st <= 0.2f, "time: the scaled tick stays within [0, clamp*scale]");

    glfwTerminate();

    if (g_fail == 0) { std::printf("time_test: ALL PASS\n"); }
    else { std::printf("time_test: %d FAIL\n", g_fail); }
    return g_fail == 0 ? 0 : 1;
}
