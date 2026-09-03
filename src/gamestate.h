#pragma once

// --- Step 11: Scene/Level Structure ---
// The game-state enum: the engine's first STATE. Pure logic, header-only
// like entity.h and collision.h — no CMake change.
//
// Design ruling (same 'don't over-engineer' principle as Step 7's ECS
// decision): an enum + ONE current-state variable + a single dispatch
// point in main.cpp. A scene-graph or push-down state stack would buy
// dynamic scene nesting and history — neither exists in a project with
// four states and linear transitions. When (if) those needs become
// real, the enum becomes the key into whatever structure replaces it;
// nothing downstream hardcodes the mechanism, only the values.
//
// Step 12 addition: GAME_OVER joined the enum rather than PAUSED being
// overloaded — "held, ESC resumes" and "run is dead, ESC is meaningless"
// are opposite meanings, and Step 11's whole point is that the STATE
// decides what a key means. One new enumerator keeps every transition
// a one-line assignment with no hidden sub-flags.
//
// enum class, not plain enum: scoped names (pe::GameState::MENU, never
// a bare MENU polluting the global namespace) and no implicit int
// conversions — same type-safety standard as the rest of the engine.
namespace pe {

enum class GameState {
    MENU,     // Start screen. No world simulated, nothing drawn but
              // the clear color. SPACE starts a game, ESC quits.
    PLAYING,  // The default scene: entities, camera, arrows,
              // collision, tint, audio. ESC pauses.
    PLAYING_ALT, // The alternate scene using the same gameplay path.
    PAUSED,   // The PLAYING world held still: drawn every frame,
              // simulated on none of them. ESC resumes, SPACE quits
              // to the menu.
    GAME_OVER, // Step 12: the run ended — the hostile caught the player.
              // Scene frozen on screen exactly like PAUSED (drawn, not
              // simulated), dark-red clear color, survival time printed
              // to the console. ESC does NOTHING (there is nothing to
              // resume); SPACE returns to the menu.
    WIN       // Step 39: the run reached its configured win time.
              // Scene frozen on screen like GAME_OVER, with a distinct
              // victory clear color; SPACE returns to the menu.
};

// --- Step 19: pure state predicates and lookups ---
// The game-state BOUNDARY addition: stateless facts about a state.
// The dispatch SWITCH and every TRANSITION stay in main.cpp (Step 16's
// ruling, reaffirmed by Step 19): transitions carry side effects that
// belong there — resetGame() before the MENU->PLAYING flip, the window
// close flag, the clear-color toggle — and the boundary must never
// absorb them. These helpers replace the scattered currentState
// comparisons with named questions, so the rules about a state live in
// one place while the consequences of them stay where they always were.
// constexpr and side-effect-free: asking costs nothing and can happen
// anywhere, any number of times.

// Does this state SIMULATE the world this frame? Only PLAYING does —
// PAUSED, GAME_OVER, and WIN hold the world still (drawn, not simulated),
// MENU has no world at all. This is the gate the survival timer, the
// entity updates, the chase, the collision scan, and the catch test
// all sit behind.
constexpr bool simulates(GameState state) {
    return state == GameState::PLAYING || state == GameState::PLAYING_ALT;
}

// Does this state DRAW the world this frame? Everything except MENU —
// PLAYING, PAUSED, GAME_OVER, and WIN share the exact draw path; only MENU
// shows nothing but the clear color.
constexpr bool drawsWorld(GameState state) {
    return state != GameState::MENU;
}

// The per-state clear color (Step 11's palette, relocated whole from
// main.cpp's if-chain — same values, same priority order):
//   MENU      -> dark PURPLE (0.16, 0, 0.24): deliberately outside
//                gameplay's black/dark-blue palette, so the menu can
//                never be mistaken for a paused or toggled game screen;
//   GAME_OVER -> dark RED (0.28, 0, 0): the engine's established danger
//                channel (collision tint), so a loss reads at a glance;
//   otherwise -> gameplay black, or dark blue (0, 0, 0.25) when Step 3's
//                toggle flag is set. The toggle flag is GAMEPLAY STATE —
//                it stays owned by main.cpp and is merely HANDED IN.
struct ClearColor {
    float r, g, b;
};

constexpr ClearColor clearColorFor(GameState state, bool blueToggled) {
    if (state == GameState::MENU) {
        return ClearColor{0.16f, 0.0f, 0.24f};
    }
    if (state == GameState::GAME_OVER) {
        return ClearColor{0.28f, 0.0f, 0.0f};
    }
    if (state == GameState::WIN) {
        return ClearColor{0.0f, 0.28f, 0.08f};
    }
    if (blueToggled) {
        return ClearColor{0.0f, 0.0f, 0.25f};
    }
    return ClearColor{0.0f, 0.0f, 0.0f};
}

} // namespace pe
