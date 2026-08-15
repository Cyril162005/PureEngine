# PureEngine — Build Continuation (v2)

## Context
PureEngine Steps 1-12 are complete, verified, and pushed. Steps 13, 14, and 15 are now
also implemented, verified by Cyril, committed, and pushed (see their notes below).
Steps 16-24 below remain **not completed**. They are a blueprint only and make no claims
about implementation, build success, or verification.

The current game already exposes several responsibilities directly through `main.cpp`:
rendering, asset loading, camera state, input polling, timing, entity data/reset, game-state
dispatch, audio triggering, and the Phase 3 UI path. The continuation therefore focuses on
separating existing responsibilities without turning the project into a framework.

## Goal
Continue learning engine architecture from the already-working PureEngine rather than
adding unrelated gameplay features.

## Rule
No step starts until the previous step is verified by Cyril, committed, and pushed.
No abstraction is added merely because engines commonly have one. The current code must
provide a concrete reason for the boundary.

## Steps

### Step 13 — Renderer Module Boundary
**Status:** Completed (verified by Cyril 2026-08-15, committed `fa80a91`, pushed)
**Goal:** Establish a renderer boundary around the existing OpenGL rendering flow.
**Definition of done:** Rendering setup and per-frame draw submission can be called through a renderer boundary instead of being directly organized as one monolithic rendering section in `main.cpp`, while existing Step 12 behavior remains unchanged.
**Notes:**
- Derived from Steps 4-10 and the Phase 3 text renderer.
- Preserve the existing shader, VAO/VBO, texture, transform, and draw behavior.
- Exact class/function design is intentionally deferred until implementation.

**Implementation (verified):**
- Header-only `pe::Renderer` in `src/renderer.h` (513 lines) — consistent with the
  project's header-only discipline, zero CMakeLists.txt change.
- Public API: `init()`, `shutdown()`, `clear(r,g,b)`, `drawWorld(projection, cameraPos,
  entities, colliding)`, `drawDigitString(text, x, y, projection)`. Private: static
  `loadRgbTexture()` and `destroyAll()`, which replaces the four duplicated GL cleanup
  chains and is safe at any partial-init point (deleting GL name 0 is a no-op).
- Owns: shader program, `transform`/`color` uniform locations, world + text VAO/VBOs,
  all five textures (checker legacy, player/scenery/hostile from Phase 5, RGBA font atlas).
- `src/main.cpp` went from 1,675 to 1,082 lines (+115/-656). Window/GLFW, audio,
  entities, state machine, simulation, camera position, projection, timing, and the
  high score all remain in `main.cpp`.
- Known seams deliberately left inside the renderer for later steps: asset loading
  (Step 14), camera/view math inside `drawWorld` (Step 15), UI digit path (Step 21).
- Two disclosed behavioral deltas: the renderer-init failure path now uninitializes
  audio (the old shader failure paths leaked it on fatal exit — unreachable in normal
  play), and blending toggles per `drawDigitString` call instead of once around both
  calls (visually identical).

**Verification evidence:**
- In-place configure (524.0 s) + Release build: zero errors and zero warnings in project
  code; the pre-existing third-party C4244 in miniaudio.h unchanged. Executable
  1,171,456 bytes, fully static, no new DLLs.
- Cyril ran `build\Release\PureEngine.exe` and confirmed the full regression checklist:
  MENU/PLAYING/PAUSED/GAME_OVER states, player movement, camera pan, all three scenery
  entities (spin, collision tint, beep), all three hostiles (chase, difficulty ramp,
  distinct Phase 5 textures), on-screen timer and high-score display, and high-score
  persistence across relaunch.

### Step 14 — Resource Loading Boundary
**Status:** Completed (independently verified by Cyril 2026-08-15, committed `4bf7027`, pushed)
**Goal:** Separate asset loading/ownership from game logic.
**Definition of done:** Texture and other currently loaded render assets can be obtained through a small resource-loading boundary without changing the existing asset files or their visual result.
**Notes:**
- Derived from Step 10 and Phase 3.
- Preserve the existing relative-path probing and stb_image loading behavior.
- No resource cache or generalized asset database is required unless implementation proves it necessary.

**Implementation (verified):**
- Header-only `src/resources.h` (116 lines), the engine's second system boundary —
  zero CMakeLists.txt change, consistent with the project's header-only discipline.
- Two free functions, relocated byte-compatible from `pe::Renderer`: `pe::loadRgbTexture`
  (the Phase 5 RGB pattern) and `pe::loadRgbaTexture` (the Phase 3 font-atlas RGBA
  block). Same 3-candidate CWD path probe, same forced-channel `stbi_load`, same
  CLAMP_TO_EDGE + LINEAR sampling, same RGB/RGBA upload, same 0-on-failure contract.
- Ownership unchanged: the renderer keeps all five texture names and deletes them in
  `destroyAll()`; `renderer.h` now contains zero `stb_image` calls (grep-verified).
- No cache or asset database — the blueprint's constraint honored.
- `src/main.cpp` changes were comments/doc-block only; the renderer API and all call
  sites are untouched. Commit `4bf7027` changed exactly four files: `src/resources.h`
  (new), `src/renderer.h`, `src/main.cpp`, `README.md` (+162/-67).

**Verification evidence:**
- Incremental Release build against the existing configured `build/` (no deletion,
  no reconfigure): zero errors and zero warnings in project code; only `main.cpp`
  recompiled. Executable 1,171,456 bytes — identical size to the verified Step 13
  binary, fully static.
- Cyril independently ran `build\Release\PureEngine.exe` and reported Step 14 PASSED
  (2026-08-15): entity textures visually identical, RGBA digit font rendering intact
  (no solid backing rectangles), collision tint, states, and high-score persistence
  unchanged.
- GitHub checkpoint verified: push `561e446..4bf7027 main -> main`; after the push
  HEAD == origin/main, working tree clean except the pre-existing untracked files
  identified during the checkpoint (game V2 trackers, `nn.md`, throwaway build scripts).

### Step 15 — Camera Module Boundary
**Status:** Completed (verified by Cyril 2026-08-15, committed `fff53c3`, pushed)
**Goal:** Separate camera state and camera math from game logic/render submission.
**Definition of done:** Camera position, movement, and view/projection calculation are handled through a camera boundary, while existing WASD camera movement and world rendering remain visually identical.
**Notes:**
- Step 6 introduced the current orthographic camera behavior.
- Step 11 continued to store camera state directly in the game loop/reset path.
- Existing projection and movement behavior must remain unchanged.

**Implementation (verified):**
- Header-only `src/camera.h` (115 lines), the engine's third system boundary —
  zero CMakeLists.txt change, consistent with the project's header-only discipline.
- `pe::Camera` owns: the position state (starts at the origin), the movement speed
  (3.0 world units/second), `reset()` (back to origin, called by `resetGame()`),
  `move(directionX, directionY, deltaTime)` applying `axis += direction * speed * dt`,
  the view matrix (`Mat4::lookAt` from position, target = position + (0,0,-1),
  up (0,1,0)), and the once-built projection
  (`Mat4::orthographic(-6, 6, -4.5, 4.5, -1, 1)`).
- Architectural facts established by this step: camera STATE, camera MOVEMENT, and
  view/projection CALCULATION all belong to the camera boundary; GLFW input polling
  remains in `main.cpp` (the camera receives direction as data and contains zero GLFW
  code); `renderer.h`'s `drawWorld` now receives the view matrix
  (`drawWorld(projection, view, entities, colliding)`) and performs no camera math.
  The Step 16 input abstraction has NOT started.
- `src/main.cpp` replaced the cameraPos/cameraSpeed/projection locals with one
  `pe::Camera` object and routes WASD through `camera.move()` (polling and the
  PLAYING-only gate stay in main.cpp). The Step 14 resource boundary was untouched.
- Commit `fff53c3` changed exactly four files: `src/camera.h` (new), `src/main.cpp`,
  `src/renderer.h`, `README.md` (+185/-54).

**Verification evidence:**
- Incremental Release build against the existing configured `build/` (no deletion,
  no reconfigure): zero errors/warnings in project code; only `main.cpp` recompiled.
  Executable 1,171,456 bytes — identical size to the verified Step 13/14 binaries.
  Startup smoke test: window opened, process alive past full init (all five textures
  loaded), no crash.
- Cyril's manual runtime verification, all PASS (2026-08-15): startup/menu, WASD
  camera movement, camera stationary while paused, resume behavior, restart/start
  camera position, ESC shutdown, and no observed visual/gameplay regression. The
  observed world-sliding-relative-to-camera effect confirmed as expected
  camera-relative behavior, not a movement regression.
- GitHub checkpoint verified: push `8eddc50..fff53c3 main -> main`; after the push
  HEAD == origin/main; the six pre-existing untracked files remained untouched.

### Step 16 — Input Module Boundary
**Status:** Not started
**Goal:** Separate keyboard polling and edge detection from state-specific game meaning.
**Definition of done:** Keyboard polling and the existing edge-detection behavior can be accessed through an input boundary without changing the meaning of MENU, PLAYING, PAUSED, or GAME_OVER controls.
**Notes:**
- Step 3 established direct `glfwGetKey()` polling and edge detection.
- Step 11 established state-dependent key meaning.
- Do not create a full action-mapping system without a concrete requirement.

### Step 17 — Time/Timestep Boundary
**Status:** Not started
**Goal:** Separate frame-time acquisition from systems that consume delta time.
**Definition of done:** Frame delta time is supplied through a timing boundary while preserving existing deltaTime-driven movement, rotation, survival timer, and difficulty scaling behavior.
**Notes:**
- Step 2 introduced deltaTime.
- Step 12 and Game Phase 2 depend on it.
- No fixed timestep is prescribed by this blueprint.

### Step 18 — Entity Lifecycle Boundary
**Status:** Not started
**Goal:** Make entity creation/reset/removal responsibility explicit without replacing the current entity-vector design.
**Definition of done:** Entity creation/reset/removal responsibilities have an explicit boundary, while the current vector-based entity representation and reset behavior remain correct.
**Notes:**
- Step 7 established `std::vector<pe::Entity>`.
- Step 12 and Game Phase 1 use `push_back` before the initial snapshot.
- Do not introduce a full ECS, registry, or component scheduler unless a concrete need appears.

### Step 19 — Game State Boundary
**Status:** Not started
**Goal:** Separate state transitions and state-specific rules from unrelated engine systems.
**Definition of done:** MENU, PLAYING, PAUSED, and GAME_OVER state transitions and state-specific input/simulation rules are organized behind a game-state boundary without changing their current behavior.
**Notes:**
- Step 11 deliberately used one enum and one current state variable.
- Step 12 added GAME_OVER.
- Preserve the simple state model unless implementation demonstrates a real need for more structure.

### Step 20 — Audio Boundary
**Status:** Not started
**Goal:** Separate game events from direct miniaudio calls.
**Definition of done:** The existing miniaudio engine and four-slot sound pool can be triggered through an audio boundary while preserving collision-edge and GAME_OVER beep behavior.
**Notes:**
- Step 9 introduced miniaudio.
- Step 10 introduced per-entity collision edges and the four-slot pool.
- Preserve current trigger semantics and sound assets.

### Step 21 — UI Rendering Boundary
**Status:** Not started
**Goal:** Separate the existing digit-only UI rendering path from the rest of world rendering.
**Definition of done:** The existing digit-only timer/high-score rendering can be invoked through a UI boundary while preserving screen-space behavior, formatting, placement, blending, and camera independence.
**Notes:**
- Game Phase 3 introduced the digit-only bitmap font.
- Game Phase 4 extracted `drawDigitString()`.
- Do not expand this into a general UI framework.

### Step 22 — World/Game Separation
**Status:** Not started
**Goal:** Establish a clearer boundary between engine-facing systems and franchise-specific gameplay rules.
**Definition of done:** The engine-facing systems and game-specific rules have a clearer boundary, while the current top-down survival loop still behaves identically.
**Notes:**
- Step 12 intentionally built franchise-specific gameplay on top of engine systems.
- Much of the implementation still lives in `main.cpp`.
- This is a separation pass, not a rewrite.

### Step 23 — Engine/Game Integration Pass
**Status:** Not started
**Goal:** Prove the separated systems still work together.
**Definition of done:** The refactored engine boundaries and the current game can be built and run together with all previously verified gameplay behavior preserved.
**Notes:**
- Regression targets include the v1 loop, three hostiles, difficulty scaling, timer, high score persistence, state machine, collision feedback, audio, reset, and camera behavior.
- No new gameplay feature is required.

### Step 24 — Clean Build and Architecture Verification
**Status:** Not started
**Goal:** Finish the continuation with a clean build and explicit regression verification.
**Definition of done:** A fresh clean build succeeds, the current game passes its regression checklist, and the resulting engine/game boundaries are documented without introducing unnecessary systems.
**Notes:**
- This follows the existing project standard of clean rebuilds and direct verification.
- No result is claimed until the step is actually performed.

## Kill Criteria
If any step's scope keeps expanding instead of shrinking, stop, cut scope, and re-record a
smaller definition_of_done before continuing. Do not introduce an abstraction, manager,
registry, or subsystem unless the current implementation demonstrates a concrete need for it.
