# PureEngine — Build Continuation (v3)

## Context
PureEngine Step 25 through Step 50 are complete and verified. This continuation begins from the hardened state established after the v2 tracker was superseded: data-driven hostile config, alternate scene reuse, periodic frame-time visibility, and the automated hostile parser test target.

The current project remains intentionally minimal. The source and build output remain the authority, and this tracker is only a forward pointer for future work.

## Goal
Continue from the verified engine hardening work without inventing new abstractions, managers, or registries before a concrete need is demonstrated.

## Rule
No step starts until there is a real demonstrated need in the source, the build, or a required conversation-driven follow-up. Steps 25 through 88 are tracked in `Blueprint/pure_engine_v3_steps.json` (Step 44 marked "superseded" by Step 48) and cover engine hardening (25-34), data-driven hostile config, alternate scenes, depth sorting, entity roles, stress-test validation, role-based collision scanning (51), frame-time scaling measurement (52), player foreground layering (53), entity role genericization (54), renderer texture index slots (55), animation data structures (56), renderer animation frame UVs (57), data-driven animation loading (58), animated Pong proof (59), physics foundation (60), physics integration (61), collision response (62), tilemap system foundation (63), scene management foundation (64), transform hierarchy foundation (65), font/text rendering foundation (66), event system foundation (67), debug console foundation (68), gamepad input foundation (69), particle system foundation (70), Arcade integration sprint (71), physics hardening for platformer (72), camera lerp follow (73), platformer proof game (74), audio volume control (75), 64x64 player sprite (76), shader system (77), sprite batching (78), 2D point lighting (79), per-sound volume (80), scene serialization (81), menu/HUD polish (82), orphan/contract freeze (83), OOB textureId log + doc refresh (84), Pong score + win (85), particle system depth (86), optional fixed-timestep (87), and basic input action map (88).

## Candidate Roadmap
The items below are non-binding, exploratory ideas only. They are not a step queue and do not define the next concrete requirement. The goal is to make PureEngine feel like a playable 2026 arcade survival game while staying flat/2D and avoiding 3D or engine-framework scope.

- Hit/damage feedback (flash, knockback, or similar juice on catch)
- Sound effects tied to real events (catch, game over, high score) — check the existing audio boundary before proposing any new API
- Visually distinct hostile types (not just speed variance) — check the existing renderer texture-by-index convention before proposing new work
- A win condition or objective beyond pure survival time
- Menu/UI polish (instructions, controls hint)
- Packaging for distribution (itch.io-ready build)
- ✓ Phase 1: Role-based collision scanning (Step 51, completed)
- ✓ Step 53: Depth layering (player=3 foreground) — completed
- ✓ Step 54: Genericize entity.h (roleId enum) — completed
- ✓ Step 55: Genericize renderer.h (texture index slots) — completed
- ✓ Step 56: Animation system foundation (data structures, Entity fields) — completed
- ✓ Step 57: Renderer animation frame UV calculation — completed
- ✓ Step 58: Data-driven animation loading (file parsing, tick loop) — completed
- ✓ Step 59: Animated Pong (per-entity layout, visual proof) — completed
- ✓ Step 60: Physics foundation (velocity + gravity fields) — completed
- ✓ Step 61: Physics integration (applyPhysics) — completed
- ✓ Step 62: Collision response (impulse resolveCollision) — completed
- ✓ Step 63: Tilemap system foundation (load, to-entities, collide) — completed
- ✓ Step 64: Scene management foundation (Scene + SceneManager) — completed
- ✓ Step 65: Transform hierarchy foundation (parentIndex + setParent + worldPosition) — completed
- ✓ Step 66: Font/Text rendering foundation (font.h + drawTextString) — completed
- ✓ Step 67: Event system foundation (EventBus + subscribe/emit) — completed
- ✓ Step 68: Debug console foundation (Console + commands + draw) — completed
- ✓ Step 69: Gamepad input foundation (poll + buttons + deadzone) — completed
- ✓ Step 70: Particle system foundation (pool + emitter + converter) — completed
- ✓ Step 71: Arcade integration sprint (9 systems adopted) — completed
- ✓ Step 72: Physics hardening for platformer (statics + controller + tests) — completed
- ✓ Step 73: Camera lerp follow (followLerp, arcade smooth) — completed
- ✓ Step 74: Platformer proof game (v1.0 gate) — completed
- ✓ Step 75: Audio volume control (master + sfx) — completed
- ✓ Step 76: 64x64 procedural player sprite — completed
- ✓ Step 77: Shader system (loadable GLSL files) — completed
- ✓ Step 78: Sprite batching (texture-group draw call reduction) — completed
- ✓ Step 79: 2D point lighting (lit.frag + LightingState) — completed
- ✓ Step 80: Per-sound volume (Beep/GameOver/NewHighScore) — completed
- ✓ Step 81: Scene serialization (scene_*.txt v1) — completed
- ✓ Step 82: Menu instructions + HUD labels (TIME/BEST) — completed
- ✓ Step 83: Orphan annotation + contract freeze (docs-only) — completed
- ✓ Step 84: OOB textureId log + doc refresh (83→84, shader/lighting taxonomy) — completed
- ✓ Step 85: Pong score + win (first to 5) — completed
- ✓ Step 86: Particle system depth (color via tint + emit wired) — completed
- ✓ Step 87: Optional fixed-timestep (prevent 1-unit tunnel) — completed
- ✓ Step 88: Basic input action map (Move/Jump/Pause/Confirm) — completed

This list is intentionally not the step queue. Steps still get pulled one at a time from real code inspection, and `kill_criteria` still applies to any future work.

**Step 52 verdict:** Broad-phase optimization deferred. Engine scales O(n)
with entity count; playable to 2000 hostiles with acceptable frame time
(~5.5ms). At 5000 hostiles (~12ms), optimization becomes necessary for
smoother gameplay. Current Phase 2 scope does NOT require broad-phase.

**Phase B triggers (no Step 54 opened):** no demonstrated need exists for any
of the three candidates — spatial partitioning (Step 52 measured O(n), fine
to 2000), a layer/group system (depth field + automatic stable-sort already
cover draw ordering), or an in-game editor (no requesting use-case). Open a
Step 54 when ONE of these becomes true: (A) a second draw-layering rule is
needed that raw depth ints cannot express; (B) a stress run shows frame time
climbing superlinearly or breaching budget below 2000 hostiles; (C) a concrete
level-design task requires in-world tweaking without recompile.

## Reference: 2D/3D Architecture Notes (non-binding)

This section is informational only. It is not a roadmap, not a step queue, and not a proposal to pre-plan future work. Its purpose is to document the architectural distinction between a flat 2D game and a true 3D engine so a future real need can be evaluated against earlier analysis instead of speculation.

PureEngine's committed direction is flat 2D. The project is intentionally built around a single world plane, shared transform assumptions, and a renderer that treats the scene as a 2D playfield rather than a full 3D world. A flat-to-2D extension is a natural evolution of the current architecture if a concrete need appears, such as:

- overlapping entities rendering wrong because depth ordering is implicit rather than explicit
- a camera/view split becoming necessary for a larger world or staged presentation
- transform/render decoupling becoming useful for world-space vs screen-space logic
- explicit depth or layer fields being needed for draw ordering and collision grouping
- broad-phase collision becoming a measurable bottleneck at substantially higher entity counts

Those are all changes that keep the project in the same design family: still a 2D game, just with clearer separation of concerns and more scalable world logic. They do not require replacing the engine's current model; they are natural extensions of it.

A 2D-to-3D transition is not an extension. It is a different project. It requires a new transform system, a different rendering pipeline, and a different physics model. A 3D project needs world matrices and camera projection for real depth, a different scene graph or spatial structure, different material and lighting assumptions, and a substantially different collision model. That is not a minor version bump of a flat arcade engine; it is a different technical foundation.

This section exists so that any future genuine need can be assessed honestly against the actual project direction and history: flat 2D remains the intended scope, and any 3D discussion should be treated as a separate endeavor rather than a default evolution of this codebase.

**Stress-test result (Step 45, re-verified post-Steps 46-49):** Tested at 3, 20, and 50 hostiles using data-driven hostile_default.txt variants. Average frame time stayed flat at ~1.5�1.6 ms across all three counts. Re-measured at 50 hostiles after Steps 46-49 (EntityRole branching, depth-sort): 1.47�1.80 ms (avg ~1.6 ms), matching the Step 45 baseline within normal noise. No disproportionate cost from the added branching or sort. Broad-phase collision remains correctly deferred � the earlier inference is now confirmed by measurement.

**2D architectural maturity status:** 4/5 criteria met (camera split,
transform decoupling, fixed coords, depth field). Broad-phase
collision deliberately deferred, backed by 3 rounds of stress-test
evidence (see Step 45/46-49 notes). Re-evaluate only if a future
stress test at a materially higher entity count shows real cost.

## Known Follow-ups

- Event audio bleed on MENU return (GAMEOVER.wav / win_sound.wav) — addressed in Step 48 with corrected ma_sound_stop() placement. Resolved; no remaining action.
- **Lighting visual tuning (Step 79 follow-up):** Tuned and display-proven 2026-09-12 — ambient `0.03,0.03,0.06` + radius `5.0` world + intensity `1.8` + background `0.02,0.02,0.08` (PLAYING) `file_path:src/main.cpp:1545`, `file_path:src/gamestate.h:111`. World-space path `u_entityWorldPos` `file_path:assets/shaders/lit.vert:5`, `file_path:src/renderer.h:364` verified (lit.frag active, clip-space bug fixed `faf1052`). Arcade PLAYING shows warm circle around player, corners darker (0.39 at 10u vs 1.0 at center), falloff visible. Resolved; unlocks Step 80.
- **Packager drift F-08 re-fix (2026-09-12):** `scripts/package.ps1` single source (16 files `file_path:scripts/package.ps1:11` + `assets/shaders/`). `CMakeLists.txt:98` mirrors it `file_path:CMakeLists.txt:98`. Guarantees `arcade_arena.txt`, `animation_default.txt`, `platformer_step72_proof.txt`, `paddle_spritesheet.png` + `paddle_animations.txt` in zip. Verified: `package/PureEngine-0.1.0-win64.zip` 16 assets + shaders, `build/Release/assets` 16 assets, clean-dir arcade alive `3s` with tiles/anims (8 rows proof). No other systems.
- **Platformer Step 72 proof (2026-09-12):** `assets/platformer_step72_proof.txt` minimal floor+platform+win (10×8) reuses existing `tilemap.h` + `physics.h:215` `updateCharacterController` (isStatic `file_path:src/entity.h:110`, coyote `file_path:src/physics.h:222`) — no new headers, no batching/lighting/audio changes. Proves grounded spawn `-2.68`, jump apex `2.5`, coyote edge, land, win pad via `games/platformer/platformer.cpp:385` static conversion. `ctest 47/47`, `Platformer.exe` alive 4s. Recorded as Step 72 completion proof (Step 74 full game remains).
- **Ship-blocker bundle (2026-09-12):** P-01 packager re-verified 16-file single source `file_path:scripts/package.ps1:11` + `file_path:CMakeLists.txt:98` (includes `arcade_arena.txt`, `animation_default.txt`, `platformer_step72_proof.txt`, `paddle_*`); P-02 anim keep `file_path:src/main.cpp:830` reassigns `walk_left` per Hostile after `buildInitialEntities` in `activateScene` (alt scene retains anim); F-01 pointer discipline `file_path:src/scene.h:26` re-take + `reserve(4)` `file_path:src/main.cpp:703`; F-04 tile cache `file_path:src/main.cpp:721` per-scene `cachedTileEntities` (rebuild on switch, not per-frame) + `assert(entities==colliding)` `file_path:src/renderer.h:395`. `ctest 47/47`, arcade/platformer alive, clean-dir zip 16 assets + shaders.

## Asset Classification (Stress Fixtures vs Runtime Assets)

The four stress configuration files (`assets/hostile_stress_50.txt`, `assets/hostile_stress_500.txt`, `assets/hostile_stress_2000.txt`, `assets/hostile_stress_5000.txt`) are stress-test-only fixtures used for offline performance measurement and scaling experiments. They are not runtime game assets, have no active consumer in `src/`, and are intentionally excluded from CMake build directory asset copying and release packaging.

**Post-Step-50 audit cycle status:** Post-Step-50 audit cycle (three independent audits) resolved: Step 44 tracker gap, doc-comment style normalization across 4 files, README/tracker drift, CTest registration, FrameTime delta clamping. All verified via real build + ctest output, not summarized claims. Remaining known items (audio cursor sharing between event/collision sounds, per-entity texture bind cost, magic-number scenery count already parameterized in Step 46) are candidates for a FUTURE session, not urgent - flat frame time confirmed at 50 hostiles, no measured pressure to act on any of them now.

**Nemotron audit fact-check (Critical Bug #3 - Audio cursor sharing):** Verified FALSE against real source. `gameOverSound` and `newHighScoreSound` are separate dedicated `ma_sound` instances, not part of the `sounds[]` pool, and no shared cursor exists. No code change was needed. Recorded here so future sessions do not re-investigate an already-debunked claim.

**Nemotron audit fact-checks (time.h CRT shadowing & renderer.h destroyAll ordering):** (1) time.h CRT-shadowing claim verified FALSE (no <time.h> inclusion or symbol collision). (2) renderer.h destroyAll ordering claim verified FALSE — shader program is deleted before textures (glDeleteProgram at line 566, glDeleteTextures at lines 567-572).

**Independent fact-check finding (time.h stale doc comment):** Doc comment in time.h was stale since before the delta-clamp commit (fdc2182), now corrected — this was NOT part of the original Nemotron audit, found independently during fact-checking.

## Kill criteria
If any step's scope keeps expanding instead of shrinking, stop, cut scope, and re-record a smaller definition_of_done before continuing. Do not introduce an abstraction, manager, registry, or subsystem unless the current implementation demonstrates a concrete need for it.
