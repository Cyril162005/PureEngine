/**
 * PureEngine — Step 68: Debug console foundation
 * File: console.h
 *
 * A minimal runtime debug console: toggleable overlay state, key-fed
 * input line, tiny command parser, three built-ins (help/clear/echo),
 * game-extensible commands via registerCommand, and screen-space
 * rendering through the Step-66 text path. Engine-only: no game logic.
 *
 * Typing rides the EXISTING input boundary (input.h untouched): the
 * game edge-detects keys with pe::Input and forwards them here via
 * feedKey(). That keeps this header fully testable without a window —
 * feedKey takes raw key codes plus shift, no GLFW calls — and keeps
 * key MEANING (including the toggle key and when typing is active) in
 * the game loop, where Step 16's ruling says it belongs. No wiring
 * exists in Step 68 (same foundation-then-adopt pattern as Steps
 * 63-67): Arcade and Pong do not include this header yet.
 *
 * Honest engine-only line: commands needing GAME data (the mission's
 * "list entities" example, toggles) cannot be built in — the engine
 * has no game to ask. Games register them with registerCommand at
 * wiring time; the proof tests register a canned "entities" command
 * to prove exactly that mechanism.
 *
 * Rendering: drawConsole draws the last <=10 history lines plus the
 * "> input" line top-down in screen space (HUD column convention from
 * ui.h) via Renderer::drawTextString. NO backdrop panel in Step 68 —
 * text draws over the scene exactly like today's HUD digits. No text
 * wrapping either: overlong lines run off the right, same as the
 * digit path. Both are later steps, not here.
 *
 * Header-only, same discipline as every project module: no console.cpp,
 * no new dependency (GLFW/glad/stb headers were already third-party
 * paths; the test target gains header search paths only).
 */
#ifndef PUREENGINE_CONSOLE_H
#define PUREENGINE_CONSOLE_H

#include <cctype>      // std::tolower for case-insensitive commands
#include <functional>  // ConsoleHandler (stdlib, events.h precedent)
#include <string>      // input line, history, parsing
#include <utility>     // std::pair for the command table
#include <vector>      // history + command table

// Key codes for feedKey (input.h precedent) — with GL headers suppressed:
// this header wants the KEY CODES only, never the GL declarations;
// glad (via renderer.h below) owns those, and including them first
// makes glad hard-error (C1189). The guard keeps the define idempotent
// for translation units that already set it.
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include "math/mat4.h"  // drawConsole projection type
#include "renderer.h"   // drawTextString (B5-A: glyph GL stays in the renderer)

namespace pe {

// A command receives its argument tokens and returns single-line output
// text (multi-line returns are caller error: only the first... no —
// the WHOLE string is pushed as ONE history line, newlines and all, so
// keep handlers to one line; documented, not enforced).
using ConsoleHandler = std::function<std::string(const std::vector<std::string>& args)>;

struct Console {
    bool open = false;
    std::string input;  // line being typed
    std::vector<std::string> lines;  // output history (capped, see below)
    std::vector<std::pair<std::string, ConsoleHandler>> commands;
    std::vector<std::string> submitHistory; // Step 90: submitted lines ring (Up/Down recall)
    std::size_t historyPos = 0; // next recall index (0..size, size = blank)
};

namespace console_detail {
inline constexpr std::size_t MAX_HISTORY = 64;  // history cap: oldest dropped
inline constexpr std::size_t MAX_SHOWN = 10;    // draw cap: last N lines

inline void pushLine(Console& c, const std::string& text) {
    c.lines.push_back(text);
    while (c.lines.size() > MAX_HISTORY) {
        c.lines.erase(c.lines.begin());
    }
}

inline std::string lower(std::string s) {
    for (std::size_t i = 0; i < s.size(); ++i) {
        s[i] = static_cast<char>(
            std::tolower(static_cast<unsigned char>(s[i])));
    }
    return s;
}

inline bool isBuiltinName(const std::string& lowered) {
    return lowered == "help" || lowered == "clear" || lowered == "echo";
}
}  // namespace console_detail

// Flip the overlay. Input and history survive toggling: closing is
// "look away", not "hang up".
inline void toggle(Console& c) { c.open = !c.open; }

// Feed one edge-detected key press into the input line. Accepted:
// A-Z (shift selects case), 0-9 (shift ignored — no shifted punctuation
// in the Step-66 charset), SPACE, '.', '-', BACKSPACE (safe on empty).
// Anything else is ignored. Unconditional by design: the CALLER forwards
// keys only while the console is open (wiring step, not here).
inline void feedKey(Console& c, int glfwKey, bool shift) {
    if (glfwKey >= GLFW_KEY_A && glfwKey <= GLFW_KEY_Z) {
        const char upper =
            static_cast<char>('A' + (glfwKey - GLFW_KEY_A));
        c.input += shift
            ? upper
            : static_cast<char>(upper - 'A' + 'a');
        return;
    }
    if (glfwKey >= GLFW_KEY_0 && glfwKey <= GLFW_KEY_9) {
        c.input += static_cast<char>('0' + (glfwKey - GLFW_KEY_0));
        return;
    }
    switch (glfwKey) {
        case GLFW_KEY_SPACE:
            c.input += ' ';
            break;
        case GLFW_KEY_PERIOD:
            c.input += '.';
            break;
        case GLFW_KEY_MINUS:
            c.input += '-';
            break;
        case GLFW_KEY_BACKSPACE:
            if (!c.input.empty()) {
                c.input.pop_back();
            }
            break;
        case GLFW_KEY_UP:
            if (!c.submitHistory.empty() && c.historyPos > 0) {
                --c.historyPos;
                c.input = c.submitHistory[c.historyPos];
            }
            break;
        case GLFW_KEY_DOWN:
            if (c.historyPos < c.submitHistory.size()) {
                ++c.historyPos;
                if (c.historyPos < c.submitHistory.size()) {
                    c.input = c.submitHistory[c.historyPos];
                } else {
                    c.input.clear();
                }
            }
            break;
        default:
            break;  // not typeable: ignored
    }
}

// Parse and run the input line. Echoes "> line" then the result into
// history, clears the input, returns the result text. Empty/blank input
// is a no-op ("", nothing pushed). Built-ins (checked first, case-
// insensitive): help (one line listing every command), clear (wipes
// history INCLUDING the just-pushed echo — a true clean slate, returns
// ""), echo (joins args with single spaces). Otherwise the registered
// table (case-insensitive); no match -> "unknown command 'x' (try
// help)" with the command token as typed.
inline std::string submit(Console& c) {
    std::vector<std::string> tokens;
    std::string current;
    for (std::size_t i = 0; i < c.input.size(); ++i) {
        if (c.input[i] == ' ') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c.input[i];
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    if (tokens.empty()) {
        return "";
    }
    const std::string rawLine = c.input;
    console_detail::pushLine(c, "> " + c.input);
    c.input.clear();
    // Step 90: remember for Up/Down recall (ring, oldest dropped)
    c.submitHistory.push_back(rawLine);
    while (c.submitHistory.size() > console_detail::MAX_HISTORY) {
        c.submitHistory.erase(c.submitHistory.begin());
    }
    c.historyPos = c.submitHistory.size();

    const std::string name = console_detail::lower(tokens[0]);
    const std::vector<std::string> args(tokens.begin() + 1, tokens.end());

    if (name == "clear") {
        c.lines.clear();
        return "";
    }
    std::string result;
    if (name == "help") {
        result = "commands: help clear echo";
        for (std::size_t i = 0; i < c.commands.size(); ++i) {
            result += " " + c.commands[i].first;
        }
    } else if (name == "echo") {
        for (std::size_t i = 0; i < args.size(); ++i) {
            if (i > 0) {
                result += ' ';
            }
            result += args[i];
        }
    } else {
        bool found = false;
        for (std::size_t i = 0; i < c.commands.size(); ++i) {
            if (console_detail::lower(c.commands[i].first) == name &&
                c.commands[i].second) {
                result = c.commands[i].second(args);
                found = true;
                break;
            }
        }
        if (!found) {
            result = "unknown command '" + tokens[0] + "' (try help)";
        }
    }
    console_detail::pushLine(c, result);
    return result;
}

// Push an output line directly (game-side logging into the console).
// Same 64-line cap as everything else.
inline void print(Console& c, const std::string& text) {
    console_detail::pushLine(c, text);
}

// Register a game-side command. False (nothing stored) on: empty name,
// null handler, duplicate of a registered name, or collision with a
// built-in (built-ins are checked first at submit time, so shadowing
// one would create a dead command — refused instead).
inline bool registerCommand(Console& c, const std::string& name,
                            ConsoleHandler h) {
    if (name.empty() || !h) {
        return false;
    }
    const std::string want = console_detail::lower(name);
    if (console_detail::isBuiltinName(want)) {
        return false;
    }
    for (std::size_t i = 0; i < c.commands.size(); ++i) {
        if (console_detail::lower(c.commands[i].first) == want) {
            return false;
        }
    }
    c.commands.push_back(std::make_pair(name, h));
    return true;
}

// Draw the overlay: last <=10 history lines top-down, then the "> input"
// line. No-op when closed. Screen-space HUD convention (see ui.h);
// long lines are NOT wrapped (documented above).
inline void drawConsole(Renderer& renderer, const Mat4& projection,
                        const Console& c) {
    if (!c.open) {
        return;
    }
    const float x = -5.75f;    // HUD column, same as the timer rows
    const float topY = 3.9f;   // just inside the top edge (box tops at 4.5)
    const float lineH = 0.8f;  // digit glyphs are 0.7 tall: no overlap
    const std::size_t total = c.lines.size();
    const std::size_t shown =
        total < console_detail::MAX_SHOWN ? total : console_detail::MAX_SHOWN;
    const std::size_t first = total - shown;
    for (std::size_t i = 0; i < shown; ++i) {
        renderer.drawTextString(c.lines[first + i], x,
                                topY - static_cast<float>(i) * lineH,
                                projection);
    }
    renderer.drawTextString("> " + c.input, x,
                            topY - static_cast<float>(shown) * lineH,
                            projection);
}

}  // namespace pe

#endif  // PUREENGINE_CONSOLE_H
