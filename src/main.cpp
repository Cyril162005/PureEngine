#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>   // Step 7: entities live in a std::vector
#include <algorithm> // Phase 2: std::min clamps the difficulty scale
#include <iomanip>   // Phase 3: fixed one-decimal-place formatting
#include <fstream>   // Phase 4: high-score save file — the engine's FIRST disk write
#include <filesystem> // Phase 4: create savedata/ before saving, without ever throwing

// --- Step 5: The Math Layer ---
// Our OWN math code (src/math/), not an external library. Header-only:
// all logic lives in these two files, so the CMake build is unchanged.
#include "math/vec3.h"
#include "math/mat4.h"

// --- Step 7: The Object System ---
// Entities as data: one struct per game object, stored in a vector.
// Header-only like the math layer, so the CMake build stays unchanged.
#include "entity.h"

// --- Step 8: Collision Detection (AABB) ---
// The overlap ALGORITHM lives apart from the entity DATA (same
// data/system split as Step 7, at file level). Header-only: no
// CMakeLists.txt change.
#include "collision.h"

// --- Step 11 / Step 19: Scene/Level Structure ---
// The game-state enum: pure logic, header-only like entity.h and
// collision.h — no CMakeLists.txt change. Step 19 grew the same
// header into the game-state BOUNDARY: pure constexpr predicates and
// lookups (simulates, drawsWorld, clearColorFor) join the enum.
// The DISPATCH that uses them still lives in main.cpp, because it
// needs every piece of per-frame state (input flags, entities,
// camera, audio) in one place — and every transition carries side
// effects (resetGame, window close, clear-color toggle) that belong
// here. The boundary answers QUESTIONS about a state; main.cpp keeps
// the CONSEQUENCES.
#include "gamestate.h"

// --- Step 13: Renderer Module Boundary ---
// The engine's first SYSTEM boundary. Everything GPU-side that Steps
// 4-10 and Phases 3/5 grew directly inside this file — shader compile
// and link, uniform lookups, the triangle and text VAO/VBO pairs, all
// five textures, the entity draw loop, and the digit UI path — now
// lives in pe::Renderer (src/renderer.h). main.cpp OWNS the window,
// audio, entities, game state, camera position, projection, and the
// high score — frame timing moved to pe::FrameTime (src/time.h) in
// Step 17; it asks the renderer to initialize once and to
// submit frames. Header-only like the math layer and entity.h — no
// CMakeLists.txt change. Every seam the continuation steps split
// out of it is now its own boundary: texture loading by Step 14
// (src/resources.h), camera state/math by Step 15 (src/camera.h),
// keyboard polling/edge detection by Step 16 (src/input.h),
// frame-time acquisition by Step 17 (src/time.h), entity lifecycle
// operations by Step 18 (src/lifecycle.h), and the UI digit path by
// Step 21 (src/ui.h — glyph GL stays here, the numbers are formatted
// there).
#include "renderer.h"

// --- Step 15: Camera Module Boundary ---
// Camera STATE (position), the movement SPEED, the reset-to-origin,
// the movement application, and BOTH matrices (the lookAt view and
// the balance-tuned orthographic projection) moved out of this file
// and out of the renderer into pe::Camera (src/camera.h). main.cpp
// still OWNS the PLAYING-only movement gate and hands the camera
// plain direction data — the KEY POLLING itself moved to the input
// boundary in Step 16 (src/input.h). Header-only like every project
// module: no CMakeLists.txt change, and camera.h never touches GLFW.
#include "camera.h"

// --- Step 16: Input Module Boundary ---
// Every glfwGetKey LEVEL read (ESC, SPACE, WASD, arrows) and the
// ESC/SPACE EDGE detection — including the previous-frame snapshot
// that edges require — moved out of this file into pe::Input
// (src/input.h). What STAYS here: glfwPollEvents() and the window
// lifecycle (shouldClose/setWindowShouldClose), and the ENTIRE state
// switch — the meaning of each key in each state. The boundary hands
// booleans in; main.cpp decides. Header-only: no CMakeLists.txt
// change. Raw GLFW key codes cross the boundary — no enum, no
// action mapping.
#include "input.h"

// --- Step 17: Time/Timestep Boundary ---
// Frame-time ACQUISITION moved out of this file into pe::FrameTime
// (src/time.h): the previous-timestamp state, the pre-loop seed, the
// once-per-frame glfwGetTime() read, current-minus-previous, the
// timestamp advance, and the float conversion. What STAYS here: what
// the seconds MEAN — survivalTime, the difficulty scale, the timer
// display, the high score, and the PLAYING-only simulation gate.
// The boundary is ticked at the SAME position the old code sampled
// (top of the loop body, before glfwPollEvents), so every delta
// still spans the entire previous frame. Header-only: no
// CMakeLists.txt change. glfwGetTime() is now exclusive to time.h.
#include "time.h"

// --- Step 18: Entity Lifecycle Boundary ---
// Entity CREATION and RESET-RESTORATION moved out of this file into
// free functions in pe (src/lifecycle.h): buildInitialEntities()
// carries the six constructions in their exact order and values,
// resetEntities() carries the snapshot assignment, and
// flagsForCount() carries the collision-flag sizing expression.
// What STAYS here: the collections themselves (entities,
// initialEntities, colliding), resetGame()'s one-block atomicity and
// its call policy, and everything entities MEAN — player/scenery/
// hostile roles, the chase, the catch test, texture-by-index. No
// manager, no container ownership, no spawning or destruction
// machinery: the engine has never added or removed an entity at
// runtime, and restoration IS its removal semantics. Header-only:
// no CMakeLists.txt change.
#include "lifecycle.h"

// --- Step 20: Audio Boundary ---
// Every miniaudio MECHANISM moved out of this file into pe::Audio
// (src/audio.h): the engine init, the 3-candidate beep.wav probe,
// the four-slot pool clone, the ONE shared round-robin cursor, the
// rewind-if-busy playback, and the slots-then-engine teardown.
// What STAYS here: the DECISIONS — when a beep happens (the
// collision-edge test, the catch), that a failed init is fatal (same
// policy as renderer.init()), and the teardown ordering relative to
// window/GLFW. Same asset, same trigger semantics: the collision
// beep and the GAME_OVER beep still rotate through the same four
// slots via one cursor. Header-only: no CMakeLists.txt change.
#include "audio.h"

// --- Step 21: UI Rendering Boundary ---
// The thinnest boundary in the engine (B5-A ruling). What moved
// here: the one-decimal number formatting shared by both HUD rows,
// the two layout constants (timer row -5.75, 4.05; record row
// -5.75, 3.2), and the two-call draw sequence in its established
// order — current run first, all-time record second. What STAYS
// here: ALL glyph GL (text VAO/VBO, font atlas, shader program,
// blending) stays in pe::Renderer; the DISPLAY GATE (the HUD shows
// in every non-menu state) stays in this file; and the numbers
// themselves (survivalTime, highScore) plus the high-score SAVE
// FILE's own formatting stay here too — persistence is not UI.
// No letters, no menus, no widgets, no layout system.
// Header-only: no CMakeLists.txt change.
#include "ui.h"

// --- Step 22: World/Game Separation (simulation mechanics) ---
// Three pure MECHANICS moved out of this file into free functions in
// src/simulation.h: the rotation update (advanceRotations), the
// hostile chase (chasePlayer), and the scenery collision scan
// (scanSceneryCollisions). What STAYS here: the complete frame-order
// chain — timer -> rotation -> difficulty -> chase -> collision
// rebuild -> collision scan -> edge detection -> catch — and every
// gameplay side effect on it: the timer accumulation, the difficulty
// calculation, the colliding-vector rebuild from zero, the edge
// detection, the audio trigger, the catch decision, the state flip,
// the high-score write. The helpers take data, move it, return
// nothing — no scheduler, no pipeline, no run() wearing an
// architectural hat. Header-only: no CMakeLists.txt change.
#include "simulation.h"

// --- Step 9: Audio Playback ---
// miniaudio — a single-file audio library fetched by CMake via
// FetchContent (same pattern as GLFW). Unlike our math/entity/
// collision code, audio touches OS audio devices and hardware
// buffers; that is the one layer we deliberately do NOT write
// ourselves. The CMake target 'miniaudio' compiles the library as
// a static lib and exports its include path. Step 20: main.cpp no
// longer includes it directly — only src/audio.h does now.

// Step 13 moved every stb_image call into src/renderer.h, and Step 14
// moved them ONCE MORE into src/resources.h (the resource-loading
// boundary — the one place that still decodes images); the
// implementation translation unit remains src/stb_impl.cpp, exactly
// as Step 10 established.

/**
 * Step 1: Window + Context Creation
 * Goal: Open a window and create a valid OpenGL context.
 *
 * Step 2: Render Loop — Delta Time Tracking
 * Goal: Measure how long each frame takes (delta time), so future
 *       movement and physics systems can run at a consistent speed
 *       regardless of the frame rate.
 *
 * Step 3: Input Handling
 * Goal: Poll the keyboard inside the loop. ESC closes the window;
 *       SPACE toggles the clear color between black and dark blue
 *       (one toggle per press, not per held frame).
 *
 * Step 4: Draw One Shape
 * Goal: Render a triangle through a basic shader program, proving
 *       the full GPU pipeline works: vertex data -> vertex shader ->
 *       rasterization -> fragment shader -> framebuffer.
 *
 * Step 5: Math Layer
 * Goal: Transform geometry with our own math code. A Mat4 built on the
 *       CPU (rotation driven by deltaTime) is uploaded to the vertex
 *       shader as a uniform every frame, and the triangle visibly rotates.
 *
 * Step 6: Sprite/Mesh Rendering + Camera
 * Goal: Objects live in WORLD space and a camera decides what is visible.
 *       Each instance gets its own model matrix; a view matrix (camera
 *       position, moved with WASD) and an orthographic projection matrix
 *       are combined as projection * view * model and uploaded per draw.
 *
 * Step 7: Basic Object/ECS System
 * Goal: Entities are DATA, not hardcoded per-object blocks. Each triangle
 *       instance is a pe::Entity struct (position, rotation, scale) in a
 *       std::vector; ONE update loop and ONE draw loop process all of
 *       them — adding an entity is a push_back, never new code.
 *
 * Step 8: Collision Detection (AABB)
 * Goal: Detect when two entities overlap using axis-aligned bounding
 *       boxes. The player drives entities[0] with the ARROW keys into
 *       the others; overlapping entities turn red via a per-draw color
 *       uniform. The overlap test is constexpr and static_assert-proven.
 *
 * Step 9: Audio Playback
 * Goal: A sound FILE loads and plays on trigger — audible proof. One
 *       short WAV (assets/beep.wav) is loaded at startup through
 *       miniaudio; it plays exactly on the EDGE where collision starts
 *       (not-colliding -> colliding), never every frame while overlap
 *       continues — the same edge-detection principle as Step 3's SPACE.
 *
 * Step 10: Asset Loading
 * Goal: Textures load from FILES instead of hardcoded data. A PNG
 *       (assets/checker.png) is decoded by stb_image, uploaded to a GL
 *       texture object, and sampled in the fragment shader through UV
 *       coordinates added to the vertex data. Two Step 9 seams are
 *       refactored first: collision-edge detection becomes per-entity
 *       (previous colliding VECTOR, not a scalar), and playback uses a
 *       round-robin POOL of ma_sound instances instead of one.
 *
 * Step 11: Scene/Level Structure
 * Goal: The engine can LOAD, HOLD, and SWITCH between distinct game
 *       states. A pe::GameState enum (MENU / PLAYING / PAUSED) and ONE
 *       current-state variable drive a single dispatch point: what a
 *       key MEANS, whether the world SIMULATES, and what gets DRAWN
 *       all branch on the state. MENU is a plain dark-purple screen
 *       (no text system exists yet) — SPACE starts, ESC quits.
 *       PLAYING is Steps 1-10 untouched; ESC pauses. PAUSED freezes
 *       the scene (drawn every frame, simulated on none); ESC resumes,
 *       SPACE returns to the menu. Starting from the menu RESETS the
 *       world to its initial data snapshot.
 *
 * Step 12: Game Logic Layer
 * Goal: Franchise gameplay on top of everything — the v1 game loop.
 *       A fifth entity, the HOSTILE, chases the player every PLAYING
 *       frame (normalize(player - hostile) * speed * dt, reusing only
 *       Step 5 math). Touching it ends the run: a new GameState
 *       GAME_OVER freezes the scene on a dark-red screen and prints
 *       the survival time (deltaTime accumulation, Step 2's pattern)
 *       to the console. SPACE returns to the menu; the next SPACE
 *       resets everything via Step 11's resetGame(). All Steps 1-11
 *       behavior — spinning triangles, camera, tint, beep, pause — is
 *       additive-adjacent and untouched.
 *
 * Game Build Phase 1: Multiple Hostiles
 * Goal: Three hostiles instead of one — added purely as DATA (two more
 *       push_backs before the initial snapshot), each with its own
 *       chase speed and spin as per-entity data (Step 7's pattern).
 *       The chase becomes a loop over the hostile range; the catch
 *       test becomes a loop where ANY overlap ends the run identically
 *       to v1. The 3-entity scenery loop and resetGame() are untouched
 *       — the snapshot/restore pattern absorbs the new entities for
 *       free. Reuse before you build new.
 *
 * Balance tuning (post-Phase 1, not a phase): feel fixes.
 * Goal: (1) WIDER ARENA — the ortho box widens from (-4..4, -3..3) to
 *       (-6..6, -4.5..4.5). Still 4:3 (12:9 = 800:600), so nothing
 *       stretches; a world unit simply covers fewer pixels. (2)
 *       TIGHTER HOSTILE HITBOXES — the three hostiles pass explicit
 *       halfExtents (0.5, 0.5) instead of the default 0.7071
 *       rotation-safe bound: Step 8's bound exists so spinning SCENERY
 *       has no blind spot at any angle, a guarantee a chasing hostile
 *       does not need; a kill box matched to the triangle's base
 *       half-width makes visible contact and actual death agree.
 *       Player and scenery hitboxes stay untouched at 0.7071.
 *
 * Game Build Phase 2: Difficulty Scaling
 * Goal: Existing hostiles get FASTER the longer you survive — no new
 *       entities, no vector growth. The chase loop multiplies each
 *       hostile's base speed by a difficulty scale derived from Step
 *       12's survivalTime (1 + t*0.01, capped at 1.33, so the fastest
 *       hostile tops out at 2.394 < the player's 2.5 — 'avoidable by
 *       construction' holds at any run length). hostileSpeeds[] stays
 *       a const BASE array; the scale is recomputed every frame and
 *       resetGame() needs zero changes.
 *
 * Game Build Phase 3: On-Screen Text (Score/Timer Display)
 * Goal: The engine's FIRST new capability since Step 12 — digit-only
 *       bitmap text, the smallest real slice of text rendering. An
 *       in-tree script (make_font.ps1, same discipline as the checker
 *       and beep generators) rasters 5x7 dot-matrix patterns for
 *       0-9 and '.' into a transparent 176x16 RGBA atlas (eleven
 *       16x16 cells). Every frame the survival timer formats to one
 *       decimal place; each character draws as a textured QUAD whose
 *       UVs select its atlas cell — SAME shader program, SAME vertex
 *       layout, SAME texture-sampling path as the world (Step 4's
 *       pipeline, Step 10's sampling), no second render system. The
 *       one architectural first: UI quads skip the CAMERA matrix —
 *       projection * model with no view — so the number stays fixed
 *       to the window while the world pans. Blending turns ON for
 *       text only; every world pixel renders exactly as before.
 *
 * Game Build Phase 4: Scoring Beyond Survival Time
 * Goal: Survival time REMAINS the score metric — the phase's ruling.
 *       What changes is PERSISTENCE: the best survival time must
 *       outlive the process. The engine's first disk-WRITE capability
 *       (Steps 9/10 only READ pre-made assets): a plain-text file,
 *       savedata/highscore.txt, holding one decimal number. At startup
 *       it is probed through the SAME 3-candidate CWD pattern as the
 *       assets — but a missing save is the normal first-run case
 *       (default 0.0), never a hard failure like a missing asset. On
 *       GAME_OVER a strict new record updates the in-memory high score
 *       FIRST (the display is always right even if disk I/O fails) and
 *       then writes the file — checked open AND checked output; a
 *       failed save logs a warning and never crashes. The record
 *       displays as a second number one row below the live timer,
 *       through Phase 3's exact font/quad pipeline — zero new
 *       rendering mechanics.
 *
 * Game Build Phase 5: Content Pass
 * Goal: The Step 4-10 placeholder checkerboard gives way to a
 *       PER-ENTITY-TYPE look — the seam Step 12's comment deliberately
 *       deferred. Three 16x16 RGB textures generated IN-TREE by
 *       make_textures.ps1 (same hand-written PNG machinery as the
 *       checker): warm green for the PLAYER, steel blue for SCENERY,
 *       crimson for HOSTILES — threat readable from decoration at a
 *       glance. The draw loop selects one of them per entity index;
 *       no shader, sampler, or pipeline change. One palette rule:
 *       every color keeps red-channel content, because the Step 8
 *       collision tint MULTIPLIES the texel by (1,0,0) — a texture
 *       with zero red would render black on collision. The checker
 *       stays loaded (legacy asset, sampled by no entity anymore).
 *
 * Step 13: Renderer Module Boundary
 * Goal: The engine's first SYSTEM boundary. Everything GPU-side —
 *       the Step 4 shader, the Step 5/8 uniform locations, the two
 *       VAO/VBO pairs, all five textures (checker legacy, the three
 *       Phase 5 entity textures, the Phase 3 font atlas), the Step 7
 *       entity draw loop with its Phase 5 per-entity texture select
 *       and Step 8 collision tint, and the Phase 3/4 screen-space
 *       digit path — moves OUT of this file into pe::Renderer
 *       (src/renderer.h, header-only like entity.h: no CMake change).
 *       main.cpp now initializes the renderer once, selects the
 *       state-dependent clear color, and submits frames through
 *       three calls (clear / drawWorld / drawDigitString). What
 *       stays: the window, audio, entities, state machine, input,
 *       simulation, camera position, projection, timer, high score.
 *       The relocation is byte-compatible — same shader sources,
 *       same vertex layout, same sampling parameters, same tint
 *       values, same digit layout constants — so every visual
 *       result of Steps 1-12 and Phases 1-5 is preserved. One
 *       incidental fix on a fatal error path: renderer-init failure
 *       now uninitializes audio before window teardown, matching the
 *       discipline the texture-load failure paths always had.
 *
 * Step 14: Resource Loading Boundary
 * Goal: Separate asset loading from the renderer. The load/upload
 *       patterns the renderer inherited (the RGB pattern extracted
 *       in Phase 5, and the RGBA font-atlas block from Phase 3) move
 *       into src/resources.h as two free functions — pe::loadRgbTexture
 *       and pe::loadRgbaTexture — same 3-candidate CWD path probe,
 *       same forced-channel stbi_load, same CLAMP_TO_EDGE + LINEAR
 *       sampling, same RGB/RGBA upload, same 0-on-failure contract.
 *       Ownership is unchanged: the renderer keeps all five texture
 *       names and deletes them in destroyAll(). Header-only, zero
 *       CMake change, zero asset change, zero visual change expected.
 *       No cache or asset database — the blueprint rules them out
 *       until the project gains more assets. No gameplay or
 *       rendering behavior is touched at all: main.cpp sees none of
 *       this directly.
 *
 * Step 15: Camera Module Boundary
 * Goal: Separate camera state and camera math from game logic and
 *       render submission. src/camera.h gains pe::Camera owning the
 *       position (starts at the origin), the 3.0 units/second speed,
 *       reset(), move(direction, dt), the lookAt view (target =
 *       position + (0,0,-1), up (0,1,0)), and the once-built
 *       orthographic(-6, 6, -4.5, 4.5, -1, 1) projection. main.cpp
 *       replaces the cameraPos/cameraSpeed/projection locals, routes
 *       WASD through camera.move (KEY POLLING stays here), resets
 *       through camera.reset(), and feeds camera.projection()/view()
 *       to the renderer. renderer.h's drawWorld now RECEIVES the view
 *       matrix and constructs nothing camera-related. Zero behavior
 *       change: same values, same arithmetic order, same per-frame
 *       view rebuild cadence. No GLFW in camera.h.
 *
 * Step 16: Input Module Boundary
 * Goal: Separate keyboard polling and edge detection from
 *       state-specific game meaning. src/input.h gains pe::Input
 *       owning every glfwGetKey level read, the ESC/SPACE edge
 *       detection, and the previous-frame snapshot that edges need
 *       (single owner). main.cpp keeps glfwPollEvents() and the
 *       window lifecycle, reads booleans through the boundary, and
 *       keeps the ENTIRE state switch — what each key means in each
 *       state. Temporal order preserved exactly: poll events, read
 *       level + edges, switch consumes them, then input.update()
 *       stores the new previous-frame snapshot. One edge per physical
 *       press, even while held. No action mapping, no callbacks.
 *
 * Step 17: Time/Timestep Boundary
 * Goal: Separate frame-time acquisition from systems that consume
 *       delta time. src/time.h gains pe::FrameTime owning the
 *       previous-timestamp state: start() seeds it pre-loop (the old
 *       lastFrameTime init), tick() reads glfwGetTime() once, computes
 *       current-minus-previous, advances the stored timestamp, and
 *       returns the float delta (Step 11's shared conversion,
 *       relocated). main.cpp calls tick() at the SAME top-of-loop
 *       position the old code sampled — before glfwPollEvents — so
 *       each delta still spans the entire previous frame. GAMEPLAY
 *       TIME MEANING stays here: survivalTime, difficulty scaling,
 *       timer display, high score, the PLAYING gate. No fixed
 *       timestep, no clamping, no accumulator, no frame limiter.
 *
 * Step 18: Entity Lifecycle Boundary
 * Goal: Make entity creation/reset/removal responsibility explicit
 *       without replacing the vector-based entity design. The six
 *       push_back constructions (exact order, exact values — the
 *       index conventions 0 = player, 1-2 = scenery, 3+ = hostiles
 *       are load-bearing) move into pe::buildInitialEntities(); the
 *       snapshot-restoration assignment moves into pe::resetEntities
 *       (the engine's only "removal" — it never erases or spawns at
 *       runtime); the collision-flag sizing becomes pe::flagsForCount.
 *       main.cpp keeps the collections, keeps resetGame() as ONE
 *       atomic block (now delegating only the entity restore), and
 *       keeps every game meaning: chase, catch, textures, states.
 *       No ECS, no registry, no container ownership, no invented
 *       lifecycle machinery. Free functions in src/lifecycle.h,
 *       header-only — no CMake change.
 *
 * Step 19: Game State Boundary
 * Goal: Separate state transitions and state-specific rules from
 *       unrelated engine systems without changing behavior. The
 *       existing gamestate.h gains pure constexpr helpers alongside
 *       the enum: simulates(state), drawsWorld(state), and
 *       clearColorFor(state, blueToggled) — the rules about a state,
 *       relocated whole (same values, same priority order). What
 *       STAYS here: the dispatch switch and every transition (they
 *       carry side effects — resetGame before the MENU->PLAYING flip,
 *       the window close flag, the clear-color toggle), the toggle
 *       flag itself, and all state meaning. The simple enum + one
 *       currentState variable model is preserved — no state stack, no
 *       scene graph, no transition tables, no callbacks.
 *
 * Step 20: Audio Boundary
 * Goal: Separate game events from direct miniaudio calls. src/audio.h
 *       gains pe::Audio owning the mechanism: ma_engine init, the
 *       3-candidate beep.wav probe, the four-slot pool clone, the ONE
 *       shared round-robin cursor, rewind-if-busy playback via
 *       playNext(), and the slots-then-engine shutdown(). main.cpp
 *       keeps the decisions: the collision-edge test and the catch
 *       decide WHEN a beep happens, a failed init stays fatal (same
 *       policy as renderer.init()), and window/GLFW teardown order
 *       stays here. Trigger semantics preserved exactly — the
 *       collision beep and the GAME_OVER beep advance the SAME
 *       cursor, so they keep rotating through the same four slots.
 *       No volume controls, no streaming, no second asset, no sound
 *       naming system. Header-only — no CMake change.
 *
 * Step 21: UI Rendering Boundary
 * Goal: Give the HUD's number formatting and layout one named home.
 *       src/ui.h gains formatDecimal1() (the shared fixed-one-decimal
 *       rule), the layout constants (timer row at (-5.75, 4.05),
 *       record row at (-5.75, 3.2)), and drawHud() — the two-call
 *       sequence, current run first, record second. ALL glyph GL
 *       stays in pe::Renderer (B5-A): ui.h owns no GL objects at
 *       all, it formats DATA and hands it to the renderer's glyph
 *       path. The display gate (HUD in every non-menu state) and the
 *       high-score save file's formatting stay in main.cpp. No
 *       letters, no menus, no widgets, no layout system.
 *       Header-only — no CMake change.
 *
 * Step 22: World/Game Separation (simulation mechanics)
 * Goal: Extract the pure mechanics out of the PLAYING branch without
 *       touching the frame order. src/simulation.h gains three free
 *       functions: advanceRotations() (the per-entity rotation loop),
 *       chasePlayer() (the hostile pursuit loop, zero-length guard
 *       included, speeds handed in as the raw const float[3]), and
 *       scanSceneryCollisions() (the unique-pair scan over the
 *       original three, both flags set on overlap). main.cpp keeps
 *       the complete chain — timer -> rotation -> difficulty ->
 *       chase -> collision rebuild -> collision scan -> edge
 *       detection -> catch — plus every side effect: timer, difficulty,
 *       flag rebuild, edge detection, audio trigger, catch decision,
 *       state flip, high-score write. No scheduler, no pipeline,
 *       no execution order inside the boundary — the order is the
 *       game's meaning and stays here.
 *       Header-only — no CMake change.
 */

// --- Step 5: Compile-time sanity tests for the math layer ---
// static_assert conditions are evaluated BY THE COMPILER. If any line of
// our math were wrong in a way visible at compile time, the build would
// fail with a clear message — math errors caught before the program even
// runs. These are tests that cost nothing at runtime.
static_assert(pe::Vec3(1.0f, 2.0f, 3.0f) + pe::Vec3(4.0f, 5.0f, 6.0f) == pe::Vec3(5.0f, 7.0f, 9.0f),
              "Vec3 addition is broken");
static_assert(pe::Vec3(1.0f, 0.0f, 0.0f).dot(pe::Vec3(0.0f, 1.0f, 0.0f)) == 0.0f,
              "Vec3 dot product is broken");
// The 4th column of a translation matrix must hold the offset itself.
static_assert(pe::Mat4::translation(pe::Vec3(3.0f, 4.0f, 5.0f)).m[3][1] == 4.0f,
              "Mat4 translation builder is broken");
// Identity must transform a point into exactly itself.
static_assert(pe::Mat4().transformPoint(pe::Vec3(2.0f, 3.0f, 4.0f)) == pe::Vec3(2.0f, 3.0f, 4.0f),
              "Mat4 identity/transformPoint is broken");
// A translation applied through transformPoint must shift the point.
static_assert(pe::Mat4::translation(pe::Vec3(1.0f, 1.0f, 1.0f)).transformPoint(pe::Vec3(1.0f, 2.0f, 3.0f)) == pe::Vec3(2.0f, 3.0f, 4.0f),
              "Mat4 translation transform is broken");
// --- Step 6 additions: prove the orthographic projection at compile time ---
// The corners of the ortho box must land EXACTLY on the clip-cube corners.
// The box below was chosen so every intermediate value is exactly
// representable in float (scales 0.5 and 1.0) — static_assert compares
// floats with ==, so the test values must be exact, no rounding luck.
// transformPoint is safe to use here: orthographic preserves w = 1, so the
// skipped perspective divide is a genuine no-op for this matrix.
static_assert(pe::Mat4::orthographic(-2.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f).transformPoint(pe::Vec3(2.0f, 1.0f, 0.0f)) == pe::Vec3(1.0f, 1.0f, 0.0f),
              "Mat4 orthographic projection is broken");
static_assert(pe::Mat4::orthographic(-2.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f).transformPoint(pe::Vec3(-2.0f, -1.0f, 0.0f)) == pe::Vec3(-1.0f, -1.0f, 0.0f),
              "Mat4 orthographic projection is broken");
// --- Step 8 additions: prove the AABB overlap test at compile time ---
// Two identical unit boxes at the same center: maximally overlapping.
static_assert(pe::aabbOverlap(pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f),
                              pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f)),
              "AABB overlap: concentric boxes must collide");
// Gap on the X axis alone kills the collision — even with Y ranges
// fully shared. This is the test that catches the classic AND->OR bug.
static_assert(!pe::aabbOverlap(pe::Vec3(3.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f),
                               pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f)),
              "AABB overlap: X gap must prevent collision");
// Mirror image: gap on the Y axis alone must also prevent it.
static_assert(!pe::aabbOverlap(pe::Vec3(0.0f, 3.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f),
                               pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f)),
              "AABB overlap: Y gap must prevent collision");
int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 2. Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    // 3. Create the Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "PureEngine - Step 1", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 4. Make the Window's Context Current
    glfwMakeContextCurrent(window);

    // 5. Initialize GLAD
    // We use the GLAD loader to get OpenGL function pointers.
    // This must be done AFTER the context is created and made current.
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        // The window already exists here, so clean it up before exiting —
        // same consistent cleanup as every other error path below.
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // --- Step 9 / Step 20: Audio engine + sound loading (one-time setup) ---
    // The MECHANISM — ma_engine_init, the 3-candidate CWD probe for
    // assets/beep.wav, decoding into slot 0, cloning the known-good
    // path into slots 1-3 — moved whole into pe::Audio::init()
    // (src/audio.h). The DECISIONS stay here: a failed init is FATAL
    // (same policy as renderer.init() below), and only the window/GLFW
    // teardown belongs to main.cpp — audio.init() has already cleaned
    // up any partial allocation before returning false.
    pe::Audio audio;
    if (!audio.init()) {
        std::cerr << "Failed to initialize audio (miniaudio engine or assets/beep.wav)" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // --- Step 10 REFACTOR (Step 9 seam #1): per-entity edge state ---
    // Step 9 collapsed every collision into ONE scalar flag, so a NEW
    // overlap starting while another was already active fired no sound
    // (the global level never dropped to false). The fix tracks the
    // previous frame's full collision VECTOR — one entry per entity —
    // so each entity's 0->1 transition is its own edge. Declared empty
    // here (entities don't exist yet); sized on first use in the loop.
    std::vector<char> wasColliding;

    // --- Step 11: Game state (before the loop) ---
    // ONE variable holds which state the engine is in. Every per-frame
    // decision — what input means, whether the world updates, what gets
    // drawn — dispatches on it from a single point below. The engine
    // boots to the MENU, not straight into gameplay.
    pe::GameState currentState = pe::GameState::MENU;
    // --- Step 16: ESC/SPACE edge state moved to the input boundary ---
    // The old escWasPressedLastFrame bool lived here; the previous-frame
    // snapshot each edge needs now lives inside pe::Input (declared
    // below, before the loop), with a single owner.

    // --- Step 2 / Step 17: Frame-time seed (before the loop) ---
    // glfwGetTime() returns the number of seconds (as a high-resolution double)
    // that have elapsed since glfwInit() was called. It is a monotonic clock,
    // perfect for measuring intervals between frames. Step 17 moved the
    // previous-timestamp STATE and its seeding into pe::FrameTime: start()
    // runs ONCE, before the loop, so the very first frame has a valid
    // reference point to subtract from. Without this initialization, the
    // first frame's delta would be garbage (uninitialized memory),
    // potentially producing a huge or negative value.
    pe::FrameTime frameTime;
    frameTime.start();

    // --- Step 3: Input State Setup (before the loop) ---
    // Tracks which clear color is currently active. false = black (the
    // Step 1/2 default), true = dark blue. SPACE toggles this flag.
    bool clearColorIsBlue = false;
    // Step 16: the spaceWasPressedLastFrame bool that used to live here
    // moved into pe::Input. Why edges need memory is unchanged:
    // glfwGetKey() only reports RIGHT NOW, so a held key would otherwise
    // look "pressed" every frame and fire hundreds of toggles per
    // second; comparing against the previous frame detects the exact
    // instant the key goes DOWN — one edge per physical press. That
    // comparison and its snapshot are now the boundary's job.

    // --- Step 13: Renderer initialization ---
    // Everything GPU-side that Steps 4-10 and Phases 3/5 used to create
    // directly in this file — shader compile and link, the 'transform'
    // and 'color' uniform lookups, the triangle VAO/VBO, the checker
    // texture, the three Phase 5 entity textures, the Phase 3 font
    // atlas plus text VAO/VBO, and the sampler-to-unit-0 bind — moved
    // into pe::Renderer::init() (src/renderer.h). The GL work is
    // byte-for-byte the same; what changed is OWNERSHIP. main.cpp
    // keeps the fatal-error DECISION and the non-rendering teardown
    // that follows it: the renderer already deleted every GL object it
    // had created before returning false, so here only audio, window,
    // and GLFW remain to clean up. (Honest note recorded in the
    // tracker: the OLD shader failure paths predated the audio-cleanup
    // discipline the texture paths established and leaked the audio
    // engine on a fatal exit; this path closes that.)
    pe::Renderer renderer;
    if (!renderer.init()) {
        // Step 20: audio teardown through the boundary — the renderer
        // already deleted its GL objects before returning false, so
        // here only audio, window, and GLFW remain to clean up.
        audio.shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // --- Step 7 / Step 18: Entities as DATA (before the loop) ---
    // Every triangle instance is one entry in this vector. The Step 5/6
    // globals rotationAngle/rotationSpeed no longer exist — that state
    // now lives INSIDE each entity, per instance. The current hostile
    // archetype starts as a tiny file-backed profile (hostile_data.h),
    // but the build order still belongs to pe::buildInitialEntities
    // (src/lifecycle.h) so the initial world remains a single named
    // responsibility. The index conventions are still load-bearing:
    // 0 = player, 1-2 = scenery, 3+ = hostiles, chased at the base
    // speed array and textured by index.
    const pe::HostileDefaults hostileDefaults = pe::loadHostileDefaults();
    std::vector<pe::Entity> entities = pe::buildInitialEntities(hostileDefaults);
    // --- Step 11: the INITIAL world, kept as DATA ---
    // A snapshot of the fresh entity list. Starting a game from the
    // menu restores it — reset is an ASSIGNMENT, not new code, which
    // is exactly what 'entities as data' (Step 7) was buying.
    const std::vector<pe::Entity> initialEntities = entities;
    // Collision flags hoisted OUT of the loop body (they lived inside
    // it in Steps 8-10). The PAUSED state still DRAWS the scene but
    // stops SIMULATING, so the last PLAYING frame's flags must survive
    // the pause (frozen tint). Rebuilt from zero every PLAYING frame.
    // Step 18: the sizing expression lives with creation in the
    // lifecycle boundary (pe::flagsForCount); main.cpp owns the buffer.
    std::vector<char> colliding = pe::flagsForCount(entities.size());

    // --- Step 6 / Step 15: Camera + Projection State (before the loop) ---
    // The camera's position IN WORLD SPACE, its movement speed, and
    // the once-built orthographic PROJECTION matrix. Step 15 moved all
    // three into pe::Camera: WASD shifts the position every frame via
    // camera.move(), the view matrix is rebuilt from it there, so the
    // whole scene appears to slide the opposite way — that IS camera
    // movement. The projection is the BALANCE-TUNED 12 x 9 box (4:3,
    // 66.7 px per world unit at 800x600 — see camera.h for the full
    // tuning note), built once; nothing about it changes per frame.
    pe::Camera camera;

    // --- Step 16: the input boundary (before the loop) ---
    // ESC and SPACE are the only keys with EDGE semantics, so they are
    // the only keys registered for previous-frame tracking (snapshot
    // starts all-false: before the program starts, nothing is pressed).
    // WASD and the arrows stay level-only reads — no tracking needed.
    pe::Input input{GLFW_KEY_ESCAPE, GLFW_KEY_SPACE};

    // --- Step 8: Player movement speed (before the loop) ---
    // World units per second for the ARROW-key-driven entity
    // (entities[0]). Deliberately a bit slower than the camera's 3.0
    // so driving into another triangle feels controlled, not twitchy.
    const float entityMoveSpeed = 2.5f;

    // --- Step 12 / Phase 1 / Phase 2: hostile BASE speeds ---
    // These are the current defaults for the one hostile archetype,
    // loaded from a tiny startup file if present. The runtime chase
    // semantics remain unchanged; only the source of the values moves
    // out of the compiled-in literal array. The data supplies one base
    // speed per loaded hostile.
    std::vector<float> hostileSpeeds;
    for (const pe::HostileDefinition& hostile : hostileDefaults.hostiles) {
        hostileSpeeds.push_back(hostile.baseSpeed);
    }

    // --- Game Build Phase 2: difficulty scaling knobs ---
    // The scale formula stays identical to the current gameplay:
    //     scale = min(1 + survivalTime * difficultyRate, maxDifficultyScale)
    // The file loader falls back to the current built-in values if it is
    // missing or malformed, which keeps the game playable and avoids
    // crashing. The actual runtime expression is unchanged.
    const float difficultyRate = hostileDefaults.difficultyRate;
    const float maxDifficultyScale = hostileDefaults.maxDifficultyScale;

    // --- Step 13: GPU geometry, textures, and sampler setup moved out ---
    // Everything that used to follow here — Step 4's triangle vertex
    // data + VAO/VBO, Step 10's checker load and upload, Phase 5's
    // three per-type entity textures, Phase 3's font atlas plus text
    // quad VAO/VBO, and the sampler-to-unit-0 bind — now lives in
    // pe::Renderer::init() (src/renderer.h), byte-compatible. The
    // educational comments that explained each GL call moved with the
    // code they describe.

    // --- Game Build Phase 4: high score — loaded ONCE at startup ---
    // The engine's FIRST persistent state: everything since Step 1
    // exists only as long as the process does. The record lives in a
    // PLAIN-TEXT file — one decimal number, nothing else. Plain text is
    // sufficient because the payload is a single float: there is no
    // STRUCTURE for JSON to describe, no size or speed problem for a
    // binary format to solve, and a file you can open in Notepad and
    // verify by eye beats any opaque encoding in a learning engine.
    // Same 3-candidate CWD probe as Steps 9/10 (the pattern exists for
    // exactly this: relative paths resolve against wherever the exe was
    // launched from) — but with a crucially different FAILURE MODE: a
    // missing ASSET aborts startup (the game cannot run without it),
    // while a missing SAVE is the normal state of a first run. It
    // defaults to 0.0 and is created by the first record. Garbage
    // content is handled the same way: the stream parse fails, the
    // default stands, nothing crashes.
    const char* highScorePathCandidates[] = {
        "savedata/highscore.txt",       // run from the repo root
        "../savedata/highscore.txt",    // run from build/
        "../../savedata/highscore.txt"  // run from build/Release/
    };
    // The in-memory record: the SINGLE source of truth while the game
    // runs. The file is only the place it survives the process in.
    float highScore = 0.0f;
    // Where the file was FOUND (if it was): the save writes back to the
    // SAME path the load read from, so the record can never fork into
    // two files. A first run (nothing found) falls back to candidate 0.
    std::string highScorePath = highScorePathCandidates[0];
    {
        bool highScoreFound = false;
        for (const char* candidate : highScorePathCandidates) {
            std::ifstream in(candidate);   // opening a missing file just
            if (!in) {                     // leaves the stream in a fail
                continue;                  // state — NO exception, no crash
            }
            highScoreFound = true;
            highScorePath = candidate;     // remember for the save below
            float stored = 0.0f;
            // '>>' parses one float and sets the stream's fail state on
            // garbage; the >= 0 guard also rejects NaN, because every
            // comparison with NaN is false. Either failure keeps 0.0.
            if (in >> stored && stored >= 0.0f) {
                highScore = stored;
            }
            break;
        }
        if (!highScoreFound) {
            std::cout << "No high score saved yet - starting at 0.0 (first record creates the file)" << std::endl;
        } else {
            std::cout << "Loaded high score: " << highScore << " s (from " << highScorePath << ")" << std::endl;
        }
    }

    // --- Step 12: survival timer ---
    // Seconds survived in the current run. Accumulated INSIDE the
    // PLAYING gate only — pausing stops the clock, which is correct:
    // the timer measures played time, not wall time. Same pattern as
    // every other deltaTime consumer since Step 2: state += rate * dt,
    // here with a rate of 1 second per second.
    float survivalTime = 0.0f;

    // --- Step 11: reset the world to its initial data ---
    // Called when a game STARTS from the menu. Every piece of play
    // state returns to its pre-game value: the entity snapshot (which
    // INCLUDES the hostile at its start position — the snapshot was
    // taken AFTER its push_back), the camera home, the clear-color
    // toggle off, BOTH collision histories empty (nothing was colliding
    // before the game began), and the survival timer back to zero.
    // Phase 4: the high score is DELIBERATELY NOT reset here — it is
    // meta-game state that outlives every individual run, exactly the
    // distinction the snapshot pattern makes explicit by omission.
    auto resetGame = [&]() {
        pe::resetEntities(entities, initialEntities);   // Step 18: the snapshot restore, via the lifecycle boundary
        camera.reset();   // Step 15: back to the world origin, via the boundary
        clearColorIsBlue = false;
        wasColliding.clear();
        colliding = pe::flagsForCount(entities.size());
        survivalTime = 0.0f;
    };

    // --- Step 13: the digit-string glyph path moved to the renderer ---
    // The lambda that used to live here — per-glyph atlas UVs, quad
    // re-upload, projection * model with NO VIEW — is now
    // pe::Renderer::drawDigitString (src/renderer.h), same mechanics,
    // same layout constants. main.cpp formats the NUMBERS (game data)
    // and the renderer draws the GLYPHS (rendering).

    // 6. The Main Loop
    while (!glfwWindowShouldClose(window)) {
        // --- Step 2 / Step 17: Delta Time Calculation (top of the frame) ---
        // Placed at the very top of the loop body so the timing covers the
        // entire frame: events, rendering, and buffer swap. Step 17 moved
        // the ACQUISITION into pe::FrameTime::tick() — read the clock,
        // subtract the previous frame's timestamp, advance the stored
        // timestamp so the NEXT frame measures against THIS one, narrow
        // to float (Step 11's shared conversion). The RESULT, dt, is the
        // duration of the last frame in SECONDS; movement/physics systems
        // multiply velocities by this value so objects move the same
        // distance per second whether the game runs at 30 FPS or 300 FPS.
        // The SAMPLING POSITION is unchanged — before glfwPollEvents,
        // before everything — because what a delta measures depends on
        // exactly where it is taken.
        const float dt = frameTime.tick();

        // A. Poll for events (input, window resize, etc.)
        glfwPollEvents();

        // --- Step 11 / Step 16: Input dispatch — what a KEY MEANS
        // depends on the STATE ---
        // The engine's first real input FORK: the same physical key has
        // different semantics per state. ESC quits from MENU, PAUSES in
        // PLAYING, resumes from PAUSED. SPACE starts from MENU, toggles
        // the clear color in PLAYING, returns to MENU from PAUSED.
        // Step 16: the raw key READS (level + edge, Step 3's pattern)
        // now come through pe::Input; the state switch below still
        // owns the MEANING and consumes the results. Every transition
        // is a one-line assignment: states are values, and switching
        // is nothing more dramatic than storing a new one.
        bool escIsPressedNow = pe::Input::isDown(window, GLFW_KEY_ESCAPE);
        bool escEdge = input.isEdge(window, GLFW_KEY_ESCAPE);
        bool spaceEdge = input.isEdge(window, GLFW_KEY_SPACE);

        switch (currentState) {
        case pe::GameState::MENU:
            // ESC quits. LEVEL polling is fine here: the only effect is
            // setting the close flag — idempotent even while held, and
            // glfwSetWindowShouldClose destroys nothing immediately (the
            // loop condition checks it, cleanup runs as usual).
            if (escIsPressedNow) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            // SPACE starts a game: reset the world to its initial data,
            // then enter PLAYING. EDGE — one start per press.
            } else if (spaceEdge) {
                resetGame();
                currentState = pe::GameState::PLAYING;
            }
            break;
        case pe::GameState::PLAYING:
            // ESC pauses. EDGE — a held key must not flip pause on and
            // off 60 times a second.
            if (escEdge) {
                currentState = pe::GameState::PAUSED;
            } else {
                // --- Step 3: SPACE clear-color toggle (EDGE, exactly one
                // toggle per physical press; held key stays false after
                // the first frame). Scope of PLAYING only now.
                if (spaceEdge) {
                    // Flip the flag: black becomes blue, blue becomes black.
                    clearColorIsBlue = !clearColorIsBlue;
                }
                // --- Step 6 / Steps 15-16: Camera Movement (WASD,
                // polled every frame) ---
                // Movement is a RATE, not a toggle: while a key is held it
                // must act on EVERY frame. No previous-frame comparison —
                // speed * deltaTime turns a per-frame key state into
                // frame-rate-independent units per second. Step 15 moved
                // the ARITHMETIC into camera.move(); Step 16 moved the
                // KEY READ into pe::Input::isDown. The direction choice
                // and the PLAYING-only gate stay here.
                if (pe::Input::isDown(window, GLFW_KEY_W)) {
                    camera.move(0.0f, 1.0f, dt);   // camera up    -> scene slides down
                }
                if (pe::Input::isDown(window, GLFW_KEY_S)) {
                    camera.move(0.0f, -1.0f, dt);  // camera down  -> scene slides up
                }
                if (pe::Input::isDown(window, GLFW_KEY_A)) {
                    camera.move(-1.0f, 0.0f, dt);  // camera left  -> scene slides right
                }
                if (pe::Input::isDown(window, GLFW_KEY_D)) {
                    camera.move(1.0f, 0.0f, dt);   // camera right -> scene slides left
                }
                // --- Step 8: Player entity movement (ARROW keys) ---
                // WASD belongs to the CAMERA (established Step 6 behavior,
                // kept untouched). The ARROW keys move entities[0] through
                // the world, so the player can drive it into the other two
                // triangles and trigger a collision on demand. Same RATE
                // pattern as camera panning. The reference below points
                // INTO the vector — writes land in the real entity.
                pe::Entity& player = entities[0];
                if (pe::Input::isDown(window, GLFW_KEY_UP)) {
                    player.position.y += entityMoveSpeed * dt;
                }
                if (pe::Input::isDown(window, GLFW_KEY_DOWN)) {
                    player.position.y -= entityMoveSpeed * dt;
                }
                if (pe::Input::isDown(window, GLFW_KEY_LEFT)) {
                    player.position.x -= entityMoveSpeed * dt;
                }
                if (pe::Input::isDown(window, GLFW_KEY_RIGHT)) {
                    player.position.x += entityMoveSpeed * dt;
                }
            }
            break;
        case pe::GameState::PAUSED:
            // ESC resumes; SPACE gives up the run and returns to the
            // menu. The abandoned world stays in memory as-is; the NEXT
            // start resets it via resetGame().
            if (escEdge) {
                currentState = pe::GameState::PLAYING;
            } else if (spaceEdge) {
                currentState = pe::GameState::MENU;
            }
            break;
        case pe::GameState::GAME_OVER:
            // --- Step 12: the run is over ---
            // SPACE returns to the menu — Step 11's navigation pattern
            // verbatim, one-line assignment. ESC is deliberately DEAD
            // here: ESC means pause/resume, and there is nothing to
            // resume from a finished run. The frozen scene stays on
            // screen (drawn below, never simulated) until SPACE moves
            // us back to the purple menu.
            if (spaceEdge) {
                currentState = pe::GameState::MENU;
            }
            break;
        }
        // --- Step 16: frame-end snapshot update, through the boundary ---
        // Same POSITION as the old escWasPressedLastFrame /
        // spaceWasPressedLastFrame stores: AFTER the switch consumed the
        // edges, the tracked keys' current levels become next frame's
        // "previous" state. Order is load-bearing — updating earlier
        // would kill every edge this frame.
        input.update(window);

        // --- Step 11 / Step 19: Simulation runs ONLY in PLAYING ---
        // PAUSED holds the world still — drawn every frame (below),
        // simulated on none of them. MENU has no world to simulate.
        // Everything Steps 7-10 do per frame is unchanged INSIDE this
        // branch: the state machine WRAPS the simulation, it never
        // touches it. That is why all Step 1-10 behavior survives.
        // Step 19: the question is asked through the game-state
        // boundary (pe::simulates); the answer is identical.
        if (pe::simulates(currentState)) {
            // --- Step 22: THE FRAME-ORDER CHAIN (this file owns it) ---
            // timer -> rotation -> difficulty -> chase -> collision
            // rebuild -> collision scan -> edge detection -> catch.
            // The three pure MECHANICS live in src/simulation.h; the
            // ORDER and every gameplay side effect live below, in this
            // order, every PLAYING frame. Read this block top to
            // bottom and you are reading the contract.

            // --- Step 12: survival timer accumulates ---
            // deltaTime integration, Step 2's pattern: count up at a
            // constant rate of 1 second per second of PLAYING time.
            survivalTime += dt;

            // --- Step 7 / Step 22: rotation update ---
            // The per-entity loop (each entity advancing at its own
            // rate via the state += rate * deltaTime pattern) moved
            // into pe::advanceRotations (src/simulation.h); the
            // per-frame semantics are byte-identical.
            pe::advanceRotations(entities, dt);

            // --- Phase 2: this frame's difficulty scale ---
            // Computed ONCE per frame (all hostiles share the same clock):
            // linear ramp from 1.0 at t = 0, clamped by std::min at the cap
            // so long runs hold a constant ceiling instead of diverging.
            // Pure function of survivalTime — no stored state, nothing to
            // reset.
            const float difficultyScale =
                std::min(1.0f + survivalTime * difficultyRate, maxDifficultyScale);

            // --- Step 12 / Phase 1 / Step 22: hostile chase ---
            // The mechanics moved into pe::chasePlayer (src/
            // simulation.h): the v1 chase unchanged per hostile,
            // looped over the hostile range (index 3 to the end),
            // direction = player - hostile normalized, scaled by the
            // hostile's base speed TIMES this frame's difficultyScale
            // TIMES deltaTime, zero-length guard included. Pure
            // pursuers — no prediction, no flanking; the difficulty
            // comes from NUMBERS. The decision of WHEN this runs
            // (this frame, in this order, only in PLAYING) stays
            // here.
            pe::chasePlayer(entities, hostileSpeeds, difficultyScale, dt);

            // --- Step 8: Collision pass (after movement, before drawing) ---
            // One flag per entity, rebuilt from ZERO every frame: collision
            // state is derived fresh from positions, never remembered. A
            // 'sticky' flag would need explicit reset logic and drift out of
            // sync; deriving it needs nothing. char over bool: zero-init is
            // unambiguous and it reads fine as a flag. (Step 11: the
            // vector itself is hoisted to loop scope so PAUSED can keep
            // drawing the last frame's tint; contents rebuilt here.)
            colliding.assign(entities.size(), 0);
            // The scan itself moved into pe::scanSceneryCollisions
            // (src/simulation.h): every unique pair among the
            // original three tested once, BOTH flags set on overlap.
            // The bound stays the literal 3 inside the helper — the
            // hostiles remain excluded, their only interaction the
            // catch test below. The rebuild line ABOVE stays here,
            // because the rebuild is the collision-state POLICY
            // (derived fresh, never remembered).
            pe::scanSceneryCollisions(entities, colliding);

            // --- Step 10: per-entity collision EDGE detection + sound pool ---
            // Step 9's scalar OR-flag is gone. Now the previous frame's full
            // collision VECTOR is compared entry by entry: an entity whose
            // flag goes 0 -> 1 is a fresh edge EVEN IF other entities were
            // already colliding. (First frame: the vector is sized here with
            // all-zero entries — before the program starts, nothing collides.)
            if (wasColliding.size() != colliding.size()) {
                wasColliding.assign(colliding.size(), 0);
            }
            bool anyNewCollision = false;
            for (size_t i = 0; i < colliding.size(); ++i) {
                if (colliding[i] && !wasColliding[i]) {
                    anyNewCollision = true;
                    break;
                }
            }
            if (anyNewCollision) {
                // Step 20: the round-robin claim, rewind-if-busy, and
                // start moved into pe::Audio::playNext() — same cursor,
                // same semantics; main.cpp keeps the EDGE DECISION that
                // gets us here.
                audio.playNext();
            }
            // Store THIS frame's vector for the next frame's edge test.
            wasColliding = colliding;

            // --- Step 12 / Phase 1: the catch test — player vs ANY hostile ---
            // v1 tested ONE pair; Phase 1 generalizes it the minimal
            // way: a loop over the hostile range (index 3 onward)
            // OR-ing Step 8's exact AABB test into a single bool.
            // Touching ANY hostile ends the run — the lose condition
            // does not care which one caught you. Everything ELSE is
            // the identical Step 12 block: reuse Step 8's aabbOverlap
            // machinery, print the survival time (console is the only
            // output channel — no font system exists), reuse the sound
            // POOL for the audible end-of-run signal (round-robin,
            // rewind-if-busy, Steps 9/10 pattern), and flip the state
            // LAST so every per-frame system above ran exactly once on
            // the final frame. Same frame, same death, three threats.
            bool caught = false;
            for (size_t h = 3; h < entities.size(); ++h) {
                if (pe::aabbOverlap(entities[0], entities[h])) {
                    caught = true;
                    break;   // one catcher is enough — stop testing
                }
            }
            if (caught) {
                std::cout << "GAME OVER — survived "
                          << survivalTime << " seconds" << std::endl;
                // Step 20: the end-of-run beep goes through the SAME
                // playNext() as the collision beep — one shared cursor,
                // exactly as the single nextCollisionSound variable
                // always did, so the two events keep rotating through
                // the same four slots.
                audio.playNext();
                // --- Game Build Phase 4: record check + save ---
                // The run's final time is complete RIGHT NOW (the timer
                // stops with the state flip below), so this is the exact
                // moment to compare. STRICT greater-than: an equal time
                // is not a new record and rewrites nothing. The
                // in-memory record updates FIRST — even if the disk
                // write fails below, the on-screen number stays correct
                // for the rest of this session.
                if (survivalTime > highScore) {
                    highScore = survivalTime;
                    // create_directories makes savedata/ if it does not
                    // exist yet (a first run never has it); the
                    // error_code overload can NEVER throw — a save must
                    // not crash the game — and the call is a silent
                    // no-op when the directory already exists.
                    const std::filesystem::path savePath(highScorePath);
                    std::error_code fsError;
                    std::filesystem::create_directories(savePath.parent_path(), fsError);
                    // CHECKED write, both halves: the OPEN (ofstream's
                    // conversion to bool fails on permissions or a
                    // bad path) and the OUTPUT state after flush (a
                    // full disk or read-only folder surfaces here).
                    // Failure is a console warning, never fatal — a
                    // lost save costs one record, not the game. The
                    // death flow below proceeds either way.
                    std::ofstream out(savePath);
                    bool saved = false;
                    if (out) {
                        out << std::fixed << std::setprecision(1) << highScore << '\n';
                        out.flush();
                        saved = static_cast<bool>(out);
                    }
                    if (saved) {
                        std::cout << "NEW HIGH SCORE: " << highScore
                                  << " s (saved to " << highScorePath << ")" << std::endl;
                    } else {
                        std::cerr << "Warning: could not save the high score to "
                                  << highScorePath
                                  << " - it stays in memory for this session only" << std::endl;
                    }
                }
                currentState = pe::GameState::GAME_OVER;
            }
        }

        // B. Clear the screen
        // --- Step 11 / Step 19: clear color is per-state ---
        // The palette RULES moved to the game-state boundary in Step 19
        // (pe::clearColorFor in src/gamestate.h): MENU dark purple,
        // GAME_OVER dark red, gameplay black — or dark blue when Step
        // 3's toggle flag (still owned HERE) is set. Same values, same
        // priority order; main.cpp asks and the renderer clears.
        const pe::ClearColor frameClear = pe::clearColorFor(currentState, clearColorIsBlue);
        renderer.clear(frameClear.r, frameClear.g, frameClear.b);

        // --- Step 11 / Step 19: gameplay rendering happens OUTSIDE the MENU ---
        // MENU draws nothing but the clear color. PLAYING, PAUSED, and
        // (Step 12) GAME_OVER share this EXACT draw path — the only
        // difference between them is whether the simulation branch
        // above advanced the data being drawn here (frozen spin, frozen
        // camera, frozen tint, frozen hostile). Step 13 changed HOW the
        // path is spelled, not WHAT it draws: the same view matrix,
        // per-entity texture select, collision tint, and screen-space
        // digits now submit through pe::Renderer. Step 19 asks the
        // question through the boundary (pe::drawsWorld).
        if (pe::drawsWorld(currentState)) {
            // --- Steps 4-10 + Phase 5: the world pass, through the
            // renderer boundary ---
            // Inside drawWorld, unchanged: glUseProgram, the shared
            // triangle VAO, texture unit 0, per-entity texture by index
            // convention (0 = player, 1..2 = scenery, 3+ = hostiles),
            // the projection * view * model upload, the white/red tint
            // from the colliding flags, and one draw call per entity.
            // Step 15: the VIEW matrix now comes prebuilt from the
            // camera boundary; the renderer performs no camera math.
            renderer.drawWorld(camera.projection(), camera.view(), entities, colliding);

            // --- Game Build Phase 3/4: UI layer — survival timer + high score ---
            // Step 21: the number formatting and the two layout
            // constants moved into the UI boundary (src/ui.h);
            // drawHud() runs the same two-call sequence in the same
            // order. The GLYPHS are still drawn by the renderer —
            // screen-space (projection * model, NO VIEW, so they stay
            // fixed to the window while WASD pans the world),
            // blending ON for text only, white tint, same atlas.
            // The DISPLAY GATE stays here: shown in every non-menu
            // state — live during PLAYING, frozen during PAUSED (the
            // simulation gate stopped the clock, so the number stops
            // with it), and the FINAL time on GAME_OVER.
            pe::drawHud(renderer, camera.projection(), survivalTime, highScore);
        }

        // C. Swap buffers
        // GLFW uses double buffering. This swaps the front buffer (what we see)
        // with the back buffer (what we just drew to).
        glfwSwapBuffers(window);
    }

    // 7. Cleanup
    // --- Step 13: GPU resources belong to the renderer now ---
    // One call replaces the ten hand-written delete calls that used to
    // live here; the reverse-creation order is preserved inside
    // pe::Renderer::shutdown(). Audio was never rendering, so its
    // teardown stays in main.cpp below.
    renderer.shutdown();
    // --- Step 9/10 / Step 20: audio cleanup, through the boundary ---
    // pe::Audio::shutdown() keeps the reverse creation order — every
    // pool slot first (each registered WITH the engine), then the
    // engine itself, which stops the mixing thread and closes the OS
    // audio device. Audio is independent of OpenGL, so its teardown
    // order relative to the GL calls does not matter.
    audio.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
