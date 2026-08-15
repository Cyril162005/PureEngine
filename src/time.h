#ifndef PUREENGINE_TIME_H
#define PUREENGINE_TIME_H

// =====================================================================
//  PureEngine — Step 17: Time/Timestep Boundary (src/time.h)
// =====================================================================
//  The engine's FIFTH system boundary (after the renderer in Step 13,
//  resource loading in Step 14, the camera in Step 15, and input in
//  Step 16). This file OWNS one thing only: the FRAME-TIME MECHANISM.
//
//    - the PREVIOUS-FRAME TIMESTAMP (the only state in this class);
//    - SEEDING it once before the loop with glfwGetTime();
//    - reading glfwGetTime() ONCE per frame;
//    - subtracting previous from current (the delta, in SECONDS);
//    - ADVANCING the previous timestamp immediately after;
//    - converting the delta to float — Step 11's shared conversion,
//      relocated whole, so every consumer keeps receiving exactly
//      the same type and value it always did.
//
//  What deliberately does NOT live here:
//    - GAMEPLAY TIME MEANING: survivalTime, the difficulty scale,
//      the timer display, the high score, and the PLAYING-only
//      accumulation gate all stay in main.cpp — the mechanism knows
//      what a second IS; the game decides what seconds MEAN;
//    - any fixed timestep, accumulator, delta clamping, minimum or
//      maximum delta, frame limiter, sleep, pause-aware timing, or
//      elapsed-time API — none of those exist in this engine and the
//      blueprint prescribes no fixed timestep;
//    - any dependency on input, camera, entities, or game state.
//
//  THE CRITICAL INVARIANT — sampling position. Step 2 deliberately
//  samples the clock at the VERY TOP of the loop body, BEFORE
//  glfwPollEvents(), so each delta spans the ENTIRE previous frame
//  (events, simulation, rendering, buffer swap). FrameTime must be
//  ticked at that exact position; ticking it anywhere else would
//  silently change what every delta measures — and therefore the
//  speed of every movement system and the survival timer. The
//  arithmetic inside tick() is the relocated Step 2/11 code, in the
//  same order: read, subtract, advance, convert.
//
//  glfwGetTime() is now EXCLUSIVE to this file (the same containment
//  Step 16 gives glfwGetKey to input.h). GLFW clock access is the
//  only GLFW touch here — no windows, no events, no keys.
//
//  Header-only, like every project module: no CMakeLists.txt change.
// =====================================================================

#include <GLFW/glfw3.h>   // glfwGetTime — the monotonic clock source

namespace pe {

class FrameTime {
public:
    // --- Pre-loop seed (the relocated Step 2 setup) ---
    // Call ONCE, before the loop starts, at the exact position where
    // main.cpp used to initialize lastFrameTime. glfwGetTime() is a
    // monotonic clock counting seconds since glfwInit(); seeding here
    // gives the very FIRST frame a valid reference point to subtract
    // from — without it, frame one's delta would be garbage
    // (uninitialized memory), potentially a huge or negative value.
    void start() {
        lastTime = glfwGetTime();
    }

    // --- Per-frame tick (the relocated Step 2/11 calculation) ---
    // Call ONCE per frame at the very top of the loop body — BEFORE
    // glfwPollEvents() — so each delta covers the entire previous
    // frame, exactly as Step 2 established. Order of operations is
    // preserved from the original inline code: read the clock, compute
    // current-minus-previous, advance the stored timestamp so the NEXT
    // frame measures against THIS one, then narrow to float (Step 11's
    // shared conversion). The result is the duration of the last frame
    // in SECONDS; movement systems multiply velocities by it so objects
    // travel the same distance per second at any frame rate.
    float tick() {
        const double current = glfwGetTime();
        const double delta = current - lastTime;
        lastTime = current;
        return static_cast<float>(delta);
    }

private:
    // The previous frame's timestamp — the class's ONLY state.
    // Seeded by start(); advanced by tick(); touched by nothing else.
    double lastTime = 0.0;
};

} // namespace pe

#endif // PUREENGINE_TIME_H
