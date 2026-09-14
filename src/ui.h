/**
 * PureEngine — Step 21: UI Rendering Boundary
 * File: ui.h
 *
 * The engine's eighth SYSTEM boundary — and deliberately the thinnest.
 * The digit-only HUD (survival timer row, all-time record row) already
 * had its GLYPH mechanics extracted in Phase 4 (pe::Renderer::
 * drawDigitString); Step 21 gives the remaining UI RESPONSIBILITY —
 * which numbers appear, how they are formatted, and where they sit —
 * one named home, without moving a single GL object.
 *
 * What this boundary is (and nothing more):
 *   - formatDecimal1(): the shared fixed-one-decimal formatting both
 *     HUD rows have used since Phase 3;
 *   - the LAYOUT CONSTANTS: the timer row at (-5.75, 4.05), the
 *     record row at (-5.75, 3.2) — screen-space positions, unchanged;
 *   - drawHud(): the two-call sequence, in its established order —
 *     current run first, all-time record second.
 *
 * What it deliberately is NOT (the blueprint's explicit constraint):
 *   - no menus, no general UI framework, no layout system beyond
 *     a plain Button (data + pure helpers, no GL of its own).
 * What it adds — the minimum viable interactive element:
 *   - one plain Button struct: position, dimensions, label;
 *     pure hit-test and draw helpers; the glyph is drawn via
 *     drawTextString — no GL resources owned here.
 * Ownership rule (B5-A ruling): ALL glyph GL stays in pe::Renderer —
 * the text VAO/VBO, the font atlas texture, the shared shader
 * program, the blending toggle, the atlas UV mechanics inside
 * drawDigitString. This boundary owns no GL resources at all; it
 * formats the game's NUMBERS (data) and hands them to the renderer's
 * GLYPH path. The screen-space contract is untouched: projection *
 * model with NO VIEW, so the digits stay fixed to the window while
 * WASD pans the world.
 *
 * The display GATE — the HUD appears in every non-menu state (live in
 * PLAYING, frozen in PAUSED, final on GAME_OVER) — is state MEANING
 * and stays in main.cpp, exactly like the numbers themselves
 * (survivalTime, highScore). main.cpp asks the UI boundary to draw;
 * the UI boundary never decides when.
 *
 * Header-only, same discipline as every project module: no ui.cpp,
 * no CMakeLists.txt change.
 */
#ifndef PUREENGINE_UI_H
#define PUREENGINE_UI_H
// Include guard, same pattern as every other project header.

#include <iomanip>   // std::setprecision — the one-decimal rule
#include <sstream>   // number -> digit string
#include <string>    // what drawDigitString consumes

#include "math/mat4.h"  // the projection handed through to the renderer
#include "renderer.h"   // the glyph path — used, never owned

namespace pe {

// --- HUD layout (Phase 3/4 constants, relocated whole) ---
// Screen-space positions in the balance-tuned ortho box (-6..6,
// -4.5..4.5): both rows left-aligned at x = -5.75, the current run on
// the top row, the all-time record one row below.
inline constexpr float hudTimerX = -5.75f;
inline constexpr float hudTimerY = 4.05f;
inline constexpr float hudBestY = 3.2f;

// --- The shared number format (Phase 3's rule) ---
// Fixed point, exactly one decimal place — the survival timer and the
// record have always been formatted identically, so they share one
// function. (The high-score SAVE FILE uses the same format too, but
// that write stays in main.cpp — persistence is not UI.)
inline std::string formatDecimal1(float value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(1) << value;
    return out.str();
}

// --- The two-row HUD, in its established call order ---
// Current run first, all-time record second — the same order the
// inline block always drew. The projection is passed THROUGH to the
// renderer untouched: the digits are screen-space (no view matrix is
// ever supplied, so camera panning cannot move them), blending and
// tint are handled inside drawDigitString exactly as before.
inline void drawHud(Renderer& renderer, const Mat4& projection,
                    float survivalTime, float highScore) {
    // Step 82: tiny TIME/BEST labels additive, digits keep exact positions
    renderer.drawTextString("TIME", hudTimerX + 3.0f, hudTimerY, projection, TextAlign::Left);
    renderer.drawTextString("BEST", hudTimerX + 3.0f, hudBestY, projection, TextAlign::Left);
    renderer.drawDigitString(formatDecimal1(survivalTime), hudTimerX, hudTimerY, projection);
    renderer.drawDigitString(formatDecimal1(highScore), hudTimerX, hudBestY, projection);
}

// --- Step (next): minimal Button widget ---
// A plain data struct + pure helpers — no GL of its own,
// no new subsystem. drawButton renders the label via
// pe::drawTextString (Step 66, screen space). hitTest uses
// window coordinates (same space as pe::Input::pollMouse /
// mouseX() / mouseY()). Games opt in; the HUD is untouched.
struct Button {
    float x = 0.0f, y = 0.0f;         // screen-space center position
    float width = 0.0f, height = 0.0f; // hit-test dimensions
    const char* label = "";           // text drawn via drawTextString
};

// Pure hit-test: is the cursor inside the button rect?
// Takes coordinates, not a MouseState — directly testable
// without hardware (same gamepad.h pure-helper discipline).
inline bool hitTest(const Button& btn, float mouseX, float mouseY) {
    float halfW = btn.width * 0.5f;
    float halfH = btn.height * 0.5f;
    return mouseX >= btn.x - halfW && mouseX <= btn.x + halfW &&
           mouseY >= btn.y - halfH && mouseY <= btn.y + halfH;
}

// Draw the button label, centered on (x, y), in screen space
// (projection * model, NO VIEW) — same contract as drawHud.
// The GLYPH is owned by pe::Renderer; this boundary owns
// nothing graphical.
inline void drawButton(Renderer& renderer, const Mat4& projection,
                         const Button& btn) {
    renderer.drawTextString(btn.label, btn.x, btn.y, projection, TextAlign::Center);
}

} // namespace pe

#endif // PUREENGINE_UI_H
