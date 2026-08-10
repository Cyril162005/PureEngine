#pragma once

// --- Step 11: Scene/Level Structure ---
// The game-state enum: the engine's first STATE. Pure logic, header-only
// like entity.h and collision.h — no CMake change.
//
// Design ruling (same 'don't over-engineer' principle as Step 7's ECS
// decision): an enum + ONE current-state variable + a single dispatch
// point in main.cpp. A scene-graph or push-down state stack would buy
// dynamic scene nesting and history — neither exists in a project with
// three states and linear transitions. When (if) those needs become
// real, the enum becomes the key into whatever structure replaces it;
// nothing downstream hardcodes the mechanism, only the values.
//
// enum class, not plain enum: scoped names (pe::GameState::MENU, never
// a bare MENU polluting the global namespace) and no implicit int
// conversions — same type-safety standard as the rest of the engine.
namespace pe {

enum class GameState {
    MENU,     // Start screen. No world simulated, nothing drawn but
              // the clear color. SPACE starts a game, ESC quits.
    PLAYING,  // The full Steps 1-10 world: entities, camera, arrows,
              // collision, tint, audio. ESC pauses.
    PAUSED    // The PLAYING world held still: drawn every frame,
              // simulated on none of them. ESC resumes, SPACE quits
              // to the menu.
};

} // namespace pe
