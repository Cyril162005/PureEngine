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
- . Step 171: Engine lifecycle/init contract — documented init order from source + audio init idempotent guard (double-init no-op, device-backed green) — recorded (137 entries)
- . Step 172: Input checkGamepadActions gamepad-action mapping contract — recorded (138 entries); no input.h changes
- . Step 173: Physics sweptAABB slab package + tunnel-catch contract (SweepHit/sweptAABB + chain gate) — recorded (139 entries); API-only, no game wire yet; Step 172 sweep-in disclosed
- . Step 174: Core loop contract — Pong tick moved above poll (sampling invariant fixed) + loop-order doc — recorded (140 entries); two mid-write races disclosed
- . Step 175: Physics sweptMoveAndCollide swept twin + P7 contract test (tunnel catch, earliest-hit) — recorded (141 entries); API-only, no game wire yet
- . Step 181: Physics controller useSwept opt-in + sweptAABB boundary-start refinement (touching = NO hit) + checkSweptControllerContract — recorded (147 entries)
- . Step 182: MAIN core slice verify pass (80-step run Steps 1-10) — already holds; board double-init note — recorded (148 entries)
- . Step 183: RENDER slice verify pass (Steps 11-20) — already holds — recorded (149 entries)
- . Step 184: PHYSICS slice verify pass (Steps 21-30) — already holds; useSwept default-false = discrete ASSERTED — recorded (150 entries)
- . Step 185: SCENE slice verify pass (Steps 31-40) — already holds — recorded (151 entries)
- . Step 186: RESOURCES slice verify pass (Steps 41-50) — already holds; 0-byte fixture tested — recorded (152 entries)
- . Step 187: INPUT slice verify pass (Steps 51-60) — already holds; rebind re-adopt residual noted — recorded (153 entries)
- . Step 188: ANIMATION slice verify pass (Steps 61-70) — already holds; multi-track/blend later-not-started — recorded (154 entries)
- . Step 189: AUDIO slice verify pass (Steps 71-80) — already holds — recorded (155 entries). 80-step run COMPLETE: full ctest 2/2 + alive x3, all departments verified.
- . Steps 214-218: the 3D arc chain - drawEntity3D yaw from rotationAngle (214), debug3d fov (215), cam reset + the per-frame setEyeTargetUp clobber fix (216 - the Step 213 cam command was broken until this), Entity::tint on the 3D debug draw via drawEntityMesh3D (217), and the 3D arc checkpoint with an integration glue test (218) - recorded (183 entries). 2D non-regression held every step.
- . Steps 220-224: the physics-in-games chain - Platformer ice (cell 2, friction 0) + bouncy (cell 3, restitution 1) tiles through the controller resolve (220; the tile contract: bounciness = restitution above the 0.5 baseline; the default ground stop = a quick ~3-substep decay - the intentional delta documented), the Pong wire verify-only (221 - Step 209 had already landed it), the Cyril playtest invitation (222 - verdict pending, his call is the completion criterion), the spawn mid-update safety verify-only (223 - already safe, no spawn inside a range-for), and the checkpoint (224 - Step 236 RECONCILIATION: the "208 finding closed" here was PREMATURE - the real completion evidence is Step 226 (Cyril's verdict on the tile playtest) plus Step 235 (Cyril PASS on the resting-contact fix); deferrals as stated: orbit controls, meshId==2, debug3d drop (since LANDED, Steps 228/231/235), the physics->3D bridge) - recorded (189 entries).
- . Steps 237-245: the 3D mesh load chain - the first real 3D mesh load (237: OBJ triangle-soup loader + the loaded-mesh slot, meshid 3 opt-in; the sample tetra), the Pong RESTING_VEL scoping (238), the Pong velocity mirror + flip read-back (239-242: the resolve never saw the ball's velocity - the grind since 209; the goal bound at the live view edge; both-axes read-back + the dev-only score log), the font digit divisor 11->37 fix (243: the HUD digits were 3.4-cell mashes since the Step-66 atlas extension), the mesh REGISTRY (244: meshId -> loaded OBJ map, clearRegisteredMeshes, the tetra under two ids), and the per-face planar UVs (245: the registered meshes SAMPLE the diffuse checker - the pixel proof variance 235 vs the flat 0) - recorded through 245 in pure_engine_v3_steps.json; Cyril CONFIRMED 243 (readable digits + one miss -> one log). 2D non-regression held every step.
- . Step 246: registered meshes sample entity.textureId when valid - the 2D path's OOB/released validation rule (a stale textureId can never bind GL name 0); meshId 1/2 never read textureId (identical face colors proven); the pixel proof variance 23 (crimson) vs 235 (checker).
- . Step 247: basic 3D mesh lighting - CPU per-face directional (outward normals via the centroid, one fixed light + ambient modulating the existing tint; one draw per face, no shader change); the lit-vs-ambient pixel difference measured (135 vs 91).
- . Step 248: the entity mesh path's depth CONFIRMED (verify-only, no code) - the same save/enable/restore pattern as Steps 195/201 (renderer.h:824/825/883-885); ONE disclosed deviation: the always depth-only clear (if (true), renderer.h:826-828, since Step 237) vs drawDebugMesh3D's opt-in - the inter-entity occlusion wipe is a future-step candidate.
- . Step 249: the single depth clear per 3D pass - the Step 201 opt-in param (the same mechanism as drawDebugMesh3D), not the per-entity always-clear; the inter-entity occlusion proven order-independent (near 0.9235 / far 0.9394 -> all 0.9235).
- . Step 250: the entity mesh path's depth enable-state round-trip CI (checkMeshDepthState, the checkDepthState pattern) - the 248 disclosure closed; the occlusion half was already checkMeshOcclusion (249).
- . Step 251: the second real OBJ (a wedge, mesh_wedge.obj) loaded at init into the registry at meshId 5 - a DISTINCT second mesh (the multi-mesh registry evidence); the 245/246 behavior on the path by construction.
- . Step 255: finding (c) RESOLVED - the entity mesh path's GL state-leak CI (checkMeshStateLeak:6033, the checkDebugFrameDepth twin): the color corner survives the depth-only clear; the depth enable, blending, program, and VAO states deterministic - all five items green.
- . Step 257: Phase D - the proof consumer of the frozen 3D version (checkFrozen3DConsumer, adversarial composition: the re-register replaces the slot, the texid fallbacks, the clear mid-session, the order-independent occlusion, the directional lighting) - the frozen contract held end-to-end; NO src/ engine edits needed.
- . Step 176: Audio simultaneous SFX+music volume matrix (headless) — recorded (142 entries); chain-reference sweep into 175 disclosed
- . Step 177: Input Input(vector) adoption ctor + union-order contract test — recorded (143 entries)
- . Step 178: Scene prefab-scene failure matrix headless contract — recorded (144 entries)
- . Step 179: Render draw/submit contract doc (headless vs SMOKE split) — recorded (145 entries)
- . Step 180: Resources released-slot checker fallback + texture lifetime contract + test — recorded (146 entries)
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

## 3D debug sandbox contract (Step 236, docs-only — recorded from source)

The debug sandbox is the opt-in diagnostic path for physics/3D testing
in Arcade. Commands (all console, default OFF unless typed):

- `debug3d on|off` — the 3D debug view toggle (Step 202; the ortho mode
  is restored exactly on off, the Step 193/196 round-trip).
- `debug3d cam <ex> <ey> <ez> <tx> <ty> <tz>` — setEyeTargetUp (Step 213;
  the `cam reset` subcommand restores the documented defaults, Step 216).
- `debug3d fov <degrees 0-180>` (Step 215), `debug3d orbit <degrees>`
  (Step 229 — the only camera motion added; no free-fly controller).
- `debug3d drop` / `drop` — spawns a STATIC ground plane + a DYNAMIC box
  via the deferred spawn queue (Steps 227/228; both routes share the
  dropSandbox lambda, Step 231). PLAYING-ONLY: physics ticks only in
  PLAYING, so a drop in MENU/PAUSED replies
  "drop: physics only runs in PLAYING" (stated, not silent).
- `meshid <index> <id>` — binds a live entity to the 3D debug draw
  (Steps 212/219; the cube follows the entity as it moves).
- `nohostiles on|off` — the debug hostile toggle (Step 234): on clears
  every ACTIVE Hostile-role entity AND pauses future spawning
  (activateScene builds no hostiles while on); off resumes normal
  spawning, no retroactive respawn. Player/Scenery/Sandbox untouched.

Mechanics (engine-pure, CI-proven unless noted):
- `ArcadeRole::Sandbox = 3` (lifecycle.h, Step 232): the drop entities'
  distinct role — excluded from the Player/Scenery catch detector and
  the hostile chase by construction.
- `pe::resolveSandboxPairs` (physics.h, Step 233): the SANDBOX-SCOPED
  all-pairs resolve (Sandbox-vs-Sandbox, Sandbox-vs-static) via the same
  resolveCollision path. The engine-wide all-pairs resolver was
  DELIBERATELY REJECTED: resolving Player-hostile overlaps would prevent
  the touch that ends the run (Step 233).
- `RESTING_VEL = 0.5` (physics.h, Step 235): the resting-contact
  threshold — exceeds the per-frame gravity delta at 60 fps (9.8/60 =
  0.163) so a resting contact holds; below the threshold no bounce
  impulse fires and the normal velocity zeroes. Manual |.| throughout
  (no std::fabs — the collision.h constexpr-safe rule).
- Resolve BEFORE draw (main.cpp, Step 235): the sandbox resolve runs
  immediately after applyPhysics, so the rendered pose is the
  post-resolve one (the end-of-frame placement was a one-frame lag).

Honest limits (disclosed, unfixed unless stated):
- Session-transient entities: the sandbox drop entities are bound to the
  live session's queue/lifecycle — never saved into a scene file
  (saveSceneToFile snapshots activeScene->entities; a fresh session
  starts clean).
- Drawn mesh size != collision extents: drawEntity3D (renderer.h) scales
  the debug cube by e.scale ONLY (drawn size 2*scale), NOT
  2*halfExtents*scale — the drawn ground/box size does not match the
  collision extents (only the center matches). Flagged Step 235, unfixed
  (drawAABBs/F1 shows the true collision boxes for comparison).
- Shared threshold affects Pong: RESTING_VEL lives in resolveCollision's
  static-dynamic branches, so EVERY static-dynamic caller shares it — a
  Pong paddle contact with ball normal speed <= 0.5 would land dead
  instead of micro-bouncing. Pong ball speeds are ~3+, so behavior is
  unchanged in practice (verified by ctest).

## 3D capability map (Step 253, docs-only — every status cites a real test or is UNVERIFIED)

Tests live in tests/hostile_data_test.cpp. On-screen pixels are
HUMAN-ONLY (SMOKE) everywhere.

| Capability | Status | Owning file | Test | Known limit |
|---|---|---|---|---|
| OBJ triangle-soup loader | VERIFIED | src/mesh3d.h:102 (loadMeshFromObj) | checkMeshLoad (:6548), checkWedgeMesh (:5938) | v/f lines only — no vn/vt/quads/index buffers; malformed faces skip silently (holes possible in hand-written bad files) |
| Per-face planar UVs | VERIFIED | src/mesh3d.h:142 | checkTexturedMesh (:6807), checkMeshLoad (:6548) | the dominant-plane projection — no per-face artist control; even density per face (stated 245) |
| Mesh registry (multiple loaded OBJs) | VERIFIED | src/renderer.h:748-755 | checkMeshRegistry (:6689) | ONE map — no handles/generations; a re-register replaces the slot's data |
| meshId 1 (hardcoded cube) / 2 (pyramid) | VERIFIED | src/mesh3d.h:38/:65 | checkEntity3D, checkWedgeMesh (:5938), checkMeshLighting (:7058) | drawn size = 2*scale, NOT 2*halfExtents*scale (open finding (b)); UVs (0,0) — flat one-texel |
| meshId 3 (tetra, loaded at init) | VERIFIED | src/main.cpp (init load) + src/renderer.h:748 | checkMeshRegistry (:6689), checkTexturedMesh (:6807) | one sample asset; the documented meshid-3 path |
| meshId 5 (wedge, loaded at init) | VERIFIED | src/main.cpp (init load) | checkWedgeMesh (:5938) | one asset; meshId 4 = the tests' second tetra id (not a real asset) |
| textureId sampling (registered meshes) | VERIFIED | src/renderer.h (drawEntity3D resolve) | checkMeshTextureId (:6926) | textureId 0 (valid) binds tex_player on a mesh — the 2D rule, disclosed |
| CPU per-face directional lighting | VERIFIED | src/renderer.h:855-895 | checkMeshLighting (:7058) | flat per-face normals (no smooth shading); the fixed light (not adjustable); LOCAL-space lighting — a rotating mesh's lighting is mesh-fixed (open finding (a)); the texture confounds slightly (disclosed 247) |
| Depth: entity mesh path | VERIFIED | src/renderer.h (drawEntityMesh3D 824-828/883-885; drawEntity3D clearDepth) | checkMeshOcclusion (:7154), checkMeshDepthState (:5845) | the clear is opt-in (clearDepth, default false — the caller issues the one clear); the color never touched |
| Depth: debug path invariants | VERIFIED | src/renderer.h (drawDebugMesh3D) | checkDepthState (:5789), checkDebugFrameDepth (:5942) | the entity path's color-leak invariant has its dedicated twin since 255 (the row below) |
| GL state-leak invariant (entity mesh path) | VERIFIED | src/renderer.h (drawEntityMesh3D) | checkMeshStateLeak (:6033) | the clear is opt-in (clearDepth, default false); the color never touched; the program and the unit-0 texture bind are LEFT SET (deterministic — every consumer sets its own bind); the blend/VAO states round-trip |
| debug3d cam/fov/orbit | VERIFIED | src/main.cpp handlers + src/camera.h | checkCamParse (:7362), checkFovParse (:7241), checkOrbitEye (:7320) | no free-fly; the orbit is the only camera motion |
| Console: meshid/texid | VERIFIED (parse) | src/main.cpp + src/console.h (parseIndexId) | checkMeshIdParse (:7404), checkTexIdParse (:7435) | the runtime behavior (the reply + the binding) is SMOKE-only (main.cpp-local) |
| debug3d toggle/drop (sandbox) | VERIFIED (parse + physics) | src/main.cpp + src/physics.h | checkDebug3dParse, checkDropPhysics (:8022), checkSandboxResolve (:7644) | the on-screen pixels SMOKE-only (SMOKE_TEST 7.9-7.12) |

## Open engine findings (Step 253, docs-only — classified, NOT fixed)

### Mutation audit (Step 258, verify-only — the 257 asserts are NON-VACUOUS)

| # | Assert (test:line) | Guarded contract | Mutation | Result |
|---|---|---|---|---|
| A | the consumer's lighting assert (checkFrozen3DConsumer :6191, bright > dark + 20) | 247 CPU lighting (the directional lights faces differently) | the light direction FLIPPED (renderer.h:866) | ctest FAILED: +X 56 / -X 70 (inverted) + the 246/247 tests' asserts fired (the shared contract) — NON-VACUOUS |
| B | the consumer's texture assert (:6132, crimsonVar < checkerVar - 10) | 246 textureId sampling (a valid textureId samples its texture) | the valid path never samples (equivalent-safe; the literal OOB-sampled mutation is UB: entityTextures[99] OOB access) | ctest FAILED: crimsonVar 166 == checkerVar 166 — NON-VACUOUS |
| C | the consumer's re-register assert (:6116, reRegDepth ~ wedgeDepth) | 244 registry (a re-register REPLACES the slot's data) | registerMesh ignores replacements | ctest FAILED: reRegDepth 0.923483 (the tetra stayed) vs wedgeDepth 0.920854 — NON-VACUOUS |
| D | the consumer's clear asserts (:6151-6157) | 244 registry (the clear empties; the fallback works) | clearRegisteredMeshes = a no-op | ctest FAILED: the count assert (0 vs 2) + checkMeshRegistry's count/fallback-strip asserts — NON-VACUOUS (nuance: the consumer's fallback-DRAW assert alone was vacuous against the no-op — a still-registered mesh also draws; the count assert catches it) |

Every mutation was an uncommitted working-tree edit to the single
relevant src file, restored by pathspec after each run; no mutation was
committed; after the last, git diff --stat showed NO src/ changes and
the full gates ran clean (build/ctest/alive x3, exit 0 each).

- **(a) Lighting vs entity rotation coordinate space — missing
  capability (next-version candidate).** The 247 CPU lighting computes
  the per-face normal in LOCAL space and dots it with a world-fixed
  light: a rotating mesh's lit factors are MESH-FIXED (the lighting
  spins with the model instead of the light staying world-fixed). The
  247 contract promised only "one FIXED directional light", so this
  is not a contract violation — it is a missing world-space lighting
  path (the 2D lit path, Step 79, IS world-space). Not fixed here.
- **(b) Drawn size vs collision extents — missing capability
  (RECLASSIFIED from engine defect, Step 254 verification).** Evidence
  (file:line): NO cross-path contract found — no documented contract
  states drawn size must equal collision extents for the 3D mesh path
  (renderer.h:277's "real box size" is scoped to the 2D debug-AABB
  method; GAME_BUILD.md:43's "visual contact and actual death agree"
  is the arcade's 2D hitbox balance contract; the Step 8 rule governs
  collision/detection, not drawing). The 3D scale-only sizing is
  ITSELF the CI-proven convention: checkEntity3D asserts "Model corner
  must be position + scale/2" and the Steps 214/237 records document
  the model = translation*rotationY*scale. A sizing change would
  invalidate ~7 measured-strip tests (checkMeshRegistry/checkTextured
  Mesh/checkMeshTextureId/checkWedgeMesh/checkMeshLighting/checkMesh
  Occlusion/checkMeshDepthState - all measured against the current
  scale-only sizing). The mismatch is the sandbox's visible artifact
  (the box appears sunk). PROPOSED PLACEMENT: "3D sizing source of
  truth" - the NEXT engine version (either the 3D draw adopting
  halfExtents*scale as the drawn size, a controlled convention change
  with the checkEntity3D amend + re-measured strips + a drawn-footprint
  regression test, or an explicit per-path sizing convention documented
  as the engine's contract). Not fixed here.
- **(c) drawEntityMesh3D GL state-leak audit — RESOLVED (Step 255: the
  dedicated CI test landed, all five state items green).** The
  checkDebugFrameDepth twin for the entity path is `checkMeshStateLeak`
  (tests/hostile_data_test.cpp:6033): THE INVARIANT — after
  drawEntityMesh3D, the GL color framebuffer's content OUTSIDE the
  drawn mesh equals its pre-call value (the depth-only clear never
  touches color), and each state item is deterministic: the
  depth-test ENABLE state equals its pre-call value (both OFF and ON
  callers), the blending enable is untouched, the program is set
  explicitly (non-zero, identical across calls), and the VAO binding
  returns to 0. Hard pass/fail, no timing, no visual inspection.

## Version 3D done-criteria (Step 253, docs-only — no new scope)

The 3D version is COMPLETE/FROZEN only when:
- every capability in the map above is VERIFIED with a real test
  (none PARTIAL/UNVERIFIED in the capability rows);
- the open findings (a)-(c) are RESOLVED (fixed with tests) or
  explicitly DEFERRED with a recorded version target;
- the 2D non-regression is green on every step (ctest 2/2 + the 2D
  byte-identical guarantees);
- the full-solution build + ctest + alive x3 have actual execution
  evidence;
- the tracker records capabilities, dependencies, verification,
  limitations, and open findings;
- the implementation is committed and pushed.

**3D engine version COMPLETE / FROZEN (Step 256 — the audit table above:
all six criteria VERIFIED with fresh execution evidence: build exit 0,
ctest exit 0 (100% 2/2, 15.69s/1.55s), alive x3 exit 0).**

FROZEN CONTRACT LIST (what the 3D version delivers):
- The OBJ triangle-soup loader (v/f lines only, the per-face planar
  UVs, failure-not-cached).
- The mesh registry (meshId -> loaded OBJ map; meshId 1/2 the
  hardcoded cube/pyramid; meshId 3 the tetra and meshId 5 the wedge
  loaded at init from assets/).
- The textureId sampling on registered meshes (the 2D path's
  OOB/released validation rule; meshId 1/2 never read textureId).
- The CPU per-face directional lighting (a fixed light + ambient,
  outward normals via the centroid, one draw per face).
- The depth contract (the save/enable/restore; the single opt-in
  clear per 3D pass; the order-independent inter-entity occlusion;
  the enable-state round-trip; the color never touched by the
  depth-only clear).
- The 3D debug harness (debug3d on/off/cam/fov/orbit/drop; meshid;
  texid; the sandbox drop).

DEFERRED to the NEXT engine version (the recorded candidates):
- (a) World-space lighting: the 247 CPU lighting computes the per-face
  normal in LOCAL space — a rotating mesh's lighting is mesh-fixed;
  the next version's candidate is a world-space lighting path.
- (b) 3D sizing source of truth: drawEntity3D scales by e.scale ONLY
  while collision uses halfExtents*scale — the next version's
  candidate is the 3D draw adopting the collision extents (a
  controlled convention change with the checkEntity3D amend +
  re-measured strips + a drawn-footprint regression test).

## ENGINE v4 — 3D correctness (Step 259, OPEN)

**Version name:** Engine v4 - 3D correctness. **Status: OPEN** — engine
work only; games frozen. **This version AMENDS the v3 3D frozen
contract at exactly the two capabilities below; EVERYTHING ELSE in the
v3 frozen contract list (the loader, the registry, the textureId
sampling, the depth contract, the 3D debug harness) is UNCHANGED.**

### v4 scope (exactly two capabilities; no others unless a classified
### finding with evidence is added)

**(a) World-space lighting for rotated entities — RESOLVED (Step 260)**
- Problem (one line): the 247 CPU lighting computes the per-face
  normal in LOCAL space, so a rotating mesh's lit factors are
  MESH-FIXED instead of tracking a world-fixed light.
- THE FIX (renderer.h, the lighting loop only): the WORLD-SPACE normal
  transform n' = R * (n / s) — the inverse-transpose of the model's
  R*S (for orthonormal R and diagonal S: (RS)^-T = R * S^-1) —
  extracted from the model matrix's upper-left 3x3 (column i = s_i *
  R's column i), so the s is the MODEL's scale: (b) cannot break it
  when it adopts halfExtents*scale. At rotation 0 and uniform scale
  the output is BIT-IDENTICAL to the pre-260 raw normal (R = I, s = 1
  -> n' = n; measured: +0 135 / -0 91 unchanged). CPU per-face STAYS
  (the rotation is a single yaw; a shader change would touch the
  shared 2D world shader).
- DISCLOSED FORMULA CONFLICT + RESOLUTION: the v4 prompt's stated
  formula ends with a normalize(); the bit-identical gate forces its
  omission (the raw cross's magnitude is the current behavior — the
  normalize would shift the slanted faces' diffuse ~4-12% at rotation
  0). Adding the normalize is a documented (b)-step follow-up with
  test recalibration.
- THE REGRESSION: checkMeshWorldLighting (the factors COMPUTED from
  the lighting formula in the test, never copied from measured
  output): the computed swap (F+@0 0.851 > F-@0 0.455; F+@pi 0.35 <
  F-@pi 0.551), the 90-degree case (F+@90 0.357, exactly predictable),
  the non-uniform scale (2,1,1) (F+nu 0.752 < F+@0 — the n/s
  correction), and the pixel level (the rotated pixels match the
  computed-factor prediction via the derived per-face texel scale;
  the non-uniform mean 110.85 < 0.9 of the uniform 135).
- MUTATION TABLE (both mutations caught, ctest FAILED):
  - the rotation removed from the transform -> the prediction assert
    (+pi 51, no response) AND the non-uniform mean (125.5 > 121.5)
    fired — NON-VACUOUS;
  - the 1/s term removed (the rotation kept) -> the rotation asserts
    PASSED (the 1/s at s=1 is a no-op — as designed) and the
    non-uniform mean assert fired (125.5 > 121.5) — the non-uniform
    case guards the 1/s term SPECIFICALLY — NON-VACUOUS.

**(b) 3D sizing source of truth — RESOLVED (Step 261)**
- Problem (one line): drawEntity3D/drawEntityMesh3D scale the mesh by
  e.scale only (drawn size 2*scale) while collision uses
  halfExtents*scale — the drawn size does not match the collision
  extents.
- THE FIX (renderer.h, drawEntity3D's model construction ONLY —
  drawEntityMesh3D consumes the caller's model, one construction
  site): the model scale = **halfExtents * scale * 2** per axis — the
  EXACT expression the 2D AABB debug draw uses (renderer.h:1084-1088,
  contract comment :1083 "halfExtents * scale, doubled"; collision.h
  :116 builds the same box). The drawn footprint now matches the
  collision extents for every meshId. With unit halfExtents
  (0.5,0.5,0.5) this is BIT-IDENTICAL to the pre-261 e.scale
  (0.5*1*2 = 1), so every existing 3D pixel/strip/depth assert held
  unchanged (verified: ctest 100% before any recalibration).
- Decision 1 evidence: native bounds — cube (mesh3d.h:46-68) and
  wedge (mesh_wedge.obj) exactly ±0.5 unit; pyramid ±0.5 base;
  tetra x/y ±0.5, z 0.866 (disclosed "unit-ish" in the asset header) —
  the formula is per-axis and correct for all four. The 2D AABB debug
  draw's OWN quad is ±1 (renderer.h:278-284) so its drawn box is
  4·hx·s = 2× the collision box (2·hx·s) — a SEPARATE 2D-debug-path
  finding, RECORDED below (frozen, out of scope for 261).
- DISCLOSED FINDING (not fixed, out of scope): the 2D AABB debug draw
  draws its box 2× the collision extents (the quad ±1 with the
  halfExtents*scale*2 scale); no CI test pins the drawn size; the 2D
  debug path is frozen for v4.
- Step 260 follow-up (Decision 2): the lit normals are now NORMALIZED
  (drawEntityMesh3D: `wn = (mr0*ns.x + mr1*ns.y + mr2*ns.z).normalized()`
  — vec3.h:88-94 zero-safe). Computed example (the diffuse changed
  with entity size without it): the tetra's +X face n=(0.8661,0.2887,0.5)
  (raw cross |n|=1.0408), L=(0.3520,0.8137,0.4620), yaw 0: model scale
  (1,1,1) -> dot 0.771 -> F 0.851; model scale (3,2,2) (halfExtents
  (1.5,1,1)) -> dot 0.335 -> F 0.567 — a 33% drop PURELY from size.
  With the normalize: F 0.874 — the size no longer affects the factor
  (the non-uniform scale's legitimate rotation effect remains).
- OLD/NEW EXPECTATION TABLE (each change justified by the lighting
  formula or the (b) contract, never by re-measuring output):
  | Test | Old | New | Justification |
  | checkEntity3D corner | corner = position + scale/2 (the Mat4 math) | UNCHANGED | the Mat4 composition test, no Entity/halfExtents |
  | checkEntity3D GL depth / checkMeshDepthState / checkWedgeMesh / checkFrozen3DConsumer / checkMeshStateLeak / checkMeshRegistry / checkTexturedMesh / checkMeshTextureId / checkMeshOcclusion | depth/variance/state asserts (the entity data: the DEFAULT halfExtents (0.7071,0.7071,0)) | UNCHANGED values; the entity data = halfExtents (0.5,0.5,0.5) | the (b) contract: the drawn size matches the collision extents (the collision full 1x1x1 = the drawn 1x1x1) |
  | checkMeshLighting | bright > dark+20; bright < 250; dark > 60 | UNCHANGED (the relative asserts survive the normalize: the diff 41.7 > 20; 90.2 > 60) | the lighting formula (the normalize changes the absolute factors, not the relative asserts) |
  | checkMeshWorldLighting computed | F+@0 0.851079; F-@0 0.45474; F+@pi 0.35; F-@pi 0.55077; F+@90 0.357062; F+nu 0.751994; F+nu < F+@0 | F+@0 0.831389; F-@0 0.450625; F+@pi 0.35; F-@pi 0.542881; F+@90 0.356784; F+nu 0.906995; **F+nu > F+@0** | the lighting formula: the normalize; the F+nu flip: the normalized n/s re-amplifies the shrunk x, rotating the normal toward y/z where L is strong (the computed example) |
  | checkMeshWorldLighting pixels | +0 135; -0 91; +pi 116; the mean < 0.9*rPlus0 | +0 132; -0 90; +pi 113; **the mean > 0.95*rPlus0** | the lighting formula (the pixel = texel*factor); the mean flip: the computed ratio analysis (the actual meanNu/rPlus0 1.014 vs the unnormalized 0.841; the 0.95 threshold between, so the mutation is caught) |
- THE NEW REGRESSIONS: checkMeshSizing (meshIds 3 AND 5 at the
  non-default halfExtents (0.6,0.4,0.5) + scale (1.25,1.25,1.0): the
  model scale (1.5,1,1) asserted exactly from the formula; the
  measured row span scales by exactly 1.5x its own calibration (the z
  unchanged -> the linear projection), tolerance ±2, real GL) and
  checkMeshSizeInvariantLighting (the SAME mesh at the scales
  (1,1,1)/(2,2,2): the computed factors IDENTICAL 0.831389 from the
  formula; the pixel at the span's 75% (the same relative position ->
  the same interpolated texel) equal within ±3).
- MUTATION TABLE (each an uncommitted edit restored by pathspec;
  ctest FAILED):
  - (i) the sizing reverted (the model scale -> e.scale): the wedge's
    footprint assert fired (16 vs the expected 21, the diff 5 > 2) —
    NON-VACUOUS; the tetra's assert missed by exactly the tolerance
    (16 vs 18, the diff = 2.0 ≤ 2.0 — a rasterization-dependent
    near-boundary, noted); the sizeinv test passed (as designed: the
    mutated model scales = e.scale are identical for unit halfExtents) —
    caught SPECIFICALLY by checkMeshSizing.
  - (ii) the normalize removed: checkMeshWorldLighting's mean assert
    fired (110.8 < 125.4; the pixels +0 135/-0 91 — the exact pre-261
    values, confirming the computed prediction) AND
    checkMeshSizeInvariantLighting's pixel assert fired (small 128 vs
    big 89, the diff 39 > 3 — the size dependence exactly as computed)
    — NON-VACUOUS.

**(a)+(b) interaction (RESOLVED, Step 260/261):** with NON-UNIFORM
halfExtents the model matrix has a NON-UNIFORM scale, so transformed
normals use the INVERSE-TRANSPOSE — (a) landed first (260: n' =
R*(n/s), the model-scale form (b) cannot break), and (b) landed with
the final normalize (261: the diffuse SIZE-INVARIANT, the computed
example recorded above); the ordering question is closed.

### Known test limits (recorded, no fix, no new scope)
- The OOB-sample mutation is UB (entityTextures[99] is an OOB vector
  access = a crash, not a clean FAIL) so the 246 fallback is proven
  INDIRECTLY (the equivalent-safe mutation, the 258 audit).
- The consumer's fallback-draw assert does NOT catch a no-op
  clearRegisteredMeshes (a still-registered mesh also draws); the
  count assert does (the 258 audit's nuance).

### Step 262 pre-freeze mutation closure (tests only)
Re-ran all four v4 mutations on the committed tree (0edf222); every
assert that passed under its mutation was tightened (test code only,
the expectations formula-derived) and its mutation re-proven:
| Assert | Mutation | Before tightening | After tightening |
| checkMeshWorldLighting rPlusPi band | (iv) the rotation removed | PASSED (51 < 132) | FAILED: the new floor `rPlusPi > rPlus0*0.7` (the mutated ratio 0.386 vs the actual 0.856, consistent with the formula's factor ratio 0.653 x the per-face texel calibration) + the prediction (the diff 57 > 6.0) |
| checkMeshWorldLighting prediction | (ii) the normalize removed | PASSED (the diff 7.6 < 32.2) | FAILED: the tolerance 32.2 -> 6.0 (the non-mutated diff 4.6, the mutated 7.6); the mean (110.8 < 125.4) and the sizeinv pixel (128 vs 89) also fire |
| checkMeshSizing tetra span | (i) the sizing reverted | PASSED (16 vs 18, the diff 2.0 <= 2.0 - the known weak point) | FAILED: the tolerance 2.0 -> 1.0 (the mutated diff 2 > 1.0); the wedge span (16 vs 21) also fires |
Already failing under their mutations (no tightening needed): the
wedge span (i), the mean (ii)/(iii), the sizeinv pixel (ii), the
prediction (iv). The computed (lambda) asserts (1)-(4)/(11) are the
formula documentation and validate via the pixel asserts.
FULL TABLE (the initial re-run): (5)/(iv) PASSED; (6)/(ii) PASSED;
(6)/(iv) FAILED; (7)/(ii) FAILED; (7)/(iii) FAILED (122.8 < 125.4,
exactly as computed); (9)/(i) PASSED; (10)/(i) FAILED; (12)/(ii)
FAILED (the diff 39 > 3). Each mutation: an uncommitted edit,
restored by pathspec, never committed, git diff --stat -- src/ empty
afterward.

### v4 done-criteria (the v3 done-criteria form)
The v4 version is COMPLETE/FROZEN only when:
- every capability assigned to v4 is implemented;
- behavior is deterministic where the contract requires it;
- required error handling exists;
- required regression tests exist;
- the new asserts are MUTATION-CHECKED (each must FAIL under a
  reverting mutation — the 258 audit pattern);
- required subsystem integration works at the engine boundaries;
- existing tests remain green;
- build/ctest/alive x3 have actual execution evidence;
- the tracker records capabilities, limits, and findings;
- implementation is committed and pushed.

### Out of v4
- No ECS, no editor, no glTF, no PBR, no game content, no new
  subsystems. Game-originated needs are CLASSIFIED (existing
  capability / engine defect / missing capability / game-specific
  behavior), never added.

## Not started / pending engine debts (Step 236, docs-only)

NOT STARTED (the freeze OUT list stands — do not plan):
- Materials system, glTF, animation blend (multi-track clips /
  cross-clip blending), editor UI, ECS, networking, 3D game content.

PENDING engine debts (inspect/adopt only on evidence):
- Broadphase-under-load: inspect/adopt ONLY on a measured superlinear
  frame-time breach below 2000 entities (Step 52 verdict; fine <=2000).
- Multi-track animation scoping: inspect-only — no blend work without an
  explicitly requested step citing a concrete engine-validation reason.

## PureEditor-lite v0 — TOOLING CONTRACT (docs foundation; explicit user-requested scope expansion)

Secondary goal: PureEditor-lite (load/select/nudge/save) — tooling ON TOP
of existing engine APIs, NOT engine internals.

**Non-goals (binding):** full hierarchy editor, animation studio,
multiplayer, ECS. No engine API changes; no game level design.

**Required engine APIs (all exist, verified):**
- \pe::loadSceneFromFile\ / \pe::saveSceneToFile\ (+ explicit-path + rename-overwrite contract, Step 162)
- \pe::pickEntityAtScreen\ (Step 128; screen-to-world + pickEntity)
- \pe::Button\ + hitTest (Step 118/121) + \pe::screenToUi\ (Step 125)
- Console \scene_dump\ / \scene_reload\ (Step 136 persistence interaction)

**v0 must do (the smallest TOOL loop):**
1. Load a scene file (v1/v2) into a SceneManager
2. Click-select an entity via pickEntityAtScreen (screen coords in, index out)
3. Nudge the selected entity (position delta via keys or per-tick)
4. Save the scene back out (same file or explicit path)
5. Success test: load -> select -> nudge -> save -> reload -> position changed and persisted (headless; the pick/nudge/save loop is engine-pure — the GL click feel stays a SMOKE gate)

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
