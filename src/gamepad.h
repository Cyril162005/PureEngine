/**
 * PureEngine — Step 69: Gamepad input foundation
 * File: gamepad.h
 *
 * Snapshot polling for standard-layout gamepads (buttons A/B/X/Y,
 * Start, shoulders, d-pad; both sticks; deadzone), on the pinned GLFW
 * 3.3.x joystick/gamepad API — the keyboard path's own dependency, so
 * NOTHING new is introduced. input.h is completely untouched: the
 * keyboard path is byte-identical, and games adopt gamepads if/when
 * they choose (no wiring in Step 69 — same foundation-then-adopt
 * pattern as Steps 63-68, so Arcade and Pong are unaffected by
 * construction).
 *
 * Style (input.h's C2 ruling, reused): raw GLFW codes flow straight
 * through — buttons are named by GLFW_GAMEPAD_BUTTON_*, axes are
 * passed raw except for the deadzone. There is NO engine button enum
 * and NO remapping. The GLFW Y-up-negative stick convention is
 * documented as-is; the GAME interprets it.
 *
 * Ownership/lifecycle: GLFW init/terminate stays the game's job (Step
 * 16 ruling). pollGamepad checks every return and never crashes:
 * absent joystick, missing mapping, out-of-range id, or uninitialized
 * GLFW all yield connected=false with zeroed state. No force
 * feedback / rumble in Step 69 — not here.
 *
 * Tap semantics WITHOUT a stateful class: the game holds two
 * snapshots and asks gamepadButtonEdge(prev, curr, button) — the same
 * expressive power as Input's owned-snapshot edges, but pure and
 * directly testable without hardware.
 *
 * Header-only, same discipline as every project module: no gamepad.cpp,
 * no CMakeLists.txt change, no new dependency.
 */
#ifndef PUREENGINE_GAMEPAD_H
#define PUREENGINE_GAMEPAD_H

// Key/button codes only semantics (console.h precedent): suppress the GL
// headers GLFW would otherwise pull — glad owns those. Idempotent for
// translation units that already set it.
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>  // joystick/gamepad API (existing dependency)

namespace pe {

// One poll of one gamepad. buttons[] is indexed by
// GLFW_GAMEPAD_BUTTON_* (A=0 .. LAST=14: fifteen slots). Sticks arrive
// already deadzoned by pollGamepad (see applyDeadzone).
struct GamepadState {
    bool connected = false;
    bool buttons[15] = {};  // GLFW_GAMEPAD_BUTTON_* indices
    float leftX = 0.0f, leftY = 0.0f;
    float rightX = 0.0f, rightY = 0.0f;
};

// Kill stick drift: inside (-dz, +dz) reads as exactly 0, everything
// else passes through untouched (the boundary |v| == dz passes: it is
// a deliberate deflection, not drift). Negative deadzone behaves as 0.
inline float applyDeadzone(float value, float deadzone) {
    const float dz = deadzone < 0.0f ? 0.0f : deadzone;
    if (value > -dz && value < dz) {
        return 0.0f;
    }
    return value;
}

// Snapshot joystick id (default: stick 1) with deadzone applied to all
// four axes. Anything that is not a readable standard-layout pad —
// out-of-range id, absent joystick, present-but-unmapped device,
// uninitialized GLFW — yields connected=false with everything
// zeroed/false. Never crashes.
inline GamepadState pollGamepad(int joystick = GLFW_JOYSTICK_1,
                                float deadzone = 0.2f) {
    GamepadState state;
    if (joystick < GLFW_JOYSTICK_1 || joystick > GLFW_JOYSTICK_LAST) {
        return state;
    }
    if (!glfwJoystickPresent(joystick)) {
        return state;
    }
    GLFWgamepadstate raw;
    if (!glfwGetGamepadState(joystick, &raw)) {
        return state;  // present but no standard-layout mapping
    }
    state.connected = true;
    for (int b = 0; b <= GLFW_GAMEPAD_BUTTON_LAST; ++b) {
        state.buttons[b] = (raw.buttons[b] == GLFW_PRESS);
    }
    state.leftX = applyDeadzone(raw.axes[GLFW_GAMEPAD_AXIS_LEFT_X], deadzone);
    state.leftY = applyDeadzone(raw.axes[GLFW_GAMEPAD_AXIS_LEFT_Y], deadzone);
    state.rightX =
        applyDeadzone(raw.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], deadzone);
    state.rightY =
        applyDeadzone(raw.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y], deadzone);
    return state;
}

// Level read of one button in a snapshot. Pure bounds-checked array
// read (no connected gate): pollGamepad already zeroes absent pads, so
// the gate would only punish hand-built/test states. Out-of-range
// button (including negatives): false.
inline bool gamepadButton(const GamepadState& state, int button) {
    if (button < 0 || button > GLFW_GAMEPAD_BUTTON_LAST) {
        return false;
    }
    return state.buttons[button];
}

// Edge read across two snapshots: pressed NOW and NOT pressed in prev.
// The game holds prev itself (poll each frame, compare, rotate).
// Out-of-range button: false.
inline bool gamepadButtonEdge(const GamepadState& prev,
                              const GamepadState& curr, int button) {
    return gamepadButton(curr, button) && !gamepadButton(prev, button);
}

}  // namespace pe

#endif  // PUREENGINE_GAMEPAD_H
