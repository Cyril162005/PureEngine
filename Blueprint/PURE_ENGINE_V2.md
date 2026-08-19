# PureEngine — Build Continuation (v2)

## Context
PureEngine Steps 1-12 are complete, verified, and pushed. Steps 13 through 18 are
implemented, verified by Cyril, committed, and pushed individually (see their notes
below). Batch 1 — Steps 19 through 23 — is implemented, verified by Cyril in one
consolidated manual runtime check, and committed as the single checkpoint `2651778`
(push `37c5b3f..2651778`). Step 24 — Clean Build and Architecture Verification — is
implemented, verified by Cyril on 2026-08-19, committed as checkpoint `fa76f58` on
top of Batch 1's tracker commit `4f8c08c`, and pushed. Steps 13 through 24 are
therefore all complete and verified.

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
**Status:** Completed (verified by Cyril 2026-08-15, committed `7749600`, pushed)
**Goal:** Separate keyboard polling and edge detection from state-specific game meaning.
**Definition of done:** Keyboard polling and the existing edge-detection behavior can be accessed through an input boundary without changing the meaning of MENU, PLAYING, PAUSED, or GAME_OVER controls.
**Notes:**
- Step 3 established direct `glfwGetKey()` polling and edge detection.
- Step 11 established state-dependent key meaning.
- Do not create a full action-mapping system without a concrete requirement.

**Implementation (verified):**
- Header-only `src/input.h` (107 lines), the engine's fourth system boundary —
  zero CMakeLists.txt change, consistent with the project's header-only discipline.
- `pe::Input` owns: keyboard level polling (its `isDown()` is now the ONLY `glfwGetKey`
  call site in the codebase), ESC/SPACE edge detection (`isEdge()`, read-only against
  the snapshot), and the previous-frame snapshot with a single writer (`update()`,
  called once per frame after edge consumption) — one edge per physical press, even
  while held.
- Architectural facts established by this step: keyboard polling and edge state
  belong to the input boundary; state-specific key MEANING (the entire MENU /
  PLAYING / PAUSED / GAME_OVER switch) stays in `main.cpp`; `glfwPollEvents()` and
  the window lifecycle (`glfwWindowShouldClose` / `glfwSetWindowShouldClose`) stay
  in `main.cpp`; raw GLFW key codes cross the boundary — no enum, no action-mapping
  system, no callbacks.
- `src/main.cpp` routes every level read (ESC, SPACE, WASD, arrows) through
  `pe::Input`, constructs `pe::Input input{GLFW_KEY_ESCAPE, GLFW_KEY_SPACE}` before
  the loop, and replaces the old esc/spaceWasPressedLastFrame bookkeeping with
  `input.update(window)` at the exact former position. Temporal order preserved:
  poll events → level + edge reads → state switch → snapshot update.
- Commit `7749600` changed exactly three files: `src/input.h` (new), `src/main.cpp`,
  `README.md` (+190/-45).

**Verification evidence:**
- Release build clean (incremental, no reconfigure): zero errors/warnings in project
  code; only `main.cpp` recompiled. Executable 1,172,480 bytes.
- Architectural greps: `glfwGetKey` exists in code only in `src/input.h`; the edge
  snapshot has a single writer (`update()`); required temporal ordering confirmed by
  line-order inspection (poll → reads → switch → update); no game meaning, deltaTime,
  or camera/player logic in `input.h`.
- Startup smoke test passed: window opened, process alive past full init (all five
  textures loaded), no crash.
- Cyril's manual physical-keyboard verification, all PASS (2026-08-15): SPACE starts
  from MENU (once per press, no repeat while held), ESC pauses/resumes with one edge
  per press, SPACE toggles clear color and navigates PAUSED/GAME_OVER to MENU, WASD
  camera pan and arrow-key player movement unchanged, ESC quits from MENU, no
  regression in timer/high score/textures/collision tint/beeps. Note: the assistant
  performed the startup smoke test only — it cannot provide physical keyboard input;
  all keyboard behavior above was verified by Cyril personally.
- GitHub checkpoint verified: push `24ca3ce..7749600 main -> main`; after the push
  HEAD == origin/main; the six pre-existing untracked files remained untouched.

### Step 17 — Time/Timestep Boundary
**Status:** Completed (verified by Cyril 2026-08-15, committed `d598ffa`, pushed)
**Goal:** Separate frame-time acquisition from systems that consume delta time.
**Definition of done:** Frame delta time is supplied through a timing boundary while preserving existing deltaTime-driven movement, rotation, survival timer, and difficulty scaling behavior.
**Notes:**
- Step 2 introduced deltaTime.
- Step 12 and Game Phase 2 depend on it.
- No fixed timestep is prescribed by this blueprint.

**Implementation (verified):**
- Header-only `src/time.h` (90 lines), the engine's fifth system boundary — zero
  CMakeLists.txt change, consistent with the project's header-only discipline.
- `pe::FrameTime` owns ONLY the frame-time mechanism: the previous-timestamp state,
  `start()` seeding it pre-loop (the old `lastFrameTime` init), `tick()` reading
  `glfwGetTime()` once per frame, computing current-minus-previous, advancing the
  stored timestamp, and returning the delta through Step 11's shared float
  conversion (relocated inside the boundary, so every consumer keeps the same type
  and value).
- Architectural facts established by this step: frame-time ACQUISITION belongs to
  the time boundary (`glfwGetTime()` is now exclusive to `src/time.h`, the same
  containment Step 16 gives `glfwGetKey` to `input.h`); gameplay time MEANING stays
  in `main.cpp` — `survivalTime`, the difficulty scale, the timer display, the high
  score, and the PLAYING-only simulation gate. The critical invariant is preserved:
  `tick()` is called at the SAME top-of-loop position the old code sampled — before
  `glfwPollEvents()` — so each delta still spans the entire previous frame.
- No fixed timestep, no accumulator, no delta clamping, no frame limiter, no
  pause-aware timing, no elapsed-time API — none were prescribed and none were added.
- Commit `d598ffa` changed exactly three files: `src/time.h` (new), `src/main.cpp`,
  `README.md` (+147/-29).

**Verification evidence:**
- Release build passed: incremental, no reconfigure, zero errors/warnings in project
  code; only `main.cpp` recompiled. Executable 1,172,480 bytes — identical size to
  the verified Step 16 binary.
- Startup smoke test passed: window opened, process alive past full init, no crash.
- Static checks: `glfwGetTime` exists in code only in `src/time.h`; the old
  `lastFrameTime`/`currentFrameTime`/`deltaTime` locals are gone from `main.cpp`;
  temporal ordering confirmed by line-order inspection (seed pre-loop, tick at loop
  top, then event polling, input reads, state switch); all four deltaTime consumers
  (camera movement, entity rotation, survival timer, hostile chase × difficulty
  scale) intact and receiving the same float values.
- Cyril's manual runtime verification, all PASS (2026-08-15): timer counts only
  during PLAYING, freezes during PAUSED, resets correctly on a new game, and stops
  at GAME_OVER; WASD camera movement, arrow-key player movement, entity rotation,
  and hostile chase speed behaviorally unchanged; survival-time difficulty scaling
  remains functional; first-frame timing showed no visible spike (no timer,
  movement, hostile displacement, or rotation jump); high-score persistence across
  a relaunch passed; regression sweep (textures, collision, collision tint,
  collision audio, state transitions, timer UI, rendering) passed. Note: the
  assistant performed build, static checks, and the startup smoke test only — it
  cannot provide physical keyboard input; all runtime behavior above was verified
  by Cyril personally.
- The time boundary preserves the existing deltaTime behavior end-to-end; no fixed
  timestep was introduced.
- GitHub checkpoint verified: push `2ea29a5..d598ffa main -> main`; after the push
  HEAD == origin/main; the six pre-existing untracked files remained untouched.

### Step 18 — Entity Lifecycle Boundary
**Status:** Completed (verified by Cyril 2026-08-15, committed `aa9fae7`, pushed)
**Goal:** Make entity creation/reset/removal responsibility explicit without replacing the current entity-vector design.
**Definition of done:** Entity creation/reset/removal responsibilities have an explicit boundary, while the current vector-based entity representation and reset behavior remain correct.
**Notes:**
- Step 7 established `std::vector<pe::Entity>`.
- Step 12 and Game Phase 1 use `push_back` before the initial snapshot.
- Do not introduce a full ECS, registry, or component scheduler unless a concrete need appears.

**Implementation (verified):**
- Header-only `src/lifecycle.h` (151 lines), the engine's sixth system boundary — zero
  CMakeLists.txt change, consistent with the project's header-only discipline. Free
  functions in namespace `pe` (the `resources.h` precedent), no class, no container
  ownership.
- `pe::buildInitialEntities()` carries the six entity constructions relocated whole
  from `main.cpp` — identical order and values, comments included. Order is contract:
  0 = player, 1-2 = scenery, 3+ = hostiles (chased at `hostileSpeeds[h - 3]`,
  caught by the `h >= 3` loop, textured by index).
- `pe::resetEntities(entities, snapshot)` carries the snapshot-restoration
  assignment — the engine's ONLY "removal": it never erases or spawns an entity at
  runtime, so no destruction or spawning machinery was invented.
- `pe::flagsForCount(count)` carries the per-entity collision-flag sizing expression
  that previously appeared inline at construction and inside `resetGame()`.
- Architectural facts established by this step: lifecycle OPERATIONS belong to the
  boundary; the COLLECTIONS (`entities`, `initialEntities`, `colliding`) and all game
  MEANING stay in `main.cpp` — `resetGame()` remains ONE atomic lambda at one call
  site (now delegating only the entity restore and flag sizing), and `hostileSpeeds[]`,
  the chase loop, the catch test, and texture selection are untouched. No ECS, no
  registry, no component scheduler, no manager.
- Commit `aa9fae7` changed exactly three files: `src/lifecycle.h` (new),
  `src/main.cpp`, `README.md` (+201/-65).

**Verification evidence:**
- Release build passed: incremental, no reconfigure, zero errors/warnings; executable
  1,172,480 bytes — identical size to the Cyril-verified Step 16/17 binaries.
- Startup smoke test passed: window opened, process alive past full init, no crash.
- Static checks: the six construction values grep-verified byte-identical to the
  pre-Step-18 committed baseline in identical order; `push_back` removed from
  `main.cpp` code; the `entities = initialEntities` assignment exists only inside
  `pe::resetEntities`; `resetGame()` confirmed one atomic block at one call site;
  `h = 3` / `h - 3` / `entities[0]` index conventions intact; renderer receives the
  same vector in the same order through the unchanged `drawWorld` contract; all
  other source headers and CMake untouched (empty diff).
- Cyril's manual runtime verification, all PASS (2026-08-15): all six entities at
  their original positions with player = index 0, scenery = 1-2, hostiles = 3-5;
  entity rotation, hostile chase speeds, difficulty ramp, collision tint, collision
  beeps, and catch -> GAME_OVER (sound, console output, high-score behavior)
  unchanged; restart via MENU restores all entities to original positions/rotations
  with camera home, timer 0.0, and no surviving state from the previous run;
  pause freezes the scene with correct collision state/tint and resume continues
  normally; ESC quits, digits and entity textures unchanged, no visual corruption;
  high score survives a complete relaunch. Note: the assistant performed the build,
  static checks, and the startup smoke test only — it cannot provide physical
  keyboard input; all runtime behavior above was verified by Cyril personally.
- The vector-based entity representation and reset behavior remain exactly as
  before; no ECS or lifecycle machinery was introduced.
- GitHub checkpoint verified: push `e171535..aa9fae7 main -> main`; after the push
  HEAD == origin/main; the six pre-existing untracked files remained untouched.

### Step 19 — Game State Boundary
**Status:** Completed (verified by Cyril 2026-08-15, committed in Batch 1 checkpoint `2651778`, pushed)
**Goal:** Separate state transitions and state-specific rules from unrelated engine systems.
**Definition of done:** MENU, PLAYING, PAUSED, and GAME_OVER state transitions and state-specific input/simulation rules are organized behind a game-state boundary without changing their current behavior.
**Notes:**
- Step 11 deliberately used one enum and one current state variable.
- Step 12 added GAME_OVER.
- Preserve the simple state model unless implementation demonstrates a real need for more structure.

**Implementation (verified):**
- The existing `src/gamestate.h` grew from 42 to 97 lines and became the game-state
  boundary (rulings B1/B2: no new `state.h`; the dispatch switch stays in
  `main.cpp`). Zero CMakeLists.txt change.
- Three pure constexpr helpers join the enum: `simulates()` (only PLAYING),
  `drawsWorld()` (everything except MENU), and `clearColorFor(state, blueToggled)`
  returning a small `ClearColor` struct that carries Step 11's palette relocated
  whole — MENU dark purple (0.16, 0, 0.24), GAME_OVER dark red (0.28, 0, 0),
  otherwise gameplay black or Step 3's dark blue (0, 0, 0.25).
- `main.cpp` rewires its simulation gate, draw gate, and clear-color chain through
  the helpers. The four-case dispatch switch and every TRANSITION stay in
  `main.cpp` — `resetGame()` before the MENU→PLAYING flip, the window close flag,
  and the toggle flag itself, which stays owned by `main.cpp` and is merely handed
  in. No state stack, no transition tables, no callbacks.

**Verification evidence:**
- Per-step automated gate: clean Release build, zero errors/warnings in project
  code; static checks confirmed the helpers are used in `main.cpp` (six call
  sites), all four switch cases intact, and `gamestate.h` free of
  GLFW/miniaudio/IO references.
- Cyril's manual runtime verification PASSED (2026-08-15), performed as the single
  consolidated Batch 1 checklist covering Steps 19-23 together: all state
  transitions, simulation/draw gating, and per-state clear colors behaved exactly
  as before. Note: the assistant performed builds, static checks, and startup
  smoke tests only — it cannot provide physical keyboard input; all runtime
  behavior was verified by Cyril personally.
- GitHub checkpoint: Batch 1 committed as `2651778` on top of `37c5b3f`; push
  `37c5b3f..2651778 main -> main` verified; the six pre-existing untracked files
  remained untouched.

### Step 20 — Audio Boundary
**Status:** Completed (verified by Cyril 2026-08-15, committed in Batch 1 checkpoint `2651778`, pushed)
**Goal:** Separate game events from direct miniaudio calls.
**Definition of done:** The existing miniaudio engine and four-slot sound pool can be triggered through an audio boundary while preserving collision-edge and GAME_OVER beep behavior.
**Notes:**
- Step 9 introduced miniaudio.
- Step 10 introduced per-entity collision edges and the four-slot pool.
- Preserve current trigger semantics and sound assets.

**Implementation (verified):**
- New header-only `src/audio.h` (8,132 bytes), the engine's seventh system boundary
  (rulings B3/B4). Zero CMakeLists.txt change.
- `pe::Audio` owns the MECHANISM: `ma_engine` init, the 3-candidate `assets/beep.wav`
  probe, the four-slot pool clone, the ONE shared round-robin cursor,
  rewind-if-busy playback via `playNext()`, and slots-then-engine `shutdown()`.
  `init()` returns `bool`; the fatal-exit policy stays in `main.cpp` (same policy
  as `renderer.init()`).
- `main.cpp` keeps the DECISIONS: the collision-edge test and the catch decide when
  a beep happens, and the teardown ordering relative to window/GLFW stays there.
  Trigger semantics preserved exactly — the collision beep and the GAME_OVER beep
  advance the SAME cursor, rotating through the same four slots as the single
  `nextCollisionSound` variable always did.

**Verification evidence:**
- Per-step automated gate: clean Release build, zero errors/warnings in project
  code; static checks confirmed zero `ma_*` calls and zero old identifiers
  (`collisionSounds`/`nextCollisionSound`/`audioEngine`) remain in `main.cpp`
  code, and `miniaudio.h` is included only by `src/audio.h`.
- Cyril's manual runtime verification PASSED (2026-08-15) as the single
  consolidated Batch 1 checklist: collision beeps rotate through the pool without
  cutting a ringing beep, and the GAME_OVER beep shares the same rotating cursor.
  Note: the assistant performed builds, static checks, and startup smoke tests
  only — it cannot provide physical keyboard input; all runtime behavior was
  verified by Cyril personally.
- GitHub checkpoint: part of Batch 1 commit `2651778`; push `37c5b3f..2651778`
  verified.

### Step 21 — UI Rendering Boundary
**Status:** Completed (verified by Cyril 2026-08-15, committed in Batch 1 checkpoint `2651778`, pushed)
**Goal:** Separate the existing digit-only UI rendering path from the rest of world rendering.
**Definition of done:** The existing digit-only timer/high-score rendering can be invoked through a UI boundary while preserving screen-space behavior, formatting, placement, blending, and camera independence.
**Notes:**
- Game Phase 3 introduced the digit-only bitmap font.
- Game Phase 4 extracted `drawDigitString()`.
- Do not expand this into a general UI framework.

**Implementation (verified):**
- New header-only `src/ui.h` (4,132 bytes), deliberately the thinnest boundary
  (ruling B5-A). Zero CMakeLists.txt change.
- The boundary owns: `formatDecimal1()` (the shared fixed-one-decimal rule), the
  layout constants (timer row at (-5.75, 4.05), record row at (-5.75, 3.2)), and
  `drawHud()` — the two-call sequence in its established order, current run first,
  all-time record second.
- ALL glyph GL stays in `pe::Renderer`: `ui.h` owns no GL objects at all; it
  formats DATA and hands it to the renderer's glyph path (`drawDigitString`
  unchanged). `main.cpp`'s inline `ostringstream` formatting and the two
  `drawDigitString` literals became one `pe::drawHud` call. The display gate (HUD
  in every non-menu state) and the high-score save file's own formatting stay in
  `main.cpp` — persistence is not UI. No letters, no menus, no widgets, no layout
  system.

**Verification evidence:**
- Per-step automated gate: clean Release build, zero errors/warnings in project
  code; executable 1,172,480 bytes — identical size to the Cyril-verified
  Step 16-18 binaries; static checks confirmed no `ostringstream` remains in
  `main.cpp` code, the layout literals are comment-only there, and `ui.h` contains
  no GL calls (its only renderer reference is the use-don't-own `renderer.h`
  include).
- Cyril's manual runtime verification PASSED (2026-08-15) as the single
  consolidated Batch 1 checklist: timer counts at one decimal on the top row,
  record row one row below, HUD frozen during PAUSED and final on GAME_OVER,
  screen-space under camera panning. Note: the assistant performed builds, static
  checks, and startup smoke tests only — it cannot provide physical keyboard
  input; all runtime behavior was verified by Cyril personally.
- GitHub checkpoint: part of Batch 1 commit `2651778`; push `37c5b3f..2651778`
  verified.

### Step 22 — World/Game Separation
**Status:** Completed (verified by Cyril 2026-08-15, committed in Batch 1 checkpoint `2651778`, pushed)
**Goal:** Establish a clearer boundary between engine-facing systems and franchise-specific gameplay rules.
**Definition of done:** The engine-facing systems and game-specific rules have a clearer boundary, while the current top-down survival loop still behaves identically.
**Notes:**
- Step 12 intentionally built franchise-specific gameplay on top of engine systems.
- Much of the implementation still lives in `main.cpp`.
- This is a separation pass, not a rewrite.

**Implementation (verified):**
- New header-only `src/simulation.h` (5,549 bytes): three pure free functions
  relocated byte-equivalent from their Steps 7/12/8 loops (ruling B6 + correction).
  Zero CMakeLists.txt change.
- `advanceRotations()` — the per-entity rotation loop. `chasePlayer()` — the
  hostile pursuit loop, zero-length guard included, speeds handed in as the raw
  `const float[3]` the caller declares. `scanSceneryCollisions()` — the unique-pair
  scan over the ORIGINAL THREE, both flags set on overlap; the bound stays the
  literal 3 so hostiles keep passing through scenery.
- `main.cpp` retains the complete frame-order chain — timer → rotation →
  difficulty → chase → collision rebuild → collision scan → edge detection →
  catch — and every gameplay side effect on it: timer accumulation, difficulty
  calculation, the colliding-vector rebuild from zero, edge detection, audio
  triggering, the catch decision, the state flip, the high-score write. No
  scheduler, no pipeline, no `run()`/`tick()`/`step()` — three helpers that take
  data, move it, return nothing.
- One disclosed defect found and fixed during implementation: a duplicated
  `colliding.assign` line introduced by the edit was removed before build
  (idempotent but misleading).

**Verification evidence:**
- Per-step automated gate: clean Release build, zero errors/warnings in project
  code; executable 1,172,992 bytes. Strict before/after ordering proof: the
  chain's eight stages verified strictly ascending by line number inside the
  PLAYING branch (992, 999, 1007, 1021, 1031, 1040, 1051, 1081 at commit time);
  exactly one `colliding.assign` in code; zero inline loops left in `main.cpp`;
  `simulation.h` free of GLFW/miniaudio/IO/state-machine references.
- Cyril's manual runtime verification PASSED (2026-08-15) as the single
  consolidated Batch 1 checklist: entity spins, hostile chase with difficulty
  ramp, scenery-only collision tint/beeps, catch → GAME_OVER, restart, and
  persistence all unchanged. Note: the assistant performed builds, static checks,
  and startup smoke tests only — it cannot provide physical keyboard input; all
  runtime behavior was verified by Cyril personally.
- GitHub checkpoint: part of Batch 1 commit `2651778`; push `37c5b3f..2651778`
  verified.

### Step 23 — Engine/Game Integration Pass
**Status:** Completed (verified by Cyril 2026-08-15, committed in Batch 1 checkpoint `2651778`, pushed)
**Goal:** Prove the separated systems still work together.
**Definition of done:** The refactored engine boundaries and the current game can be built and run together with all previously verified gameplay behavior preserved.
**Notes:**
- Regression targets include the v1 loop, three hostiles, difficulty scaling, timer, high score persistence, state machine, collision feedback, audio, reset, and camera behavior.
- No new gameplay feature is required.

**Implementation (verified):**
- Verification-and-documentation-only step (ruling B7): zero code and zero
  architecture change. The containment sweep across all nine boundaries, the
  startup smoke test, and the README boundary-list alignment were performed
  against the post-Step-22 source; no documentation change was required beyond
  the per-step README lines already added in Steps 19-22.

**Verification evidence:**
- Automated integration evidence: containment greps re-run across the whole tree —
  `glfwGetKey` exclusive to `src/input.h`, `glfwGetTime` exclusive to
  `src/time.h`, `ma_*` exclusive to `src/audio.h`, `stbi_*` exclusive to
  `src/resources.h` (+ `stb_impl.cpp`), zero GL calls in any non-renderer
  boundary header; every glfw/gl reference in `main.cpp` is a comment or a
  window-lifecycle call `main.cpp` legitimately owns. Startup smoke test passed
  (window opened, alive past 6 s, clean stop). README lists all nine boundaries.
- Cyril's manual runtime verification PASSED (2026-08-15) as the single
  consolidated Batch 1 checklist covering every regression target: menu/start,
  movement and camera, chase with difficulty ramp, scenery collision tint/beeps,
  pause/resume, catch/GAME_OVER with the shared audio cursor, restart, and
  high-score persistence across relaunch. Note: the assistant performed builds,
  static checks, and startup smoke tests only — it cannot provide physical
  keyboard input; all runtime behavior was verified by Cyril personally.
- GitHub checkpoint: part of Batch 1 commit `2651778`; push `37c5b3f..2651778`
  verified.

### Step 24 — Clean Build and Architecture Verification
**Status:** Completed (verified by Cyril 2026-08-19, committed `fa76f58`, pushed)
**Goal:** Finish the continuation with a clean build and explicit regression verification.
**Definition of done:** A fresh clean build succeeds, the current game passes its regression checklist, and the resulting engine/game boundaries are documented without introducing unnecessary systems.
**Notes:**
- This follows the existing project standard of clean rebuilds and direct verification.
- No result was claimed until the step was actually performed; the results below were
  recorded only after the clean build, the architecture checks, and Cyril's manual
  regression run were completed.

**Implementation (verified):**
- Verification-and-documentation-alignment step: the fresh clean build, the regression
  run, and the architecture containment checks were performed against the post-Step-23
  source; the ONLY code-tree change was documentation alignment.
- Commit `fa76f58` changed exactly two files, comments only (+14/-11): `src/main.cpp`
  (the Step 13 doc block's seam list updated to record every seam the continuation
  steps split out — Steps 14/15/16/17/18/21 — instead of describing future work) and
  `src/renderer.h` (the boundary doc block and the `drawDigitString` comment updated
  to record that Step 21 gave the UI path its `src/ui.h` boundary, glyph GL staying
  in the renderer). Zero code-behavior change, zero CMakeLists.txt change, no new
  system, no new abstraction.

**Verification evidence:**
- Automated — genuinely clean Release build: fresh clean build with zero errors and
  zero warnings in project code; executable 1,172,992 bytes — identical size to the
  Cyril-verified Step 22 binary — fully static, committed as checkpoint `fa76f58`.
- Automated — architecture verification: 16/16 containment checks passed with
  code-vs-comment separation (`glfwGetKey` exclusive to `src/input.h`, `glfwGetTime`
  exclusive to `src/time.h`, `ma_*` exclusive to `src/audio.h`, `stbi_*` exclusive to
  `src/resources.h` (+ `stb_impl.cpp`), zero GL calls in any non-renderer boundary
  header; every remaining glfw/gl reference in `main.cpp` a comment or a
  window-lifecycle call it legitimately owns). Startup smoke test passed (window
  opened, full init, no crash).
- Cyril's manual runtime verification, all PASS (2026-08-19): the full 19-item
  regression checklist covering menu/start, all four states, player movement and
  camera pan, chase with difficulty ramp, scenery collision tint/beeps, pause/resume,
  catch/GAME_OVER with the shared audio cursor, restart, timer/high-score display,
  and high-score persistence across relaunch. Note: the assistant performed the
  clean build, the architecture checks, and the startup smoke test only — it cannot
  provide physical keyboard input; all keyboard and gameplay behavior was verified
  by Cyril personally.
- GitHub checkpoint verified: commit `fa76f58` pushed on top of Batch 1's tracker
  commit `4f8c08c`; after the push HEAD == origin/main; the pre-existing untracked
  files remained untouched.

## Kill Criteria
If any step's scope keeps expanding instead of shrinking, stop, cut scope, and re-record a
smaller definition_of_done before continuing. Do not introduce an abstraction, manager,
registry, or subsystem unless the current implementation demonstrates a concrete need for it.
