# PureEngine — Build Continuation (v3)

## Context
PureEngine Steps 25 through 159 are complete and verified (Steps 25–34 in the v2 tracker, Steps 35–117 here). This continuation begins from the hardened state established after the v2 tracker was superseded: data-driven hostile config, alternate scene reuse, periodic frame-time visibility, and the automated hostile parser test target.

The current project remains intentionally minimal. The source and build output remain the authority, and this tracker is only a forward pointer for future work.

## Goal
Continue from the verified engine hardening work without inventing new abstractions, managers, or registries before a concrete need is demonstrated.

## Rule
No step starts until there is a real demonstrated need in the source, the build, or a required conversation-driven follow-up. Steps 25 through 34 are tracked in `Blueprint/pure_engine_v2_steps.json`; Steps 35 through 159 are tracked in `Blueprint/pure_engine_v3_steps.json` (Step 44 marked "superseded" by Step 48) and cover engine hardening (25-34), data-driven hostile config, alternate scenes, depth sorting, entity roles, stress-test validation, role-based collision scanning (51), frame-time scaling measurement (52), player foreground layering (53), entity role genericization (54), renderer texture index slots (55), animation data structures (56), renderer animation frame UVs (57), data-driven animation loading (58), animated Pong proof (59), physics foundation (60), physics integration (61), collision response (62), tilemap system foundation (63), scene management foundation (64), transform hierarchy foundation (65), font/text rendering foundation (66), event system foundation (67), debug console foundation (68), gamepad input foundation (69), particle system foundation (70), Arcade integration sprint (71), physics hardening for platformer (72), camera lerp follow (73), platformer proof game (74), audio volume control (75), 64x64 player sprite (76), shader system (77), sprite batching (78), 2D point lighting (79), per-sound volume (80), scene serialization (81), menu/HUD polish (82), orphan/contract freeze (83), OOB textureId log + doc refresh (84), Pong score + win (85), particle system depth (86), optional fixed-timestep (87), basic input action map (88), texture slot growth + registry (89), console history + scrollback (90), event polish (91), time scale (92), animation helper (93), resource unload (94), console wrap (95), debug overlay (96), spatial helper (97), audio music slot (98), hierarchy freeze (99), engine freeze v1.0 (100), resource cache & blob (101), pe_core static lib (102), lightweight component helpers (103), editor-time scene dump/load (104), loadable input rebinding (105), fix animation clip loss on scene switch (106), and fix raw scene pointer discipline (107), wire applyPhysics into PLAYING loop (108), OOB textureId/tile warning in debug builds (109), wire Emitter::emit for player dust (110), and persistence v2 versioned full-fidelity serialization (111), world-sprite alpha blending (112), and runtime entity lifecycle with alive flag plus spawn/kill API (113), tracker consistency pass plus Platformer proof verification and lifecycle guard (114), and rebinding confirmation plus Platformer bindings usage with missing-include fix (115), and resolution/viewport handling with Camera::onResize plus GLFW callback (116), and mouse input with position plus button polling and edge detection (117), and minimal Button widget (118), and music integration wiring playMusicLoop/stopMusic (119), and prefab/template system loadPrefab plus instantiatePrefab (120), and MENU Button integration START/ALT plus click handling (121), and prefab used in a live game with spawn_prefab clip resolution plus end-to-end scene spawn test (122), and music asset music_loop.wav plus bundle copies closing the Step 119 gap (123), and mute/master volume control toggleMute plus M key plus console mute (124), and screen-to-world mouse helper screenToUi plus Camera wrappers (125), and world-to-screen helper uiToScreen plus Camera wrappers with round-trip tests (126), and point pick against entity AABBs pickEntity plus overlap rule (127), and screen-space pick convenience wrapper pickEntityAtScreen plus test-expectation fixes (128), and entity world AABB helper WorldAABB plus entityWorldAABB (129), and Arcade console pick command as live consumer for the pick trio (130), and Arcade console blob command as live consumer for the Step 101 binary blob API (131), and Arcade console health/damage/heal proof as live consumer for the Step 103 component helpers (132), and Arcade console textures register/unload round-trip as live consumer for the Step 94 unload (133), and README plus AGENTS documentation drift sweep (134), and .gitignore hygiene for IDE/agent folders plus diagnostic logs (135), and spawn-dump-reload persistence interaction proof composing prefab plus lifecycle plus v2 serialization (136), and manual smoke-test checklist docs-only (137), and package single-source final verify with prefabs dir added to the zip (138), and v1.1 toolkit freeze document docs-only (139), and tracker final consistency pass with zero drift found (140), and final freeze confirmation read-only audit all green (141), and freeze guardrails mirrored into AGENTS.md (142), and package.ps1 dry-run checklist note (143), and console help lists-all-commands confirmation plus test deepening (144), and entityWorldAABB debug path confirmed already wired via console pick (145, no-code), and GameState gate-table headless test (146), and highscore file semantics headless test (147), and input bindings reinforcement failed-load-keeps-overrides (148), and README v1.1 frozen section (149), and performance budget statement docs-only (150), and post-freeze health audit read-only all green (151), and README/AGENTS step-count catch-up docs-only (152), and recommended-stop marker docs-only (153), and the scene/prefab/persistence test campaign manager save plus spawn-after-kill plus multi-spawn persist (156), and the input keysForAllActions union helper with test (157), and the input gamepad-action bridge with Action::Console rebindable toggle (158-159, input campaign complete).

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
- ✓ Step 89: Texture slot growth + registry (growable vector, 6th texture) — completed
- ✓ Step 90: Console history + scrollback (Up/Down recall, 64 ring) — completed
- ✓ Step 91: Event system polish (noexcept, once, throw-safe) — completed
- ✓ Step 92: Time scale / pause-aware clock — completed
- ✓ Step 93: Animation clip switch helper — completed
- ✓ Step 94: Basic resource unload — completed
- ✓ Step 95: Console multi-line / wrap — completed
- ✓ Step 96: Debug overlay expansion (ENTS/TILES/DT) — completed
- ✓ Step 97: Simple spatial helper (broadphaseGrid) — completed
- ✓ Step 98: Audio music / loop slot — completed
- ✓ Step 99: Hierarchy contract test + final freeze — completed
- ✓ Step 100: Engine freeze + v1.0 readiness (docs-only) — completed
- ✓ Step 101: Resource cache & binary blob (loadBinaryBlob + pack) — completed
- ✓ Step 102: pe_core static lib (header-only still alive) — completed
- ✓ Step 103: Lightweight component helpers (health/tag/timer/velocity) — completed
- ✓ Step 104: Editor-time scene dump/load (console dump/reload) — completed
- ✓ Step 105: Loadable input rebinding (Action=Key1,Key2) — completed
- ✓ Step 106: Fix animation clip loss on scene switch (clip name on Entity) — completed
- ✓ Step 107: Fix raw scene pointer discipline (reserve + document) — completed
- ✓ Step 108: Wire applyPhysics into PLAYING loop (was orphan) — completed
- ✓ Step 109: OOB textureId warning (debug builds, one-per-id) — completed
- ✓ Step 110: Wire Emitter::emit for player dust (was orphan) — completed
- ✓ Step 111: Persistence v2 (versioned full-fidelity scene serialization) — completed
- ✓ Step 112: World-sprite alpha blending (GL_BLEND in drawWorldInternal) — completed
- ✓ Step 113: Runtime entity lifecycle (alive flag + spawn/kill API) — completed
- ✓ Step 114: Tracker consistency + minimal Platformer proof (verify, 1-line lifecycle guard) — completed
- ✓ Step 115: Rebinding confirmed + minimal proof (unordered_map include, Platformer usage + bundle) — completed
- ✓ Step 116: Resolution/viewport handling (Camera::onResize + GLFW callback) — completed
- ✓ Step 117: Mouse input (position + button polling + edge detection) — completed
- ✓ Step 118: Minimal Button widget (Button struct, hitTest, drawButton) — completed
- ✓ Step 119: Music integration (playMusicLoop/stopMusic wired) — completed
- ✓ Step 120: Prefab/template system (loadPrefab + instantiatePrefab) — completed
- ✓ Step 121: MENU Button integration (START/ALT buttons + click handling) — completed
- ✓ Step 122: Prefab used in a live game (spawn_prefab clip resolution + end-to-end scene spawn test) — completed
- ✓ Step 123: Music asset + audible path proof (music_loop.wav) — completed
- ✓ Step 124: Mute / master volume control (toggleMute + M key + console mute) — completed
- ✓ Step 125: Screen-to-world mouse helper (screenToUi + Camera wrappers) — completed
- ✓ Step 126: World-to-screen helper (uiToScreen + Camera wrappers) — completed
- ✓ Step 127: Point pick against entity AABBs (pickEntity + overlap rule) — completed
- ✓ Step 128: Screen-space pick convenience wrapper (pickEntityAtScreen) — completed
- ✓ Step 129: Entity world AABB helper (WorldAABB + entityWorldAABB) — completed
- ✓ Step 130: Arcade console pick command (live consumer for Steps 127-129) — completed
- ✓ Step 131: Arcade console blob command (live consumer for Step 101) — completed
- ✓ Step 132: Arcade console health/damage proof (live consumer for Step 103) — completed
- ✓ Step 133: Arcade console unload proof (live consumer for Step 94) — completed
- ✓ Step 134: README + documentation drift sweep (docs-only) — completed
- ✓ Step 135: Junk cleanup / .gitignore hygiene — completed
- ✓ Step 136: Spawn -> dump -> reload persistence interaction proof — completed
- ✓ Step 137: Manual smoke-test checklist (docs-only) — completed
- ✓ Step 138: Package/asset single-source final verify (prefabs/ added) — completed
- ✓ Step 139: v1.1 toolkit freeze document (docs-only) — completed
- ✓ Step 140: Tracker final consistency pass (docs-only) — completed
- ✓ Step 141: Final freeze confirmation (read-only audit + short record) — completed
- ✓ Step 142: Freeze guardrails in AGENTS.md (docs-only) — completed
- ✓ Step 143: package.ps1 dry-run checklist note (comment-only) — completed
- ✓ Step 144: Console help lists all commands (test deepening) — completed
- ✓ Step 145: entityWorldAABB debug path (no-code confirmation) — completed
- ✓ Step 146: GameState gate-table headless test — completed
- ✓ Step 147: Highscore file semantics headless test — completed
- ✓ Step 148: Input bindings reinforcement — completed
- ✓ Step 149: README v1.1 frozen section (docs-only) — completed
- ✓ Step 150: Performance budget statement (docs-only) — completed
- ✓ Step 151: Post-freeze health audit (read-only) + record — completed
- ✓ Step 152: README/AGENTS step-count catch-up (docs-only) — completed
- ✓ Step 153: Recommended stop marker (docs-only) — completed
- ✓ Step 154: Input rebind lifecycle reset (resetActionOverrides) — completed
- ✓ Step 155: Input keyNameToGLFW widening + direct test — completed
- ✓ Step 156: Scene/prefab/persistence test campaign (manager save, spawn-after-kill, multi-spawn persist) — completed
- ✓ Step 157: Input keysForAllActions() union helper + test — completed
- ✓ Step 158: Input gamepad-action bridge — completed
- ✓ Step 159: Input Action::Console (rebindable toggle, campaign complete) — completed
- . Step 160: gamepad-action bridge adopted in games (Jump+Pause) + keysForAllActions union includes Action::Console (default 12->13, remap 13->14) — recorded (126 entries); adoption commits 3e51b8f/8bfd7e8 carry duplicate messages (tangle, left as-is); union follow-up 2cacf5f split-staged (scene in-flight test hunks left unstaged)
- . Step 161: Scene loadSceneManagerFromFile + manager round-trip test — recorded (127 entries); fresh sanity green before commit (ctest 1/1 Passed 2.28s)
- . Step 162: Scene manager save path symmetry + Windows re-save rename fix (fs::rename onto existing file) + test case 5e — recorded (128 entries); scene campaign complete (load + symmetry + re-save)
- . Step 163: Resources load system contract test (failure-not-cached, copy independence, pack hit/fallback semantics) — recorded (129 entries); boundary already held, gaps were test gaps only
- . Step 164: Animation system contract (load/bind/update/switch/loop/end) — recorded (130 entries); no engine source changes
- . Step 165: Camera followLerp headless contract (blend/convergence/overshoot/clamp) — recorded (131 entries); split-staged around events in-flight hunks; chain-gating pending events commit
- . Step 166: Resources system CONTRACT block (probe/failure/cache/ownership) + dedicated headless resources_test CTest target — recorded (132 entries)
- . Step 167: Events contract gaps (reentrant/once/throw/mid-dispatch) + followLerpOk chain gate fixed — recorded (133 entries)
- . Step 168: Console contract gaps (wrap/cap, empty-history recall, submitHistory-vs-clear, closed-console no-op) + glad link into hostile_data_test — recorded (134 entries)
- . Step 169: Particles system contract headless lock (spawn guard, accumulator carry, life conversion, swap-with-back) — recorded (135 entries); no src changes
- . Step 170: Time/timestep contract headless lock (tick advance, 0.1s clamp observed, pause/scale semantics; glfwInit self-contained) — recorded (136 entries); no time.h changes
- ✓ Step 158: Input gamepad-action bridge (fixed gamepadButtonsForAction mapping + optional GamepadState tails on isActionDown/Edge) — completed
- ✓ Step 159: Input Action::Console (rebindable console toggle; loader maps console properly, pumps use isActionEdge(Console)) — completed

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

## Engine freeze — v1.0 readiness (Step 100)
**In:** v1.0 core frozen at Step 100 (cumulative Steps 1–100): header-only, window/context, math, entity/collision/state, renderer+batching+OOB log, shader/lighting, resources (growable registry + unload), camera (follow/limits), input (raw + Action), time (scale/pause + fixed substeps), lifecycle, audio (master/sfx/per-sound/music loop), ui (HUD+menu), simulation/physics (statics, controller, fixed), tilemap/scene (serialization + cache + asserts), hierarchy (attachment-only), font, events (noexcept/once), console (history/wrap), gamepad, particles (color via tint), Pong/Platformer/Arcade proven. **Frozen:** no ECS, no broadphase replacement, no TRS, no material/editor/net/hot-reload. **Deferred:** full materials, editor, networking, 3D, hot-reload. Docs updated: README/AGENTS step count 100, PURE_ENGINE_V3.md Steps 25–100. Steps 101+ are an explicit additive Phase 2 continuation on the frozen v1.0 core: new capabilities only, no frozen behavior changed or unfrozen.

## Engine freeze — v1.1 toolkit (Step 139)

**In (capability map, all with at least one proven consumer or a headless test + documented future-use status):** window/context, math (Vec3/Mat4), entity/collision/gamestate, renderer (batching, OOB guard, alpha blending, lit path), shader loading (GLSL files), resources (texture load, binary blob/pack + cache, unload), camera (follow/lerp, resize/viewport, screen↔world, world-to-screen), input (raw keys, Action map, file rebinding, mouse snapshot/edges, gamepad), time (scale/pause, fixed substeps), lifecycle (snapshot restore, alive/spawn/kill), audio (SFX pool, event sounds, master/sfx/per-sound/music loop + asset, mute), ui (HUD, Button + MENU click, screen-space text), simulation/physics (statics, controller, coyote), tilemap, scene (SceneManager, serialization v1/v2 + manager save), hierarchy (attachment-only), font, events (noexcept/once), console (history/wrap + 14 game commands incl. pick/blob/health/spawn_prefab/textures), particles, prefab system (load/instantiate + live spawn), spawn→dump→reload persistence proof. Three games green: Arcade, Pong, Platformer.

**OUT (do not plan, separate endeavor if ever needed):** no ECS, no editor, no networking, no 3D, no hot-reload, no retained-mode UI framework, no mixer graph, no broadphase replacement (measured unnecessary ≤2000 hostiles, Step 52), no materials system.

**Freeze lineage:** v1.0 core froze at Step 100 (cumulative Steps 1–100); Phase 2 is an explicit additive continuation through the current step (Step 138 recorded) — new capabilities only, frozen v1.0 behavior unchanged and unfrozen.

**Honest limits (not CI-verified, human-only — see Blueprint/SMOKE_TEST.md):** audible audio (music loop seamlessness, mute feel, event cue mix), mouse click feel in the MENU, console typing paths, resize feel. The automated suite (78 behavior cases) proves the underlying logic; the human checklist is the release gate.

## Performance budget statement (Step 150, docs-only)

Standing stance on the entity/draw path, unchanged by the post-freeze polish:

- **Entity path scales O(n)** and is fine ≤2000 entities (Step 52:
  ~1.21 ms at 50, ~12.09 ms at 5000; the post-Steps 46-49 re-measure
  matched the baseline within noise). **Broadphase stays deferred** —
  re-open ONLY on a measured superlinear breach below 2000 entities
  (the v1.1 freeze OUT list carries the same ruling).
- **Batching already present** (Step 78: texture-group draw-call
  reduction); no further draw-path work is justified by any current
  measurement.
- Integration workload (README Performance note): tilemap→entities for
  the 14-tile arena + 24-particle update/convert ~0.003 ms/frame;
  10k-tile conversion ~3.8 ms one-off; tile collision ~70 ns; the live
  arcade scene runs at several hundred fps.
- Nothing here justifies new systems: the budget statement exists so a
  future measurement is compared against these recorded numbers instead
  of speculation.

## Recommended stop (Step 153, docs-only)

**The v1.1 toolkit is COMPLETE for the stated goal** (Steps 141/151
confirmations, all green). The project deliberately marks a clean
recommended stop under the freeze:

- Further steps happen ONLY on a measured need in the source, the
  build, or a required conversation-driven follow-up — not as a queue,
  not as polish momentum, not to justify activity.
- The freeze guardrails (AGENTS.md, Step 142) and the OUT list above
  remain binding: still no ECS, no editor, no networking, no 3D, no
  hot-reload, no retained-mode UI, no mixer graph, no broadphase
  without a measured >2000-entity breach.
- Re-entry criteria: a measured superlinear frame-time breach, a
  concrete engine-validation need demonstrated in real conditions, or
  an explicitly requested step citing one.

This note exists so a future session reading the tracker sees an
explicit, sanctioned stopping point instead of inferring one.

## Multi-session coordination

Live board: Blueprint/SESSION_BOARD.md — does not replace step history.

## Integration backlog — system sessions in flight (main coordination note)

Observed working-tree state at the recommended stop (HEAD 16e926f, not
committed by main — these are the system sessions' in-flight files):

- **PureEngine_audio** (owns audio.h + its wiring): in-flight =
  queryable load state (isLoaded(Sound)/isMusicLoaded) + load-failure
  log normalized to stderr + main.cpp isMusicLoaded read.
  **Expected DoD:** build clean, CTest green with its in-flight tests,
  games alive, honest audible-not-CI note, code commit + step record at
  its own step number.
- **PureEngine_input** (owns input.h + its tests): in-flight =
  resetActionOverrides() labeled Step 154 (reset half of the rebind
  lifecycle: load -> remap -> reset -> defaults).
  **Expected DoD:** same verification bar + a lifecycle round-trip
  proof (remap survives failed loads, reset restores Step 88 defaults).
- **PureEngine_physics** (owns physics.h): no in-flight changes visible
  in the working tree.

**What MAIN does when a system session finishes and merges:** record the
real hashes + step counts in pure_engine_v3_steps.json and this file
(status_note/context/rule-tail/list), refresh the README step count,
update Blueprint/SMOKE_TEST.md if new console commands or keys appear,
run full verification (build/CTest/alive), and re-check the
recommended-stop marker against the re-entry criteria. **MAIN does not:**
implement audio/input/physics, commit another session's in-flight files,
or force a conflict.

## Kill criteria
If any step's scope keeps expanding instead of shrinking, stop, cut scope, and re-record a smaller definition_of_done before continuing. Do not introduce an abstraction, manager, registry, or subsystem unless the current implementation demonstrates a concrete need for it.
