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
- ✓ Step 102: pe_core static lib (header-only still alive; 2026-10-14: the "header-only primary" rule is SUPERSEDED INCREMENTALLY for the Core Foundation category by Cyril's migration decision - see the Step 316 slice section; a module stays header-only until its slice moves it) — completed
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
cover draw ordering), or an in-game editor (no requesting use-case).
**AMENDED Step 284 (2026-10-03): the editor/tools phase is now ALLOWED
(explicit allow by Cyril) as a CONSUMER phase — see Phase Tools; the
"no requesting use-case" note above was the pre-284 state.**
Step 54's re-open list: ONE of these becomes true: (A) a second draw-layering rule is
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

**Stress-test result (Step 45, re-verified post-Steps 46-49):** Tested at 3, 20, and 50 hostiles using data-driven hostile_default.txt variants. Average frame time stayed flat at ~1.5–1.6 ms across all three counts. Re-measured at 50 hostiles after Steps 46-49 (EntityRole branching, depth-sort): 1.47–1.80 ms (avg ~1.6 ms), matching the Step 45 baseline within normal noise. No disproportionate cost from the added branching or sort. Broad-phase collision remains correctly deferred — the earlier inference is now confirmed by measurement.

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
  **AMENDED Step 284 (2026-10-03, explicit allow by Cyril):** the
  editor/tools work is a CONSUMER phase (see Phase Tools below) — the
  ENGINE-side bans above stand unchanged; an editor/tool is a consumer
  (own directory, own CMake target, documented APIs only, engine src/
  read-only). The 3D contracts landed via the frozen v3/v4/v5
  versions.
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

## ENGINE v4 — 3D correctness (Step 259, COMPLETE/FROZEN Step 263)

**Version name:** Engine v4 - 3D correctness. **Status: COMPLETE/FROZEN
(Step 263, date 2026-10-02, HEAD f56e7d1)** — the done-criteria audit
(10/10 VERIFIED, zero UNVERIFIED; fresh build/ctest/alive x3 exit 0 at
f56e7d1) closes the version. Games frozen. **The version AMENDED the
v3 3D frozen contract at exactly the two capabilities below ((a) and
(b), both RESOLVED); EVERYTHING ELSE in the v3 frozen contract list
(the loader, the registry, the textureId sampling, the depth contract,
the 3D debug harness) is UNCHANGED.**

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

### Frozen contract list (v4, Step 263) — binding
- OBJ loader (v/f only, per-face planar UVs, missing-file clean
  failure) — checkMeshLoad, checkWedgeMesh:5938.
- Mesh registry (meshId→OBJ map; 1/2 hardcoded cube/pyramid; 3 tetra,
  5 wedge loaded at init; re-register replaces; clear falls back) —
  checkMeshRegistry:7147, checkFrozen3DConsumer:6030.
- textureId sampling (the 2D validation rule: OOB and released fall
  back to the checker) — checkMeshTextureId:7384.
- Depth contract (save/enable/restore, the single opt-in clear,
  order-independent occlusion, the round-trip CI) —
  checkMeshDepthState:5845, checkMeshOcclusion:7612.
- 3D draw: the sizing source of truth (halfExtents*scale*2, the 2D
  AABB expression) + the world-space lighting (n' = normalize(R*(n/s)),
  size-invariant) — checkMeshSizing, checkMeshWorldLighting:6325,
  checkMeshSizeInvariantLighting.
- 3D debug harness (debug3d/meshid/texid) — checkDebug3dParse,
  checkMeshIdParse, checkTexIdParse.

### Known limits after v4 (Step 265 consolidation; the four items from 263)
- **ENGINE DEFECT (candidate v5 first step; NOT in v4): the 2D AABB
  debug draw's quad is ±1** (src/renderer.h:278-284) **with the model
  scale halfExtents*scale*2** (src/renderer.h:1086-1088), **so the
  drawn debug box is 2x the collision size (4·hx·s vs the collision
  2·hx·s, collision.h:85-90), contradicting the documented contract at
  src/renderer.h:277 ("scales by halfExtents*2 to get the real box
  size") and src/renderer.h:1083 ("This is the exact same box
  collision.h uses: halfExtents * scale, doubled").** Not fixed in v4;
  no CI test pins the drawn size; **candidate v5**.
- **OOB texture sample is UB** (entityTextures[99] is an OOB vector
  access = a crash, not a clean FAIL), so the 246 fallback is proven
  only INDIRECTLY (the equivalent-safe mutation, the 258 audit) —
  **not in v4 / not scheduled**.
- **The consumer's fallback-draw assert does NOT catch a no-op
  clearRegisteredMeshes** (a still-registered mesh also draws);
  the count assert does (checkFrozen3DConsumer:6151-6152, the 258
  audit's nuance) — **not in v4 / not scheduled**.
- **Out-of-v4 list stands**: no ECS, no editor, no glTF, no PBR, no
  game content, no new subsystems — **not in v4 / not scheduled**.

### Recommended stop after Step 266 (2026-10-02)
Engine v4 - 3D correctness is COMPLETE/FROZEN (263, 10/10 done-criteria
VERIFIED, zero UNVERIFIED); the pre-freeze mutation closure (262)
tightened every assert that passed under a reverting mutation, so all
four v4 mutations are caught; the Phase-D consumer was refreshed on the
frozen v4 (264: the sized+rotated composition — the sizing source of
truth, the world-space lighting, the size-invariant diffuse, and the
order-independent occlusion all hold end-to-end, all three mutations
caught first-try); the docs are consolidated (265: the step counts, the
known limits, the stale facts swept to match the tracker). The health
gate at Step 266 is green: cmake --build build --config Release exit 0;
ctest -C Release 2/2 100% exit 0; alive x3 (PureEngine.exe,
Platformer.exe, Pong.exe) exit 0. **Recommended stop: no further engine
work is scheduled.** v5 candidates (classified findings only): the 2D
AABB debug-draw 2x overscale (the ENGINE DEFECT recorded above — the
only classified finding from this phase); no other findings were
exposed. The out-of-v4 list stands (no ECS, no editor, no glTF, no PBR,
no game content).

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

**AMENDED Step 284 (2026-10-03, explicit allow by Cyril):** this
PureEditor-lite v0 contract is the historical foundation of the
TOOLS/Editor phase (see Phase Tools below) — the tooling was partially
opened before (the editor-lite v0 headless path landed at Step 197);
Phase Tools opens it FORMALLY as a consumer phase.

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

## ENGINE v5 - 2D debug-draw correctness (Step 267, COMPLETE/FROZEN Step 269)

**Version name:** Engine v5 - 2D debug-draw correctness. **Status:
COMPLETE/FROZEN (Step 269, date 2026-10-02, HEAD c8475e5)** — the
done-criteria audit (10/10 VERIFIED, zero UNVERIFIED; fresh
build/ctest/alive x3 exit 0 at c8475e5: ctest 2/2 100%, 24.49s/2.12s)
closes the version. Games frozen. **v4 stays COMPLETE/FROZEN (Step
263); this version AMENDED only the capability below (now RESOLVED);
EVERYTHING ELSE in the v3 and v4 frozen contract lists is UNCHANGED**
(the v4 drift check: exactly two hunks in src/ since the v4 freeze
5967667 — the aabbVertices array + its comments and the drawAABBs
contract comment, nothing else).

### v5 scope (exactly one capability; no others unless a classified
### finding with evidence is added)

**(c) 2D AABB debug-draw size vs collision size — RESOLVED (Step 268)**
- Problem (one line): the debug quad is ±1 and the model scale is
  halfExtents*scale*2, so the drawn box is 2x the collision box.
- THE FIX (Option A — renderer.h's aabbVertices array ONLY): the quad
  corners to ±0.5 (the SAME native convention as the 3D meshes). The
  scale expression (halfExtents*scale*2) is UNTOUCHED — checkMeshSizing
  cites it as the 3D sizing source of truth, so A keeps the 3D tests'
  comment valid; B (the scale expression) would have broken it. The
  one-line reason: the contract text says halfExtents*2 is the real box
  size and the 3D meshes already use that convention.
- Decision-1 inspection CORRECTIONS to the 267 blast radius: the
  overlay is drawn as a thin GL_LINE_LOOP OUTLINE, not a filled quad
  (src/renderer.h:1120 — the 267 record did not state this); the line
  numbers shifted — the scale expression is now :1100-1104 (the z
  stays 1.0f flat), the drawAABBs contract comment :1095-1104, the
  draw site :1120, the quad :278-284 (unchanged). The single draw
  site, the F1 toggle (main.cpp:1717/2157, Input :870), and the
  private VAO/VBO (:285-289, :1246) all confirmed unchanged.
- THE REGRESSION: checkAABBDebugSize (hidden window, real GL; the
  expected extents COMPUTED in the test from the collision size and
  the Camera's default ortho projection (320/12 = 240/9 = 26.6667 px
  per unit), never from measured output): the entity at the
  non-default halfExtents (0.4,0.3) + scale (2,2) -> the collision box
  (1.6,1.2) world -> the expected outline (42.67, 32) px; the drawn
  (44, 33) — within ±2 (the line rasterization). The orange-tint pixel
  scan bounds the outline; drawn == collision asserted.
- MUTATION TABLE (an uncommitted edit restored by pathspec; ctest
  FAILED): the quad reverted to ±1 -> the drawn outline measured
  86 x 65 vs the expected 42.67 x 32 — EXACTLY 2x the collision box
  (the (c) defect reproduced perfectly) — NON-VACUOUS.
- STATUS: **pixel-verified, visual confirmation pending** (the F1
  overlay's on-screen look is a HUMAN smoke check; SMOKE_TEST.md has
  no F1/AABB row — recorded as a known limit).

### Carry forward (not fixed in v5, recorded)
- OOB texture sample is UB (entityTextures[99] is an OOB vector access
  = a crash, not a clean FAIL), so the 246 fallback is proven only
  INDIRECTLY (the equivalent-safe mutation, the 258 audit).
- The consumer's fallback-draw assert does NOT catch a no-op
  clearRegisteredMeshes (a still-registered mesh also draws); the count
  assert does (checkFrozen3DConsumer, the 258 audit's nuance).

### v5 done-criteria (the v4 done-criteria form)
The v5 version is COMPLETE/FROZEN only when:
- every capability assigned to v5 is implemented;
- behavior is deterministic where the contract requires it;
- required error handling exists;
- required regression tests exist;
- the new asserts are MUTATION-CHECKED (each must FAIL under a
  reverting mutation, run on the committed tree — the 258 audit
  pattern);
- required subsystem integration works at the engine boundaries;
- existing tests remain green;
- build/ctest/alive x3 have actual execution evidence;
- the tracker records capabilities, limits, and findings;
- implementation is committed and pushed.

### Frozen contract (v5, Step 269) — binding
- **The debug outline equals the collision size**: the F1 AABB overlay
  draws each entity's ACTUAL collision box — halfExtents*scale*2 with
  the ±0.5-native quad (the 3D meshes' convention), as a thin
  GL_LINE_LOOP outline — the exact box collision.h:85-90 tests against,
  doubled. The scale expression is the 3D sizing source of truth's
  twin (checkMeshSizing cites it). Pixel-verified (checkAABBDebugSize:
  the drawn (44,33) vs the computed (42.67,32), ±2; the mutation
  reproduced the 2x defect exactly). **Visual confirmation: pending**
  (the F1 overlay's on-screen look is a HUMAN smoke check; SMOKE_TEST.md
  has no F1/AABB row — never record CONFIRMED without it).
- Carry forward (unchanged, not fixed in v5): OOB texture sample is UB
  (the 246 fallback proven only indirectly, the 258 audit); the
  consumer's fallback-draw assert does not catch a no-op
  clearRegisteredMeshes (the count assert does, the 258 nuance).
- Out of v5 stands: no ECS, no editor, no networking, no glTF, no PBR,
  no new 3D work, no game content, no AI/MCP layer.

### v5 done-criteria audit (Step 269, verbatim criteria from V3:820-833)
| Criterion | Evidence | Status |
|---|---|---|
| every capability assigned to v5 is implemented | (c) 4c6c710 (268); V3 (c) record | VERIFIED |
| behavior is deterministic where the contract requires it | the drawn outline 44x33 identical across the 268 re-runs | VERIFIED |
| required error handling exists | the (c) fix has no new error paths; the existing loader/OOB/zero-safe paths hold | VERIFIED |
| required regression tests exist | checkAABBDebugSize (tests/hostile_data_test.cpp:6881) | VERIFIED |
| the new asserts are MUTATION-CHECKED | the 268 mutation table (V3 above): the quad reverted -> 86x65 (exactly 2x) -> FAILED on the committed tree | VERIFIED |
| required subsystem integration works at the engine boundaries | the overlay = the collision box (collision.h:85-90) via the pixel proof | VERIFIED |
| existing tests remain green | ctest 2/2 100% (the 268 gates + the fresh 269 re-run) | VERIFIED |
| build/ctest/alive x3 have actual execution evidence | fresh at c8475e5: build exit 0, ctest 2/2 100% exit 0, alive x3 exit 0 | VERIFIED |
| the tracker records capabilities, limits, and findings | V3 + steps.json (267/268/269) + board + README | VERIFIED |
| implementation is committed and pushed | 4c6c710 + c8475e5 pushed | VERIFIED |

### Step 270: the hands-on Arcade findings (verify-only, no src/test changes)

**Human findings, recorded verbatim:**
- "F1 overlay appears in the Arcade only. F1 is not wired in Platformer
  or Pong."
- "Edge alignment (outline vs touching entity):" — **UNREPORTED** (the
  FLUSH / OVERLAP / GAP slot was not filled in this session's prompt;
  the v5 visual confirmation therefore stays **pending**).
- "Q/E did not orbit the 3D camera; `debug3d orbit 30` did." Failing
  context: **UNREPORTED** (the menu / play / debug3d-on only / after
  cam slot was not filled).
- "3D check via debug3d on, meshid 0 5, texid 0 <n>, debug3d orbit 30:
  wedge visible, texture changes, lighting works."

**Classifications (each exactly one class, not fixed; RECLASSIFIED
Step 271 with the full grep evidence):**
- **"F1 only in Arcade" = game-specific behavior.** Evidence: the
  engine provides the public drawAABBs (src/renderer.h:1082); the F1
  toggle is wired ONLY in the Arcade (src/main.cpp:1717-1718, the
  Input binding :870, the call :2157); the Platformer and Pong have NO
  F1/drawAABBs references (grep: none in
  games/platformer/platformer.cpp, games/pong/pong.cpp) — each game
  chooses to wire the toggle.
- **"Q/E did not orbit" = RECLASSIFIED (Step 271) into TWO parts:**
  - **The false claim at src/main.cpp:1250-1252 (the Step 229 comment:
    "Q/E keys also orbit in the debug3d block below") = documentation
    defect — FIXED in Step 271.** The comment now states the only
    orbit is the `debug3d orbit <degrees>` console command and records
    the 271 correction. Evidence: the wiring NEVER existed (`git log
    --all -S 'GLFW_KEY_Q)'` empty; Q/E entered the key list at e1488f2
    with no consumer); the authoritative tracker (the Step 229 record)
    promises only the `debug3d orbit` command ("debug3d-only").
  - **The Q/E keyboard orbit = missing capability, NOT scheduled, NOT
    a v6 candidate** unless Cyril deliberately chooses it. Evidence:
    no isEdge/isDown handler for GLFW_KEY_Q/E exists anywhere in
    main.cpp; the action map (src/input.h:90-98) has no orbit action
    and Q/E map to nothing; Q/E are only REGISTERED in the Input key
    list (src/main.cpp:870-876); checkOrbitEye covers only the
    headless orbitEye math (a shortcut, not the input path).
- The FULL Q/E claim map (Step 271, grep): the ONLY behavior claim was
  the main.cpp:1251 comment; the other hits (main.cpp:876 the key
  list, input.h:160 the key-name parse, V3:157 the 224-era deferral
  list) are registrations/history, not behavior claims;
  SMOKE_TEST.md/AGENTS.md/README/console usage strings have none; no
  test asserts the usage string's exact text.

**v6 candidate list (Step 270, REVISED Step 271): EMPTY** (the 270's
candidate #1 — the Q/E orbit wiring — was reclassified into the
documentation defect fixed in 271 + the missing capability NOT
scheduled; removing it from the candidate list).

**v6 stays NOT OPEN** (recorded here only as candidates; opening v6 is
a separate step's decision).

### Cost attribution (Step 273) — measurement + docs only, v6 stays NOT OPEN

**PRE-REGISTERED THRESHOLDS (written BEFORE measuring; not changed
afterward):**
- A candidate is **ELIGIBLE** for a version only if the cost it removes
  is **>= 1.0 ms per frame** (about 6% of a 16.67 ms budget) in a
  REALISTIC workload, **or** the cost grows superlinearly with entity
  count.
- **"Realistic workload"** = the measured per-frame counts from the
  section below (2D sprites drawn, 3D entities drawn, draw calls,
  buffer uploads — min/median/max over a fixed frame count per game).

**Realistic workload counts (Step 273, indicative, loop-structure-exact,
not asserted).** METHOD: a throwaway scratch counter (uncommitted,
deleted before commit) with a hidden-window GL context; each game's
REALISTIC play state scripted from the shipped data (the Arcade:
buildInitialEntities + the C++-default hostiles = 7 sprites — NOTE: the
shipped hostile_default.txt has 0 hostiles, so the realistic range is
4-7 sprites; + the two drop entities for the debug3d-on case; the
Platformer: level1's 20 solid tiles + the player + the companion = 22
sprites; the Pong: 3 entities); the REAL draw paths
(drawWorld/drawEntity3D) run per frame over 300 frames; the counters
are the loop-structure-exact values (the 2D: 1 sprite = 1 draw + 1
upload + 1 uniform set; the 3D: F faces = F draws + F uploads). The
counts are deterministic, so min = median = max:
- Arcade(play, debug3d off): sprites 7 (4-7 realistic), 3D 0, draws 7,
  uploads 7
- Arcade(play, debug3d on + 2 drops): sprites 7, 3D 2, draws 31,
  uploads 31
- Platformer(play): sprites 22, draws 22, uploads 22
- Pong(play): sprites 3, draws 3, uploads 3

**Ablation timing (Step 273, indicative, dev machine, NOT asserted).**
Scratch (uncommitted, deleted before commit), hidden-window GL, 3
repeats, the median reported; BOTH the CPU submit time and the total
with glFinish at frame end. 3D, 100 wedge entities (F=8):
| Variant | submit ms | total ms |
| baseline | 10.616 | 10.926 |
| skip VAO/VBO create+delete (reuse hack) | 0.180 | 0.348 |
| collapse the F per-face uploads into one | 2.746 | 2.997 |
| collapse the F draws into one | 8.176 | 8.735 |
| remove the per-face uniform sets | 10.544 | 10.818 |
| CPU lighting only | 0.000 | 0.148 |
ATTRIBUTION: the VAO/VBO create/delete dominates (~10.4 ms submit,
98% of the baseline); the per-face uploads cost ~7.9 ms; the draw
calls cost ~2.4 ms; the per-face uniforms are negligible (~0.1 ms);
the CPU lighting is negligible (~0.15 ms). 2D sprites, baseline
drawWorld:
| N | submit ms | total ms |
| 100 | 0.287 | 0.461 |
| 1000 | 1.899 | 2.110 |
| 5000 | 9.345 | 9.568 |
ATTRIBUTION: LINEAR scaling (4.6x sprites -> 4.6x time; 5x -> 4.5x) —
no superlinear breach anywhere.

**Verdict table (Step 273; the 3 candidates from 272, at the REALISTIC
counts from above, vs the pre-registered 1.0 ms/frame threshold):**
| Candidate | ms/frame removed at the realistic counts | Scaling | Verdict |
| Per-face upload batching (3D) | ~0.38 (the Arcade's 2 3D entities) | linear | NOT ELIGIBLE |
| VAO/VBO reuse (3D) | ~0.21 (the Arcade's 2 3D entities) | linear | NOT ELIGIBLE |
| 2D sprite batching | ~0.05 (the Platformer's 22 sprites) | linear | NOT ELIGIBLE |
**NOTHING IS ELIGIBLE — performance is not the next capability.** All
three candidates remove 0.05-0.38 ms/frame at the realistic counts
(4-22 sprites, 2 3D entities) — every one below the 1.0 ms threshold —
and the cost grows linearly with entity count everywhere (no
superlinear breach). The ablation numbers at 100 entities are large
(the VAO/VBO create/delete ~10.4 ms) but the realistic 3D count is 2
entities; a future workload with 100+ concurrent 3D entities would
re-open this by measurement.

**Hygiene (Step 273):** all scratch code deleted
(scratch_bench272.cpp from 272 was already deleted; scratch273.cpp
deleted); the temporary CMake targets reverted by pathspec
(git diff -- CMakeLists.txt is empty — verified); git status shows
only hostile_default.txt and Testing/ — stated explicitly, verified.
No src/ or test changes in this step.

### Phase D consumer brief (Step 274) — docs only, v6 stays NOT OPEN

**1) The limits of the 273 verdict (recorded; the verdict itself NOT
changed):**
- The 273 numbers are dev-machine only, not target low-end hardware.
- **Re-open triggers, computed from the 273 numbers:**
  - **3D entity count for VAO/VBO reuse to remove >= 1.0 ms:** the 273
    ablation's baseline-vs-reuse delta at N=100 is 10.616 - 0.180 =
    10.436 ms submit; the removable fraction is 10.436/100 = 0.1044
    ms/entity (the source: the 273 ablation table, the baseline and the
    reuse rows at N=100); the trigger count is 1.0/0.1044 = **9.58, so
    ~10 concurrent 3D entities**.
  - **2D sprite count for sprite batching to remove >= 1.0 ms:** the
    removable fraction was NOT measured in 273 (the batch-hack variant
    was stubbed out of the scratch); the measured baseline is
    0.461/100 = 0.0046 ms/sprite (the source: the 273 2D table at
    N=100); even a hypothetical 100% removal needs 1.0/0.0046 = **217
    sprites**; a realistic 50% removable fraction needs ~434; the 273's
    realistic maximum (the Platformer) is 22 sprites — far below any
    trigger. The re-open trigger: **re-measure with the batch hack
    actually wired**.
  - **Re-measure on real low-end hardware** (the dev-machine numbers do
    not transfer).
- **The scratch benchmark was deleted** (the 273 hygiene), so any
  re-measure needs it rebuilt — a known cost; NOT rebuilt now.

**2) Consumer coverage analysis (grep, file:line; UNEXERCISED only if
grep across ALL THREE games AND main.cpp finds no call site):**

| Subsystem/API | Exercised by | NOT exercised by |
|---|---|---|
| drawWorld (2D sprites) | Arcade (main.cpp), Platformer (the tiles :441/:562), Pong | — |
| drawDigitString | Arcade (main.cpp:1506), Pong (:298-299) | Platformer |
| drawTextString | Arcade (:2166), Platformer (:579-581), Pong (:302-303) | — |
| drawAABBs (the F1 overlay) | Arcade ONLY (:2160) | Platformer, Pong |
| drawEntity3D/drawEntityMesh3D | Arcade ONLY (the debug3d pass) | Platformer, Pong |
| drawDebugMesh3D | Arcade ONLY (the debug cube) | Platformer, Pong |
| input (the action map, the edges) | all three | — |
| mouse input | Arcade (the pick) | Platformer, Pong |
| gamepad | Arcade, Platformer (:35) | Pong |
| audio | Arcade, Platformer (:38) | Pong |
| physics (resolveCollision) | Arcade (the drop), Platformer (the controller), Pong (:25) | — |
| sweptAABB | Platformer (the controller) | Pong |
| animation | Arcade, Platformer (:29) | Pong |
| scenes (SceneManager/loadScene/switchTo) | Arcade (main.cpp:767), Platformer (:157) | Pong |
| serialization (saveSceneToFile :1464, loadSceneFromFile :1470) | Arcade ONLY | Platformer, Pong |
| **saveSceneManagerToFile/loadSceneManagerToFile (scene.h:651/:714)** | **UNEXERCISED** (grep 'saveSceneManagerToFile\|loadSceneManagerToFile': platformer none, pong none, main.cpp none) | all three |
| resources (the texture load via Renderer::init) | all three | — |
| registerMesh | Arcade (main.cpp:712/:730) | Platformer, Pong |
| **clearRegisteredMeshes** | **UNEXERCISED by games** (grep 'clearRegisteredMeshes': platformer none, pong none, main.cpp none — the tests exercise it) | all three |
| registerNonCoreTexture/unloadNonCoreTextures | Arcade ONLY (:1488/:1497) | Platformer, Pong |
| **prefab (loadPrefab/instantiatePrefab, prefab.h)** | **UNEXERCISED** (grep 'prefab\|instantiate': platformer none, pong none, main.cpp comments only) | all three |
| particles | Arcade (main.cpp:177), Platformer (:36) | Pong |
| killRole | Arcade (:924/:1299) | Platformer, Pong |
| camera (setEyeTargetUp/orbitEye) | Arcade ONLY (:1226) | Platformer, Pong |
| buildInitialEntities | Arcade (main.cpp:774) | Platformer, Pong |
| tilemapToEntities/loadTilemapIntoScene | Platformer ONLY (:441/:562/:177) | Arcade, Pong |
| **ui.h button layout helpers** | **UNEXERCISED** (grep 'buttonLayout\|uiButton': platformer none, pong none, main.cpp none) | all three |
| console | all three | — |
| math | all three | — |
| components.h helpers | Arcade (main.cpp:156/:1416, the health command) | Platformer, Pong |
| simulation (simulates) | Arcade, Platformer (:388) | Pong |
| lighting.h (LightingState) | Arcade (:2128) | Platformer, Pong |
| events (the bus) | Arcade (:174), Platformer (:33) | Pong |

**3) Candidate consumers (at most 3, each from the UNEXERCISED set; DO
NOT choose one, DO NOT start building):**
1. **Scene-manager persistence consumer.** Two lines: a headless
   consumer that builds 2-3 scenes, saves the MANAGER via
   saveSceneManagerToFile, reloads via loadSceneManagerToFile, and
   verifies the round-trip at the manager level. Covers:
   saveSceneManagerToFile/loadSceneManagerToFile (UNEXERCISED). Size:
   ~2-3 steps. Plausible findings: the manager index file's path
   handling, the per-scene file naming, the version handling, the
   fs::rename edge cases.
2. **Prefab consumer.** Two lines: a headless consumer that loads
   prefab templates (loadPrefab) and instantiates them
   (instantiatePrefab) into scenes, verifying the template field
   propagation and the instance overrides. Covers: prefab.h
   (UNEXERCISED). Size: ~2-3 steps. Plausible findings: the prefab
   file format edges, the template defaults vs instance data, the
   entity field propagation gaps.
3. **UI-primitives consumer.** Two lines: a real-GL consumer that lays
   out buttons via ui.h's pure helpers and verifies the layout
   math/hit-testing against the engine's coordinate conventions.
   Covers: ui.h button layout (UNEXERCISED). Size: ~2-4 steps.
   Plausible findings: the layout math edges, the hit-test conventions
   vs the AABB rules, the retained-UI-freeze boundary (v1.1).

**The Phase D protocol (recorded; applies to ANY consumer):** uses
documented APIs only; engine src/ is read-only for the duration; own
directory and CMake target; every friction point is logged and given
exactly one class (existing capability / engine defect / missing
capability / game-specific behavior) with evidence; no engine fix
inside a consumer step; stop when the findings log has no new entries
for 2 consecutive steps or at the step cap.

### Phase D consumer: level pipeline (Step 275) — step 1 of at most 5, v6 stays NOT OPEN

**The consumer:** consumers/level_pipeline/ (its own directory + its
own CMake target LevelPipelineConsumer + the CTest registration — the
only CMake changes made); a headless executable (no GL) returning a
nonzero exit code on any failed check; 3 committed prefab data files
(prefab_tile.txt / prefab_pickup.txt / prefab_enemy.txt — differing
fields, the expected values written in the test from the FILE
CONTENTS, never from observed output). Engine src/ READ-ONLY for the
duration (git diff --stat -- src/ EMPTY — verified).

**The checks (all PASS):** loadPrefab succeeds for each of the three;
instantiatePrefab produces the entities whose fields equal the
prefab's (the position/scale/halfExtents/textureId/depth/roleId/
moveSpeed/gravityScale/health/coyoteTime/jumpImpulse/maxFallSpeed/
cols/rows/tag/clip, alive=true, isStatic); instantiating the same
prefab twice gives independent entities (mutating one leaves the
other); instantiating at distinct positions does not alias state
(tag/scale); the Scene composition (queueSpawn's distinct indices +
flushSpawns merges the queued two + the entities present + alive +
the positions preserved).

**The hostile cases (OBSERVED and recorded, stable across THREE
runs):**
| Case | Observed | Class |
|---|---|---|
| missing file | loadPrefab false, "[prefab] File not found", no crash, the out stays default | existing capability |
| empty file | false, "[prefab] Missing or wrong header" | existing capability |
| malformed line (no '=') | partial parse: loadPrefab TRUE, the valid fields kept, the bad line warn-skipped | existing capability (the partial-parse contract is documented; the return value carries no dropped-line count — an API-clarity note) |
| unknown field | warn + skip, loadPrefab TRUE | existing capability |
| duplicate field | loadPrefab TRUE, the LAST assignment wins (20.0, not 10.0), no duplicate warning | existing capability, with a doc-gap note (silent last-wins undocumented) |
| out-of-range numeric (health=1e39) | the parse fails, the field keeps its DEFAULT (100.0), and the warn message is the MISLEADING "[prefab] Unknown key 'health'" (the parse-fail falls through the else-if chain to the unknown-key else, prefab.h:153-172) | existing capability, with a doc-gap note (the misleading message is undocumented; API-clarity friction, not a crash) |

**The findings log:** consumers/level_pipeline/FINDINGS.md — 8 entries:
(1) the parse-fail KNOWN field reported as "Unknown key" (the misleading
message; API-clarity); (2) the duplicate field silently last-wins
(doc-gap); (3) the partial-parse return carries no dropped-line count
(API-clarity); (4) the empty file fails clean (positive); (5) the
missing file fails clean (positive); (6) the true unknown key
warn-skipped (positive); (7) the probe paths never probe a consumer's
data subdir — the bare filename resolves against the CWD root only
(worked around inside the consumer by referencing the subdir path;
doc-gap); (8) a src include dir breaks targets using the standard
<ctime>/<time.h> (the MSVC resolves <time.h> through the /I dirs to
the engine's src/time.h, shadowing it; the existing tests' RELATIVE
includes are the workaround) — **class: engine defect (candidate),
worked around inside the consumer**.
NO engine code fixed for any entry. Nothing blocked the consumer.

**The process friction (the consumer's own build):** the src include
dir (Entry 8) and the data-subdir probe (Entry 7) cost one build
failure each; both worked around inside the consumer (the relative
includes; the subdir paths); the CTest working directory needed the
explicit WORKING_DIRECTORY "$<TARGET_FILE_DIR:...>" (the CTest's
default CWD is the build root, not the target dir).

**Gates:** cmake --build build --config Release exit 0; ctest -C
Release: the count ROSE from 2 to 3, 3/3 100% (5.14s/0.08s/0.06s), exit
0; alive x3 exit 0; git diff --check clean; git diff --stat -- src/
EMPTY (verified).

### Phase D consumer: level pipeline (Step 276) — the scene save/load round-trip, v6 stays NOT OPEN

**Entry 8's exact repro (recorded in FINDINGS.md):** the TU
`#include <ctime>` / `int main() { return 0; }` (3 lines) with
`target_include_directories(repro_time PRIVATE "${CMAKE_SOURCE_DIR}/src" "${glfw_SOURCE_DIR}/include")` +
`cmake --build build --config Release --target repro_time`; the exact
compiler error: `ctime(21,25): error C2039: 'clock_t': is not a member
of '`global namespace''` (+ asctime, clock — the same pattern). Scratch
only; the TU deleted, the CMake reverted (git status clean).

**The round-trip checks (all PASS; ctest 3/3):**
- The scene built from the 3 prefabs at distinct positions; save;
  load into a fresh Scene; EVERY documented serialized field compared
  per entity (the 34-field v2 list: position, rotationAngle,
  rotationSpeed, scale, halfExtents, textureId, depth, roleId,
  moveSpeed, velocity, gravityScale, isStatic, coyoteTime, jumpImpulse,
  maxFallSpeed, tint, cols, rows, health, timer, tag, parentIndex,
  animationSpeed, currentClipName) — the round-trip is FIELD-EXACT
  (within the 4dp format).
- The empty scene: saves; loads with 0 entities and the name.
- The float edge values (OBSERVED and recorded, not asserted as
  correct): 1e-6 → 0.0000 (the 4dp truncation); 0.12345678 → 0.1235;
  0.1f round-trips exactly; 1e30 round-trips; **the floats do NOT
  round-trip bit-exact for values with more than 4 decimals** (the 4dp
  fixed precision, scene.h:418 — UNDOCUMENTED in the format comment,
  scene.h:372-380 — FINDINGS Entry 10).
- save->load->save is **byte-identical** (the 4dp rounding is
  idempotent).
- The documented non-serialized runtime state NOT compared (the
  coyoteTimer/wasGrounded/animationState are not in the 34-field list).
- **The negative control:** the loaded copy's health altered to 999;
  the comparator reports the exact field ("health") ✓.

**NEW FINDINGS (Entry 9, Entry 10):** (9) saveSceneToFile's fs::rename
fails with a sharing violation when the destination has an OPEN read
handle (the consumer's unclosed ifstream; the engine's error return
worked — existing capability, doc-gap note: the rename-fails-if-open
is undocumented; worked around inside the consumer); (10) the 4dp
float precision (the values with >4 decimals do not round-trip
bit-exact; the 4dp precision is undocumented; observed, not asserted).

**Gates:** build exit 0; ctest -C Release 3/3 100% (20.48s/2.66s/
0.29s), exit 0; alive x3 exit 0; git diff --check clean; git diff
--stat -- src/ EMPTY (verified).

### Phase D consumer: level pipeline (Step 277) — the scene-manager persistence, v6 stays NOT OPEN

**The inspection (file:line):** saveSceneManagerToFile (:651-711): the
index format "# scene manager v1" + scenes=<count> + current=<index> +
one scene_file=scene_<name>.txt per scene; the per-scene saves go
through saveSceneToFile (the bare names → the 3-candidate probe); the
tmp+rename atomic save (fs::rename :704-709); if a per-scene save
fails, the index's tmp is removed and false returned — but the
per-scene files saved before the failure REMAIN (the partial state);
loadSceneManagerToFile (:724-794): strict whole-file (the duplicates,
the count mismatch, the bad current index, any failed scene load, the
unknown prefix → false, out untouched); current=-1 valid; **the
explicit paths are NOT honored by the loader** (the candidates:
assets/ only, :726-728 — the save honors them, the load does not);
**load into a non-empty manager: out = tmp (REPLACE, observed)**;
scenes= ≤0 rejected at :759.

**The checks (all PASS; ctest 3/3):**
- The manager with 3 scenes of different content (alpha/beta/gamma);
  save; load into a fresh manager; the scene count, order, names, each
  scene's content (the beta entity field-exact), the active scene
  index — all round-trip.
- The overwrite: the re-save with the existing destination works
  (fs::rename replaces).
- The save to a nonexistent directory: create_directories honored
  (the documented dir auto-create).
- **Load into a non-empty manager: OBSERVED = REPLACE** (the loaded
  content replaces; no merge — recorded, not asserted as correct).
- **Repeated save/load ×20: no growth or drift** (the count, the name,
  the content stable; the manager reassigned each iteration).
- **A leftover .tmp file: does not affect the load** (the loader
  parses only the index itself; observed, recorded).
- **The negative control:** the loaded manager's current index altered
  to 5; the comparator reports it ✓.

**NEW FINDINGS (Entry 11, Entry 12, Entry 13):** (11) the
**save/load path asymmetry** — the loader honors ONLY the
3-candidate probe (assets/), the save honors explicit paths; the
round-trip breaks for every non-assets path (the repro: the save to
consumers/... returns true; the load of the same path returns false
silently) — **missing capability** + doc-gap; worked around inside
the consumer (the bare name, probe-reachable); (12) the **dangling
Scene&** — a loadScene call may reallocate the scenes vector; a
Scene& taken before a later loadScene dangles; the consumer took s2
and used it after the s3 loadScene → **the access violation
(0xC0000005), the process crashed with the stdout buffer lost** — the
documented rule (scene.h:108-109: reserve + re-take) exists and was
violated by the consumer — existing capability (the engine behaved as
documented) with the safety note that the raw-reference API makes the
rule load-bearing; worked around inside the consumer (reserve(3) +
the compared values copied); (13) the **loader's silent false
returns** — no stderr for the count mismatch/bad index/failed scene
load/unknown prefix (only the unknown version warns) — the failure
reason had to be deduced by inspection — API-clarity friction.

**Gates:** build exit 0; ctest -C Release 3/3 100% (10.45s/0.77s/
0.12s), exit 0; alive x3 exit 0; git diff --check clean; git diff
--stat -- src/ EMPTY (verified).

### Phase D consumer: level pipeline (Step 278) — adversarial persistence, v6 stays NOT OPEN

**The corrupt inputs GENERATED PROGRAMMATICALLY** (not hand-edited)
from a valid scene save + a valid manager save; for each the outcome
recorded; the CTest TIMEOUT (60 s) guards hangs (the consumer's entry
now carries it); the input sizes capped (the 1 MB line, the 4 KB
garbage):

**All 27 outcomes: clean rejects, NO crash, NO hang, NO huge
allocation, NO silent data loss:**
- **Truncated at every 1/8 (7 points): 0 partial loads, 7 clean
  rejects** — the strict shape check catches a truncated entity line
  before any partial use (the OBSERVED printout recorded).
- **The version field:** missing header → rejected (the v1 default
  path, OBSERVED); v99 → rejected (warn + false, out untouched);
  v-1 → rejected (the unknown-version check); the manager v99 →
  rejected.
- **The count fields:** the manager scenes= -3 → rejected; 0 →
  rejected; **2^31 → rejected by the file-count mismatch BEFORE any
  allocation** (scene.h:781 checks the mismatch before the reserve at
  :783 — no hang, no crash, no huge allocation, OBSERVED); the count
  mismatching the actual entries → rejected.
- **The duplicate scene= line** → rejected (the loadSceneFromFile's
  haveScene check). NOTE: the save format has NO entity ids — the
  entity lines are positional; there is nothing to duplicate at the
  entity level (recorded).
- **A 1 MB entity line** → rejected (the strict shape check; no hang —
  the TIMEOUT guards).
- **4 KB of binary garbage** → rejected.
- **An empty file** → rejected (the header check).
- **A directory path** → rejected (the ifstream's getline fails → the
  header check, OBSERVED).
- **Saves do not reference prefabs** (recorded): the 34-field entity
  lines carry raw values, no prefab pointers; the closest file
  reference is tilemap=<basename>; a MISSING referenced tilemap file
  rejects the load (scene.h:642-646).

**NEW FINDINGS:** (14) the strict-load contract works as documented
under programmatic corruption (positive, no action); (15) saves do
not reference prefabs; the missing tilemap reference rejects
(positive); (16) the consumer's exe was briefly locked by an external
process (LNK1104; tasklist showed no owner — an antivirus scan; the
wait resolved it) — game-specific behavior (the dev environment).

**Gates:** build exit 0; ctest -C Release 3/3 100% (8.88s/0.33s/
0.36s), exit 0 (the TIMEOUT 60 s active on the consumer's entry); alive
x3 exit 0; git diff --check clean; git diff --stat -- src/ EMPTY
(verified).

### Phase D result: classification and handoff (Step 279) — docs only, v6 stays NOT OPEN

**1) The full findings table (all 275-278 entries; 16 entries; the
final class, the evidence in FINDINGS.md, the impact + the fix size
for each defect/missing capability):**

| # | What | Class | Impact | Fix size estimate |
|---|---|---|---|---|
| 1 | A parse-fail KNOWN field (health=1e39) reported as "Unknown key" (prefab.h:153-172) | existing capability, doc-gap | misleading diagnostic | one function (the parse-fail branch's message) |
| 2 | A duplicate field silently last-wins, no warning (prefab.h:154-171) | existing capability, doc-gap | silent data choice | one function (the duplicate detection) |
| 3 | The partial-parse return carries no dropped-line count (prefab.h:101-102) | existing capability (API-clarity) | doc gap | one function (the return signature) |
| 4 | The empty file fails clean (the header check) | existing capability | — | none (positive) |
| 5 | The missing file fails clean (no crash, the out default) | existing capability | — | none (positive) |
| 6 | A true unknown key warn-skipped | existing capability | — | none (positive) |
| 7 | The probe paths never probe a data subdir; the bare name resolves against the CWD root only (prefab.h:69-75/:111-115) | existing capability, doc-gap | doc gap + the first-run friction | one function (the probe list) or the doc |
| 8 | **A src include dir makes MSVC resolve <time.h> to the engine's src/time.h, shadowing the standard header; ctime's using ::clock_t fails** (src/time.h:48-52) | **ENGINE DEFECT** | **portability** (any target with a src include dir + the standard <ctime>/<time.h> breaks) | **mechanical rename** (src/time.h -> an engine-unique name) or the doc (the relative-include pattern) |
| 9 | saveSceneToFile's fs::rename fails with a sharing violation on an open read handle (scene.h:443-448) | existing capability, doc-gap | doc gap | doc (the rename-fails-if-open) |
| 10 | The 34-field v2 format serializes floats at 4dp; >4-decimal values do not round-trip bit-exact (scene.h:418) | existing capability, doc-gap | doc gap | doc (the precision) |
| 11 | **loadSceneManagerFromFile honors ONLY the 3-candidate probe (assets/); the save honors explicit paths; the round-trip breaks for every non-assets path** (scene.h:726-728 vs :658-674) | **MISSING CAPABILITY** | **data-loss-adjacent** (the save succeeds, the load of the same path fails silently) | **one function** (the explicit-path handling, the same as loadSceneFromFile's :461-467) |
| 12 | A loadScene call may reallocate the scenes vector; a Scene& taken before it dangles (0xC0000005 when the documented reserve/re-take rule, scene.h:108-109, is missed) | existing capability (the engine behaved as documented) | the crash when the rule is missed (safety) | design change (the handle-based access) — the doc already states the rule |
| 13 | The loader's common failure paths return false SILENTLY (no stderr; only the unknown version warns, scene.h:775-777) | existing capability, doc-gap | misleading (no diagnostic) | one function (the stderr messages) |
| 14 | The strict-load contract holds under programmatic corruption (27 clean rejects) | existing capability | — | none (positive) |
| 15 | Saves do not reference prefabs; the missing tilemap reference rejects | existing capability | — | none (positive) |
| 16 | The consumer's exe briefly locked by an external process (LNK1104; an antivirus scan) | game-specific behavior | — | none (the dev environment) |

**Findings count by class:** existing capability 13 (6 with doc-gap
notes, 5 positive, 2 API-clarity); engine defect 1; missing
capability 1; game-specific 1.

**2) CANDIDATE v6 SCOPE list (evidence-ranked, NOT opened; only the
items with class engine defect or missing capability AND a repro):**
1. **Entry 8 — src/time.h shadows the standard <time.h>** (the engine
   defect, portability). REPRO (the 276 exact): a 3-line TU
   `#include <ctime>` / `int main() { return 0; }` with
   `target_include_directories(t PRIVATE "${CMAKE_SOURCE_DIR}/src" "${glfw_SOURCE_DIR}/include")`;
   the exact error: `ctime(21,25): error C2039: 'clock_t': is not a
   member of 'global namespace'` (+ asctime, clock). Fix: the
   mechanical rename (src/time.h -> an engine-unique name) or the
   documented relative-include pattern.
2. **Entry 11 — loadSceneManagerFromFile does not honor explicit
   paths** (the missing capability, data-loss-adjacent). REPRO: the
   save to "consumers/level_pipeline/rt_manager.txt" returns true; the
   load of the same path returns false silently. Fix: one function
   (the explicit-path handling, the same as loadSceneFromFile's
   :461-467).

**v6 stays NOT OPEN** — this list is a candidate scope only; opening
v6 is a separate step's decision.

**3) The Phase D result:** the steps used 275-278 (4 of the <= 5 cap);
the findings 16 (by class: above); **the stop condition did NOT fire**
(the findings kept arriving through 278; the cap not reached); the
consumer: consumers/level_pipeline/ (its own directory + target +
CTest, the TIMEOUT 60 s), headless, engine src/ read-only throughout
(git diff --stat -- src/ EMPTY at every step — verified); NO engine
code fixed in any step.

### Capability audit and candidate list (Step 272) — docs/measurement only, v6 stays NOT OPEN

**1) Capability inventory (whole engine).** 173 test functions in
tests/hostile_data_test.cpp (grep count); MISSING means grep found
nothing, with the grep named.

| Subsystem | exists (file:line) | tests (names) | known limits | status |
|---|---|---|---|---|
| rendering 2D | renderer.h:454-670 (drawWorld per-entity loop :608-628; drawDigitString :690-719; drawTextString :1046-1067) | checkFrameUV (:10181), checkFollowLerp (:3602), checkFontCells (:547), checkFontMetrics (:580), checkDebugFrameDepth (:6960) | per-entity upload+draw (one glBufferData + one uniform set + one draw per sprite, :608-628); no batching | VERIFIED |
| rendering 3D | renderer.h:734-949 (drawEntity3D :734; drawEntityMesh3D :826; drawDebugMesh3D :955-1010) | checkEntity3D (:7377), checkMesh3D (:9563), checkTint3D (:7207), checkView3D (:9505), checkCamera3D (:9607), check3DArcIntegration (:8216), checkMeshStateLeak (:6356), checkDebugFrameDepth | per-face upload/draw/uniform (60F bytes/entity); VAO/VBO created+deleted per draw call | VERIFIED |
| input | input.h (the action map :90-98; keyNameFor :160; edges) | 14 tests (checkInputEdges, checkKeysForAllActions, checkGamepadActions, checkKeyNames, checkInputAdoptionHelper, checkInputBindings, checkActionMap, checkMouseInput, ...) | Q/E registered but unwired (the 271 correction); no orbit action | VERIFIED |
| audio | audio.h (Audio, 15 public/private members) | 5 tests (checkVolumeClamp, checkMuteToggle, checkPerSoundVolume, checkMusicVolumeIndep, checkPreInitGuards, checkAudioDeviceLifecycle) | no mixer graph (the v1.1 ruling) | VERIFIED |
| physics/collision | physics.h (11); collision.h (7: aabbOverlap :85, resolveCollision, sweptAABB) | 13 tests (checkResolve, checkFriction, checkSweptAABBContract, checkDropPhysics, checkSandboxResolve, checkRestitution, checkMassWeighting, checkBounceRestHeights, ...) | AABB only, no rotation in collision | VERIFIED |
| animation | animation.h (7) + animation_data.h | 3 tests (checkAnimationClipSwitch, checkAnimationSystem, ...) | clip-level only, no blending states | VERIFIED |
| scenes | scene.h (36: Scene, SceneManager, loadScene :117, switchTo) | 17 tests (checkSceneSerialization, checkSceneManagerSave, checkSceneDumpReload, checkSceneByName, checkPlatformerLevelSwitch, ...) | pointer re-take after structural change (:108-109) | VERIFIED |
| resources/assets | resources.h (7: loadRgbTexture, loadRgbaTexture, registry) | 4 tests (checkTextureRegistry, checkMeshLoad, checkWedgeMesh, resources_test target) | OOB texture sample is UB (the 258 audit) | VERIFIED |
| serialization | scene.h:381 (saveSceneToFile), :459 (loadSceneFromFile), :651 (saveSceneManagerToFile) | 9 tests (checkSceneSerialization, checkScenePersistenceV1Compat/V2/V99, checkPrefabScenePersist, ...) | text format only, no binary | VERIFIED |
| entities | entity.h (10: Entity struct, modelMatrix, defaults) | 8 tests (checkEntityLifecycle, checkSpawnQueue, checkEntity3D, ...) | plain structs (the v1.1 ruling) | VERIFIED |
| particles | particles.h (7) | 6 tests (checkParticleColorAndEmit, checkParticleSpawn, checkEmitterRate, checkParticleMotion, checkParticleDeath, checkParticleConvert) | pool/emit only | VERIFIED |
| UI primitives | ui.h (10) | 1 test (checkButtonLayout) | plain data + pure helpers (the v1.1 ruling); retained-mode UI out | VERIFIED |
| console | console.h (18) | 6 tests (checkConsoleContract, checkConsoleHistoryRecall, checkDebug3dParse, checkNohostilesParse, checkMeshIdParse, checkCamParse, ...) | no tab completion; 7 mojibake '?' in the board (unrelated) | VERIFIED |
| math | math/vec3.h (6), math/mat4.h (2 builders + members) | 3 tests (checkRotationY, checkOrbitEye, checkPerspective) | no SIMD | VERIFIED |
| lifecycle | lifecycle.h (6) | covered by checkEntityLifecycle + the spawn tests | killRole/queueSpawn only | VERIFIED |
| asset loading | resources.h + mesh3d.h + font.h | checkMeshLoad, checkWedgeMesh, checkFontCells | the loader does not normalize bounds (disclosed v3) | VERIFIED |

**2) 3D path cost, by inspection (drawEntityMesh3D, renderer.h:826-949).**
For F = the mesh's face count (the tetra meshId 3: F=4; the wedge
meshId 5: F=8), per entity per frame:
- GL object creations/deletions: 2 creates + 2 deletes —
  glGenVertexArrays :856 + glGenBuffers :857 + glDeleteBuffers :947 +
  glDeleteVertexArrays :948 (independent of F)
- glBufferData: F calls x 60 bytes (15 floats, GL_DYNAMIC_DRAW, :938-940)
  = 60F bytes/entity/frame (independent of the mesh's total size)
- draw calls: F x glDrawArrays(GL_TRIANGLES, 0, 3) (:941)
- uniform sets: 2 + F — glUniform3f tint :840 + glUniformMatrix4fv
  mvp :867 + F x glUniform3f lit color :934
- one-time state per entity: 1 glBindTexture :838, 1 glUseProgram
  :839, the depth save/enable/restore :841-842/:943-944, 2
  glVertexAttribPointer + 2 glEnableVertexAttribArray :862-865, 2
  glBindVertexArray :858/:946, 2 glBindBuffer :859/:909, the optional
  1 glClear(GL_DEPTH_BUFFER_BIT) :852-853 (the opt-in)
For N entities: N=1 -> 4 GL object ops, 4-8 bufferData (240-480 B),
4-8 draws, 6-10 uniforms; N=10 -> 20/40-80 (2.4-4.8 KB)/40-80/60-100;
N=100 -> 200/400-800 (24-48 KB)/400-800/600-1000.
Per-frame CPU work (separate): the centroid O(V) (:883-886); per face
1 cross :914 + 1 toFace :915 + 1 dot :916 + the optional flip :917 +
the ns 3 scales :927-929 + 1 wn (3 mul-adds) + 1 normalize (sqrt)
:930 + 1 dot :931 + the max/diffuse/factor :932-933 — ~10 float ops +
1 sqrt per face, O(F) per entity.

**3) Indicative timing (NOT asserted):** a throwaway scratch benchmark
(C:\Temp workflow; the file deleted and the temporary CMake target
reverted before commit; git status verified clean afterward), hidden
window, 100 entities of meshId 5 (the wedge, 8 faces), 300 frames:
**mean drawEntity3D cost 11.764 ms/frame for all 100 entities**
(~0.118 ms/entity; ~140 entities at a 60 fps budget). Indicative only —
nothing timing-based enters tests or the tracker as pass/fail.

**4) Candidate list (at most 6; NO ranking, deciding, or opening).**
Every candidate has evidence from steps 1-3:
1. **Per-face upload batching in drawEntityMesh3D.** Evidence: 60F
   bytes + F draws + F uniforms per entity per frame (the formula,
   renderer.h:934-941); the indicative 11.764 ms/frame at N=100. Composes
   with: the existing per-face loop + the mesh registry. Tested without a
   game: the pixel asserts (the lit/color/depth) + a frame-count scaling
   check. The main risk: the batching changes the per-face color path
   (the uniform/draw ordering the 247 ruling chose for clarity). The open
   design questions: a per-entity single upload vs a persistent dynamic
   VBO; the face-count crossover where batching pays.
2. **VAO/VBO reuse across drawEntityMesh3D calls.** Evidence: 2 creates
   + 2 deletes per entity per frame (the formula, renderer.h:855-857/
   :947-948); the indicative 11.764 ms/frame at N=100. Composes with: the
   renderer's GL object ownership. Tested without a game: the state-leak
   invariant (checkMeshStateLeak asserts the VAO returns to 0 — the
   contract would need updating). The main risk: the leak-invariant
   contract change. The open design questions: a persistent VAO/VBO vs
   the temp-per-call; the invariant's new form.
3. **2D sprite batching in drawWorld.** Evidence: one glBufferData + one
   uniform set + one draw per sprite (renderer.h:608-628; the audit row);
   N sprites -> N draws. Composes with: the existing per-entity 2D loop.
   Tested without a game: the pixel asserts (checkFrameUV/
   checkFollowLerp) + a frame-count scaling check. The main risk: the
   2D path is the game's visual contract (the sprites' draw order).
   The open design questions: instancing vs batching; the sprite-count
   crossover.

**v6 stays NOT OPEN** (this section records candidates only; opening v6
is a separate step's decision).

### Step 271: the Q/E orbit false-claim correction (documentation defect)
The full grep (Step 271) found the ONLY behavior claim was the
main.cpp:1250-1252 comment ("Q/E keys also orbit in the debug3d block
below" — the wiring never existed); the authoritative tracker (the
Step 229/230 record) promises only the `debug3d orbit` console command
("debug3d-only"). The 270's engine-defect classification was based on
an incomplete claim map and is RECLASSIFIED into two parts: (1) the
false claim in the comment = **documentation defect — FIXED in Step
271** (the comment now states the only orbit is the `debug3d orbit
<degrees>` console command and records the correction; the diff shows
only comment lines, no logic — verified by build/ctest/alive exit 0);
(2) the Q/E keyboard orbit = **missing capability, NOT scheduled, NOT
a v6 candidate** unless Cyril deliberately chooses it. The 270's "v6
candidate #1" is REMOVED (the candidate list is now EMPTY). No test
asserts the usage string's exact text (the correction required no test
updates). v6 stays NOT OPEN.

### Out of v5
- No ECS, no editor, no networking, no glTF, no PBR, no new 3D work, no
  game content, no AI/MCP layer. Game-originated needs are CLASSIFIED
  (existing capability / engine defect / missing capability /
  game-specific behavior), never added.

## ENGINE v6 — third-party readiness (Step 280, COMPLETE/FROZEN Step 283)

**Version name:** Engine v6 - third-party readiness. **Status:
COMPLETE/FROZEN (Step 283, date 2026-10-03, HEAD c6282ae)** — the
done-criteria audit (10/10 VERIFIED, zero UNVERIFIED; fresh
build/ctest/alive x3 exit 0 at c6282ae: ctest 4/4 100%,
27.89s/0.94s/1.99s/0.93s) closes the version. Games frozen. **v4 and
v5 stay COMPLETE/FROZEN (Steps 263/269); this version AMENDED only the
two capabilities below (both RESOLVED); EVERYTHING ELSE in the
v3/v4/v5 frozen contract lists is UNCHANGED** (the drift check: the
rename (100% similarity), the (h) function, the new checks, the
include sites, and the pre-v6 271 comment correction — nothing else).

### v6 scope (exactly two capabilities; no others unless a classified
### finding with evidence is added)

**(g) Engine header-name hygiene (Phase D Entry 8) — RESOLVED (Step 281)**
- THE INSPECTION: 31 headers under src/ (glob); checked each against
  the standard C headers, the platform headers, and the third-party
  (GLFW, glad, stb, miniaudio): **exactly ONE collision — src/time.h
  vs the standard C <time.h>** (the src/math DIRECTORY does not shadow
  the math.h FILE; no other name matches). The #include sites for
  src/time.h: 2 code sites (src/main.cpp:96, src/pe_core.cpp:27) + 3
  RELATIVE sites (platformer.cpp:27, pong.cpp:29, hostile_data_test.cpp
  :24 — the first grep missed these; the build errors caught them);
  0 direct in the consumers; 3 CMake comment references; the docs:
  AGENTS.md:49, README.md:170, V3 (the finding records + the historical
  audit mentions), SMOKE_TEST.md none.
- THE DECISION: **rename (the default, following the project's plain
  descriptive-name convention — renderer.h, camera.h, entity.h, ...);
  the pe_ prefix NOT needed since a convention exists.** The one-line
  reason: the project's naming convention exists, so the rename follows
  it with a descriptive non-colliding name.
- THE CHANGE (mechanical, git mv): src/time.h -> src/engine_time.h
  (100% similarity); the 5 code include sites updated; the comment
  references updated (main.cpp x4, pong.cpp, CMakeLists.txt); the doc
  references updated (AGENTS.md, README.md). NO logic changes. The
  level_pipeline consumer's relative-include workaround LEFT (working;
  "if unsure, leave it").
- THE PERMANENT CHECK: tests/header_hygiene.cpp + the CMake target
  header_hygiene (the CTest registration): compiled with src ON its
  include path; its source includes <time.h>, <ctime>, and the
  engine's engine_time.h; runs and returns 0. THE POINT: src on the
  include path + the standard headers must still compile.
- MUTATION TABLE (an uncommitted stub src/time.h, restored by
  Remove-Item (untracked); ctest FAILED to build): the stub
  reintroduced a colliding header name -> the hygiene target FAILED
  with the EXACT pre-281 errors (ctime(21,25) C2039 'clock_t' +
  time_t — the stub's shadow breaks the standard header) —
  NON-VACUOUS. PROCESS NOTE: the MSVC does not track /I headers as
  build dependencies, so the first mutation build was cached
  (up-to-date) and did NOT fail; the mutation needed the exe/obj
  deleted + a forced rebuild — recorded.

**(h) Scene-manager load honors explicit paths (Phase D Entry 11) — RESOLVED (Step 282)**
- THE INSPECTION: saveSceneManagerToFile (scene.h:658-674) honors the
  explicit paths (the /, the \, the drive letter; the dir
  auto-created); loadSceneManagerFromFile (scene.h:726-728, the
  pre-282) probed ONLY assets/, ../assets/, ../../assets/ — NO
  explicit-path handling; the sibling loadSceneFromFile (scene.h:
  461-473) honors explicit paths directly first (the fileName +
  the ../fileName + the ../../fileName variants) — THE EXISTING
  PROJECT CONVENTION: the explicit paths honored directly first; the
  bare names probe the assets/-relative candidates; the absolute
  paths honored directly (the drive-letter explicitPath check).
- THE DECISION: make the manager loader follow the sibling loaders'
  EXACT path resolution (the default). The one-line reason: the
  save/load symmetry and the project convention; the existing assets/
  behavior and the absolute-path guard preserved exactly as the
  siblings implement them.
- THE CHANGE (the smallest, loadSceneManagerFromFile only): the
  explicitPath check (the drive-letter form, the siblings' exact
  shape) + the explicit candidates (the fileName + the ../fileName +
  the ../../fileName) + the bare candidates (assets/... unchanged);
  NO other function touched.
- THE TESTS (the consumer's checkManagerExplicitPaths; all PASS): the
  Entry 11 repro (the explicit-path round-trip) now passes; the
  existing assets/ round-trip (the bare name) unchanged; a
  nonexistent path still returns false; the ABSOLUTE-path load
  matches the sibling loaders (the drive-letter path, the round-trip
  content matches); the negative control (the loaded copy's health
  altered; the comparator reports the exact field "health") ✓.
- MUTATION TABLE (an uncommitted edit to src/scene.h, restored by
  pathspec; ctest FAILED): the path resolution reverted (the
  explicitPath forced false) -> THREE explicit-path tests failed (the
  Entry 11 repro, the ABSOLUTE-path load, the negative-control load)
  while the assets/ round-trip PASSED (unchanged, as designed — the
  mutation only removes the explicit-path handling) — NON-VACUOUS.

### Carry forward unchanged (recorded)
- OOB texture sample is UB (the 246 fallback proven only indirectly,
  the 258 audit).
- The consumer's fallback-draw assert does not catch a no-op
  clearRegisteredMeshes (the count assert does, the 258 nuance).
- v5 visual confirmation: **pending**.
- Q/E orbit = missing capability, NOT scheduled (the 271
  reclassification).
- Performance not eligible (the 273 verdict; the dev-machine limits).

### v6 done-criteria (the v5 done-criteria form)
The v6 version is COMPLETE/FROZEN only when:
- every capability assigned to v6 is implemented;
- behavior is deterministic where the contract requires it;
- required error handling exists;
- required regression tests exist;
- the new asserts are MUTATION-CHECKED (each must FAIL under a
  reverting mutation, run on the committed tree — the 258 audit
  pattern);
- required subsystem integration works at the engine boundaries;
- existing tests remain green;
- build/ctest/alive x3 have actual execution evidence;
- the tracker records capabilities, limits, and findings;
- implementation is committed and pushed.

### Out of v6
- No ECS, no editor, no scripting, no networking, no glTF, no PBR, no
  retained UI, no materials, no new 3D work, no prefab diagnostics
  rework, no game content, no AI/MCP layer. Game-originated needs are
  CLASSIFIED (existing capability / engine defect / missing capability
  / game-specific behavior), never added.

### Frozen contract list (v6, Step 283) — binding
- **(g) Header-name hygiene**: no src header shadows a standard, C, or
  third-party header when src is on the include path; src/time.h is
  named engine_time.h (the 281 rename, 100% similarity); the permanent
  check is the CTest target header_hygiene (compiled with src ON its
  include path; <time.h>, <ctime>, and engine_time.h coexist; runs
  and returns 0) — a reintroduced colliding header name makes it FAIL
  to build (mutation-proven).
- **(h) The manager load honors explicit paths**: loadSceneManagerFromFile
  follows the sibling loaders' exact path resolution (the explicit
  paths — the /, the \, the drive letter — honored directly first, the
  dir auto-created; the bare names probe assets/, ../assets/,
  ../../assets/ exactly as before; the absolute-path guard = the
  siblings' explicitPath form); the existing assets/ behavior
  unchanged (mutation-proven: the resolution reverted → the three
  explicit-path tests failed while the assets/ round-trip passed).
- Carry forward (unchanged): OOB texture sample is UB (the 246
  fallback proven only indirectly, the 258 audit); the consumer's
  fallback-draw assert does not catch a no-op clearRegisteredMeshes
  (the count assert does, the 258 nuance); v5 visual confirmation
  **pending**; Q/E orbit = missing capability, NOT scheduled (the 271
  reclassification); performance not eligible (the 273 verdict, the
  dev-machine limits).
- Out of v6 stands: no ECS, no editor, no scripting, no networking, no
  glTF, no PBR, no retained UI, no materials, no new 3D work, no
  prefab diagnostics rework, no game content, no AI/MCP layer.

### v6 done-criteria audit (Step 283, verbatim criteria)
| Criterion | Evidence | Status |
|---|---|---|
| every capability assigned to v6 is implemented | (g) 44f5de7 (281), (h) c395bc8 (282) | VERIFIED |
| behavior is deterministic where the contract requires it | the hygiene check enforces the rename; the loader's path resolution deterministic | VERIFIED |
| required error handling exists | a nonexistent path still returns false (the consumer's check); the strict-load paths | VERIFIED |
| required regression tests exist | header_hygiene (the CTest) + the consumer's checkManagerExplicitPaths | VERIFIED |
| the new asserts are MUTATION-CHECKED | the 281 mutation (the stub -> the C2039 FAIL); the 282 mutation (the resolution reverted -> 3 FAILS) | VERIFIED |
| required subsystem integration works at the engine boundaries | all targets build (the include sites updated); the loader via the consumer | VERIFIED |
| existing tests remain green | ctest 4/4 100% | VERIFIED |
| build/ctest/alive x3 have actual execution evidence | fresh at c6282ae: build exit 0, ctest 4/4 100% exit 0, alive x3 exit 0 | VERIFIED |
| the tracker records capabilities, limits, and findings | V3 + steps.json (280/281/282) + board + README | VERIFIED |
| implementation is committed and pushed | 44f5de7 + c395bc8 pushed | VERIFIED |

## Editor-0 brief (Step 286) — docs only, no editor code, no option chosen

**1) The editor API inventory (exists | tests | limits; file:line):**

| API an editor would use | exists | tests | limits |
|---|---|---|---|
| saveSceneToFile (scene.h:381) | YES | checkSceneSerialization, checkScenePersistenceV1Compat/V2/V99, the consumer's round-trip | 4dp floats; the explicit paths + the dir auto-create; the tmp+rename atomic; the dead skipped |
| loadSceneFromFile (scene.h:459) | YES | the same + the consumer | STRICT whole-file: any malformed line → false, out untouched; v1/v2 shapes |
| saveSceneManagerToFile (scene.h:651) | YES | checkSceneManagerSave, the consumer | the index + the per-scene saves via the 3-candidate probe |
| loadSceneManagerFromFile (scene.h:724) | YES | the consumer's checkManagerExplicitPaths | the 282 fix: the explicit paths honored (the sibling convention); the strict whole-file |
| Scene::queueSpawn/flushSpawns (scene.h:73/:81) | YES | checkSpawnQueue, the consumer | queueSpawn DEFERS to pendingSpawns; flushSpawns at end-of-frame OUTSIDE any iteration; the returned index = size+pending-1 |
| buildInitialEntities (lifecycle.h:76) | YES | checkEntityLifecycle | the Arcade's constructions (an editor builds its own) |
| resetEntities (lifecycle.h:142) | YES | checkEntityLifecycle + the snapshot tests | the snapshot-restore assignment |
| killRole (scene.h:252) | YES | checkNohostilesKill | the role-filtered kill |
| The mouse snapshot (input.h:264) | YES | checkMouseInput, checkInputEdges | the position + the buttons; Step 117's third responsibility |
| screenToWorld / screenToWorldUi (camera.h:304/:299) | YES | checkPerspective (the camera math) | screenToWorld ADDS the camera position; screenToWorldUi IGNORES it (the raw ortho box conversion); the ortho only (no 3D picking) |
| worldToScreen / worldToScreenUi (camera.h:315) | YES | checkPerspective | the inverse mapping for inspector overlays |
| pickEntity (collision.h:323) | YES | the 127-129 pick logic (CI); the SMOKE pick is HUMAN (4.3) | the AABB containment; an entity index or -1 |
| ui.h: Button/hitTest/drawButton (ui.h:98/:107/:118) | YES | checkButtonLayout | the plain data + pure helpers; **NO text-input widget** (a missing capability, pre-classified) |
| fontCellFor (font.h:34) + drawTextString | YES | checkFontCells, checkFontMetrics | the 37-cell atlas; the bitmap font renders labels/values |
| drawAABBs (renderer.h) | YES | the v5 frozen contract (checkAABBDebugSize) | the F1 debug overlay; reusable for the AABB visualization |
| registerCommand (console.h) | YES | checkConsoleContract + the parse tests | the typed commands; no tab completion |
| **PureEditor-lite v0** (V3:724; the headless path LANDED at Step 197) | YES | checkEditorLiteSuccess, checkEditorLiteSafety, checkEditorLiteKill, checkEditorLiteSpawn (4 tests) | load/select/nudge/save on TOP of the existing APIs; the non-goals (binding): full hierarchy editor, animation studio, multiplayer, ECS; the headless loop |
| **The 4-decimal float format** (scene.h:418; Phase D Entry 10) | YES | the consumer's float-edge checks | the >4-decimal values do NOT round-trip bit-exact (1e-6 → 0.0000; 0.12345678 → 0.1235); save→load→save is byte-identical (the 4dp rounding idempotent). **What it means for an editor that saves:** a nudged value with more than 4 decimals loses precision on save/load — the editor's save writes 4dp text; the round-trip is stable (byte-identical) but NOT bit-exact; an editor should either round user input to 4dp deliberately or treat the saved file as the source of truth after save. |

**2) Editor-0 scope options (at most 3, each at most 5 steps; EVALUATED,
NOT chosen):**

**A) Viewer (load, render, pan/zoom).** Capabilities used:
loadSceneFromFile/loadSceneManagerFromFile, drawWorld (the sprites),
drawAABBs, the camera's follow/position (the pan), the mouse+keys.
Steps: ~2-3. Gaps expected (pre-classified): none for the load path
(the (h) fix landed); the zoom is an unmet capability if the camera has
no zoom API (a classified finding for a later engine version if
needed). The findings it can produce: the load-path frictions (the
strict whole-file vs partial files, the malformed level files), the
render-order questions, the camera pan edges. Write-safety: nothing is
written (the read-only viewer).

**B) A + pick + read-only inspector.** Capabilities used: A +
pickEntity + screenToWorld (the picking), worldToScreenUi (the
inspector overlay), ui.h Button/hitTest/drawButton + drawTextString
(the panel), the bitmap font. Steps: ~3-4. Gaps expected
(pre-classified): **missing text input** (no engine text-input widget —
the inspector displays values but cannot edit text — a classified
finding for a later engine version); **no field display helper** (the
inspector's label/value rows are built from ui.h's plain helpers —
possible but raw). The findings: the pick's AABB-only limits (the
rotated entities pick by their AABB), the hit-test vs the coordinate
conventions, the ui.h helper edges. Write-safety: nothing written.

**C) B + nudge + save-as.** Capabilities used: B + the entity mutation
(the nudge = the position field), saveSceneToFile (the SAVE-AS). Steps:
~4-5. Gaps expected (pre-classified): the save-as must NOT overwrite
the source scene (the write-safety rule below); the 4dp precision (the
nudged values beyond 4dp lose precision on save); the missing text
input still binds (the nudge is key/arrow-driven, not typed). The
findings: the save-path frictions (the explicit paths, the tmp+rename),
the round-trip stability, the field-mutation edges. **Write-safety
rule (ALL options): an editor save writes a NEW file and NEVER
overwrites the source scene until the round-trip is proven** (the
Phase D pattern: save→load→compare before any overwrite is even
considered).

**3) Do NOT choose an option; do NOT write editor code** — this brief
is the evaluation basis; the choice is a human decision.

## PHASE TOOLS/EDITOR (Step 284, OPEN - a phase, NOT a numbered engine version)

**The unattended-phase protocol (Step 285) is now a standing order in
AGENTS.md ("Unattended phase protocol") — binding on unattended runs:
the preflight checks, the per-step gate/record/push/verify loop, the
STOP conditions, the mutation-check discipline, the consumer/Tools
read-only rule, the docs hygiene, the SSH-alias push route and
credential rules, and the human-only decisions list. See AGENTS.md for
the verbatim orders.**

**AMENDED Step 284 (2026-10-03, explicit allow by Cyril):** an
editor or tool is a CONSUMER of the frozen engine, like a game. The
frozen engine itself ships no editing tools; v4/v5/v6 stay frozen and
their Out lists are unchanged (they describe the ENGINE's per-version
scope, not consumer work).

**The Tools rules (binding on any Tools step):**
- Own directory and own CMake target; documented APIs only; engine
  src/ is read-only during any Tools step (git diff --stat -- src/
  empty — verified).
- Every friction is logged in a findings log with exactly one class
  (existing capability / engine defect / missing capability /
  game-specific behavior) and evidence; no engine fix inside a Tools
  step; engine changes happen only through a normal engine version,
  opened docs-first from classified findings.
- **Prohibited loop:** EDITOR NEED → ENGINE FEATURE → EDITOR TEST →
  NEW ENGINE FEATURE.
- Tools steps are marked "Tools" in the tracker; unmarked steps remain
  runtime engine steps. The one-step rule binds.
- UI: the editor builds its own immediate-mode panels in its own
  target using existing ui.h helpers and the bitmap font. NO engine
  retained-UI framework and no engine text-input widget. A UI need
  that cannot be met becomes a classified finding for a later engine
  version.
- Still banned, each needing its own explicit phase: ECS, networking,
  scripting language, hot-reload, retained-mode UI in the engine,
  AI/MCP layer, materials system. Not a Unity/UE/Godot clone; no
  feature-parity goal.

### Phase Tools consumer: editor0 (Step 288) - step 1 of 5

TOOLS/Editor consumer step; no engine version opened, v4/v5/v6 stay
frozen. tools/editor0/ landed: the PureEditor0 CMake target (CMake
changes limited to adding it + its fixture copy, the consumer
pattern), editor0_core.h (editor-owned; loadSceneForEditor(path,
pe::Scene&, err) -> bool - loads into a temporary Scene via
pe::loadSceneFromFile, replaces `current` only on success; on failure
`current` is untouched and err is set), main.cpp (headless --selftest
only, no window/no GL), FINDINGS.md (3 entries, one class each, none
fixed inside the Tools step). --selftest: 21 checks green - the
3-prefab scene from the existing consumers/level_pipeline prefab
files, save to a temp path, the editor load, the entity count and
every one of the 24 compared fields round-trip, the missing-file
failure returns false + err set + `current` untouched, the negative
control names the altered field (health). CTest TIMEOUT 60, count
4 -> 5 (5/5 100%). No engine code added or changed (git diff --stat
-- src/ empty); findings recorded: the loader's bare false with no
reason code (existing capability), the per-consumer fixture-copy
duplication (existing capability), the pe::-qualified engine types
(existing capability).

### Phase Tools consumer: editor0 (Step 289) - step 2 of 5

TOOLS/Editor consumer step; no engine version opened, v4/v5/v6 stay
frozen. Implementation committed FIRST (the anchoring rule): commit
`e799b90` - tools/editor0/ extended: editor0_core.h gains the pure
makeStatusLine(path, count, err) (ok: "editor0: <path> | <count>
entities"; err: "editor0 ERROR: <err>", err takes precedence);
main.cpp rewritten: `PureEditor0.exe <scene>` opens a window (the
Pong pattern: glfwInit -> hints -> window -> context+glad ->
renderer.init -> camera+onResize -> loop -> shutdown), loads through
loadSceneForEditor, draws all entities + their debug AABBs
(drawWorld + drawAABBs with playerRoleId -1: all yellow) + the
bitmap-font status line (file name, entity count); on load failure
the error text is on screen and the editor keeps running; NO saving
of any kind. --selftest: the pure parts need no window; + the
status-line checks (ok/error formats + the precedence negative
control) + ONE hidden-window frame (GLFW_VISIBLE FALSE, real GL:
flagsForCount flags, clear + drawWorld + drawAABBs + drawTextString
+ swap, glGetError() == 0 after it, the one-iteration loop exits
cleanly) - 27 checks green. CMakeLists.txt: the PureEditor0 target
gains stb_impl.cpp + glad_gl.c + the deps/stb/glfw include dirs +
link glfw. GATES against the committed tree (all as separate calls:
a multi-quote one-liner broke cmd's parsing): cmake --build build
--config Release exit 0; ctest -C Release exit 0 5/5 100% (12.92s);
alive x3 exit 0; git diff --check exit 0 clean; git diff --stat --
src/ exit 0 EMPTY. FINDINGS: 6 entries (the 288 three + the renderer
asset probe's implicit repo-root dependency, the mandatory colliding
flags for a stateless viewer, the wide GL-consumer include set - all
existing capability, none fixed inside the Tools step). Visual
confirmation: PENDING.

### Phase Tools consumer: editor0 (Step 290) - step 3 of 5

TOOLS/Editor consumer step; no engine version opened, v4/v5/v6 stay
frozen. Step 289 visual feedback: the window opened/stayed open; the
on-screen error text was NOT reported - the error line display stays
UNCONFIRMED, visual confirmation PENDING. Implementation committed
FIRST (the anchoring rule): commit `6ae05f5` - tools/editor0/
extended: editor0_core.h +69 (kEditorZoomMin 0.25 / kEditorZoomMax
4.0 EDITOR-documented limits - the engine documents none; clampedZoom;
cameraPanDelta (the formula: worldPerPixel = 2*half/(zoom*fb); the
camera moves (-dx*wppx, +dy*wppy), the y flip); zoomedProjection
(pe::Mat4::scale composed into the caller's projection - a
screen-center zoom; the engine API takes the caller's matrix
unchanged); screenToWorldAtZoom/worldToScreenAtZoom (the documented
pe::screenToUi/uiToScreen fed the zoomed half-extents + camPos)).
main.cpp: the --make-sample mode (buildSampleScene + makeSampleScene-
File: saves with pe::saveSceneToFile, REFUSES to overwrite an
existing file, the file untouched); the viewer wiring: pe::Input
edge-tracked keys (ESC quit, +/= zoom in x1.25, -/_ zoom out /1.25 -
one step per press; NO wheel - input.h exposes no scroll), left-drag
pan via cameraPanDelta + Camera::follow (the documented position
setter; position is private), the zoomed projection for drawWorld/
drawAABBs/drawTextString. --selftest: 41 checks green - the sample
generator (written, loads, count 3, REFUSED, unchanged), the pan/zoom
math (the 12x9 box, the round-trips at tolerance 1e-4, the engine
Camera agreement, the formula delta (-1.5, -0.75), the known-point
move (100, -50) px at tolerance 1e-3, BOTH negative controls, the
zoom clamps, the zoomed round-trip) + the 288/289 checks. MUTATION
(on the committed tree, never committed): the zoom max clamp dropped
-> 'FAIL: pan/zoom: zoom clamps at the documented max (4.0)' exit 1 -
NON-VACUOUS; restored by pathspec; git diff --stat -- src/ EMPTY.
GATES against the committed tree: cmake --build build --config
Release exit 0; ctest -C Release exit 0 5/5 100% (43.22s, PureEditor0
16.02s); alive x3 exit 0; git diff --check exit 0 clean; git diff
--stat -- src/ exit 0 EMPTY; --make-sample smoke: the path printed
exit 0, the second run REFUSED. FINDINGS: 9 entries (the camera zoom
API = missing capability, the wheel/scroll input = missing
capability, the pe::-qualification + header self-containment =
existing capability). Visual confirmation (the drawn pixels + the
pan/zoom feel): PENDING.

### Phase Tools consumer: editor0 (Step 291) - step 4 of 5

TOOLS/Editor consumer step; no engine version opened, v4/v5/v6 stay
frozen. Step 290 visual feedback CONFIRMED by Cyril (the viewer
works, no problems reported). Implementation committed FIRST (the
anchoring rule): commit `df152d1` - tools/editor0/main.cpp: the
reload key R (edge-tracked; the current path through
loadSceneForEditor; on success the scene is replaced and the status
re-made, on failure the error shows and the scene is untouched);
checkReload (the 3-entity sample loaded, the file rewritten with a
DIFFERENT 2-entity count programmatically, saved to the SAME path
(the engine saver's rename-overwrite contract), reloaded - the scene
equals the second file exactly (count + per-entity fields), NO
leftover entities, NO pending spawns from the first load; the
negative control names the altered field); checkHostileLoads (the
inputs generated PROGRAMMATICALLY into the temp dir, never
hand-edited, nothing committed: the empty file, 7 truncations, wrong
v99 + negative v-1 versions, 4 KB garbage, the 1 MB line, count
fields 0/-1/2^31/5 (the SCENE v2 format has NO count field -
unknown-key strict rejects, observed), a directory path, a missing
path - EVERY case: false + a non-empty err + the previous scene
UNCHANGED (the full field comparison against the snapshot); NO
crash, no hang; the negative control on the unchanged-checker). 65
selftest checks green. MUTATION (on the committed tree, never
committed): loadSceneForEditor made to replace `current` BEFORE
parsing succeeds -> 18 checks FAILED (every "leaves the scene
unchanged" for the parse-reaching cases) - NON-VACUOUS; the
missing-file check still passed (the probe's early return protects
it - FINDINGS Entry 12); restored by pathspec; git diff --stat --
src/ EMPTY. GATES against the committed tree: cmake --build build
--config Release exit 0; ctest -C Release exit 0 5/5 100% (13.31s);
alive x3 exit 0; git diff --check exit 0 clean; git diff --stat --
src/ exit 0 EMPTY. FINDINGS: 12 entries (the hostile outcomes, the
directory classification + the ifstream inconsistency, the mutation
nuance - all existing capability; NO engine defect found). Visual
confirmation: PENDING (the 289 error-line display stays
unconfirmed; the 290 viewer confirmed by Cyril).

### Phase Tools consumer: editor0 (Step 292) - step 5 of 5, Editor-0 slice COMPLETE

DOCS ONLY (no code, no features, no version/phase opened; v4/v5/v6
stay frozen). Human feedback recorded VERBATIM: the prompt's
feedback placeholder was not replaced - the 291 hostile visual
feedback is recorded as "none"; the 289 error-line display stays
PENDING (the on-screen text was not reported); the 290 viewer
visual CONFIRMED by Cyril.

**The slice, 288-291 (5 steps, 6 commits, 65 checks):** 288
(`a532f41`): tools/editor0/ + the PureEditor0 target +
loadSceneForEditor + the headless --selftest (21 checks; the CTest
count 4 -> 5). 289 (`e799b90` impl + `44b3094` record): the window
viewer (entities + debug AABBs + the bitmap-font status line; load
failure keeps running) + the hidden-window GL frame
(glGetError() == 0; 27 checks). 290 (`6ae05f5` impl + `5fdeb1d`
record): --make-sample (refuses to overwrite) + mouse-drag pan +
+/- zoom (clamped 0.25-4.0, editor-documented limits; 41 checks).
291 (`df152d1` impl + `d93e85d` record): the R reload (the scene
replaced exactly, no leftovers/spawns) + the hostile loads (12+7
case types, all clean rejects, the scene unchanged every time, NO
crash/hang; 65 checks). Both mutations NON-VACUOUS (290: the zoom
max clamp; 291: the replace-before-parse). FINDINGS: 12 entries
classified (10 existing capability - 4 with doc-gap notes, 2
missing capability: the camera zoom API + the wheel/scroll input,
0 engine defect, 0 game-specific).

**Is Editor-0 option B blocked? NO - evidence, not chosen:** the
pick trio exists (pe::pickEntity collision.h:323 +
pe::pickEntityAtScreen collision.h:364-369), worldToScreenUi
(camera.h:293-300), ui.h Button/hitTest/drawButton
(ui.h:98/107/118) + the bitmap font (renderer.h:1027) - all
verified. The blockers: NO text-input widget - blocks NEITHER B
(read-only inspector) NOR C-as-specified (the nudge is
key/arrow-driven), it would block a future typed-edit feature; NO
camera zoom API - blocks NEITHER (the editor-side
zoomedProjection + screenToWorldAtZoom composition is proven,
290); NO wheel input - blocks NEITHER (zoom is +/- keys, 290).
What can proceed in tools/ WITHOUT lifting any freeze: option B (a
Tools step prompt), option C (the nudge + save-as - the
write-safety rule: an editor save writes a NEW file, never
overwrites the source until round-trip proven), docs work.

**Options open to the human (prerequisites; NONE recommended):**
(a) option B in tools/ - prerequisites: the verified APIs above, a
Tools step prompt, no freeze lift, ~3-4 steps (the V3 brief).
(b) a docs direction spec - prerequisites: none (docs only).
(c) a seed-and-noise version open - prerequisites: a HUMAN-ONLY
decision (opening a version is never agent-initiated); per the
version rules a real demonstrated need in the source/build
(classified findings) + a docs-first scope; additive/opt-in/
engine-pure; the freeze guardrails apply.
(d) stop - the phase ends; the engine stays frozen; the Tools
phase closes at 5 of 5.

Gates (docs only, unchanged): cmake --build build --config Release
exit 0; ctest -C Release exit 0 5/5 100%; alive x3 exit 0; git
diff --check exit 0 clean; git diff --stat -- src/ exit 0 EMPTY.

### PureEngine Direction Specification (Step 293, docs only)

**HUMAN DECISION (Cyril, explicit, recorded by the Step 293 prompt,
2026-10-08, verbatim):** PureEngine's product goal is a
general-purpose 2D + 3D engine. 2D is the proven production path;
3D is introduced progressively, one capability per version, through
the engine-first process. This decision does NOT lift any other
freeze: ECS, networking, scripting, hot-reload, engine retained UI,
PBR, glTF, materials system and volumetric generation stay blocked
until each gets its own explicit human phase-open step.

**The rule:** the current state does NOT define the final boundary.
Frozen is not rejected: only a human phase-open step unfreezes
anything. A phase is a numbered list of steps written by a human in
the prompt (the Step 285 protocol); the agent executes only those
steps, never invents steps, and never marks anything CONFIRMED that
a human must confirm.

**The 5-category map (a filing system, NOT a roadmap).** Every
capability, subsystem, or finding files into exactly ONE category.
The placement questions:
1. Core foundation - does it define or guard the engine's own
   data/time/lifecycle? (math, entity, collision, state, events,
   time, lifecycle, input, gamepad)
2. World + content - does it describe or load the WORLD the game
   runs in? (scene, tilemap, prefab, hostile data, animation data)
3. Graphics + audio + assets - does it draw, light, or sound?
   (renderer, shader, lighting, camera, font, ui, particles, audio,
   mesh3d, resources)
4. Simulation + game systems - does it simulate or drive gameplay?
   (physics, simulation, components)
5. Editor + build + distribution - does it edit, build, package, or
   ship? (tools/, CMakeLists.txt, package.ps1, the game targets)
The map files work; it does NOT schedule work or define the next
step - the version rules and the Step 285 protocol do that.

**Current state per category (file:line evidence, grep-verified
2026-10-08; the 2026-10-02 audit (Step 272, V3:1387: 16/16
subsystems VERIFIED, 173 test functions in tests/
hostile_data_test.cpp) is the BASELINE - no invented progress
bars):**

**1 Core foundation - EXISTS (the v1.0 core, Step 100; audit
VERIFIED):** **[Step 314, 2026-10-13: the Core Foundation CATEGORY AUDIT is COMPLETE (the inventory + the gap list + the acceptance standard + the 5 proposed slices - the audit section below); the .h/.cpp migration is IN PROGRESS pending Cyril's approval of the standard; no migration code written; the audit covered math/time/window-bootstrap/hygiene/pe_core+tests ONLY - entity/collision/state/events/lifecycle/input/gamepad were NOT inventoried.]** math (src/math/vec3.h, src/math/mat4.h - rotationZ
:125 + rotationY :260 builders; known limit: no SIMD), entity
(src/entity.h:46-47 the SINGLE-AXIS rotation - "current Z rotation
in RADIANS"; :279 Mat4::rotationZ; :128 no TRS composition; plain
structs per the v1.1 ruling), collision (src/collision.h:323
pickEntity, :364-369 pickEntityAtScreen; AABB only), state
(src/gamestate.h), events (src/events.h), time (src/engine_time.h
:58-61 the pre-loop CLOCK seed - not an RNG seed), lifecycle
(src/lifecycle.h:152 flagsForCount), input (src/input.h:264-290
the MouseState snapshot + the raw/edge reads; "no scroll, no
cursor management" :267), gamepad (src/gamepad.h).

**2 World + content - EXISTS (audit VERIFIED):** scene
(src/scene.h:117 loadScene, :381 saveSceneToFile, :459
loadSceneFromFile, :651/:724 the manager save/load; text format
only, no binary), tilemap (src/tilemap.h), prefab (src/prefab.h
:103 loadPrefab, :181 instantiatePrefab), hostile data (src/
hostile_data.h - data-driven config), animation data (src/
animation.h + src/animation_data.h; clip-level only, no blending).

**3 Graphics + audio + assets - EXISTS, with the 3D versions v3/
v4/v5 COMPLETE/FROZEN (audit VERIFIED):** renderer
(src/renderer.h:141 init, :401 drawWorld, :1085 drawAABBs, :1027
drawTextString; known limits: the per-entity 2D upload+draw, no
batching; the per-face 3D VAO/VBO create+delete per draw), shader
(src/shader.h), lighting (src/lighting.h), camera (src/camera.h
:204 onResize with the halfHeight=4.5 LOCK - no zoom API; :237-249
orbitEye, the XZ-PLANE-ONLY orbit, debug3d-only), font
(src/font.h:31 TextAlign, :58 alignOffsetX), ui (src/ui.h:98
Button, :107 hitTest, :118 drawButton; plain data + pure helpers -
NO text-input widget), particles (src/particles.h:105 std::rand -
NON-seedable, :23-28 documents the gap and the call sites), audio
(src/audio.h - the SFX pool/event sounds/music loop/mute; no mixer
graph per the v1.1 ruling), mesh3d (src/mesh3d.h:102
loadMeshFromObj - v+f lines only, NO vn/vt :92/:184, per-face
planar UVs, no glTF, no index buffers, no materials), resources
(src/resources.h - blob/pack/cache/probe; OOB texture sample UB
disclosed).

**4 Simulation + game systems - EXISTS (audit VERIFIED):** physics
(src/physics.h - gravity, impulse resolve, statics, the character
controller; Step 173's sweptAABB opt-in, the game wire still on
request), simulation (src/simulation.h), components
(src/components.h).

**5 Editor + build + distribution - PARTLY EXISTS:** the Editor-0
slice (tools/editor0/ - editor0_core.h, main.cpp, FINDINGS.md: the
Steps 288-292 viewer/sample-generator/pan/zoom/reload/hostile-load
proof, 65 checks, the CTest count 5; option B NOT BLOCKED per the
292 classification), CMakeLists.txt (the targets + the fixture
copies), package.ps1 + the six asset-generation scripts
(make_beep.ps1, make_checker.ps1, make_font.ps1, make_music.ps1,
make_paddlesheet.ps1, make_textures.ps1 - the committed assets/),
the game targets (games/pong/pong.cpp, games/platformer/
platformer.cpp). The UNIFIED editor is NOT OPEN: the Editor-0
slice is a viewer + a sample generator, not an editor.

**The phases (EVERY one NOT OPEN; the map files them, the human
opens them). For each: entry criteria, first capability (citing
verified evidence), test path, exit criteria.**

**P1 shared foundation** - PARTLY EXISTS (the v1.0 core's 9 Core
subsystems above; audit VERIFIED). NOT OPEN.
- Entry criteria: a human phase-open step naming the capability;
  a classified finding showing the need in the source/build.
- First capability (candidate, cited): a seedable deterministic
  RNG framework - the gap is documented IN-SOURCE
  (src/particles.h:23-28: "a real rng (seedable, deterministic)"
  deferred; std::rand used, the tests assert ranges never exact
  values) and the engine_time "seed" is the clock only
  (src/engine_time.h:58-61).
- Test path: headless (the pure math - deterministic asserts
  become possible once seedable; no GL, no window).
- Exit criteria: the capability verified by a dedicated test; the
  audit scores re-run; no frozen behavior changed or unfrozen.

**P2 2D stabilization** - PARTLY EXISTS (v5 COMPLETE/FROZEN Step
269: the 2D AABB debug-draw correctness (c) RESOLVED Step 268 -
the drawn outline = the collision box, pixel-verified, the visual
confirmation PENDING). NOT OPEN.
- Entry criteria: a human phase-open step; a measured or
  classified 2D defect (the 2D re-open trigger is UNMEASURED -
  Step 274).
- First capability (candidate, cited): the 2D sprite-batching
  re-open trigger measurement (src/renderer.h:608-628 the
  per-entity upload+draw; Step 274: "the 2D trigger unmeasured -
  the batch hack was stubbed").
- Test path: headless + the pixel checks (the Step 268 pattern) +
  the HUMAN visual confirmation.
- Exit criteria: the pending visual confirmations closed (human);
  no drift; the audit re-run.

**P3 asset/scene foundation** - PARTLY EXISTS (scene/tilemap/
prefab/resources/animation above; the field-exact v2 round-trip,
Step 276). NOT OPEN.
- Entry criteria: a human phase-open step; a classified finding.
- First capability (candidate, cited): the load/save REASON CODES
  - pe::loadSceneFromFile returns bare false with no reason
  (src/scene.h:459-473; FINDINGS Entry 1 - the editor had to probe
  the file itself to distinguish not-found from parse-failed).
- Test path: headless (the loader tests - the Steps 276-278
  pattern: the field-exact round-trip, the hostile inputs
  generated programmatically).
- Exit criteria: the round-trip stays field-exact; the reason
  codes verified; no frozen behavior changed.

**P4 3D foundation** - PARTLY EXISTS (the 3D ENGINE VERSION
COMPLETE/FROZEN Step 256 + v4 COMPLETE/FROZEN Step 263:
Mat4::perspective/lookAt (Step 191), the camera 3D mode (Step
193), the single depth clear (Step 201), the world-space lighting
+ the sizing (Steps 260-261)). NOT OPEN.
- Entry criteria: a human phase-open step; a classified 3D
  finding (the re-open trigger ~10 concurrent 3D entities,
  Step 274).
- First capability (candidate, cited): the entity rotation is
  SINGLE-AXIS (src/entity.h:46-47 "current Z rotation in RADIANS",
  :279 Mat4::rotationZ - the rotationY builder exists at
  src/math/mat4.h:260 but the entity path never consumes it) and
  the camera orbit is XZ-PLANE-ONLY (src/camera.h:237-249) - a
  candidate: the per-axis entity rotation or the full camera
  orbit.
- Test path: the hidden-window GL + the depth/state checks (the
  Step 261 pattern) + the SMOKE gates.
- Exit criteria: the done-criteria audit (the Step 256/263
  pattern); both mutations caught; no frozen behavior changed.

**P5 3D assets** - PARTLY EXISTS (the OBJ loader src/mesh3d.h:102
loadMeshFromObj - v+f lines ONLY, NO vn/vt :92/:184; the mesh
registry Step 242; the textured loaded meshes Steps 245/250).
NOT OPEN.
- Entry criteria: a human phase-open step; a classified finding.
  glTF stays BLOCKED (the decision above) until its own
  phase-open step.
- First capability (candidate, cited): the vn/normals support in
  loadMeshFromObj - the loader skips vn/vt (src/mesh3d.h:92/:184)
  and the loaded meshes' lighting normals are COMPUTED per-face
  (the v4 path), not read.
- Test path: headless (the loader) + the hidden-window GL (the
  sampling, the Step 261 pattern).
- Exit criteria: the round-trip + the sampling verified; no
  frozen behavior changed.

**P6 unified editor** - PARTLY EXISTS (the Editor-0 slice; option
B NOT BLOCKED - the 292 classification). NOT OPEN.
- Entry criteria: a human phase-open step (a Tools phase step
  list); the editor stays a CONSUMER (own directory, own target,
  engine src/ read-only).
- First capability (candidate, cited): option B - the pick +
  read-only inspector; the APIs verified (pe::pickEntityAtScreen
  src/collision.h:364-369, pe::worldToScreenUi src/camera.h:293-300,
  ui.h Button/hitTest/drawButton :98/:107/:118 + the bitmap font
  src/renderer.h:1027).
- Test path: the headless pure parts + the hidden-window GL frame
  (the Step 289 pattern) + the HUMAN visual confirmation (the
  on-screen error-line text unreported so far - the 289 display
  stays UNCONFIRMED).
- Exit criteria: the selftest green; the visual confirmations;
  git diff --stat -- src/ empty.

**P7 simulation** - PARTLY EXISTS (physics.h + Step 173's
sweptAABB opt-in; the game wire on request). NOT OPEN.
- Entry criteria: a human phase-open step; a classified finding
  with a concrete engine-validation reason.
- First capability (candidate, cited): the useSwept controller
  game wire (the board: "useSwept controller opt-in landed
  (Step 181); game wire still on request").
- Test path: headless (the physics twin tests - the Step 175
  pattern).
- Exit criteria: the twin tests green; no drift; the audit
  re-run.

**P8 procedural generation** - DOES NOT EXIST (no seedable RNG, no
noise functions anywhere in src/ - grep-verified 2026-10-08;
particles.h:23-28 is the only randomness and it is non-seedable
std::rand). NOT OPEN.
- Entry criteria: a HUMAN-ONLY decision (the 292 option (c)) + a
  docs-first version brief; the freeze guardrails apply
  (additive, opt-in, engine-pure).
- First capability (candidate, cited): the seedable deterministic
  RNG (src/particles.h:23-28 documents the gap and the exact call
  sites) - then the noise functions (none exist).
- Test path: headless (the pure math; deterministic asserts
  possible once seedable).
- Exit criteria: the deterministic tests; the audit re-run; no
  frozen behavior changed.

**P9 build/package/export** - PARTLY EXISTS (package.ps1 + the six
asset-generation scripts + CMakeLists.txt; Step 138's
single-source zip, Step 143's dry-run note). NOT OPEN.
- Entry criteria: a human phase-open step; a classified finding.
- First capability (candidate, cited): the second-game packaging -
  Pong/Platformer are NOT packaged (CMakeLists.txt:316-317: "No
  POST_BUILD asset copy yet: run Pong from the repo root ...
  packaging a second game is a separate decision").
- Test path: the package.ps1 dry-run + the exe launches (the
  alive-probe pattern).
- Exit criteria: the exes run from the package (the alive
  probes); no frozen behavior changed.

**Agent rules (the Direction Specification's own):**
- The repository is truth: inspect before claiming; every name and
  file:line in this section was grep-verified 2026-10-08; anything
  not verified is marked UNVERIFIED (the 5x console-print
  mechanism, FINDINGS; the 289 on-screen error-line display).
- One capability per version; no feature dumping.
- Do NOT copy Unity or Unreal checklists; the product is its own
  2D+3D engine with its own proven step model.
- Any game or editor need is CLASSIFIED (existing capability /
  engine defect / missing capability / game-specific behavior,
  with evidence), never added silently; the classified findings
  are the ONLY input to a future version open.
- Frozen is not rejected: only a human phase-open step unfreezes
  anything.

### Phase Tools consumer: editor0 option B (Step 294) - step 1 of at most 4

TOOLS/Editor consumer step; no engine version opened, v4/v5/v6 stay
frozen. Implementation committed FIRST (the anchoring rule): commit
`f55c0ef` - tools/editor0/ extended: editor0_core.h +80
(kEditorClickThresholdPx 4.0 EDITOR-documented; PointerGesture +
classifyPointerGesture - the Euclidean pixel distance, the
inclusive boundary; reportMessage(std::ostream&) - the print sink;
pickEntityAtScreenZoomed - the zoom-aware pick composed from
screenToWorldAtZoom + the DOCUMENTED pe::pickEntity (the engine's
pickEntityAtScreen has no zoom knowledge, collision.h:364-369);
reloadForEditor - on SUCCESS the scene replaced AND the selection
CLEARED, on FAILURE both UNCHANGED (documented); the
makeStatusLine overload with the selection: " | selected <i>
(<tag>)", err takes precedence). main.cpp: the selection wiring
(the press captures the position; on release the gesture is
classified - a CLICK selects via the zoomed pick or CLEARS on empty
space, a DRAG panned as before); the highlight = the selected
entity's ROLE GROUP via the engine's playerRoleId parameter (no
engine change; the sample's distinct roles make it exact); the R
reload via reloadForEditor; the status shows the selection; ALL
diagnostic sites routed through reportMessage. --selftest: 82
checks green - the pick at a NON-DEFAULT pan and zoom (the screen
position from the formulas = (500,100), the click selects), a
click outside (-1), the negative control (the wrong zoom 0.75
lands at (4,5) and MISSES), the overlapping pick order OBSERVED
(index 0, depths 5 then 2 - the highest depth wins, ties keep the
earlier index, collision.h:341-343; recorded as a finding, NOT
asserted as a contract), the dead entity NOT selectable (-1 - the
engine skips alive=false, collision.h:330-332), the gesture
threshold boundary, the reload/selection lifecycle (cleared on
success, kept on failure), the print discipline (ONE failed load =
EXACTLY ONE report in the test sink). MUTATION (on the committed
tree, never committed): the zoom IGNORED in the pick conversion ->
the pick test FAILED exit 1 - NON-VACUOUS; restored by pathspec;
git diff --stat -- src/ EMPTY. GATES against the committed tree:
cmake --build build --config Release exit 0; ctest -C Release exit
0 5/5 100% (17.03s; the PureEditor0 duration 0.49s - under the 40s
guard); alive x3 exit 0; git diff --check exit 0 clean; git diff
--stat -- src/ exit 0 EMPTY. FINDINGS: 17 entries (the zoomed pick
composition, the role-group highlight + the per-entity-color
missing capability, the observed overlap order, the dead skip, the
print discipline - all existing capability). Visual confirmation:
PENDING (the on-screen error-line display stays UNCONFIRMED; the
selection/highlight visual confirmation PENDING).

### Phase Tools consumer: editor0 option B (Step 295) - step 2 of at most 5, COMPLETE

Tools/Editor consumer step; no engine version opened, v4/v5/v6 stay
frozen. The live-path wiring + the coordinate ratio fix landed
(commit `95b81a8`; the STOP note recorded the size-cap breach: net
+354 > ~300; the step completed by the human's follow-up). The
coordinate-space audit (grep-verified): the cursor = WINDOW
coordinates (glfwGetCursorPos, input.h:374, camera.h:60/:286 - the
engine documented the space correctly); the sizes = FRAMEBUFFER
pixels (glfwGetFramebufferSize, main.cpp:724/:777/:829/:851). THE
DEFECT (editor-side, found and fixed): an unconverted cursor picked
the wrong world point at display scaling != 100% (the reported
no-selection). The fix: windowToFbX/Y (the ratio conversion) before
any pan or pick in stepEditorFrame; runViewer rewired to EditorState
+ stepEditorFrame (GLFW-free, plain data in); the status feedback
(reloaded <file> (<N> entities) / reload failed: <reason> / the
startup 'no selection') exact-string tested; --diag-input (ONE line
per click: the cursor/window/fb/world/pick). 102 selftest checks
green (the ratio tests at 1.0/1.25/1.5 + the negative controls: the
raw cursor MISSES; the drag pans but does not select; the
empty-space click clears; the R reload through the real step
function; the failed reload keeps both). MUTATION: the ratio
skipped in the pick path -> both ratio tests FAILED (1.25 + 1.50) -
NON-VACUOUS; restored by pathspec; git diff --stat -- src/ EMPTY.
GATES against 95b81a8: cmake --build build --config Release exit 0;
ctest -C Release exit 0 5/5 100% (14.50s; the PureEditor0 duration
1.22s - under the 40s guard); alive x3 exit 0; git diff --check
clean. HUMAN PASS (Cyril, verbatim, 2026-10-09): the live select/
clear/drag/zoom/R/ESC all work; the truncated load printed 1 line
(the 294 fix confirmed). The 294/295 visual confirmations:
CONFIRMED.

### Phase Tools consumer: editor0 option B (Step 296) - step 2 of at most 5, COMPLETE

Tools/Editor consumer step; no engine version opened, v4/v5/v6 stay
frozen. The read-only inspector panel landed (commit `f47f1aa`; the
work was stashed unverified during the LNK1104 lock blocker and
recovered cleanly - FINDINGS Entry 23). THE INSPECTOR:
editor0_core.h gains f4 (the SCENE SAVER's 4-decimal style -
the panel shows exactly what the file round-trips), makeInspectorLines
(the FIXED field order: 36 lines = the 24 scene-saver fields +
the context lines; the per-component lines so every line fits the
12x9 default view; the "+N more" overflow; no selection -> ONE "no
selection" line; the panel NEVER edits), inspectorLinesForSelection,
pointInPanelRect (the panel's ACTUAL extent - the 12-line inspector
when selected, the one-line strip otherwise; the line count is
deterministic so a startup strip never suppresses the entity clicks
- the FIRST fixed-extent rect suppressed them and the 3 selection
tests caught it, FIXED); the click suppression in stepEditorFrame (a
click inside the panel rect NEVER changes the selection - a complete
no-op; the entities behind the panel are not clickable - pan/zoom to
reach them; documented); the panel draw in runViewer (UI space, the
UNZOOMED base projection - immune to pan and zoom). --selftest: 113
checks green (the 36 exact lines, the field coverage, the +N more
boundary, the no-selection, the click-in-panel keeps selection + the
outside-rect clears control, the hidden-window panel draw
glGetError() == 0, and every 288-295 check unchanged). MUTATION: the
health field dropped from makeInspectorLines -> 6 checks FAILED (the
coverage: "the label [health] has no line"; the exact lines; the +N
more boundary) - NON-VACUOUS; restored by pathspec; git diff --stat
-- src/ EMPTY. GATES against f47f1aa: cmake --build build --config
Release exit 0; ctest -C Release exit 0 5/5 100% (26.17s; the
PureEditor0 duration 1.73s - under the 40s guard); alive x3 exit 0;
git diff --check clean. THE ROTATION TRUTH (the Direction Spec
correction): the 293 "Z-only" was INCOMPLETE - rotationAngle is ONE
field with the AXIS PER MODE: the 2D path reads it as a Z-SPIN
(src/renderer.h:591 Mat4::rotationZ), the 3D path (drawEntity3D,
src/renderer.h:732-734/:777) REINTERPRETS the same field as YAW
about +Y (Mat4::rotationY, Step 214, documented in-source as the
explicit choice); the Direction Spec's rotation facts corrected
below. FINDINGS: 23 entries (the inspector, the panel-rect defect
fixed, the rotation truth, the stash/lock incident - all existing
capability).

**Direction Spec correction (Step 296):** in the "1 Core foundation"
current-state text, the entity rotation reads "the SINGLE-AXIS
rotation - 'current Z rotation in RADIANS'": the SINGLE-AXIS part is
correct; the axis is PER MODE (z-spin in the 2D path, yaw about +Y
in the 3D path) - see the Step 296 section above.

### Phase Tools consumer: editor0 option B (Step 297) - step 4 of at most 5, COMPLETE

Tools/Editor consumer step; no engine version opened, v4/v5/v6 stay
frozen. The navigation + the line caps + the N-entity sample landed
(commit `b1ca958`). THE KEY AUDIT FIRST (grep-verified): the
edge-tracking registration is the CONSUMER'S ctor list
(src/main.cpp:866-882; untracked keys simply report no edge); Tab is
MAPPED (input.h:176) but was NOT registered anywhere; the Shift-as-
LEVEL read is ENGINE-PROVEN (src/main.cpp:1609-1610). FINAL KEY
CHOICE: Tab = the next (registered in the editor's ctor list),
Shift+Tab = the previous (the Tab edge + the Shift level) - both
usable, no missing capability. THE IMPLEMENTATION: navigateSelection
(the direction-matched scan: the next walks FORWARD, the previous
walks BACKWARD; the wrap always non-negative; the dead skipped; no
selection -> the first/last alive; the empty scene = a safe no-op);
kInspectorMaxLineChars 40 + the '...' cap (the extreme values
bounded); the makeStatusLine tag cap 20 + '...'; the EditorInput nav
edges + the stepEditorFrame nav handling (the diag record); the
EditorState nav fields; buildSampleSceneN (the count<=3 branch
copies the 288-296 layout VERBATIM; count > 3 = the prefab cycle on
a grid; count 0 = the empty scene; the cap 10000);
makeSampleSceneFile(path, err, count = 3); --diag-input prints ONE
line per nav key press (key=<next|prev> index <before> -> <after>).
--selftest: 151 checks green (the 40-entity cycle + the wrap + the
dead-skip + the empty scene + the caps (1e30/3.4e38/NaN/inf/the
long tag) + the N-entity samples + the pick at 40 entities + the nav
wiring through the REAL stepEditorFrame). MUTATION: the wrap removed
(the start clamped) -> 3 checks FAILED - NON-VACUOUS; restored by
pathspec; git diff --stat -- src/ EMPTY. GATES against b1ca958:
cmake --build build --config Release exit 0; ctest -C Release exit 0
5/5 100% (14.41s; the PureEditor0 duration 0.63s - under the 40s
guard); alive x3 exit 0; git diff --check clean. HUMAN PASS (the
296 inspector, Cyril's photos): CONFIRMED; the polish finding (the
big bitmap font + the crowded top status line) = cosmetic, recorded,
not scheduled.

**PROMPT CONFLICT (Step 297, reported, not resolved by guessing):**
the prompt contained TWO versions of this step's spec. THE FIRST
(governing - the filled human feedback + 'Key audit FIRST'): the key
audit, the navigation, the line capping, --make-sample [count] with
40 entities, ONE mutation. THE SECOND (an unfilled-placeholder
draft): the raw Tab/Shift assumption, the panel scroll/toggle
(Up/Down + I), a 2000-entity scene, a SECOND mutation (the scroll
clamp). THE FIRST WAS FOLLOWED; the second's extras are NOT
implemented - a candidate for a later step if the human confirms.

### Editor-0 option B slice CLOSED (288-297) - what works / what is out / honest limits

**What works (all verified; the visual confirmations from Cyril's
photos/hands-on runs):**
- load: loadSceneForEditor (strict, replace-only-on-success, the
  reason-coded errors; the hostile loads all clean rejects - no
  crash/hang/huge allocation)
- pan/zoom: mouse-drag pan (the cameraPanDelta formula) + +/- zoom
  (clamped 0.25-4.0, editor-documented); the window->fb ratio
  conversion (the cursor coordinates fixed for display scaling - the
  reported no-selection live defect found and fixed)
- pick: the zoomed pick (screenToWorldAtZoom + the documented
  pe::pickEntity); the dead skipped; the highest depth wins (the
  observed order recorded, not asserted)
- inspector: the 36-line read-only panel (the 24 saver fields in the
  4dp saver style, the 40-char cap with '...', the +N more overflow,
  no selection -> 'no selection'; a click inside the panel keeps the
  selection; UI space, immune to pan/zoom)
- Tab nav: Tab = the next, Shift+Tab = the previous (wrapping, the
  dead skipped, the empty scene a safe no-op; the keys documented via
  the audit)
- sample N: --make-sample <path> [count] (0..10000; the count 3 = the
  original layout; the grid for larger; refuses to overwrite)
- diagnostics: --diag-input (ONE line per click + one per nav press);
  the print discipline (ONE report per failed load - CONFIRMED by
  Cyril: 1 line)
- rotation axis-per-mode: rotationAngle is ONE field; z-spin in the
  2D path (renderer.h:591), yaw about +Y in the 3D path
  (renderer.h:732-734/:777) - the Direction Spec corrected
- src/ still read-only for tools (git diff --stat -- src/ empty at
  EVERY step; 7 steps, no engine change)

**What is out (NOT started / later):**
- panel scroll + the visibility toggle (the 297 prompt's second
  version - a candidate if the human confirms)
- the 2000-entity stress + the second mutation (the scroll clamp)
- save-as (option C; the write-safety rule applies: an editor save
  writes a NEW file)
- gizmos, retained UI, engine changes - the freezes stand

**Honest limits:**
- the visual confirmations: the 288-297 slice CONFIRMED (Cyril);
  the optional Tab retest: NOT RUN (the placeholder unfilled)
- the polish finding: the big bitmap font + the crowded top status
  line (cosmetic; not scheduled unless the human asks)
- the missing capabilities: the camera zoom API, the wheel/scroll
  input (engine work for a later version, opened docs-first from
  the classified findings if the human chooses)

### Phase Tools consumer: editor0 option C (Step 299) - step 1 of at most 4, COMPLETE

Tools/Editor consumer step; no engine version opened, v4/v5/v6 stay
frozen. THE SAVE-AS landed (commit `ae4d516`) - the editor's FIRST
write path: saveAsForEditor(path, const pe::Scene&, err, sourcePath
= '') wraps the frozen pe::saveSceneToFile (scene.h:381, the
explicit-path + rename-overwrite contract) with the WRITE-SAFETY
refusals (the V3 option C brief): an empty path, the loaded SOURCE
path, and an EXISTING destination are REFUSED (the file untouched);
a NEW file path ONLY - the source is NEVER overwritten and the
round-trip is proven BEFORE any overwrite is even considered. THE
TRIGGER: Ctrl+S (the S edge + the Ctrl level - the engine-proven
combo pattern, src/main.cpp:1609-1610; no text-input widget needed).
THE TARGET: a derived NEW path savedata/<base>_edit<N>.txt
(makeSaveAsPath - the basename + _edit<N>, the first free N via the
auto-incrementing counter; deterministic, no clock). The status
feedback 'saved <file> (<N> entities)' / 'save failed: <reason>'
(makeSaveFeedback, exact-string tested). The counter bug (the break
skipped the loop's increment) was caught by the test and fixed.
--selftest: 167 checks green - the round-trip (save -> load ->
field-exact, all 24 checks per entity; save -> load -> save
BYTE-IDENTICAL, the 276 pattern), the refusals (the refused file
unchanged), the exact strings, the derived paths, the Ctrl+S trigger
through the REAL stepEditorFrame, the negative control (a broken
compare caught: health), and every 288-298 check unchanged.
MUTATION: the source refusal SKIPPED -> the refusal test FAILED -
NON-VACUOUS; restored by pathspec; git diff --stat -- src/ EMPTY.
GATES against ae4d516: cmake --build build --config Release exit 0;
ctest -C Release: the first run 4/5 (PureEditor0 'Not Run' - the
transient post-link AV-scan lock, the link succeeded; the re-run
after a 10s wait) 5/5 100% (10.54s; the PureEditor0 duration 1.11s -
under the 40s guard); alive x3 exit 0; git diff --check clean.
FINDINGS: 28 entries (Entry 28: the save-as = existing capability).
Visual confirmation: the save-as feedback line on screen PENDING.

### Phase Tools consumer: editor0 option C (Step 300) - step 2 of at most 4, COMPLETE

Consumer step; engine src/ untouched (git diff --stat -- src/ EMPTY).
THE KEY AUDIT FIRST: pe::Input registers the edge-tracked key list at
construction (src/input.h:300-333; an untracked key reports NO edge)
and the game itself registers the arrows (src/main.cpp:872), so the
editor's list gained the four arrows (kEditorKeys: ESC, +/=, -/_, R,
Tab, S + UP/DOWN/LEFT/RIGHT = 10 keys, introspection-checked); the
Shift LEVEL read is the engine-proven static-isDown pattern. FINAL
KEYS: the arrows = the nudge edges, Shift = the coarse step,
R/Tab/S/ESC/+/-= unchanged. THE NUDGE: nudgeSelected (pure,
editor-owned) moves the SELECTED entity in WORLD space 0.1
(Shift 1.0): up +y, down -y, right +x, left -x - independent of
pan/zoom; no selection/dead/stale = a safe no-op; ONLY the selected
entity's position changes; the inspector re-reads the scene so the
new values show at once. THE MODIFIED STATE: set by an applied nudge;
cleared by a successful save-as and by a successful reload; the
status line gains the ' | modified' marker (the 6-param
makeStatusLine). THE RELOAD GUARD (the chosen rule, documented): the
FIRST R while modified does NOT reload - it shows exactly 'unsaved
changes: press R again to discard'; a SECOND R within 120
stepEditorFrame calls (FRAMES chosen over key events) discards and
reloads; ANY OTHER KEY EDGE or a failed action clears the pending
confirmation; past the window it expires silently (the next R asks
again). SAVE INTEGRATION: Ctrl+S after nudging writes the EDITED
scene to the next free savedata/<base>_editN.txt (the write-safety
refusals unchanged; the source NEVER overwritten); a successful save
clears the flag, a failed save keeps it (the savePathOverride test
hook exercises the refusals through the REAL step function).
--diag-input prints ONE line per nudge key press (key, selected
index, position before -> after, modified flag); GLFW has no
programmatic key injection, so the live mapping is proven by the
registration introspection + Cyril's manual --diag-input run
(FINDINGS Entry 31). --selftest: 241 PASS / 0 FAIL lines (the Step
300 functions add 49 checks: the nudge sums tol 1e-4, the wrong-axis
+ wrong-entity negative controls, the field-by-field unchanged
compare, the save round-trip with the SOURCE bytes identical, the
exact guard message, the 120-frame expiry + the inclusive boundary,
the failed-save/failed-reload rules). MUTATION: the modified-flag
guard on R removed -> 8 guard checks FAILED NON-VACUOUS; restored by
pathspec; git diff --stat -- src/ EMPTY. GATES against 38c40de:
cmake --build build --config Release exit 0; ctest -C Release 5/5
100% (23.41s; the PureEditor0 duration 1.42s - under the 40s guard);
alive x3 (all three True); git diff --check exit 0 clean. SIZE: the
first measurement net +463 EXCEEDED the 450 cap - 25 comment lines
trimmed (no behavior or test change), net +438, disclosed; 2 files.
The human feedback (the 299 Ctrl+S paste) arrived UNFILLED (the
template placeholder) = not run; the 299 save-as and the 300 nudge
visual confirmations stay PENDING. FINDINGS: entries 29-31.

### Phase Tools consumer: editor0 option C (Step 301) - step 3 of at most 4, COMPLETE

Consumer step; engine src/ untouched (git diff --stat -- src/ EMPTY).
THE KEY AUDIT FIRST: Z and Y were NOT registered in the editor's
tracked list (input.h:300-333; the consumer ctor-list rule) - they
joined (kEditorKeys = 12 keys, introspection-checked); the Ctrl LEVEL
is the engine-proven static isDown. FINAL KEYS: Ctrl+Z = undo,
Ctrl+Y = redo, Ctrl+Shift+Z = redo (the documented alternative);
plain Z/Y without Ctrl = a safe no-op. THE MODEL (documented): only
POSITIONS are undoable (the nudge is the only editor mutation); each
applied nudge pushes ONE UndoEntry {index, before, after} on the undo
stack (the cap 32 - the oldest falls off; a new nudge CLEARS the redo
branch); undo pops the newest entry and restores the before position
(the entry moves to the redo stack); redo mirrors. The stacks clear
on a successful reload (stale indices) and SURVIVE a save. Undo/redo
NEVER touch files (byte-compare proven). THE HONEST MODIFIED FLAG: a
position baseline (capturePositions) is captured at the initial
loads, at every successful reload and at every successful save;
`modified` = the current positions differ from it - so the undo back
to the baseline CLEARS the flag and the redo re-sets it, and an undo
after a save honestly re-sets it when the state differs from the NEW
baseline (the save re-baselines; the stacks survive). THE ATTACH
POINTS (all editor-owned, no engine change): the stack push in the
nudge block, the applyUndo/applyRedo executor between the nudge and
the save blocks in stepEditorFrameInner, the baseline captures in the
reload/save paths and in main.cpp's two initial loads.
--diag-input prints ONE line per applied undo/redo (the direction,
the index, the position before -> after, the flag).
--selftest: 280 PASS / 0 FAIL lines (39 new checks: the exact
restores, the redo chains, the cap boundary (33 nudges -> 32 undos ->
the first nudge lost), the redo cleared on a new nudge, the
wrong-entity negative control, the source bytes identical through
nudge/undo/redo, the modified honesty both directions, the
reload-clears-both-stacks, the pure key mapping + negative controls,
the Z/Y registration). MUTATION: applyUndo always no-op -> 13
undo-dependent checks FAILED NON-VACUOUS; restored by pathspec.
GATES against 6937fb9: cmake --build build --config Release exit 0;
ctest -C Release 5/5 100% (12.99s; the PureEditor0 duration 1.10s -
under the 40s guard); alive x3 (all three True); git diff --check
exit 0 clean. SIZE: net +414, 2 files - within the 450 cap. Two
defects found by the run and fixed in-step BEFORE the commit: a
test-setup bug (the 301 (g) missing the second R edge) and a C2065
UndoEntry definition-order error (the struct moved above EditorState;
FINDINGS Entry 34). The human feedback (the 300 visual paste) arrived
UNFILLED = not run; the 299 + 300 visual confirmations stay PENDING;
the 301 visual (nudge, Ctrl+Z, Ctrl+Y, the inspector matches; Ctrl+S
still a new file only) PENDING. FINDINGS: entries 32-34.

### Editor-0 option C slice CLOSED (299-301) - what works / what is out / honest limits

**What works (all editor-owned; engine src/ untouched across 299-301,
git diff --stat -- src/ empty at EVERY step; the verification = the
selftest + the non-vacuous mutations; the human visuals PENDING):**
- save-as: Ctrl+S -> a NEW file savedata/<base>_editN.txt (the
  auto-increment counter, the first free name); the write-safety
  refusals (an empty path, the loaded SOURCE, an EXISTING destination
  - the refused file untouched); the round-trip proven (save->load->
  save byte-identical, the 276 pattern); the exact feedback strings
- nudge: the arrows move the SELECTED entity in WORLD space (0.1,
  Shift = 1.0), pan/zoom-independent; no selection/dead/stale = a
  safe no-op; ONLY the selected entity changes; the inspector shows
  the new values at once
- modified state: the honest baseline rule (a position baseline
  captured at every successful load/save; the flag = the positions
  differ from it); the ' | modified' status marker
- R guard: the first R while modified asks (the exact message, no
  reload); the second R within 120 frames discards and reloads; any
  other key edge or a failed action clears the pending; the window
  expires silently
- bounded undo/redo: Ctrl+Z / Ctrl+Y / Ctrl+Shift+Z; positions only;
  one entry per applied nudge; the cap 32 (the oldest falls off); a
  new nudge clears the redo branch; the stacks clear on a successful
  reload and survive a save; undo/redo NEVER touch files
- diagnostics: --diag-input prints ONE line per click, per nav press,
  per nudge press and per applied undo/redo
- the selftest: 280 PASS / 0 FAIL lines; the CTest PureEditor0
  ~1.1s (the 40s guard); the per-step mutations all NON-VACUOUS
  (299: the source refusal skipped; 300: the modified-flag guard on R
  removed - 8 FAILs; 301: the undo always no-op - 13 FAILs)

**What is out (named, NOT implemented; each needs a human-authorized
later step):**
- saving OVER the source or an existing file (the write-safety rule
  stands; a rule change is human-only)
- path-switch-after-save: after Ctrl+S the editor keeps the ORIGINAL
  path as the reload source; switching the working file to the saved
  _editN file is not implemented
- multi-field editing: positions only - no other field is editable;
  no gizmos / drag-to-move
- the 297 deferred extras: the panel scroll + the visibility toggle,
  the 2000-entity stress, a second mutation per step
- the engine-side missing capabilities (docs-first version opens
  only): the camera zoom API, the wheel/scroll input, the
  programmatic key injection (a test-harness gap)

**Honest limits:**
- the live key mappings are proven by the registration introspection
  + the synthetic stepEditorFrame tests; the PHYSICAL keystrokes are
  witnessed only by --diag-input runs (GLFW has no injection)
- the undo model is position-only by design: a future multi-field
  edit would need a generalized entry (a named deferred item, not
  started)
- the human visual confirmations for 299, 300 AND 301 are ALL still
  PENDING (the three unfilled placeholders, FINDINGS 29/32/35); the
  commands were provided in the final reports
- the option-C slice used 3 of at most 4 steps; the phase Tools stays
  OPEN - the next lane is the human's choice (the slice close is NOT
  a phase close)

### Phase Tools follow-on (Step 303): editor0 path-switch-after-save - AUTHORIZED, COMPLETE

Consumer step; engine src/ untouched (git diff --stat -- src/ EMPTY).
THE HUMAN AUTHORIZATION (verbatim): 'option C closed at 302
~a828992. Path-switch authorized. Visuals for 299-301 still PENDING
(do not block this step).' THE 302 DEFERRED ITEM RESOLVED (FINDINGS
Entries 36-37). THE INSPECT: the save block's success branch set
state.lastSavePath but never state.path - the switch attaches exactly
there. THE DESIGN (documented + tested): on a SUCCESSFUL save-as the
loaded/source path becomes the NEW saved file; a FAILED save changes
NOTHING (the path, the baseline, the stacks - the flag held TRUE
going in as the stronger negative control). THE FLAT NAMING RULE
(chosen over nesting): makeSaveAsPath strips a trailing
_edit<digits> suffix before appending the new _edit<N> - sample_
scene.txt -> _edit1 -> _edit2 (bounded names; a strip that would
empty the base or a non-digit tail is skipped; the 299 double-save
test's exact strings stay green). THE WRITE-SAFETY PRESERVED: the
CURRENT loaded path is refused by the source rule (it follows the
switch); the ORIGINAL file stays protected by the existing-file rule
- no existing file is ever overwritten. THE UNDO/REDO STACKS ARE
KEPT on the switch (the scene objects are unchanged, the indices stay
valid; the undo after the save honestly re-sets the flag vs the NEW
baseline; a reload still clears them). --selftest: 315 PASS / 0 FAIL
(35 new checks: the success updates the path + baseline + status +
counter; the failed-save negative controls; the source refusal now
protecting the CURRENT path; the ORIGINAL bytes unchanged; the second
save under the flat rule; the undo/redo across the switch; 6 pure
naming cases). MUTATION: the path updated even on a failed save ->
6 checks FAILED NON-VACUOUS; restored by pathspec. GATES against
4e9644e: cmake --build build --config Release exit 0; ctest -C
Release 5/5 100% (11.10s; the PureEditor0 duration 0.46s - under the
40s guard); alive x3 (all three True); git diff --check exit 0 clean.
SIZE: net +204, 2 files - within the 450 cap. Two test adjustments
were REQUIRED by the new semantics (the 300 (c) source-byte check now
reads the LITERAL source path; the cleanups) and the idempotency
pre-removals (a leftover savedata file auto-advanced the scan - caught
by running the selftest twice, fixed before the commit). The
verification commands for Cyril (updated for the switch):
open the sample -> nudge -> Ctrl+S (the status shows the saved
savedata/<base>_edit1.txt path AND the status line now reports that
path as the loaded file) -> nudge again -> Ctrl+S (a SECOND new file
_edit2) -> the ORIGINAL sample hash unchanged.

### Phase Tools follow-on (Step 304): editor0 inspector panel scroll - COMPLETE

Consumer step; engine src/ untouched (git diff --stat -- src/ EMPTY).
THE KEY AUDIT: PAGE_ appears nowhere in the engine or the editor
input paths - PgUp/PgDn joined the editor's tracked list (kEditorKeys
= 14 keys, introspection-checked). THE WHEEL IS NOT AVAILABLE (the
recorded missing capability, FINDINGS 8/31) - the keys are the
documented substitute. THE DESIGN (pure + tested): clampedPanelScroll
([0, total-visible]; the degenerate cases clamp to 0) +
inspectorLinesScrolled (the FULL 36-line list sliced
[scroll, scroll+12); the window is exactly 12 lines when a selection
exists - the pointInPanelRect geometry holds; at scroll 0 the LAST
line is the SAME '+N more' hint the capped list shows - the 296
compatibility; a short tail window when total < 2*visible - honest).
THE RESETS: a pick (including the empty-space clear), a nav press and
a successful reload restart the panel view; an empty selection is a
safe no-op. The live viewer draws the scrolled window.
--selftest: 345 PASS / 0 FAIL (29 new checks). MUTATION: the scroll
never moves -> 5 checks FAILED NON-VACUOUS; restored by pathspec.
GATES against a97b34f: build exit 0; ctest -C Release: the first run
4/5 (PureEditor0 'Not Run' - '[resource busy or locked]'), the second
run after the 10s wait ALSO 'Not Run' (the one-rerun budget spent),
the lock probe confirmed an exclusive external holder (no stray
process; the lock appeared after the successful link - a slow AV
scan of the fresh binary), the lock CLEARED after a further ~60s
wait and the THIRD run was 5/5 100% (9.56s; PureEditor0 1.27s -
under the 40s guard) - NO code changed; the deviation (a third run)
disclosed here; alive x3 (all three True); git diff --check exit 0
clean. SIZE: net +232, 2 files - within the 450 cap. One test-setup
bug (the flag cleared too early) and one definition-order error (the
forward declarations + the constant moved above the frame functions)
found and fixed in-step BEFORE the commit. FINDINGS: entries 38-39.
The human note (verbatim): 'Human visuals 299-303 still PENDING -
do not block these steps.'

### PHASE TOOLS STOP NOTE (Step 305, 2026-10-11): the 296-class lock returned - STOP at the 305 gates

TOOLS-305 (43eef8d) is COMMITTED: the always-on key legend (5 compact
bottom lines, every live key token, GL-proven; net +77; the selftest
349 PASS / 0 FAIL). The mutation (a legend line dropped) FAILED 3
checks NON-VACUOUS and was restored by pathspec. THE RESTORE +
REBUILD then hit LNK1104 (cannot open PureEditor0.exe) and the lock
PERSISTED: read OK, write BLOCKED, delete BLOCKED, no stray process -
the 296-class exclusive external holder (FINDINGS Entry 40). ~7
minutes of waits did not clear it (the 304 lock had cleared in ~70 s).
THE GATES FOR 305 ARE BLOCKED; NO DOCS-305 record; Step 306 NOT
started (the next-steps rule: only if green AND pushed - the gates
are not green). THE RECOVERY (the human): clear the lock (resmon ->
CPU -> Associated Handles -> search 'PureEditor0', or delete
build/Release/PureEditor0.exe and rebuild), then: the build, the
selftest (349 PASS / 0 FAIL must hold), ctest 5/5, alive x3, the diff
checks, then the DOCS-305 record (the tracker/V3/board/README) - the
same shape as the 296 recovery. The legend LINES (for the record):
'arrows nudge Shift=1.0' / 'Tab cycle, PgDn panel' / 'R reload,
Ctrl+S save' / 'Ctrl+Z/Y undo/redo' / 'ESC quit'.

RECOVERED (the Step 305 resume, 2026-10-11): the lock cleared and the
HUMAN rebuilt + verified (the verbatim evidence: 'Rebuild PureEditor0
succeeded after lock cleared'; 'PureEditor0.exe --selftest -> all
checks passed (includes scroll + help legend lines)'; 'ctest -C
Release -> 5/5, 100% (PureEditor0 Passed 0.35s)'). The agent
reproduced the selftest + alive x3 + the diff checks - all green. The
tracker 305 flipped stopped -> completed; the DOCS-305 commit marks
it. The legend visual stays PENDING.

### Editor-0 tools mini-slice (304-305) CLOSED - what works / what is out / honest limits

**What works (all editor-owned; engine src/ untouched; the
verification = the selftest 349 PASS / 0 FAIL + two NON-VACUOUS
mutations; the 305 gates human-proven and reproduced):**
- inspector scroll: PgUp/PgDn (registered, 1 line per press) move a
  clamped visible window of the 36 inspector lines
  (clampedPanelScroll: [0, total-visible]; the window is exactly the
  12-line panel geometry so pointInPanelRect holds); at scroll 0 the
  last line is the SAME '+N more' hint the capped list shows (the 296
  compatibility); a pick (incl. the empty-space clear), a nav press
  and a successful reload reset the offset; an empty selection is a
  safe no-op; the wheel is NOT available (the recorded missing
  capability) - the keys are the documented substitute
- key legend: the always-on compact bottom overlay - FIVE lines (each
  <= 22 chars, the 12x9 font fit; the documented adaptation of the
  'one line' brief): 'arrows nudge Shift=1.0' / 'Tab cycle, PgDn
  panel' / 'R reload, Ctrl+S save' / 'Ctrl+Z/Y undo/redo' / 'ESC
  quit' - every required token, GL-proven in the hidden-window frame
- the 305 lock incident handled by the protocol: the STOP recorded
  (the tracker stopped + the V3 STOP note + FINDINGS 40), then the
  RECOVERY (the human cleared the lock + verified; the agent
  reproduced the gates) and the flip to completed (FINDINGS 41)

**What is out (named, NOT implemented; each needs a human-authorized
later step):**
- saving OVER the source or an existing file (the write-safety rule
  stands; the path-switch-after-save landed at 303)
- multi-field editing (positions only), gizmos / drag-to-move
- the scroll-by-wheel (needs the engine wheel input), the camera zoom
  API - the missing capabilities, docs-first version opens only
- MCP / object pools / ImGui / ECS / glTF / networking - the freezes
  stand

**Honest limits:**
- the live key mappings are witnessed by --diag-input and the human
  runs; the PHYSICAL keystrokes are not programmatically injectable
  (GLFW)
- the 2000-entity stress and a second mutation per step remain
  deferred (the 297 extras)
- the human visuals for 299-305 ALL stay PENDING (the standing
  'do not block' note); the recorded verification is the selftest +
  the mutations + the human's 305 ctest run
- the Tools phase stays OPEN - the next lane is the human's choice

### Tools recommended pause (Step 307, docs only) - a complete tools milestone; the phase stays OPEN

Editor-0 is now a COMPLETE tools milestone across option B, option C
and the follow-ons (288-306): load (strict, reason-coded), pan/zoom
(the ratio fix), pick, inspect (the 36-line panel), scroll
(PgUp/PgDn), nudge (0.1 / Shift 1.0), the modified state (the honest
baseline), the R guard (120 frames), bounded undo/redo (the cap 32),
save-as (a NEW file only, the path switch, the flat _editN naming)
and the always-on key legend - all editor-owned, engine src/
untouched at EVERY step, the selftest 349 PASS / 0 FAIL and every
code step's mutation NON-VACUOUS.
THE VISUAL CONFIRMATIONS: all six step visuals (299-305) are NOT
TRIED as of this step (the 307 checklist arrived with every row an
unfilled placeholder - FINDINGS 42); the recorded verification is the
selftest + the mutations + the human's 305 ctest run (5/5, 0.35s).
THE RECOMMENDED PAUSE (docs only; NOT a phase close - a phase close
is a human-only decision): the milestone is COMPLETE; further Tools
steps happen only on MEASURED NEED (a superlinear breach, a
consumer-blocking gap) or Cyril's EXPLICIT authorization of a named
item.
HONEST LIMITS (what the editor still cannot do): no entity
create/delete, no play mode, no typed edits (the engine has no
text-input widget), no wheel zoom (no engine wheel input), no
per-entity highlight (the role-group coloring only), no drag-to-move,
no multi-field editing. The deferred lists stand (the option-B, the
option-C and the mini-slice sections above).

### Vertical slices (Step 308, docs only) - the 2D and 3D slice workflows, grep-verified

Every claim below cites the current file:line (grep-verified this
step) or a named test. Status: VERIFIED (exists + tested) / PARTIAL
(exists but incomplete) / MISSING (nothing exists) / BLOCKED (a named
freeze - none of the items below are BLOCKED by a freeze; the freezes
stand at AGENTS.md v1.1 + the per-version frozen lists). Categories
per the Direction Spec 5-category map (V3, Step 293): 1 Core
foundation, 2 World + content, 3 Graphics + audio + assets,
4 Simulation + game systems, 5 Editor + build + distribution.

**a) 2D slice: create project -> create scene -> add entity/sprite ->
modify -> save -> play/test -> build/package -> export**

| step | what exists today (file:line, test) | status | cat | first capability to close |
|---|---|---|---|---|
| create project | NOTHING: no project file/manifest anywhere (the root PS1s = the 6 asset generators; CORRECTED 309: scripts/package.ps1 also exists - the packaging row); a "project" is the CMake repo + loose scene .txt files | MISSING | 5 | a project manifest format + a tools-side loader (no freeze) |
| create scene | pe::saveSceneToFile (src/scene.h:381, the 34-field v2 format, the tmp+rename atomic) + pe::loadSceneFromFile (src/scene.h:459, strict whole-file); the editor boundary loadSceneForEditor (tools/editor0/editor0_core.h:39); the round-trip tests (LevelPipelineConsumer 276; the editor selftest save-as checks) | VERIFIED | 2 + 5 | nothing (works today, headless + in the viewer) |
| add entity/sprite | engine-side: Scene::queueSpawn (src/scene.h:73) + flushSpawns (src/scene.h:81) + the SceneManager (switchTo src/scene.h:180, removeEntity src/scene.h:213); sprites = the FIXED texture slots the renderer loads at init (renderer.h init fatal set - the 288 finding); the EDITOR has NO add/remove UI (stepEditorFrameInner mutates positions only, editor0_core.h:609) | PARTIAL | 5 (+3 for the texture browsing) | wire the existing create/remove APIs into the editor (tools-side; no freeze) |
| modify it | positions: the nudge (editor0_core.h:486), the undo/redo (editor0_core.h:549/:572, the cap 32), the inspector (editor0_core.h:864, 36 lines); OTHER fields: MISSING - no typed edits (no text-input widget; the raw material = the console's feedKey, src/console.h:10-15, the key-fed input line) | PARTIAL | 5 | the field-edit UI on the feedKey raw material (tools-side) |
| save | saveAsForEditor (editor0_core.h:321, the write-safety refusals) + the path switch (the Step 303) + the flat _editN naming (makeSaveAsPath editor0_core.h:291); the round-trip byte-identical (the 299 tests) | VERIFIED | 2 | nothing |
| play/test | the ENGINE simulates (src/simulation.h:69 advanceRotations, :84 chasePlayer, :117 applyPhysics, :140 scanSceneryCollisions; the arcade loop src/main.cpp); the EDITOR has NO play mode (the viewer loop draws only, tools/editor0/main.cpp runViewer) | PARTIAL | 4 + 5 | an editor play-mode toggle wiring simulation.h into the editor loop (tools-side; no freeze) |
| build/package | build: CMake (the VS-bundled toolchain; ctest 5/5 every step). package: scripts/package.ps1 EXISTS (git-tracked; CORRECTED 309 - the 308 claim was a NON-RECURSIVE search error, FINDINGS 44): the arcade release ZIP (PureEngine.exe + 18 runtime assets + shaders/ + prefabs/ -> package/PureEngine-0.1.0-win64.zip; the Step 143 dry-run documented); the EDITOR is not packaged; no installer/export | PARTIAL | 5 | packaging the editor itself (the script exists for the arcade) |
| export | NOTHING: no export of any kind (resources.h holds the blob/pack/cache CONTRACT half - the resources_test target; no runtime asset pack/export exists) | MISSING | 5 + 3 | the packaging EXISTS (scripts/package.ps1, corrected 309); the next capability: an export format decision (human) |

**b) 3D slice: create scene -> import mesh -> place it -> camera ->
lighting -> save -> play/test -> build/package**

| step | what exists today (file:line, test) | status | cat | first capability to close |
|---|---|---|---|---|
| create scene | the same scene format carries the 3D fields (Entity meshId src/entity.h:119, the y position; the 3D done-criteria V3 Step 263); the round-trip tests | VERIFIED | 2 | nothing |
| import a mesh | loadMeshFromObj (src/mesh3d.h:102): the v/f triangle-soup ONLY - no vn/vt, no quads, no index buffers (src/mesh3d.h:92; the silent vn/vt skip src/mesh3d.h:184); the meshes register at init (renderer.h registerMesh :752, clearRegisteredMeshes :755; the tetra/wedge loaded in main) | PARTIAL | 3 | the loader extension (a material/index-buffer pass) - an engine version item, human-opened |
| place it | hand-editing the scene .txt today (meshId + the position); the editor cannot place or create | PARTIAL | 5 | the same create/place wiring as the 2D add-entity row |
| camera | the 3D camera mode EXISTS: the perspective/ortho setters (src/camera.h:228-235), setFov (src/camera.h:264), orbitEye the XZ-plane orbit (src/camera.h:247, the Step 229 fix), rebuildProjection (src/camera.h:273); the debug3d consumer drives it via the console path | VERIFIED (the editor zoom stays the composed screen-center scale, editor0_core.h:114 - the engine has no zoom API) | 3 | nothing for placement; the zoom API stays the recorded missing capability (src/camera.h:290, the halfHeight=4.5 lock) |
| lighting | the 3D per-face directional lighting on the loaded meshes (the Step 260 inverse-transpose + the Step 261 sizing; renderer.h setLightUniforms :423); the 2D LightingState (src/lighting.h:41, MAX_LIGHTS=4 :44) | VERIFIED (the mutation-caught v4 tests) | 3 | nothing (the world-light editing UI is a later editor item) |
| save | the same VERIFIED scene round-trip | VERIFIED | 2 | nothing |
| play/test | the same PARTIAL as the 2D row | PARTIAL | 4 + 5 | the editor play-mode toggle |
| build/package | the same as the 2D row (CMake yes; scripts/package.ps1 packages the arcade - CORRECTED 309, FINDINGS 44) | PARTIAL | 5 | packaging the editor itself |

**The recorded engine gaps (each already classified; none new):**
- no camera zoom API - src/camera.h:290 (the halfHeight=4.5 lock); the
  editor composes its own (editor0_core.h:114). cat 3, MISSING.
- no wheel input - src/input.h:264-273 (MouseState: left+right only).
  cat 1, MISSING.
- no per-entity AABB color - renderer.h drawAABBs :1085 colors by
  roleId only (the editor's role-group trick). cat 3, MISSING.
- no text-input widget - the raw keys only; the console feedKey
  (src/console.h:10-15) is the raw material. cat 1 + 5, MISSING.
- no seedable RNG or noise - src/particles.h:23-28 (std::rand; the
  in-source note names the day). cat 4, MISSING.

**The dependency graph (text; an arrow = "is needed by"):**

```
text-input raw material (console.h feedKey) --> editor typed field edits --> multi-field undo entries
scene create/remove APIs (scene.h:73/:81, :213) --> editor add/remove UI --> 2D+3D scene authoring
project manifest --> asset browsing paths --> texture/sprite authoring
project manifest --> packaging manifest --> export
camera zoom API (engine) --> editor zoom without the composition
wheel input (engine) --> wheel zoom + wheel scroll (needs the zoom API for zoom)
per-entity AABB color (engine) --> a true selected-entity highlight
play mode (simulation.h wiring) --> play/test inside the editor
mesh loader extension (engine) --> richer 3D asset import
seedable RNG (engine) --> deterministic procedural content (no current consumer)
```

**The candidate list (ordered ONLY by dependency and evidence - the
leaf capabilities first; NOT a preference ranking; NOTHING is chosen
or opened here - choosing is a human decision):**

1. Entity create/delete wiring in the editor - cat 5, lives in
   tools/ (the engine APIs exist: scene.h:73/:81, scene.h:213); no
   freeze touched. Test path: the selftest create/remove round-trip +
   the save/load field-exact compare.
2. Editor play mode - cat 4+5, tools/ (simulation.h:69-140 exist); no
   freeze touched. Test path: the headless simulation-wiring test
   (the deterministic advanceRotations/applyPhysics steps) + a manual
   toggle.
3. Packaging the editor - cat 5, tools/ (CORRECTED 309: scripts/package.ps1 EXISTS for the arcade - the FINDINGS 44 correction; the gap: the editor is not in any package); no freeze touched. Test path: a package dry-run check extended to the editor (the SMOKE_TEST.md gate stays the human release gate).
4. Per-entity AABB color parameter - cat 3, an ENGINE version item
   (renderer.h:1085 is the frozen v1.1 debug-draw surface; additive
   opt-in only, the 201/249 precedent). Test path: the debug-draw CI
   extension (the color contract).
5. Text-input raw-material adoption - cat 5 (+1), tools/ first (the
   console feedKey pattern, console.h:10-15); an editor widget needs
   it. Test path: the headless feed-driven field-edit test.
6. Camera zoom API - cat 3, an ENGINE version (camera.h:290; the
   editor's composed zoom stays the fallback). Test path: the camera
   contract test (the zoom invariant).
7. Wheel input - cat 1, an ENGINE version (input.h:264-273); feeds
   the zoom + the scroll. Test path: the input snapshot extension
   test.
8. Project manifest + asset browsing - cat 5+3, tools/ (depends on 3
   for the paths to browse); no freeze touched. Test path: the
   manifest parse + the directory-scan test.
9. Mesh loader extension - cat 3, an ENGINE version (mesh3d.h:92/
   :184; the vn/vt/quads gap). Test path: the mesh3d round-trip CI.
10. Export - cat 5, tools/ (depends on 3 + 8); the format decision is
   a human decision. Test path: set by the future step.
11. Seedable RNG - cat 4, an ENGINE version (particles.h:23-28); no
    current consumer. Test path: the seeded-determinism test.

### Phase Tools follow-on (Step 309): editor0 entity delete - COMPLETE (entity editing, step 1 of at most 3)

Consumer step; engine src/ untouched (git diff --stat -- src/ EMPTY).
THE DOCS CONTRADICTION RESOLVED FIRST (FINDINGS 44): scripts/package.ps1
EXISTS and is git-tracked (git ls-files; the recursive search with the
build/, .git/, .kilo/ and .goose/ exclusions finds exactly one
main-tree copy, 3911 bytes) - it is the ARCADE release ZIP packager
(PureEngine.exe + 18 runtime assets + shaders/ + prefabs/ ->
package/PureEngine-0.1.0-win64.zip, the Step 143 dry-run documented);
the 308 claim was WRONG (a non-recursive search - the FINDINGS 44
correction; the 308 rows, the 308 tracker note, the board and the
README corrected with dated pointers). THE PROCESS FINDING (FINDINGS
45): .kilo/ and .goose/ hold another tool's worktree copies inside
the repo directory (southern-catshark 24bec72, zesty-omelet 6013172
- observed, NOT touched); the standing search rule excludes build/,
.git/, .kilo/ and .goose/.
THE DELETE SEMANTICS (the inspect, evidence-cited): the removal uses
the ALIVE FLAG - entity.h:183-189 ("kills never erase, so indices
stay stable... and persistence drops it on save") and the saver's
skip at scene.h:419-420 ("dead entities don't exist in saved state").
The saved file therefore EXCLUDES the deleted entity; the undo
restores alive=true at the SAME index with every serialized field
untouched (the vector order never changed); the redo re-deletes.
THE KEY AUDIT: Delete was NOT registered (the consumer ctor-list
rule, input.h:300-333) - added (kEditorKeys = 15 keys, selftest-
checked). THE IMPLEMENTATION: the typed UndoEntry (isDelete), the
delete block in stepEditorFrameInner (the alive kill + the stack
push + the selection cleared + the honest recompute), the
applyUndo/applyRedo alive branches, captureAliveFlags + aliveCount +
sceneDiffers (the baseline gained the alive dimension - the modified
flag is honest across deletes), the visible counts in the status and
the saved/reloaded feedback strings, --diag-input one line per
delete/undo/redo (the key, the index, the visible count before ->
after, the flag). --selftest: 385 PASS / 0 FAIL (36 new checks).
MUTATION: the delete-undo does not restore -> 8 checks FAILED
NON-VACUOUS; restored by pathspec. GATES against 56ac346: cmake
--build build --config Release exit 0; ctest -C Release 5/5 100%
(18.91s; the PureEditor0 duration 0.95s - under the 40s guard); alive
x3 (all three True); git diff --check exit 0 clean. SIZE: net +322,
2 files - within the 450 cap. The in-step defects found by the run
and fixed BEFORE the commit: the CRLF-restore aftermath (the mixed
endings defeated the multi-line anchors - the one-shot .ps1 route),
one missing isDelete field in the nudge push (C2440), and the
idempotency pre-removes for the AV-lock delete races (the
non-deterministic run-2 failures). The human visual line (308)
arrived UNFILLED; the NOT-TRIED status stands (FINDINGS 43).
FINDINGS: entries 44-46.

### Phase Tools follow-on (Step 310): editor0 entity create - COMPLETE (entity editing, step 2 of at most 3)

Consumer step; engine src/ untouched (git diff --stat -- src/ EMPTY).
THE INSPECT (the evidence): loadPrefab (prefab.h:103-177 - the
3-candidate probe :71-73, the CWD bare fallback :113, the header
check :135, the partial parse warn+skip) -> instantiatePrefab
(prefab.h:181-205 - the 17 config fields + the position + alive=true)
-> queueSpawn/flushSpawns (scene.h:73/:81 - the new index = the old
count, the vector GROWS). THE KEY AUDIT: N, ] and [ were NOT
registered - all three added (kEditorKeys = 18 keys, selftest-
checked); no conflicts with R/Tab/arrows/Delete/PgUp/PgDn/+/-/Ctrl+
S/Z/Y/ESC. FINAL KEYS: N = create the current prefab at the view
center, ] = the next prefab, [ = the previous prefab (wrapping; the
index + the name in the status line: ' | prefab <i>/<n>: <name>').
THE CREATE SEMANTICS (evidence-cited): the create appends a NEW
index (the vector grows); the undo KILLS it via the alive flag
(never erasure - the index and the fields stay); the redo REVIVES it
at the same index with the same fields; the saved file contains the
created-and-alive entities and omits the dead ones (scene.h:419-420).
The listing is editor-owned (std::filesystem, mirroring the engine
probe, sorted; empty = the create is a no-op with the status reason;
a missing/malformed file = loadPrefab warns without throwing - the
status reason, nothing changes). The created position = the VIEW
CENTER in world space = the camera position (zoom-independent by the
conversion formula: the screen center maps to the camera position).
THE HONESTY FIX: sceneDiffers is now growth-aware - the entities
BEYOND the baseline count as edited only while ALIVE (a created-and-
undone entity is the clean state again). --diag-input: one line per
create (the prefab, the index, the position, the count before ->
after, the flag), one per prefab cycle, and the create forms in the
undo/redo lines. --selftest: 441 PASS / 0 FAIL (56 new checks).
MUTATION: the redo of a create does not revive -> 3 checks FAILED
NON-VACUOUS; restored by pathspec. GATES against 7792b59+18c1f1f:
cmake --build build --config Release exit 0; ctest -C Release 5/5
100% (14.01s; the PureEditor0 duration 0.57s - under the 40s guard);
alive x3 (all three True); git diff --check exit 0 clean. SIZE: the
first impl commit measured net +451 (1 OVER the 450 cap) - a 2-line
trim commit (18c1f1f) brought the step to +449, DISCLOSED; 2 files.
THE IN-STEP PROCESS FINDINGS (FINDINGS 48): the git checkouts
rewrite the mixed endings to CRLF (the .ps1 route for the multi-line
edits); a PowerShell Write-Output+return function POLLUTED the file
content (repaired by stripping to the first /** header with the
modifications intact); the std::remove vs the AV-lock race (the
range pre-removes + the actual-path asserts). The human visuals
299-309 stay NOT TRIED (the standing note). FINDINGS: entries 47-48.

### PHASE TOOLS STOP NOTE (Step 311, 2026-10-12): the 296-class lock returned - STOP at the 311 gates

THE PACK (the dual-viewport + the 3D draw + the inspector mesh + the
play foundation) is CODED AND COMMITTED: TOOLS-311 45e7be1 (the code)
+ d593c8d (the tests) - but the BUILD is blocked by the 296-class
exclusive lock on PureEditor0.exe (LNK1104; write + delete blocked,
read OK, no stray process; ~12 minutes of waits did not clear it -
FINDINGS 49). THE GATES ARE UNRUN; the tests are UNVERIFIED; the
mutation is NOT RUN. WHAT IS IN THE COMMIT: the V toggle (the 2D
ortho path byte-identical / the 3D perspective path: drawEntity3D
per meshId!=0 entity with the FIRST carrying clearDepth=true, the
meshId==0 entities not drawn in 3D - documented, the AABB wireframes
still draw), the meshId keys 0/1/2 (the engine built-ins cube/
pyramid; NOT serialized - documented, the test asserts the
non-round-trip), the play foundation (P: the snapshot of the
positions/velocities/rotationAngles/alive, the fixed-dt 1/60 tick
via advanceRotations + applyPhysics ONLY, the edit ops disabled
while playing, the field-exact restore on exit, the source never
written), the status VIEW:2D/VIEW:3D/PLAYING. THE RECOVERY (the
human): clear the lock (resmon/Associated Handles, or delete +
rebuild), then: the build, the selftest (~455 checks), THE MUTATION
(the play-restore broken -> the restore test must FAIL; restore by
pathspec), ctest 5/5, alive x3, the diff checks, then the DOCS-311
record (the tracker 311 stopped -> completed, this section updated,
the board, the README). One compile pass BEFORE the lock confirmed
the core edits compile (the C2039 setPerspectiveMode -> the actual
setPerspective(fovY, nearZ, farZ) camera.h:225 fixed; the proj
scoping in the two draw loops fixed); the FINAL compile+link state
is UNKNOWN (the lock hit at the link).
RECOVERED (the Step 311 gate recovery, 2026-10-12): the lock cleared
and the HUMAN rebuilt + ran the gates (the verbatim evidence: 'Build
Release OK after lock clear'; the selftest 2 FAIL only - BOTH the
view-toggle harness bug; 'ctest 4/5 PureEditor0 failed' = the STALE
pre-fix exe). THE DIAGNOSIS: the view-toggle test called the SECOND
stepEditorFrame with keyViewToggle = FALSE (no edge -> no toggle) -
the 310 (g) harness-bug class; the PRODUCT code was correct. THE FIX
(commit 206a35c): the second V press carries a REAL edge. THE
POST-FIX GATES (all green): the selftest 462 PASS / 0 FAIL (x2
runs); ctest -C Release 5/5 100% (10.27s; the PureEditor0 duration
0.46s - under the 40s guard); alive x3 (all three True); git diff
--check exit 0 clean; git diff --stat -- src/ EMPTY. THE MUTATION
(the deferred play-restore broken) FAILED 1 check NON-VACUOUS
('play: the exit restored the snapshot FIELD-EXACT'); restored by
pathspec. The tracker 311 flipped stopped -> completed. The
on-screen visuals (V 2D<->3D, the meshId keys, P play/stop) stay
PENDING.

### Measurement harness (Step 312, docs + tools implementation) - the numbers are DATA, not a budget

THE AUTHORIZATION: the Step 306 rule ("no measurement harness unless
a later step explicitly asks") was lifted by THIS prompt, sent by
Cyril. SCOPE: measurement only - NO timing assertions anywhere,
nothing timing-based in ctest, no budgets recorded as pass/fail;
every printed number says "indicative, this machine, not a budget".
THE GL FACTS (grep-verified, the searches excluded build/, .git/,
.kilo/ and .goose/): the engine REQUESTS GL 3.3 Core
(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3) + MINOR 3 +
GLFW_OPENGL_PROFILE CORE at tools/editor0/main.cpp:2833-2835 (the
selftest), :2898-2900 (the viewer), :3071-3073 (the diag); the
games match); WindowGuard sets NO hints; the shaders are ALL
"#version 330 core" (assets/shaders/default.vert/frag, lit.vert/
frag). --gl-info prints the reported strings + REQUESTED + the
shader ints (330|330|330|330 from the four assets/shaders files) +
COMPATIBLE/RISK (the pure editor-owned parseGlVersion/
parseShaderGlsl/glCompatible in editor0_core.h - the versions
compared as ints in the GLSL convention: major*100+minor*10 so
GL 3.3 = 330 = GLSL 330). --bench: the hidden window, the swap
interval 0, 60 warmup + N measured frames of the editor's own draw
path (the 2D view; the 3D view forces meshId=1 on every alive
entity IN MEMORY, never saved), glFinish at the end of each frame;
the output = the machine info (the CPU brand via __cpuid, the
logical cores, the total RAM, the OS version via RtlGetVersion -
the Windows APIs editor-side), the scene entity count, the view,
the frames, the median/p95/max/mean ms, the RSS working set (the
Windows API), and draw_calls/bytes_uploaded printed as UNAVAILABLE
(not guessable without an engine change); the optional --out CSV.
THE LIVE RUN (this machine): the GL 3.3.0 Build 10.18.10.4885 /
Intel(R) HD Graphics 4000 / the GLSL 3.30 / the max texture 16384 /
COMPATIBLE; the bench 200 entities, 2d, 5 frames: the median ~1.0ms,
the p95 ~1.06ms (the indicative numbers from ONE machine - the
comparative data comes from Cyril's runs on every machine he has,
the weakest first). THE TESTS (the selftest 489 PASS / 0 FAIL): the
percentiles on 1/2/100 samples incl. the unsorted, the negative
control (the p95 is NOT the index-93 value), the GL parser on the
Intel-/NVIDIA-style strings + the unparseable, the COMPATIBLE/RISK
matrix, the CSV formatting, the 5-frame bench run asserting exactly
5 samples + glGetError()==0 - NO timing assertions. MUTATION: the
p95 index changed -> 3 checks FAILED NON-VACUOUS; restored by
pathspec. THE HOUSEKEEPING: editor0_tmp/ added to .gitignore (the
only .gitignore change). GATES against ae05b35: cmake --build build
--config Release exit 0; ctest -C Release 5/5 100% (23.23s;
PureEditor0 2.76s - under the 40s guard); alive x3; git diff
--check exit 0 clean; git diff --stat -- src/ EMPTY. FINDINGS:
Entry 51 (the search rule applied: build/, .git/, .kilo/ and
.goose/ excluded from every grep and search).

### T0 measurement record (Step 313, 2026-10-13) - Cyril's machine data, recorded verbatim

Step 312's harness run by Cyril on his machine. T0 = the first recorded performance
baseline. The Step 312 policy stands: the numbers are DATA, not a budget -
"indicative, this machine, not a budget". Nothing here gates a build.

Machine (verbatim):
- i3-3120M 2.50GHz, 4 logical cores, 8052 MB RAM, OS 10.0.22621
- GL 3.3.0 Build 10.18.10.4885, Intel HD Graphics 4000, GLSL 3.30, max tex 16384, REQUESTED 3.3 core, COMPATIBLE

T0 bench (bench200 = the 200-entity sample, 600 measured frames, glFinish per frame,
swap interval 0; times in ms):

| run | median | p95 | max | mean | rss (MB) | gl_error |
| --- | ------ | --- | --- | ---- | -------- | -------- |
| bench200 2D 600f | 1.0709 | 4.0543 | 22.0148 | 1.5718 | 34 | 0 |
| bench200 3D 600f | 32.5299 | 54.7805 | 510.2322 | 37.1151 | 35 | 0 |

draw_calls / bytes_uploaded: UNAVAILABLE (the Step 312 policy - not guessable without
an engine change).

Budget notes (indicative targets for comparison on T0-class machines; NOT ctest
timing asserts; NOT pass/fail):
- 2D, 200 entities: keep median < ~8ms, p95 < ~16ms on T0.
- 3D, 200 entities: the median (32.5299ms) is ABOVE the 16.6ms line on T0 - flagged
  as FINDINGS Entry 52 (the 3D draw path is expensive on Intel HD 4000; the max
  510.2322ms shows a long tail). Optimization is DEFERRED until a measured need or
  an explicit human open; no src/ perf rewrite in Step 313.

The bench CSVs stay under savedata/ - gitignored runtime output, not source (the
standing rule); the numbers above are the record.

### Category Core Foundation audit (Step 314, 2026-10-13) - AUDIT COMPLETE, migration NOT STARTED

Docs-only audit step (the prompt: "AUDIT ONLY (no .cpp migration yet)"). src/ was read
fully; no engine file modified. Search hygiene: build/, .git/, .kilo/ and .goose/ were
excluded from every grep and search (the standing FINDINGS 45 rule).

SCOPE. The prompt's six inventory bullets, NOT the whole V3 category: math + time +
window bootstrap + header hygiene + pe_core.cpp/CMake + the tests covering them. The
OTHER Core-foundation members per the 5-category map (entity, collision, state, events,
lifecycle, input, gamepad - the "1 Core foundation" block above) are NOT inventoried
here and their migration is NOT proposed; Entity/Scene work stays forbidden by the prompt.

INVENTORY (each file: header-only impl | already has .cpp | mixed):

| file | lines | form | key evidence (grep-verified 2026-10-13) |
| --- | --- | --- | --- |
| src/math/vec3.h | 121 | header-only impl | <cmath> only (vec3.h:30); struct Vec3 :36; dot/cross/length/normalized + operators :54-116; normalized() :88 is the only non-constexpr member (std::sqrt); ODR sweep: 0 non-inline file-scope definitions |
| src/math/mat4.h | 351 | header-only impl | <cmath> + "vec3.h" (mat4.h:36-37); struct Mat4 :41; constexpr builders translation(Vec3) :88, scale(Vec3) :101, orthographic :152, scale(x,y,z) :278, translation(x,y,z) :292, operator* :311, transformPoint :341; RUNTIME builders rotationZ :125, lookAt :189, perspective :236, rotationY :260 (std::cos/sin/tan + normalized()); perspective names its parameters near/far (mat4.h:236) - the Windows-macro clobber target (the Step 312 incident); no vec2/vec4, no SIMD (the recorded known limit) |
| src/engine_time.h | 109 | header-only impl | class FrameTime :56; start() :65; tick() :79 with the 0.1s clamp (std::min, :83); the Step 92 timeScale/pause surface :90-95; state lastTime/timeScale/paused :102-104; NO fixed-step/accumulator/frame-limiter - documented OUT (engine_time.h:23-26; main.cpp:435-436); the only fixed-dt in the repo is editor-owned kEditorPlayDt=1/60 (editor0_core.h:398, playTick :683 - Step 311, consumer-side) |
| src/window_guard.h | 76 | header-only impl | Step 190 RAII WindowGuard :50; release() :57; ~WindowGuard destroys + terminates :62; GLFW only, no GL calls (the documented contract, :1-47); GLFW_INCLUDE_NONE defined first (:43-46); opt-in by design (the deliberate-NOT list, :23-34) |
| window/GLFW bootstrap patterns | - | NOT a boundary | embedded in src/main.cpp: glfwInit :559, glfwCreateWindow(800,600) :574, makeContextCurrent :582, gladLoadGL :587; WindowGuard adopted at 5 call sites (main.cpp:2811, 2842, 3157, 3225, 3395; the renderer.shutdown()-before-guard contract main.cpp:3205); the games carry their own copies (pong.cpp:34/49/57/58, platformer.cpp:116/124/130/131) - the pre-190 duplication window_guard.h:7-12 documents |
| src/pe_core.cpp | 32 | already has .cpp (the ONLY core .cpp) | Step 102 "Optional compiled core: header-only path remains primary" (pe_core.cpp:3-5; CMake:302-304); 22 headers included :7-28; pe_core_version()=102 :31 |
| CMake linkage | - | - | PureEngine = main.cpp + stb_impl.cpp + glad_gl.c (CMake:82-84; links glfw+miniaudio :92); pe_core STATIC :306-310 is linked by NO target (a build-only instantiation check; "Games may either include headers directly (header-only, current) or link pe_core" CMake:302-304); Pong/Platformer consume engine headers by RELATIVE include with src/ deliberately OFF the include path (CMake:314-315; pong.cpp:23-29, platformer.cpp:22-27) - the Step 281 time.h-shadowing rule |

Header hygiene facts: the Step 281 rename src/time.h -> engine_time.h is enforced by the
header_hygiene ctest target (tests/header_hygiene.cpp:9-18 includes <time.h> + <ctime> +
engine_time.h; CMake:271-278 - a reintroduced colliding name makes that target FAIL to
build); the rename left the engine_time.h banner saying "(src/time.h)" (engine_time.h:3)
- cosmetic doc-rot. Include rules observed: engine headers that include glfw3.h define
GLFW_INCLUDE_NONE FIRST - console.h:50-53, gamepad.h:40-43, window_guard.h:43-46 do;
engine_time.h:52 and input.h:53 include glfw3.h BARE (gap 1 below).

TESTS covering the inventory (names + what they assert):
- header_hygiene (ctest; CMake:273-278): the <time.h>/<ctime>/engine_time.h coexistence -
  the Step 281 rename invariant. Nothing else from this inventory.
- hostile_data_test (ctest; ~10.3K lines, main at :10307): WindowGuard DIRECTLY tested
  headless (the null guard is a no-op :9652; guards on real windows :9660/:9670; the
  Step 190 helper :9640); FrameTime exercised (:5715, and the tick-position check
  :9679-9699 - tick at the VERY TOP before glfwPollEvents; start() in the init-order
  check :3392); Mat4/Vec3 exercised INDIRECTLY through camera/renderer/collision/
  simulation checks (Mat4 84 uses; perspective/lookAt/orthographic 35; rotationY 14;
  rotationZ 0 direct); windows.h enters this TU only for the input test (:3025-3030).
- resources_test / LevelPipelineConsumer / PureEditor0 (ctest): no direct core coverage
  (the editor selftest exercises math indirectly via pick/unproject/projection).
- NO dedicated math unit test exists (zero direct rotationZ coverage anywhere in tests/);
  no deterministic FrameTime clamp (0.1s) assertion exists.

GAP LIST (Core Foundation only; evidence-cited; no feature wishlist):
1. Include-order trap: engine_time.h:52 and input.h:53 include <GLFW/glfw3.h> bare.
   Without GLFW_INCLUDE_NONE, glfw3.h pulls the system GL headers (GLFW's documented
   default). Today the big TUs stay GL-free ONLY BY ORDERING LUCK: in pe_core.cpp the
   alphabetical order puts console.h (:11, which defines GLFW_INCLUDE_NONE) 16 positions
   before engine_time.h (:27); in hostile_data_test.cpp console.h (:10) precedes
   engine_time.h (:24). The safe in-repo pattern exists (console/gamepad/window_guard).
   One include reorder or one new consumer and the GL headers arrive before math - and on
   Windows the near/far macro set is one explicit #include <windows.h> away (the Step 312
   clobber, fixed editor-side only: tools/editor0/main.cpp:47-49).
2. Incomplete pe_core.cpp contract: the TU claims to "force instantiation of core headers"
   (pe_core.cpp:3-5) but omits math/vec3.h + math/mat4.h (they arrive only transitively
   via camera.h :9), window_guard.h, mesh3d.h, components.h, gamestate.h, hostile_data.h,
   prefab.h and animation.h. The Core Foundation category's own math headers sit in the
   lib by transitive luck, not by contract.
3. Missing direct tests: no math unit test (rotationZ has zero direct coverage; the
   builders are only exercised through camera/renderer paths); no deterministic FrameTime
   clamp test (the 0.1s clamp is documented at engine_time.h:25/:83 but untested).
4. pe_core is linked by nobody (CMake:306-310; no consumer links it) - documented as
   optional (CMake:302-304), but the "or link pe_core" path is UNEXERCISED: a link break
   in pe_core would not surface in any game/test build until someone links it.
5. Undocumented-in-code init order: the Step 171 contract lives in prose (window_guard.h
   doc block; main.cpp comments) and the games DUPLICATE the bootstrap (pong.cpp:34-58,
   platformer.cpp:116-131) instead of sharing a window.h/.cpp - the duplication is
   documented and opt-in (window_guard.h:23-34), but there is no shared, tested bootstrap
   boundary to migrate TO (the extraction is slice 5).
6. Portability: the math/time/window-guard trio is GLFW+std only (portable); the
   Windows-specific risk is macro injection (near/far - the Step 312 incident), currently
   defended only in the editor TU (tools/editor0/main.cpp:48-49), NOT in the engine
   headers; mat4.h:236 near/far parameter names stay a per-TU hazard on Windows.
7. Cosmetic doc-rot: the engine_time.h:3 banner still names "(src/time.h)" - the Step 281
   rename did not update its own banner.

CATEGORY ACCEPTANCE STANDARD - Core Foundation ACCEPTED when ALL of these hold
(APPROVED by Cyril at Step 315 with no listed amendments; each slice lands as its own step gated on this standard):
A. Build: cmake --build build --config Release exits 0 with NO new warnings vs the
   pre-migration baseline (the warning delta recorded per slice, not asserted to zero).
B. Tests: ctest -C Release all green, and the count GROWS ADDITIVELY: a new math unit
   test target (Vec3 ops + all 9 static Mat4 builders + operator* + transformPoint,
   incl. rotationZ, with the near/far plane semantics pinned to hand-computed matrices)
   and a deterministic FrameTime test (glfwInit is legal headless; the 0.1s clamp
   asserted via a forced >=0.1s stall; the timescale/pause semantics) - ctest 5 -> 7;
   NO timing assertions (the Step 312 policy).
C. Live window check: PureEngine.exe opens the real window, renders, closes clean (the
   SMOKE_TEST.md release gate); the alive x3 protocol green.
D. Structure: every migrated boundary is exactly one .h (declarations + the doc contract)
   + one .cpp (definitions); the ODR grep (non-inline file-scope definitions in the
   inventoried headers) returns 0 before AND after; no API signature changes (any
   intentional change listed and approved separately).
E. Include hygiene, grep-verified: every engine header that includes glfw3.h defines
   GLFW_INCLUDE_NONE first (console, gamepad, window_guard, engine_time, input ALL
   conform); no engine header includes <windows.h>; header_hygiene stays green.
F. Consumer linkage complete: PureEngine, hostile_data_test, resources_test,
   LevelPipelineConsumer, PureEditor0, Pong, Platformer AND pe_core all build + link
   after every slice (any new engine .cpp must be added to every consumer's CMake source
   list - the games' relative-include rule makes this mandatory, not optional).
G. pe_core.cpp's instantiation list matches its documented claim (or the claim is amended
   to say exactly what it instantiates).
H. Measurement, not budget: one T0 bench re-run (--bench 2d/3d) after the final slice,
   recorded as DATA ("indicative, this machine, not a budget"; no gate).
OUT OF SCOPE for this category (stays out even when ACCEPTED): ECS, networking, glTF,
scripting (the v1.1 freezes); Entity/Scene (forbidden by this prompt - a separate
category decision); editor features; gameplay; the broadphase/materials/
generational-handle freezes; NO fixed-step/accumulator introduction into engine_time
(the Step 17 boundary stays mechanism-only; the editor's kEditorPlayDt stays
consumer-side).

MIGRATION SLICES (max 5, small; each lands as its own step with
the full Step 285 gates; slice 1 COMPLETE at Step 315 (the section below); slices 2-5 NOT started):
- Slice 1 (hygiene first, ~zero risk): add GLFW_INCLUDE_NONE to engine_time.h + input.h
  (the console.h:50-53 pattern); fix the engine_time.h:3 banner; document the two include
  rules (the GLFW_INCLUDE_NONE rule + the windows.h/near-far rule) in the header
  contracts. Files: src/engine_time.h, src/input.h (+ docs). Risk: near-zero (defines
  only; the include guards are idempotent either way). Test plan: full build; ctest 5/5;
  header_hygiene green; the editor + games still build; grep - no engine header includes
  glfw3.h bare.
- Slice 2 (pe_core contract, low risk): make pe_core.cpp's include list match its claim -
  add math/vec3.h + math/mat4.h FIRST (before any GLFW-touching header), plus
  window_guard.h, mesh3d.h, components.h, gamestate.h, hostile_data.h, prefab.h,
  animation.h; leave pe_core_version()=102 untouched (a Step 102 marker, not a version).
  Files: src/pe_core.cpp. Risk: low (compile-only; math-first also bakes the ordering
  rule into the TU). Test plan: pe_core builds; full build; ctest 5/5.
- Slice 3 (math: tests THEN split, medium risk): (a) add tests/math_test.cpp (a ctest
  target) pinning Vec3 ops and every Mat4 builder against hand-computed matrices, incl.
  rotationZ and the near/far semantics; (b) only then move the four RUNTIME builders
  (rotationZ :125, lookAt :189, perspective :236, rotationY :260) to src/math/math.cpp,
  keeping the constexpr builders and ALL of vec3.h header-only (for tiny leaf math,
  header-only is the RIGHT form - the audit says so explicitly). Files: src/math/mat4.h,
  src/math/math.cpp (new), tests/math_test.cpp (new), CMakeLists.txt (math.cpp added to
  PureEngine, pe_core, BOTH games, and every Mat4-consuming test target). Risk: medium -
  the linkage fan-out IS the cost; behavior identical if code only moves. Test plan:
  full build; ctest 6/6; the editor selftest 489 PASS unchanged; the T0 bench re-run as
  data.
- Slice 4 (engine_time split, low-medium risk): move FrameTime's bodies (start/tick/
  scaledTick/setTimeScale/setPaused) to src/engine_time.cpp; the header keeps the class
  declaration and DROPS <GLFW/glfw3.h> (the GLFW dependency moves into the .cpp - the
  hygiene payoff); add the deterministic FrameTime test. Files: src/engine_time.h,
  src/engine_time.cpp (new), a new test target, CMakeLists.txt (the .cpp into every
  FrameTime consumer - the same fan-out as slice 3). Risk: low-medium (the tick-position
  call-site contract is prose and unchanged; every consumer must link the new .cpp).
  Test plan: ctest 7/7; header_hygiene green; the hostile_data_test tick-position check
  stays green.
- Slice 5 (window bootstrap extraction, HIGHEST risk, optional/last): extract the shared
  init pattern (glfwInit -> create -> makeCurrent -> gladLoad) into src/window.h/.cpp
  with the Step 171 order documented IN code; src/main.cpp migrates; the games adopt
  OPT-IN only (the Step 190 ruling - no forced adoption); WindowGuard stays exactly as
  is. Files: src/window.h (new), src/window.cpp (new), src/main.cpp, CMakeLists.txt
  (games only if they opt in). Risk: highest - the failure paths and the Step 171
  contract live in main.cpp prose today. Test plan: ctest; alive x3; the live window
  check (the SMOKE_TEST gate); the WindowGuard headless checks stay green.

STATUS: Core Foundation: NOT AUDITED -> AUDIT COMPLETE (Step 314, 2026-10-13) /
IN PROGRESS pending migration. The migration starts ONLY after Cyril approves the
acceptance standard above (the prompt's own gate). No .cpp migration code was written
in this step.

### Core Foundation slice 1 (Step 315, 2026-10-13) - hygiene defines, COMPLETE

Cyril APPROVED the Category Acceptance Standard (no amendments listed); this step
implements slice 1 of the audit section above, extended by the Step 315 prompt to ALSO
include the mat4 near/far defense (gap 6's engine half - the prompt made it explicit)
and main.cpp (an engine include site the audit's slice-1 bullet had not named; the
wider file set is disclosed here). src/ touches are limited to the slice's four files.

IMPLEMENTED (commit CORE-315, `76aeed6`, 4 files +14/-5):
- GLFW_INCLUDE_NONE now precedes <GLFW/glfw3.h> at EVERY src/ include site (the grep
  evidence: the guard sits in the 3 lines before the include at all six sites) -
  console.h:53, engine_time.h:55, gamepad.h:43, input.h:56, window_guard.h:46,
  main.cpp:5. The two audit gaps (engine_time.h, input.h) are closed; main.cpp joined
  them (glad/gl.h at main.cpp:1 owns the GL declarations - the console.h:46-49 contract
  now holds repo-wide in src/).
- mat4.h perspective: the parameters renamed near/far -> zNear/zFar (the signature
  mat4.h:236, the body, and the caller-contract comment) - API-preserving (both call
  sites are positional: camera.h:275, hostile_data_test.cpp:5754, and checkPerspective
  at hostile_data_test.cpp:5745 already pins the matrix values, so the rename is
  behavior-verified by the existing suite); the non-comment near/far grep in mat4.h
  now returns 0 - the Windows macro clobber (the Step 312 incident class) can no longer
  break this header.
- engine_time.h:3 banner: "(src/time.h)" -> "(src/engine_time.h; was src/time.h,
  renamed in Step 281)".
- NO .cpp splits, no new features, no fixed-step into engine_time (the prompt's limits).

GATES (against `76aeed6`): cmake --build build --config Release exit 0 (the first full
run hit a 300s tool timeout mid-hostile_data_test, and its orphaned MSBuild node caused
a TRANSIENT MSB6003 tlog contention on pe_core on the rerun; the next rerun built
everything incl. pe_core.lib and a no-op rebuild printed BUILD EXIT=0 - a build-infra
hiccup, NOT the 296-class exe lock: no LNK1104, every exe linked on every attempt);
ctest -C Release 5/5 100% (13.26s; PureEditor0 1.76s - under the 40s guard); alive x3
(PureEngine True, Pong True, Platformer True - the SMOKE_TEST.md:12 probe pattern);
git diff --stat scoped to the slice (4 files, +14/-5); src/ outside the slice EMPTY.

MUTATION (the Step 285 check): the input.h guard removed from the COMMITTED tree ->
the slice's verification grep reported exactly 1 violation (input.h bare) - NON-VACUOUS
(the expected value from the contract); restored by pathspec; git diff --stat -- src/
EMPTY afterward.

STATUS: Core Foundation IN PROGRESS - slice 1 of 5 done; slices 2-5 (the pe_core
contract, the math tests-then-split, the engine_time split, the window bootstrap
extraction) NOT started; each waits on its own step. Residuals in FINDINGS Entry 54.

### Core Foundation slice 2 (Step 316, 2026-10-14) - the pe_core contract + the ODR link probe, COMPLETE

Cyril's migration decision recorded THIS STEP (the prompt's item 1): the Step 102
"header-only path remains primary" rule (the V3 Step 102 record line above + the
pe_core.cpp doc comment) is SUPERSEDED INCREMENTALLY by the migration objective
(header-first -> conventional .h/.cpp, category by category) - a module stays
header-only until its slice moves it. The pointers: the V3 Step 102 record line now
carries the dated bracket, and the pe_core.cpp doc comment states the supersession
(pe_core.cpp:3-8).

THE 16-HEADER CORE FOUNDATION LIST (named, per the Step 314 audit + its gap 2):
math/vec3.h, math/mat4.h, entity.h, collision.h, gamestate.h, events.h,
engine_time.h, lifecycle.h, input.h, gamepad.h, window_guard.h, mesh3d.h,
components.h, hostile_data.h, prefab.h, animation.h.

IMPLEMENTED (commit CORE-316, `a85caa6`, 4 files +94/-1):
- pe_core.cpp: 22 -> 31 includes - the 16-list complete (math/vec3.h + math/mat4.h
  FIRST at :11-12, before every other include; window_guard/mesh3d/components/
  gamestate/hostile_data/prefab/animation appended at :35-41); pe_core_version()=102
  untouched (a Step 102 marker, as the 314 slice-2 proposal ruled).
- pe_core builds in the default build (re-verified in stage 1: pe_core.lib).
- NEW target core_link_probe (CMake:317-324): tests/probe_a.cpp + probe_b.cpp, EACH
  including all 16 headers; probe_a (the main) calls Mat4::translation (math/mat4.h)
  and drives probe::b_entry(); probe_b calls Vec3::lengthSquared (math/vec3.h) - a
  different header; linked against pe_core; add_test + set_tests_properties TIMEOUT
  60. ctest count 5 -> 6 (stated per the prompt).
- NO game/editor/test target links pe_core this slice (their CMake is untouched);
  no code moved out of headers; no Entity/Scene work; no new features.

AUDIT EVIDENCE (every grep excluded build/, .git/, .kilo/ and .goose/):
- who links pe_core: NOBODY before this slice (the only target_link_libraries line
  mentioning pe_core was pe_core's own PUBLIC line, CMake:310) - the probe is the
  first exerciser of the compiled-core link path.
- the ODR sweep over the 16 headers (column-0 definition candidates, excluding
  guards/namespace/inline/static/constexpr/template/typedef/using/extern and
  comments): ZERO real hazards. Two column-0 candidates inspected and benign:
  entity.h:211 is a constructor member-init continuation (gravityScale(0.0f), inside
  a ctor), lifecycle.h:67 is an `enum class ArcadeRole` type declaration (the
  enumerators carry no linker symbols). RESULT: 0 inline additions (the cap was 10;
  none were needed - nothing was recorded as a blocking finding).

GATES (against `a85caa6`, staged, each with its exit code):
- stage 1 (pe_core + core_link_probe): exit 0 - the probe LINKED with zero
  multiple-definition errors.
- stage 2 (PureEngine, PureEditor0, Pong, Platformer, --parallel): exit 0.
- stage 3 (hostile_data_test, resources_test, LevelPipelineConsumer,
  header_hygiene, --parallel): exit 0.
- ctest -C Release: RUN 1 = 5/6 - hostile_data_test FAILED once (the failure
  signature was NOT captured: the immediate --rerun-failed overwrote
  Testing/Temporary/LastTest.log; the class is UNDETERMINED - nothing in this slice
  touches that test's TU: pe_core.cpp is not in it and no header changed); the
  rerun PASSED (6.79s) and two further full runs were 6/6 (8.50s, 8.37s; exit 0
  each). Disclosed as a transient; if it recurs, capture --output-on-failure FIRST
  (FINDINGS Entry 55).
- alive x3: PureEngine True, Pong True, Platformer True.
- git diff --check clean; the diff scoped to src/pe_core.cpp, CMakeLists.txt and
  the two probe files (zero inline fixes - none needed).

MUTATION (the Step 285 check): a non-inline definition appended to a Core Foundation
header (pe::odr_mutation_316 in src/components.h, never committed) -> core_link_probe
FAILED to link: "error LNK2005: ... already defined in probe_a.obj" + "fatal error
LNK1169: one or more multiply defined symbols found", exit 1 - the probe catches
exactly the hazard class it exists for. Restored by pathspec; git diff --stat -- src/
empty; the probe relinked exit 0.

BASELINE (optional, non-gating, "indicative, this machine"): the incremental
PureEditor0 rebuild after touching src/renderer.h = 33 s (exit 0); the full recompile
of pe_core + the probe = 17 s (exit 0). No 300 s interruption.

SMOKE: the slice-1 smoke template arrived UNFILLED - recorded "smoke: not tried";
the visuals 299-311 stay NOT TRIED; the live-verification row below is
VERIFICATION REQUIRED until Cyril's smoke line is recorded exactly as given.

ACCEPTANCE TABLE (the mandatory criteria; statuses from the vocabulary NOT AUDITED /
IN PROGRESS / BLOCKED / VERIFICATION REQUIRED / ACCEPTED):
| criterion | status | evidence |
| --- | --- | --- |
| A build exit 0 | ACCEPTED (this slice) | the three staged builds, exit 0 each |
| B tests grow, all green | ACCEPTED | ctest 8/8 (the probe + math_test + time_test, all TIMEOUT 60); both named test targets landed (slices 3-4) |
| C live window check | ACCEPTED | Cyril's verbatim smoke (Step 318): "smoke: PASS â€” editor opened with sample_scene; V hides meshId0, V restores 2D; games OK; probe OK; HEAD 8137a23"; alive x3 True at every gate |
| D one .h/.cpp per boundary + ODR 0 | ACCEPTED (the ODR half) | core_link_probe links clean; the mutation LNK2005/LNK1169 caught; 0 hazards in the 16 headers; math STAYS header-inline by the Step 317 decision (small hot bodies, a 6-8-target fan-out for no gain); engine_time STAYS header-inline by the Step 318 decision (1-5-line bodies, no ODR reason, a 5+-consumer fan-out; the GLFW dep already guarded NONE in slice 1) |
| E include hygiene | ACCEPTED (slice 1) | the guard at all 6 src/ GLFW sites (Step 315) |
| F consumer linkage | ACCEPTED | every target builds + links after every slice (the staged gates); the games share the Step 319 bootstrap (pe::bootWindow) |
| G pe_core list matches its claim | ACCEPTED | 31 headers = the 16-list + the 15 the TU already carried |
| H bench re-run as data | ACCEPTED | the Step 319 re-run, 600 frames of the 200-entity sample: 2D median 1.1379ms (T0 1.0709), 3D median 30.5809ms (T0 32.5299) - DATA, "indicative, this machine, not a budget" |

STATUS: Core Foundation IN PROGRESS (2 of 5). The ODR hazard list (EMPTY - zero
hazards found) is the slice-3 input; slice 3 (the math tests-then-split) NOT started.

### Core Foundation slice 3 (Step 317, 2026-10-14) - the math unit test + the STAY-INLINE decision, COMPLETE (tests-only ACCEPTED for math)

SMOKE: the human smoke line arrived UNFILLED again (<PASTE HIS ONE LINE>) - recorded
"smoke: not tried"; the visuals 299-311 stay NOT TRIED; acceptance row C stays
VERIFICATION REQUIRED.

THE STAY-INLINE DECISION (the prompt's item 3 - documented with evidence, NO code
moved): Vec3 and Mat4 STAY header-inline. The evidence:
- body sizes: every Vec3 function is <= 8 lines (vec3.h:54-116); the largest Mat4
  body is lookAt (mat4.h:189-211, ~20 lines), perspective :236-245 (9 lines),
  rotationZ :125-135, rotationY :260-270 - all trivially small;
- hotness: the per-frame camera path calls perspective/lookAt every drawn frame
  (camera.h:275 proj = Mat4::perspective; camera.h view() -> lookAt) - the T0 3D
  bench (median 32.5ms on HD 4000, Step 313) is dominated by the draw path, and a
  .cpp boundary adds call overhead against it;
- fan-out: a math.cpp would need adding to PureEngine, PureEditor0, Pong, Platformer,
  hostile_data_test, core_link_probe/pe_core (+ every future consumer) - 6-8 CMake
  touch points for zero behavior or clarity gain;
- the supersession is incremental by Cyril's ruling (Step 316): a module stays
  header-only until its slice moves it - THIS slice's measured decision is that math
  stays; the Core Foundation .h/.cpp work concentrates where bodies are non-trivial
  or carry dependencies (engine_time's GLFW - slice 4; the window bootstrap - slice 5).
Slice 3 is therefore TESTS-ONLY ACCEPTED for math (the prompt's own allowance).

THE MATH TEST (commits CORE-317 `96e9ece` + the fix `1dd3a33`; the CMake block after
the probe block; ctest count 6 -> 7, TIMEOUT 60 - stated per the prompt):
- tests/math_test.cpp: deterministic value asserts ONLY (no timing, no GL, no
  window, no files), hand-computed from the formulas (column-major m[col][row]):
  the Vec3 ops the engine uses (dot/cross/lengthSquared/length/normalized + the zero
  guard/operators/==!=); Mat4 identity (exact); multiply traced from operator*
  (T(1,2,3)*S(2,3,4): the diagonal of S + the translation column of T;
  transformPoint(T*S,(1,1,1)) = (3,5,7)); translation/scale (Vec3 and xyz forms);
  orthographic(0,8,0,6,0.1,100) (0.25, 2/6, -2/99.9, the centers -1);
  perspective(90deg, aspect 1, zNear 1, zFar 3): f=1, m[2][2]=-2, m[3][2]=-3, the
  w-row -1, AND the plane mapping (the near plane z=-1 -> pre-divide -1; the far
  plane z=-3 -> pre-divide +3, w=3 -> NDC +1); rotationZ(90deg) +X->+Y, rotationZ(0)
  the identity; rotationY(90deg) +Z->+X (the current behavior pinned); lookAt: the
  eye -> the origin, the target -> z=-5.
- THE IN-STEP DEFECT (disclosed): the first run FAILED 1 check - a test-authoring
  bug of mine, NOT the product: the I*T assert expected transformPoint(T,(1,2,3)) =
  (1,2,3) (identity semantics) instead of (2,4,6) (the translation semantics);
  traced from the formula (I*T = T exactly), fixed in `1dd3a33`, gates re-run green.
  The product code needed no change (the 310(g)/311 harness-bug class).

GATES (against `1dd3a33`): the staged builds exit 0 (stage 1 math_test; stage 2 the
full default build; the fix rebuild); ctest -C Release 7/7 100% (41.06s; PureEditor0
10.34s - under the 40s guard); alive x3 True (PureEngine, Pong, Platformer);
git diff --check clean; the diff scoped to CMakeLists.txt + tests/math_test.cpp.

MUTATION (the Step 285 check, on the committed tree): the lengthSquared expected
value 25 -> 24 in tests/math_test.cpp -> math_test FAILED 1 check ("FAIL: vec3:
lengthSquared((3,4,0)) = 25", exit 1) - NON-VACUOUS; restored by pathspec;
git diff --stat -- tests/ empty.

STATUS: Core Foundation IN PROGRESS (3 of 5). Slice 4 (the engine_time split + the
FrameTime test) NOT started - waits on Cyril's go.

### Core Foundation slice 4 (Step 318, 2026-10-15) - the FrameTime contract test + the engine_time STAY-INLINE decision, COMPLETE

THE LIVE SMOKE (Cyril's line, recorded VERBATIM - the first filled smoke of the
migration; it covers the editor + games + the probe at HEAD 8137a23):
"smoke: PASS â€” editor opened with sample_scene; V hides meshId0, V restores 2D;
games OK; probe OK; HEAD 8137a23"
The acceptance row C moves VERIFICATION REQUIRED -> ACCEPTED on this string. Narrow
visual note: the line states the V behaviors passing (the 311 V-toggle family) at
HEAD 8137a23 - recorded as the partial visual evidence it is; the 299-310 checklist
rows were not filled and stay NOT TRIED.

THE INSPECTION (engine_time.h, post-slice-1, file:line): the GLFW_INCLUDE_NONE guard
:52-54 before <GLFW/glfw3.h> :55 (the slice-1 rule holds; the clock comment keeps its
original em dash); class FrameTime :59; start() :68-69 (1 line); tick() :82-87
(5 lines; the clamp std::min(delta, 0.1) at :86 - the documented 0.1s maximum); the
scale/pause surface :93-96 (one-liners; the negative-scale guard at :93); scaledTick()
:97-99 (3 lines; the paused path returns the literal 0.0f); the state :105-107
(lastTime/timeScale/paused).

THE STAY-INLINE DECISION (the prompt's bar - NO code moved): engine_time STAYS
header-inline. The evidence: every body is 1-5 lines (the largest is tick at 5);
there is NO ODR reason (core_link_probe stays green - nothing changed); a .cpp would
fan out to 5+ consumer targets (src/main.cpp:99 with the uses at :651-652/:1536,
games/pong/pong.cpp:146-147, games/platformer/platformer.cpp, tests/hostile_data_test.cpp:24
with the uses at :5715/:9699) for zero behavior or clarity gain; the only hygiene
motivation (the GLFW include) was already addressed by the slice-1 NONE guard, and the
full GLFW-removal payoff belongs to a consumer-linkage slice if ever needed. Same bar
and same outcome as slice 3's math.

THE FRAMETIME CONTRACT TEST (commit CORE-318 `b9f5faf`; CMake after the math_test
block; ctest count 7 -> 8, TIMEOUT 60 - stated per the prompt):
- tests/time_test.cpp: deterministic asserts ONLY - the only real-clock use is a
  forced 150 ms stall, which is load-robust BY CONSTRUCTION (sleep overshoots, never
  undershoots, so the raw delta is always >= 0.15 s and the clamp returns the
  constant bit-exact; a longer stall under load changes nothing):
  the normal dt path within [0, 0.1] (the clamp invariant); THE CLAMP - a 150 ms
  stall -> tick() == 0.1f bit-exact; the scale state (setTimeScale(2) stored; the
  negative scale clamps to exactly 0 at :93; scale 0 -> scaledTick == 0.0f exact);
  the pause state machine (paused -> scaledTick == 0.0f EXACTLY regardless of scale;
  the unpause stored); the scaled clamp (a 150 ms stall at scale 2 -> scaledTick ==
  0.2f bit-exact - 0.1f*2 is an exact float exponent bump); glfwInit is legal
  headless (no window); glfwTerminate at the end.
- No flake surface: no assert depends on a SMALL or specific delta - every clock-
  dependent assert pins an OVERSHOOT-tolerant constant.

GATES (against `b9f5faf`): the staged builds exit 0 (stage 1 time_test; stage 2 the
full default build); ctest -C Release 8/8 100% (49.39s; PureEditor0 16.45s - under
the 40s guard); alive x3 True (PureEngine, Pong, Platformer).

MUTATION (the Step 285 check, on the committed tree): the clamp expected value
0.1f -> 0.2f in tests/time_test.cpp -> time_test FAILED 1 check ("FAIL: time: a
150ms stall clamps tick() to EXACTLY 0.1f (bit-exact)", exit 1) - NON-VACUOUS;
restored by pathspec; git diff --stat -- tests/ empty.

STATUS: Core Foundation IN PROGRESS (4 of 5). Slice 5 (the window-bootstrap
extraction) NOT started - waits on Cyril's explicit go.

### Core Foundation slice 5 (Step 319, 2026-10-15) - the window bootstrap extraction, COMPLETE - CATEGORY ACCEPTED (5 of 5)

SMOKE BASELINE (Cyril, verbatim - the 318 baseline, NOT a new visual claim):
"smoke: PASS â€” editor opened with sample_scene; V hides meshId0, V restores 2D;
games OK; probe OK". The runtime proof for THIS slice is alive x3 (all three games
START through the shared bootstrap).

THE INSPECT-FIRST REPORT (file:line, before any edit):
- Arcade main.cpp :560-597: glfwInit (:562, fail -> cerr "Failed to initialize
  GLFW" -> return -1), the 3.3 Core hints + the APPLE forward-compat guard
  (:568-574), glfwCreateWindow(800,600,"PureEngine - Step 1") (:577, fail -> cerr +
  terminate -> return -1), makeContextCurrent (:585), gladLoadGL (:590, fail ->
  cerr + destroy + terminate -> return -1). NO WindowGuard on this path (the guard
  sites :2811/:2842/:3157/:3225/:3395 are the other entry points).
- pong.cpp :32-63: the same 5 steps, the "Pong: " prefix, return -1; then the
  renderer init with its OWN cleanup (:66-72) - stays game-side.
- platformer.cpp :115-136: the same 5 steps, the "platformer: " prefix, return 1;
  then audio init (:138+) - stays game-side.
- window_guard.h: the RAII teardown API (:50/:57/:62) - UNTOUCHED; its call sites
  (main.cpp x5, hostile_data_test :9652-9670) keep working as-is.
- PureEditor0: its own hidden-window path (createHiddenGlWindow) - NOT forced onto
  the helper (the prompt: prefer games first) - the recorded residual.
- Verdict: SAFE in one step - the three blocks are the same 5-step sequence; the
  divergences are titles, tag-prefixed messages, and return codes.

IMPLEMENTED (commit CORE-319, `e63095b`, 7 files +138/-84):
- NEW paired src/window.h + src/window.cpp - pe::bootWindow(width, height, title,
  logTag): the FIRST Core Foundation .h/.cpp split of the migration. It owns the
  Step 171 sequence (init, the 3.3 Core hints + the APPLE guard, create, context,
  glad) with the failure cleanup INSIDE and the unified "<logTag>: <step> failed"
  stderr line. THE DELIBERATE WORDING CHANGE (disclosed): main.cpp's "Failed to
  initialize GLFW"/"Failed to create GLFW window"/"Failed to initialize GLAD" and
  pong's "Pong: Failed to ..." become the platformer-style "<tag>: glfwInit failed"
  / "<tag>: window failed" / "<tag>: glad failed" (all three sites already used
  stderr; the exit codes are unchanged and caller-side; the titles/sizes are
  byte-identical).
- The three games wired: main.cpp (the include in the engine_time.h region, the
  block replaced, return -1 kept), pong.cpp (return -1), platformer.cpp (return 1).
  WindowGuard + the post-glad steps (renderer/audio init with their own cleanup)
  untouched.
- hostile_data_test: the new checkWindowBoot negative check (a degenerate 0x0 size
  must fail CLEANLY - ok=false, window null, the cleanup inside; a 64x64 window
  must succeed with the caller owning the session), wired into the main
  aggregation. ctest count 8 -> 8 (no new target; the check lives in the existing
  suite - stated per the prompt).
- CMake: src/window.cpp added to PureEngine, hostile_data_test, Pong, Platformer.
- pe_core is NOT extended with window.h this slice (the recorded residual); the
  editor bootstrap is NOT touched (the recorded residual).

GATES (against `e63095b`, staged): stage 1 (PureEngine, Pong, Platformer,
hostile_data_test, --parallel) exit 0; stage 2 (the full default build) exit 0;
ctest -C Release 8/8 100% (13.63s); alive x3 True (PureEngine, Pong, Platformer -
the games START through the shared bootstrap); git diff --check clean; the diff
scoped to window.h/window.cpp + the three mains + hostile_data_test + CMakeLists.

MUTATION (the Step 285 check, on the committed tree): the window-failure path
mutated to report ok=true -> the checkWindowBoot negative check FAILED
("boot: a degenerate size must fail cleanly"; hostile_data_test FAILED, exit 8) -
NON-VACUOUS; restored by pathspec; git diff --stat -- src/ empty.

CRITERION H (the bench re-run, DATA - "indicative, this machine, not a budget"):
the 200-entity bench, 600 frames: 2D median 1.1379ms (the T0 baseline 1.0709); 3D
median 30.5809ms (the T0 baseline 32.5299). Same machine class, same policy - no
gate.

THE IN-STEP DEFECT (disclosed): the CMake source-list edits were first applied by
a broken loop that passed the REPLACEMENT string as the PATTERN (and $null as the
replacement), mangling the PureEngine source list (the src/main.cpp line lost);
recovered by `git checkout -- CMakeLists.txt` and REDONE with fully literal string
surgery (no capture groups, no $-substitutions) + per-anchor count checks. Nothing
was committed in the broken state; the four regions were re-verified by reading.

ACCEPTANCE TABLE (final): A build exit 0 - ACCEPTED; B tests 8/8 - ACCEPTED;
C live window - ACCEPTED (the 318 verbatim smoke + alive x3); D structure/ODR -
ACCEPTED (window.h carries declarations only; the probe green); E include hygiene -
ACCEPTED (all 7 src/ GLFW sites guarded NONE: console, engine_time, gamepad, input,
window_guard, window, main); F consumer linkage - ACCEPTED (every target builds +
links after every slice; the games share the bootstrap); G pe_core's list matches -
ACCEPTED (31 headers = the 314 16-list + the 15 the TU carried); H bench re-run as
data - ACCEPTED (the numbers above, recorded as DATA).

STATUS: Core Foundation ACCEPTED (5 of 5 slices, 2026-10-15). The freeze
guardrails stand: no ECS/net/glTF/scripting, no Entity/Scene work, no editor
redesign. RESIDUALS (FINDINGS Entry 58): the editor keeps its own hidden-window
bootstrap; pe_core does not instantiate window.h; the stderr wording unification.

### Entity / Scene / Persistence category audit (Step 320, 2026-10-15) - AUDIT COMPLETE, migration NOT STARTED

Docs-only audit step (no fixes, no splits). GROUPING NOTE: this prompt's category
cuts across the V3 5-category map (entity/lifecycle were filed under "1 Core
foundation", scene/prefab/hostile under "2 World + content") - the audit follows
the prompt's grouping; the map placements stay as recorded. Core Foundation remains
ACCEPTED (Step 319, 5 of 5 slices). Search hygiene: build/, .git/, .kilo/ and
.goose/ were excluded from every grep and search.

INVENTORY (each file: header-only impl | already has .cpp | mixed):
| file | lines | role | form | main call sites | ctest coverage | honest limits (recorded contracts) |
| --- | --- | --- | --- | --- | --- | --- |
| src/entity.h | 360 | the entity datum: the plain struct (the Step 103/v1.1 ruling) - rotationAngle :46 (Z radians), rotationSpeed :47, scale/halfExtents :60, roleId :75, animationState :78+, velocity :91, meshId :119, the static + kinematic flags :133-137, gravityScale :211 | header-only impl | everywhere (the arcade main, the games, the editor, the tests) | the whole suite constructs entities; the 309-310 editor delete/create tests | the SINGLE-AXIS rotation per mode (the 296 correction); no TRS composition :128 |
| src/components.h | 44 | Step 103 "lightweight component helpers (not ECS)": the inline sugar over Entity.velocity/health/timer/tag (16 one-line functions) | header-only impl | opt-in; the games unchanged (the defaults) | the component check in hostile_data_test | explicitly NOT ECS (no archetypes, no query language) - the frozen ruling |
| src/scene.h | 820 | struct Scene :52 (queueSpawn :73 / flushSpawns :81 - the append-here design, the pointer-invalidation note :26); SceneManager :110 (current as an INDEX :112, addScene :116, currentScene :132/:143, sceneByName :159, switchTo :178); the persistence: saveSceneToFile :381 (the atomic tmp+rename :397, the explicit-path handling :384-396), loadSceneFromFile :459 (the strict v1(14)/v2(34) parse :600-631, the unknown-version reject :636-639), the manager save/load :692/:808 (the fs::rename C-rename note :700) | header-only impl (the LARGEST header in src/) | main.cpp :1135 (a comment re the persisted-path exclusion), the editor's Ctrl+S path (the 299-310 records), the tests | checkSceneLifecycle/NoOps/ByName/Serialization/PointerStability, checkSceneManagerSave/RoundTrip, checkScenePersistenceV1/V2/V99/Compose, checkSceneDumpReload, checkSpawnQueue/AfterKill | the alive drop-on-save :419-420 ("Step 113: dead entities don't exist in saved state"); the 4-decimal float format :418; the no-count-field format (the 291 observation); meshId NOT serialized (the 311 gap); the strict unknown-version reject |
| src/lifecycle.h | 158 | buildInitialEntities (:15/:70 - the six arcade constructions), resetEntities (:22), flagsForCount (:26/:152), ArcadeRole :67, the atomic resetGame block (:37-39) | header-only impl | the arcade main + the tests | checkHierarchyChain/Refusals/EdgeCases, checkHierarchyContractFreeze | no generational handles, no spawning system, no destruction (:32) - the recorded boundary |
| src/prefab.h | 209 | loadPrefab :103 (the 3-candidate probe :71-73, the "# PureEngine prefab v1" header check :135, warn+skip partial parse), instantiatePrefab :181 (the 17 config fields + position + alive) | header-only impl | the editor (the N/[ keys, the 310 records), the tests | checkPrefabSystem/SceneSpawn/Live, checkPrefabMultiSpawnPersist, checkPrefabSceneFailureMatrix | the malformed-warn+skip contract (:16); the probe CWD-dependence (the run-from-root note) |
| src/hostile_data.h | 187 | HostileDefinition :27, HostileDefaults :34, parseFloatList :54, loadHostileDefaults :86 (the data-driven defaults + the per-hostile overrides) | header-only impl | the arcade main, the tests | the 12+7 case-type matrix (the recorded loads) | the malformed clean-reject semantics (recorded) |

THE SAVE/LOAD PATHS (already proven): the format v2 (Step 111; v1 = the first 14
fields with backward compatibility :372-374); 34 comma-separated entity fields
(:378-380); the atomic tmp+rename (:397); the manager save/load through the same
functions (:692/:808); the editor's Ctrl+S (the editor-side path, proven by the
editor selftest's checkSaveAs/checkPathSwitch family at 489 PASS); the proven test
matrix: checkSceneSerialization, checkScenePersistenceV1/V2/V99/Compose,
checkSceneManagerSave/RoundTrip, checkSceneDumpReload, checkSpawnAfterKill,
checkPrefabMultiSpawnPersist, checkScenePointerStability, checkSceneByName.

ODR BASELINE (the sweep, the same pattern as the 314/316 audits): ZERO real hazards
across the six headers - the only two column-0 candidates are the KNOWN benign pair
(entity.h:211 the ctor member-init continuation; lifecycle.h:67 the enum class
declaration). Nothing to fix; nothing invented.

HONEST LIMITS = RECORDED CONTRACTS (changes need explicit human approval): the
dead-drop-on-save (:420), the 4-decimal format (:418), the append-here index
semantics (the kill keeps the index; queueSpawn appends at the old count), the
no-count-field format, meshId non-serialization, the strict unknown-version reject
(:636-639).

PROPOSED MIGRATION SLICES (max 5, PROPOSED ONLY, none started; the Core Foundation
shape: inspect -> test-first where possible -> optional .h/.cpp only when justified
-> ACCEPTED criteria):
- Slice 1 (tests-first, ~zero risk): the persistence FORMAT pins - extend the
  existing checkPersist*/checkSceneSerialization family with the byte-stability and
  rounding pins (the 4-decimal rounding of a chosen coordinate, the dead-drop in a
  saved file, the no-count-field parse, the unknown-version reject, the tmp+rename
  atomicity). Files: tests only. Risk: near-zero. Test plan: ctest green, the count
  grows.
- Slice 2 (the scene persistence split, medium risk): saveSceneToFile/
  loadSceneFromFile + the manager save/load (scene.h :381-820, ~350 lines of real
  bodies) -> the paired scene_persist.h/.cpp (the Scene/SceneManager data + the
  inline list ops STAY header). Files: src/scene.h, src/scene_persist.h/.cpp (new),
  CMake (the consumers: PureEngine, PureEditor0, hostile_data_test, pe_core -
  scene.h is in pe_core's includes), the tests if they link. Risk: medium - the
  fan-out IS the cost; the format bytes must not change. Test plan: slice 1's pins
  + the full ctest + the editor save/load smoke.
- Slice 3 (the prefab split, low-medium risk): loadPrefab/instantiatePrefab
  (prefab.h :103-205, ~100 lines) -> the paired prefab .h/.cpp (the Prefab struct
  STAYS header). Files: src/prefab.h, src/prefab.cpp (new), CMake (the editor, the
  tests, pe_core). Risk: low-medium. Test plan: the existing checkPrefab* matrix +
  the failure matrix.
- Slice 4 (the hostile parse split, low-medium risk): loadHostileDefaults/
  parseFloatList (hostile_data.h :54-187) -> the paired hostile .h/.cpp (the
  structs STAY header). Files: src/hostile_data.h, src/hostile_data.cpp (new),
  CMake (the arcade main, the tests, pe_core). Risk: low-medium. Test plan: the
  12+7 case matrix.
- Slice 5 (the STAY-INLINE decision, near-zero risk): entity.h (the plain structs -
  the v1.1 ruling), components.h (one-line sugar), lifecycle.h (small,
  arcade-shaped) - documented STAY with the fan-out evidence (the same measured
  pattern as the Core Foundation math/engine_time decisions). Files: docs only.
  Risk: near-zero. Test plan: the existing suite stays green.
WHAT STAYS INLINE (the named set): entity.h, components.h, lifecycle.h (slice 5
documents it). THE CANDIDATES TO MOVE: the scene persistence half (slice 2), the
prefab load/instantiate (slice 3), the hostile parse (slice 4).

DRAFT ACCEPTANCE STANDARD (this category; PROPOSED, not yet approved):
A. build exit 0 (staged).
B. ctest green, the count grows additively (the format pins; NO timing asserts).
C. the live editor save/load smoke (the Ctrl+S -> R round-trip) - VERIFICATION
   REQUIRED until Cyril's line is recorded verbatim.
D. structure: one .h/.cpp per migrated boundary; the ODR grep 0 on the category
   headers (the 320 sweep = the baseline: 0 real hazards, the 2 known benign
   candidates); no API signature changes without a listed approval.
E. include hygiene: no new GLFW include without the NONE guard; no windows.h in
   the category headers.
F. consumer linkage complete: every target calling the moved bodies links the new
   .cpp (pe_core included - the category headers are in its instantiation list).
G. pe_core's instantiation list stays matched to its claim.
H. format stability: the save->load->save byte-identity re-proven after any
   persistence move (the existing matrix stays green).
I. measurement not budget (no timing asserts; any bench run is DATA).
OUT (frozen or out of scope): ECS (archetypes/query languages), networking,
generational handles, the editor redesign, gameplay, and CHANGES to the recorded
contracts (the dead-drop, the 4-decimal format, the index semantics, the
no-count-field format, meshId non-serialization, the strict version reject) - those
need explicit human approval.

STATUS: Entity/Scene/Persistence: NOT STARTED -> AUDIT COMPLETE (Step 320,
2026-10-15) / migration NOT STARTED. Core Foundation remains ACCEPTED (5 of 5,
Step 319). Slice 1 NOT started - it waits on Cyril's go.
***STOP NOTE (Step 321, 2026-10-15): THE 296-CLASS LOCK (RESOLVED 2026-10-15: the
human cleared the locks - the verbatim local evidence: "cmake --build build --config Release -> all targets linked (incl. hostile_data_test, time_test)" and "ctest -C Release -> 8/8 Passed (hostile_data_test 12.98s ... time_test 1.42s), 100%"; the gate-recovery step re-ran the gates green (the full build exit 0, ctest 8/8, the direct pins run exit 0 with zero "pins:" failure lines + the aggregate PASS), the mutation break-PIN-A NON-VACUOUS (exit 8, restored, REBUILT, 8/8 again), alive x3 True, 321 flipped completed - see the tracker + FINDINGS 60.) THE ORIGINAL TEXT BELOW IS KEPT FOR HISTORY.*** Slice 1
(the persistence format/contract pins) is CODED AND COMMITTED (`bc851d2`,
tests-only, hostile_data_test) but the gates are BLOCKED: the Release link
fails with LNK1104 on hostile_data_test.exe AND time_test.exe (write+delete
blocked, read OK), 9 probe attempts over ~9 minutes all failed, and the lock
SPREAD TO EXECUTION - Start-Process cannot run PureEngine.exe/Pong.exe/
Platformer.exe ("used by another process"; the three "ALIVE True" lines from
the probe script are a null-handling ARTIFACT and are recorded as INVALID,
not as passes). ctest ran 6/8 (the 2 locked tests "Not Run"). UNVERIFIED:
the five pins have never executed; the mutation was NOT performed; alive x3
INVALID. THE RECOVERY (the 305/311 pattern): the human clears the lock
(resmon -> Associated Handles, or delete the locked exes + rebuild), then a
gate-recovery step reruns the gates (the staged build, ctest 8/8, alive x3),
performs the slice's mutation (break PIN A -> FAIL -> restore -> REBUILD ->
ctest), and flips 321 stopped -> completed. The FINDINGS 59 rule stands:
after any mutation restore, REBUILD before ctest.

### Entity/Scene/Persistence Step 322 (2026-10-15) - INSPECT + DECIDE (numbers only) - scene.h + prefab.h SPLIT-JUSTIFIED (awaiting Cyril), the rest STAY-INLINE

Docs-only. No .cpp, no code changes. The rule applied verbatim: STAY-INLINE if the
bodies are small OR the fan-out is narrow; SPLIT-JUSTIFIED if the persist bodies are
large AND the I/O includes are paid by many targets that never save. Per the prompt:
a SPLIT-JUSTIFIED verdict is RECORDED and the .cpp WAITS for Cyril.

THE NUMBERS (grep-verified; build/, .git/, .kilo/ and .goose/ excluded):
| header | lines | non-trivial bodies | the I/O includes forced | the fan-out (TUs including it) | the never-save/load consumers |
| --- | --- | --- | --- | --- | --- |
| scene.h | 820 | the persistence region :279-820 = 542 lines; the save/load/manager bodies alone :381-816 = 436 (save :381-450, load :459-649, the manager save :651-711, the manager load :724-816) | <filesystem> :38, <fstream> :39, <iomanip> :40, <iostream> :41, <sstream> :42, <cstdio> :37 | 6: src/main.cpp, games/platformer/platformer.cpp, tools/editor0/editor0_core.h, tools/editor0/main.cpp, consumers/level_pipeline/consumer.cpp, src/pe_core.cpp | main.cpp (the ONLY mention :1135 is a comment - it never calls the persist functions), platformer.cpp (zero persist calls), pe_core.cpp (instantiation only) = 3 of 6 |
| prefab.h | 209 | loadPrefab :103-179 (~77) + instantiatePrefab :181-205 (~25) + the parse helpers :61-102 (~42) = ~144 | <iostream> :26, <fstream> :27, <sstream> :28, <filesystem> :29 | 6: src/main.cpp, tools/editor0/main.cpp, consumer.cpp, src/pe_core.cpp, tests/probe_a.cpp, tests/probe_b.cpp | pe_core.cpp (instantiation only), probe_a, probe_b (include-only, never call) = at least 3 of 6 |
| hostile_data.h | 187 | loadHostileDefaults :86-187 (~102) + parseFloatList :54-72 (~19) + the helpers :45-53 (~9) = ~130 | <filesystem> :16, <fstream> :17, <iostream> :18, <sstream> :19 | DIRECT 3: src/pe_core.cpp, probe_a, probe_b; TRANSITIVE 5 via lifecycle.h :58 (adds src/main.cpp, tools/editor0/main.cpp) | the direct payers: pe_core (never loads), the probes (never call) |
| entity.h | 360 | ~0 non-trivial (the data struct + one-liners; the :211 candidate is the ctor member-init continuation) | light: <cstddef> :28, <string> :29, <vector> :30 + math + animation.h - NO I/O streams | 8 (the widest): src/main.cpp, pong.cpp, platformer.cpp, tools/editor0/main.cpp, consumer.cpp, src/pe_core.cpp, probe_a, probe_b | n/a - nothing heavy to pay |
| components.h | 44 | 0 (16 one-line sugar functions) | light: <string> :12 | 4: src/main.cpp, src/pe_core.cpp, probe_a, probe_b | n/a |
| lifecycle.h | 158 | ~80 (buildInitialEntities :76-141 - the six arcade constructions, DATA-shaped; resetEntities :142-151; flagsForCount :152-157) | light of its own (<cstddef> :54, <vector> :55); TRANSITIVE: it includes hostile_data.h :58, so the I/O includes reach its 5 consumers | 5: src/main.cpp, tools/editor0/main.cpp, src/pe_core.cpp, probe_a, probe_b | the transitive-I/O note - not a lifecycle defect |

ODR/link pain: NONE recorded (the 320 baseline: 0 real hazards; the 2 benign
candidates entity.h:211 + lifecycle.h:67; no duplicate-symbol, no circular include,
no build-time anomaly in evidence anywhere).

THE VERDICTS (the rule applied to the numbers above):
- scene.h: SPLIT-JUSTIFIED - the bodies are large (436-542 lines) AND the I/O
  includes are paid by 6 TUs of which 3 (main.cpp, platformer.cpp, pe_core.cpp)
  never call the persist functions. THE .cpp WAITS FOR CYRIL. The 320 audit's
  Slice 2 shape stands (the paired scene_persist.h/.cpp; the Scene/SceneManager
  data + the inline list ops stay header; the fan-out to wire: PureEngine,
  PureEditor0, hostile_data_test, pe_core).
- prefab.h: SPLIT-JUSTIFIED - the bodies ~144 lines (a real parse+I/O body) AND
  the I/O includes are paid by 6 TUs of which at least 3 (pe_core.cpp, probe_a,
  probe_b) never load a prefab. THE .cpp WAITS FOR CYRIL (the 320 audit's Slice 3
  shape).
- hostile_data.h: STAY-INLINE (borderline - the numbers cited) - the bodies ~130
  lines (moderate: between the Core Foundation's stayed 20-66-line bodies and
  scene.h's 436-542) but the DIRECT fan-out is narrow (3 TUs); the transitive
  reach (5 TUs via lifecycle.h :58) is lifecycle's dependency choice - decoupling
  THAT include is the cheap first move if the transitive I/O cost is ever
  measured as a problem, WITHOUT splitting hostile_data itself.
- entity.h: STAY-INLINE - ~0 non-trivial bodies, light includes, the plain-struct
  v1.1 ruling; the 8-TU fan-out pays almost nothing.
- components.h: STAY-INLINE - 0 non-trivial bodies (one-line sugar), light
  includes.
- lifecycle.h: STAY-INLINE - the bodies are DATA-shaped (the six constructions),
  the own includes are light; the transitive hostile_data.h include is recorded
  above.

STATUS: Entity/Scene/Persistence IN PROGRESS 1/5 (the pins done at 321 + the
recovery). Slice 2 (the scene persistence split): SPLIT-JUSTIFIED, AWAITING CYRIL
- no .cpp until the explicit go. Slice 5 (the entity/components/lifecycle
STAY-INLINE): DECIDED with the numbers. Slices 3-4: unimplemented (prefab's
SPLIT-JUSTIFIED verdict recorded above; hostile_data STAY-INLINE). No persist
contract changed. No other category opened.

### Entity/Scene/Persistence Step 323 (2026-10-15) - slice 2: the scene persistence split, COMPLETE

The first Entity/Scene .h/.cpp split, executed per the Step 322 SPLIT-JUSTIFIED
verdict (Cyril's go). THE INSPECT (before any edit, file:line): the persistence
region scene.h :279-820 holds 11 plain-inline functions - the 7 helpers
sceneTrim :279 / sceneParseFloat :286 / sceneParseInt :295 / sceneSplitComma :304 /
sceneQuote :320 / sceneUnquote :329 / sceneSplitCommaQuoted :348 (ALL with ZERO
external users - grep-verified) + the 4 bodies saveSceneToFile :381-450 /
loadSceneFromFile :459-649 / saveSceneManagerToFile :651-711 /
loadSceneManagerFromFile :724-816. ZERO templates, ZERO anon-namespace blocks -
the move is clean.

THE GOLDEN METHODOLOGY (the proof of no change): a temp golden generator
(editor0_tmp/golden_gen.cpp, gitignored) saved, from the PRE-SPLIT tree: a
3-entity scene (one DEAD entity, a tilemap reference, a meshId=3 entity, quoted
tags) -> golden_scene.txt; a 2-scene manager (current=1, a dead entity in beta)
-> golden_manager.txt + the per-scene scene_alpha.txt/scene_beta.txt. ALL FOUR
recorded (sizes + SHA256): 475 B D7BABC38...F0E / 91 B 228BA511...3147 / 236 B
CD3A4B4A...070D / 229 B 29630231...FEF58C. After the split the SAME generator
re-ran: ALL FOUR BYTE-IDENTICAL (hash match) - the contracts provably unchanged.
The per-scene files land via the bare-name assets/ probe (CWD-dependent - the
process lesson: probe all three candidates when hashing).

THE SPLIT (commit CORE-323, `8aa07e5`, 4 files +612/-547, net +65):
- NEW paired src/scene_persist.h (37 lines: the 4 declarations + the contract
  comment) + src/scene_persist.cpp (565 lines: the bodies VERBATIM, inline
  stripped; the 7 helpers file-local in the anonymous namespace).
- scene.h slimmed 820 -> 279 lines: the data types + the inline list ops
  (spawnEntity/killEntity/killRole/loadTilemapIntoScene/queueSpawn/flushSpawns)
  STAY; the persistence region replaced by the guarded include of
  scene_persist.h placed AFTER the namespace close (the guarded-include pattern
  - no circular dependency, both include orders work).
- scene.h's I/O includes are GONE: <fstream>/<filesystem>/<iomanip>/<iostream>/
  <sstream>/<cstdio> all 0 (were 6). scene_persist.cpp carries all five
  (justified - it IS the I/O). NO TU needed an include fix for the lost
  transitive includes (the build proved it - the only break was the LINK below).
- The linkage: scene_persist.cpp added to PureEngine, PureEditor0,
  hostile_data_test, pe_core, AND LevelPipelineConsumer (the consumer CALLS the
  persistence in the Step 278 adversarial tests - the link error LNK2019 x4
  caught it; the 322 audit's fan-out missed it because consumer.cpp's calls were
  not in the 320 grep window - recorded as the audit-precision residual).

THE IN-STEP DEFECTS (both caught by the build, both recovered before the commit):
1. the include-inside-namespace bug: the scene.h tail placed the
   scene_persist.h include INSIDE namespace pe -> scene_persist.h's own
   `namespace pe` nested -> pe::pe (the C2039 cascade in simulation.h);
   fixed by moving the include AFTER the namespace close.
2. the descending-range corruption: a fix attempt used lines[275..274] (a
   descending PowerShell range) which silently yielded a reversed 2-line slice
   and duplicated the include; recovered by git checkout + the deterministic
   index-based re-assembly with the nested-pe guard.

GATES (against `8aa07e5`): the full build exit 0 (all 11 targets); ctest -C
Release 8/8 100% (44.19s pre-commit on the identical tree, 14.59s post-restore);
the golden re-run: ALL FOUR BYTE-IDENTICAL; alive x3 True with the FIXED probe.

MUTATION (the Step 285 check, on the MOVED code): setprecision(4) -> (5) in
scene_persist.cpp -> hostile_data_test FAILED with TWO pin failures ("pins: A
the writer is not 4-decimal bound" + "pins: A the reload must carry the ROUNDED
values", exit 8) - NON-VACUOUS; restored by pathspec; REBUILT -> ctest 8/8
(14.59s).

STATUS: Entity/Scene/Persistence IN PROGRESS (2 of 5). Slice 3 (the prefab
split - SPLIT-JUSTIFIED at 322) and Slice 4 (the hostile parse) NOT started -
they wait on Cyril. Slice 5 (the STAY-INLINE decision) was decided at 322.

### Entity/Scene/Persistence Step 324 (2026-10-16) - slice 3: the prefab I/O split, COMPLETE

The second Entity/Scene .h/.cpp split, per the Step 322 SPLIT-JUSTIFIED verdict (the
scene_persist pattern). THE INSPECT (before any edit): prefab.h 209 lines - struct
Prefab :33-56 + instantiatePrefab :181-205 (pure construction, STAY) + the
prefab_detail helpers :59-97 (prefabTrim/prefabCandidates/prefabParseFloat/
prefabParseVec3/prefabParseBool - ZERO external users, grep-verified) + loadPrefab
:103-177 (the 3-candidate probe + the CWD/absolute fallbacks + the strict header
check + the malformed/unknown-key warn+skip). The caller grep: main.cpp (2),
editor0_core.h (1) + editor0/main.cpp (12), hostile_data_test (11), consumer.cpp
(23) - the linkage set = PureEngine, PureEditor0, hostile_data_test,
LevelPipelineConsumer.

THE SPLIT (commit CORE-324, `8154c98`, 4 files +180/-126):
- NEW paired src/prefab_persist.h (32 lines: the loadPrefab declaration + the
  contract comment) + src/prefab_persist.cpp (~120 lines: the bodies VERBATIM,
  inline stripped; the prefab_detail helpers file-local).
- prefab.h slimmed 209 -> 88 lines: the Prefab struct + instantiatePrefab STAY;
  the I/O includes <iostream>/<fstream>/<sstream>/<filesystem> GONE (were 4);
  the guarded prefab_persist.h include AFTER the namespace close (the
  scene_persist pattern); the stale "Header-only, no CMakeLists.txt change" doc
  line updated.
- The load contracts UNCHANGED: the 3-candidate probe, the strict "# PureEngine
  prefab v1" header check, the malformed-warn+skip, the unknown-key warn+skip,
  out assigned only on the full success.
- CMake: prefab_persist.cpp added to the four caller targets. NOT added:
  pe_core (it includes prefab.h but never calls loadPrefab - the declarations
  suffice; the recorded residual), the probes (include-only).

GATES (against `8154c98`): the full build exit 0 (11 targets); ctest -C Release
8/8 100% (24.59s; the checkPrefab* family + the persistence pins all green);
alive x3 True with the FIXED probe.

MUTATION (the Step 285 check, on the MOVED code): loadPrefab mutated to always
return false (the parsed result dropped) -> hostile_data_test FAILED with FOUR
load-path assertions ("shipped enemy.txt failed to load" x2, the multi-persist
test, the failure-matrix check; exit 8) - NON-VACUOUS; restored by pathspec;
REBUILT -> ctest 8/8 (15.60s).

THE IN-STEP FRICTION (all caught before any damage): the anchor asserts caught
three label/idx off-by-ones (the read-display labels are idx+1) and one leftover
assembly line from an abandoned approach (it duplicated the doc-fix line and
swallowed the */ close -> the C1071 EOF-in-comment build error) - fixed by the
byte-level tail dump + the leftover removal; the mutation's first anchor used LF
against the CRLF file (count 0 - nothing changed) and its first replacement
dropped the closing brace (the C1075 imbalance) - both fixed before the mutation
landed.

STATUS: Entity/Scene/Persistence IN PROGRESS (3 of 5). Slice 4 (the hostile_data
parse split - STAY-INLINE was the 322 verdict, revisitable) NOT started; it waits
on Cyril. Slice 5 (the STAY-INLINE decision) was decided at 322.

### Entity/Scene/Persistence Step 325 (2026-10-16) - CATEGORY ACCEPTED (close, docs + gates)

The category close per Cyril's Step 325 authorization. THE GATES AT CLOSE (all
green): the full build exit 0; ctest -C Release 8/8 100% (14.42s); alive x3 True
(PureEngine, Pong, Platformer, the $null-checked probe).

THE DELIVERED PROGRAM (the 320 audit's 5-slice shape, as executed):
- Slice 1 (Step 321, RECOVERED the same day after the 296-class lock): the
  persistence FORMAT/CONTRACT pins in hostile_data_test - PIN A the 4-decimal
  writer bound + the rounded reload, PIN B meshId NOT serialized (the 34-field/
  33-comma shape + the default 0), PIN C the file-level dead-drop, PIN D the
  unknown-PREFIX strict reject with out untouched, PIN E no .tmp after the
  atomic save. The already-pinned contracts named and not re-pinned.
- Slice 2 (Step 323): the paired src/scene_persist.h/.cpp - the 4 persistence
  bodies + the 7 file-local helpers moved VERBATIM from scene.h :277-816;
  scene.h 820 -> 279 lines with ZERO I/O includes; scene_persist.cpp linked
  into 5 targets (incl. LevelPipelineConsumer - the fan-out the link caught);
  the golden byte-identity proof ALL FOUR files vs the pre-split SHA256.
- Slice 3 (Step 324): the paired src/prefab_persist.h/.cpp - loadPrefab + the
  prefab_detail helpers moved VERBATIM from prefab.h :58-177; prefab.h 209 ->
  88 lines with ZERO I/O includes; prefab_persist.cpp linked into the 4 caller
  targets.
- Slice 4 (the hostile_data parse split): NOT EXECUTED - SUPERSEDED by the Step
  322 INSPECT+DECIDE verdict (STAY-INLINE: the bodies ~130 lines moderate, the
  DIRECT fan-out narrow at 3, the transitive reach 5 via lifecycle.h :58;
  revisit only on a measured need).
- Slice 5 (the STAY-INLINE decisions, decided at 322 with the numbers):
  entity.h (~0 non-trivial bodies, light includes, the 8-TU fan-out pays almost
  nothing; the plain-struct v1.1 ruling), components.h (16 one-line sugar
  functions), lifecycle.h (~80 DATA-shaped lines; the transitive hostile_data.h
  include via :58 noted).

THE ACCEPTANCE CRITERIA (the 320 draft A-I) - ALL MET:
A the build exit 0 at every slice; B ctest green, the additive count 5 -> 8
(the Core's probe/math/time targets; the category's pins joined
hostile_data_test); C the live editor save/load smoke - Cyril's VERBATIM PASS
at 318 + alive x3 at every slice; D structure: one .h/.cpp per migrated
boundary, the ODR grep 0 (the 320 baseline held through both splits), no API
signature changes beyond the declared linkage moves; E include hygiene: all
src/ GLFW sites guarded, no windows.h in the category headers, the I/O
includes now live ONLY in scene_persist.cpp/prefab_persist.cpp; F consumer
linkage complete (5 targets link scene_persist.cpp, 4 link
prefab_persist.cpp, every caller covered); G pe_core's instantiation list
still matches its claim; H format stability: the golden byte-identity proof
ALL FOUR (323) + the pins (321); I measurement not budget (no timing asserts
anywhere).

THE HONEST LIMITS - UNCHANGED RECORDED CONTRACTS: the dead-drop-on-save, the
4-decimal format, the append-here index semantics, meshId not in the scene
file, the strict version/prefix rejects, the atomic tmp+rename. Changes need
explicit human approval.

THE RESIDUALS (recorded, none blocking): the editor keeps its own
hidden-window bootstrap; pe_core instantiates the category headers but links
only scene_persist.cpp (it never calls loadPrefab, so prefab_persist.cpp is
not in its sources - the declarations suffice); hostile_data.h keeps its
transitive I/O reach via lifecycle.h :58.

STATUS: Entity/Scene/Persistence ACCEPTED (2026-10-16). The migration program:
3 implemented slices + 2 decision-slices (one superseding a proposed split).
The next category (Render/Physics/math or any other) opens ONLY on Cyril's
explicit prompt.

### Render category audit (Step 326, 2026-10-16) - AUDIT COMPLETE, migration NOT STARTED

Docs-only audit (no refactors, no math phase, no Physics migration). Core
Foundation remains ACCEPTED (319/325); Entity/Scene/Persistence remains ACCEPTED
(325). Search hygiene: build/, .git/, .kilo/ and .goose/ excluded from every grep.

INVENTORY (grep-verified; all seven headers header-only impl, ODR sweep: ZERO
candidates across all seven - the cleanest category yet):
| header | lines | role + key surfaces | the includes it forces | the fan-out (TUs) | classification |
| --- | --- | --- | --- | --- | --- |
| renderer.h | 1269 (the largest header in src/) | the Renderer class: init/destroyAll :141/:1218, drawWorld x2 :401/:408 + drawWorldInternal :448 (the 2D sorted path), drawDigitString :649, the mesh registry + clearRegisteredMeshes :755, drawEntity3D :763 (the clearDepth param), drawEntityMesh3D :826, drawDebugMesh3D :970, drawTextString :1026, drawAABBs :1085, releaseTexture :1201; the OOB texture diagnostics :527/:811 | <glad/gl.h> (intrinsic), <algorithm>/<cassert>/<iostream>/<set>/<string>/<unordered_map>/<vector> + resources/shader/font/lighting/math/entity/mesh3d | 7 TUs (main, pong, platformer, editor0/main, hostile_data_test, resources_test, pe_core) | contract+tests (the 3D half) + GAP (the 2D half - see below); STAY-INLINE (every consumer actually draws - the 322 never-use condition fails; the size is the intrinsic weight of the GL boundary) |
| mesh3d.h | 196 | unitCubeVertices :49, pyramidVertices :76 (the built-in data), loadMeshFromObj :102 (the OBJ triangle-soup loader, ~95 parse-I/O lines) | <fstream> :38, <sstream> :39, <iostream> :40 (I/O ONLY for the loader) + <vector>/<string> + math | 5 TUs (main, hostile_data_test, pe_core, probe_a, probe_b) | SPLIT-JUSTIFIED candidate (the loader half): the bodies ~95 lines (moderate) AND the I/O includes paid by 5 TUs of which at least 3 (pe_core instantiation-only, the probes include-only) never load an OBJ; the vertex-data builders STAY header (pure data). AWAITS CYRIL - no .cpp until the go |
| shader.h | 142 | readShaderFile :38, compileShader :50, the program link; the assets/shaders/ probe | <glad/gl.h> (intrinsic), <fstream> :29, <iostream> :30, <sstream> :31 | DIRECT 1 (pe_core) + TRANSITIVE 7 via renderer.h :73 | thin-to-moderate; STAY-INLINE (the GL coupling intrinsic; the transitive <fstream>/<sstream> via renderer.h noted - a renderer.h include-audit would lighten it) |
| lighting.h | 66 | PointLight :29, AmbientLight :36, LightingState :41, addLight :49 - pure data + a one-line mutator; MAX_LIGHTS=4 (the shader-array bound documented at one place) | math + <vector> only - the CLEANEST header | 3 direct + transitive via renderer.h :77 | thin; STAY-INLINE |
| resources.h | 197 | loadRgbTexture :93, loadRgbaTexture :123, loadBinaryBlob :151, loadPackEntry :175, binaryCache :181/:185, clearBinaryCache :193 | <glad/gl.h>, <stb_image.h> :76 (the declarations; the impl in stb_impl.cpp), <fstream> :80, <unordered_map> :81 | 4 TUs (main, hostile_data_test, resources_test, pe_core) | contract+tests (the dedicated resources_test target + checkTextureRegistry); STAY-INLINE pending a measured need (the init-time loads; the blob/cache is the Step 101-102 machinery) |
| font.h | 64 | fontCellFor :34, textWidth :53, alignOffsetX :58 - pure math | <string> only | 3 direct + transitive via renderer.h :76 | thin + contract+tests (checkFontCells/checkFontMetrics); STAY-INLINE |
| ui.h | 125 | the HUD number formatting (the one-decimal rule :49) + the drawDigitString hand-through ("used, never owned" :54) | <iomanip>/<sstream> (the number formatting, NOT file I/O) + renderer.h | 2 direct (main, pe_core) | thin; STAY-INLINE |

THE DRAW CONSUMERS: the arcade main.cpp (the 2D batch + the HUD + the 3D debug),
pong.cpp/platformer.cpp (the 2D batch), PureEditor0 (the 2D + the drawEntity3D 3D
path with the caller-side first-draw clearDepth discipline, Step 311), the tests
(the headless GL context via the hidden-window pattern).

DEPTH CLEAR DISCIPLINE: the caller-side contract (drawEntity3D/drawEntityMesh3D/
drawDebugMesh3D default clearDepth=false; the FIRST 3D draw carries true - the
Step 311 contract) - pinned by checkDebugFrameDepth/checkDepthState/
checkMeshDepthState. meshId REGISTRY: the Step 244 registry + clearRegisteredMeshes
:755 - pinned by checkMeshRegistry/checkMeshHandle/checkMeshIdParse/
checkMeshStateLeak. RESOURCE LIFETIME: releaseTexture/destroyAll + the OOB
diagnostics :527/:811 - pinned by checkTextureRegistry/checkMeshStateLeak +
resources_test. 3D DEBUG PATH: drawDebugMesh3D + drawAABBs - pinned by
checkAABBDebugSize/checkDebug3dParse/checkFrozen3DConsumer. font/HUD: pinned by
checkFontCells/checkFontMetrics.

THE LARGEST GAP (one primary): the 2D DRAW PATH - drawWorld/drawWorldInternal
(renderer.h :401-640, ~190 lines: the depth-based stable_sort draw order, the
sprite UV frames, the OOB texture diagnostics, the lit overload) has NO headless
test - hostile_data_test mentions drawWorld only twice (indirect), and the ONLY
2D contract evidence is the F-04 cassert (renderer.h :453: entities.size() ==
colliding.size()). Every mesh/depth/texture/lighting/occlusion check exercises
the 3D/mesh path; the engine's PRIMARY 2D surface is verified only by the live
games. FEASIBLE TO FIX: the headless-window pattern already proven by
checkWindowBoot/checkInputEdges - a Render slice 1 (tests-first) can drive
drawWorld against a hidden window and pin the draw-order/UV/OOB contracts.

THE DRAFT ACCEPTANCE CRITERIA (Render; PROPOSED, not yet approved):
A. the build exit 0 (staged).
B. ctest green, the count grows additively (a render-contract target driving
   drawWorld/drawEntity3D against the hidden window; NO timing asserts).
C. the live smoke: alive x3 + the Cyril visual checklist (the 2D frame, the 3D
   depth discipline, the HUD digits) - VERIFICATION REQUIRED until recorded.
D. the structure: one .h/.cpp per split boundary (only mesh3d is
   SPLIT-JUSTIFIED today); the ODR grep 0 held; no renderer API changes beyond
   the declared moves.
E. the include hygiene: the GL/glad stays intrinsic to the render headers; no
   new heavy includes in the headers the .cpps can own.
F. the consumer linkage complete: every caller of a moved body links the new
   .cpp (the 323/324 pattern).
G. the contracts pinned before any move (the tests-first rule).
H. the behavior proof: the golden/regression evidence for any moved draw path
   (the headless frame compare or the contract asserts - no pixel golden unless
   Cyril asks).
I. measurement not budget (no timing asserts; any bench run is DATA).
OUT: no new GL features, no batch rewrite, no shader abstraction, no renderer
API redesign, no Post-processing/materials, no Physics migration, no math
phase - the next category opens only on Cyril's explicit prompt.

STATUS: Render AUDITED / migration NOT STARTED. Slice 1 candidate: the
tests-first 2D draw-contract target (the largest gap). The mesh3d SPLIT-JUSTIFIED
verdict waits on Cyril. No .cpp until the go.

### Render Step 327 (2026-10-16) - slice 1: the headless 2D drawWorld contract checks, COMPLETE

The Render migration slice 1, tests-first per the 326 audit's largest-gap finding
(the untested 2D draw path). THE PATTERN: the hidden-window GL context (the
checkWindowBoot/checkDepthState idiom) + the full Renderer init + glReadPixels
probes. THE KEY TECHNIQUE: TEXEL-FREE CHANNEL ISOLATION - the collision tint
override (renderer.h :599-600) zeroes the green channel in the whole footprint
REGARDLESS of the texel, and a non-colliding (0,1,0) tint zeroes the red channel -
so the depth-sort winner is observable without knowing any texture content. The
non-vacuity is guaranteed by the documented palette rule (renderer.h :213-214:
every palette color keeps red-channel content).

THE PINS (commit CORE-327, `46d97c6`, tests/hostile_data_test.cpp +189/-1, ctest
8 -> 8 no new target):
1. THE DEPTH SORT: the higher-depth colliding-red entity covers the lower-depth
   green-tint entity - all probes G == 0 (texel-free) AND at least one probe
   R > 0 (the palette rule). The probe grid: 5 points inside the entity
   triangle at a known ortho position.
2. THE COLLISION OVERRIDE: a colliding entity zeroes the green channel (the
   red override x the texel) regardless of its tint.
3. THE DEAD ENTITIES draw nothing - the clear color survives at their
   positions.
4. meshId>0 entities are SKIPPED in the 2D pass (the Step 232 single-clear
   rule) - the clear color survives.
5. THE STATE RESTORE: GL_BLEND disabled + the depth OFF after the world pass
   (renderer.h :632-633).
6. THE OOB textureId falls back to a drawn texture (the checker) WITHOUT
   crashing - the diagnostic is NDEBUG-gated (silent in Release, no flake);
   the assert is "something drew", not the message text.
7. THE LIT OVERLOAD is callable headless and draws (its fallback path when
   litProgram==0, renderer.h :412-416, or its own path).

GATES (against `46d97c6`): the full build exit 0; ctest -C Release 8/8 100%
(8.73s pre-mutation, 9.04s post-restore); the direct hostile_data_test run
prints ZERO draw2d failure lines (all 7 pins green); alive x3 True with the
FIXED probe.

MUTATION (the Step 285 check, on renderer.h): the depth-sort comparator
flipped (< -> >, renderer.h :496) -> hostile_data_test FAILED with exactly
"draw2d: the higher depth must win the overlap (the red-colliding A covers
the green B)" (exit 8) - NON-VACUOUS, delivered by the palette rule exactly
as designed; restored by pathspec; REBUILT -> ctest 8/8 (9.04s).

STATUS: Render migration IN PROGRESS (1 of N - the pins landed; the mesh3d
SPLIT-JUSTIFIED candidate and any further Render slices wait on Cyril's go).
The live pixel-perfect smoke stays VERIFICATION REQUIRED (human), not CI.

### Render Step 328 (2026-10-16) - the 3D contract coverage audit (docs-only, matrix + gaps + the 329 recommendation)

Docs-only. No new tests, no source edits, no mesh3d.cpp, no renderer API change.
The gates at audit time: the build exit 0, ctest 8/8 (22.73s), alive x3 True.

THE INVENTORY (file:line, all header-only): the mesh registry registerMesh
renderer.h :752-754 (meshRegistry_[id] = move - the re-register REPLACES, the
Step 89 rule; meshId 1/2 stay the hardcoded paths, the registry serves 3+),
clearRegisteredMeshes :755-758; the lookup :789-799 (the registered draws its
data; the unregistered >2 AND the empty registered vector fall back to the cube;
meshId==0 -> no 3D draw :765-767); drawEntity3D :763-834 (the Step 261 sizing =
halfExtents*scale*2 :776-780; the tint drives the color :781-785); the depth
discipline :841-854 (the depthWasOn capture, glEnable, the clearDepth OPT-IN
clear :852-854, the color buffer never touched) and :986-989 (drawDebugMesh3D's
own); drawEntityMesh3D :826-969 (the per-face loop :860-866 note); the CPU
per-face lighting :868+; drawAABBs :1085-1129; the TEXTURE LIFETIME CONTRACT
:1140-1168 (LOAD/REGISTER/DRAW/RELEASE/CLEAR/TEARDOWN/STALE IDS - the core
0..4 refuse in releaseTexture :1201-1207, clearNonCoreTextures :1208-1212,
destroyAll :1218-1241 safe at ANY partial-init point); drawWorldInternal's
sort/batch :490-547; mesh3d.h loadMeshFromObj :102-192 (the 3-candidate probe
:105-115, the 1-based indexing with the bounds checks :134-138, the malformed
warn+skip :130/:181, the per-face planar UV scheme :142-178, the triangles==0
reject :187-189, out UNTOUCHED on failure :104/:190); unitCubeVertices :49 /
pyramidVertices :76; shader.h readShaderFile :38 + compileShader :50;
lighting.h MAX_LIGHTS=4 + LightingState :41; resources.h loadRgbTexture :93 /
loadRgbaTexture :123 / the blob cache :151-193.

THE COVERAGE MATRIX (the exact assertions read from the test bodies - no
coverage claimed from names):
| contract | symbol file:line | intended behavior + evidence | existing test + the EXACT assertion | status | mode | the strongest missing assert | conf |
| --- | --- | --- | --- | --- | --- | --- | --- |
| the registry register/replace | renderer.h :752-754 | the re-register replaces the slot | checkMeshRegistry :7809+ registers ids 3 AND 4, then the MEASURED depth-strip probe distinguishes the loaded tetra from the cube fallback | Covered | hidden-window GL | a same-id RE-REGISTER with different data changes the drawn silhouette | 0.85 |
| the registry lookup + the fallback | renderer.h :789-799 | the registered draws its data; the unregistered >2 -> the cube; meshId==0 -> no 3D draw | checkMeshRegistry (the strip) + the 327 PIN 4 (the 2D skip) | Covered | hidden-window GL | an EMPTY registered vector -> the cube fallback (:792 !empty() untested) | 0.8 |
| the OBJ load success | mesh3d.h :102-192 | the 4-face fixture -> 60 floats; the per-face UVs in [0,1]; the bounds ~unit | checkMeshLoad :7629-7644 (size==60 EXACT, the UV bounds, the position bounds) | Covered | headless | nothing material | 0.95 |
| the OBJ failure paths | mesh3d.h :115/:130/:181/:187-190 | the missing -> false + out untouched; the malformed warn+skip; the 0-triangle -> false | checkMeshLoad :7647-7651 (the missing -> false + untouched) + the malformed fixture (v 999 + f 1 2 99 skipped, the good triangle loads) | Covered | headless | the malformed-SKIP count (the implied-by-load assert) | 0.9 |
| the depth enable/write/restore | renderer.h :841-854/:986-989 | the 3D enables + the opt-in clear; the 2D leaves OFF | checkDepthState :5943-5962 (before==after + the 2D OFF) + checkMeshDepthState (an ON caller's ON survives + the depth WRITES) + checkDebugFrameDepth | Covered | hidden-window GL | the glDepthMask state (never asserted) | 0.85 |
| the occlusion order-independence | renderer.h :842-854 | the near hides the far regardless of the order | checkMeshOcclusion :8252+ (the near/far entities + the pixel probe) | Covered | hidden-window GL | the >2-entity overlap chain | 0.85 |
| the clearDepth opt-in + the color preservation | renderer.h :852-854 | the depth-only clear never touches the color | checkMeshStateLeak :6539-6545 (the SENTINEL corner pixel survives clearDepth=true) | Covered | hidden-window GL | nothing material | 0.9 |
| the texture registry/release | renderer.h :1169-1212 + resources.h :93-123 | the growable slots; the release >=5 only (the core 0..4 refuse); the OOB/released -> the checker | checkTextureRegistry :4529+ (the body NOT read for this audit) + the 327 PIN 6 | Partial | hidden-window GL | the core-slot REFUSE + the double-release REFUSE asserts quoted | 0.75 |
| destroyAll partial-init safety | renderer.h :1218-1241 | safe at ANY point of a partial init | the init-SUCCESS paths only | Partial | hidden-window GL | an init-failure -> destroyAll -> re-init sequence | 0.6 |
| the lighting uniforms | renderer.h :423-446 | the ambient + the MAX_LIGHTS loop uploads | checkMeshLighting :8121+ (the lit-draw pixel evidence) | Covered | hidden-window GL | n/a (the pixel evidence is the right form - GL uniform read-back is impractical) | 0.8 |
| the lit fallback | renderer.h :412-416 | litProgram==0 -> the unlit path | the 327 PIN 7 (callable + something drew) | Partial | headless GL | the fallback ENGAGED (the pixel == the unlit output for the same input) | 0.7 |
| the 2D depth sort | renderer.h :494-497 | the lower depth first; the higher wins | the 327 PIN 1 (the texel-free channel isolation + the comparator-flip mutation) | Covered | headless GL | nothing material | 0.95 |
| the collision tint override | renderer.h :599-600 | the colliding -> the red channel only | the 327 PIN 2 | Covered | headless GL | the mutation gap (below) | 0.95 |
| the meshId 2D skip | renderer.h :515-519 | the dead + the meshId>0 draw nothing in 2D | the 327 PINs 3-4 | Covered | headless GL | the mutation gap (below) | 0.9 |
| the blend restore | renderer.h :632-633 | the blending disabled after the world | the 327 PIN 5 | Covered | headless GL | n/a | 0.9 |
| the AABB debug draw | renderer.h :1085-1129 | the line loop sized halfExtents*scale*2 (the Step 261 truth) | checkAABBDebugSize + checkMeshSizing | Covered | hidden-window GL | the line-loop pixel evidence | 0.8 |
| the font/digit path | renderer.h :649+ + font.h :34-58 | the screen-space glyphs, the 37-cell atlas | checkFontCells/checkFontMetrics (font.h) - the renderer's glyph HALF untested | Partial | headless (font.h) / human (the glyphs) | the glyph-position pixel probe | 0.7 |

THE 327 REASSESSMENT (PIN 2-7): the ONLY mutation run against the 2D pins is
PIN 1's (the comparator flip, 327). PIN 2-7 have NO mutation. What PIN 6 and
PIN 7 ACTUALLY observe: PIN 6 = after a drawWorld with textureId 999, at least
one probe pixel is non-black - i.e., the checker fallback PRODUCED output; it
does NOT verify the checker's identity, and the diagnostic message is
unobservable (NDEBUG-gated). PIN 7 = after the lit drawWorld, at least one
probe pixel is non-black - it does NOT distinguish the litProgram path from
the :412-416 fallback, nor any lighting math. THE RANKED MUTATION GAPS:
1. PIN 2 (the collision override - the gameplay-visible feedback; the tint
   logic :599-600), 2. PIN 4 (the meshId 2D skip - the double-draw hazard),
3. PIN 3 (the dead skip), 4. PIN 6 (the fallback output), 5. PIN 7 (the lit
dispatch), 6. PIN 5 (the state hygiene - partially covered by
checkDepthState's depth-OFF assert).

THE TOP 5 GAPS (impact x likelihood):
1. PIN 2's mutation gap: the collision-override regression would silence the
   gameplay's collision feedback (the red flash); the likelihood moderate (the
   :599-600 lines were touched by the Steps 86/112 history).
2. PIN 4's mutation gap: the meshId 2D-skip regression = the documented
   2D/3D double-draw hazard returns; the likelihood low-moderate.
3. The releaseTexture refusal contracts unquoted (the matrix row 9): a
   regression = the runtime unload breaking a running frame (the documented
   SAFETY RULE :1132-1135); the likelihood low.
4. The lit-fallback distinguishability (PIN 7): a regression = the lit call
   drawing nothing when litProgram==0; the likelihood low.
5. The destroyAll partial-init safety: a regression = the failure-path
   crash; the likelihood very low (the :1214-1216 no-op-on-0 design).

THE RECOMMENDED SMALLEST SLICE 329 (NOT implemented): two renderer.h one-line
mutations run against the EXISTING pins - (a) drop the collision override
(:600 -> the tint passes through) -> PIN 2 must FAIL (the probe G>0 for the
green-tint colliding entity), (b) drop the meshId skip (:519) -> PIN 4 must
FAIL (the probe non-black) - each: FAIL -> pathspec restore -> REBUILD ->
ctest 8/8. PLUS: read checkTextureRegistry :4529 FIRST (the self-review
caveat) and add the release-refusal asserts ONLY if absent: releaseTexture(2)
== false (the core refuse), releaseTexture(5)==true then releaseTexture(5)==
false (the double-release refuse). The pass/fail: both mutations non-vacuous,
the refusals green, ctest 8/8, alive x3.

THE OBJ SPLIT (mesh3d.h) - the numbers: the loader mesh3d.h :102-192 = 91
lines (the parse + the I/O) + the includes <fstream>/<sstream>/<iostream>
forced on 5 TUs; the vertex-data builders :49-101 (~53 lines, pure data -
STAY); the never-load consumers: pe_core (instantiation-only), probe_a,
probe_b = 3 of 5; the ODR 0; the behavior-preservation risk LOW (the pure
function move; the out-untouched + the expansion contracts pinned EXACTLY by
checkMeshLoad); the compile/link risk LOW (the scene_persist/prefab_persist
pattern; the probe/header-only consumers need NO link - they never call the
loader). VERDICT: SPLIT-JUSTIFIED (the same shape and thresholds as
prefab_persist at 324: the moderate-large I/O bodies + the includes paid by
majority-never-load consumers). The .cpp WAITS for Cyril (no .cpp this step
per the prompt).

THE SELF-REVIEW - THE WEAKEST INFERENCE: the matrix row 9 (the texture
registry/release) is classified Partial with the release-refusal gap WITHOUT
reading checkTextureRegistry's body (:4529) - the refusal asserts may already
exist there; the classification rests on the name + the Step 89/94 records,
not the read body. The 329 recommendation accounts for this (read FIRST, add
ONLY if absent). The second-weakest: PIN 6/7's "something drew" evidence
depends on the checker/texture assets being loadable in the ctest CWD - a
missing-asset environment would fail them spuriously (never observed in the
gate runs to date).

STATUS: Render AUDIT DEEPENED (the 326 audit + this coverage matrix);
Render migration IN PROGRESS 1/N (the 327 pins); Slice 329 RECOMMENDED, not
started; the mesh3d split SPLIT-JUSTIFIED, awaiting Cyril; the live visual
smoke stays VERIFICATION REQUIRED (human).

### Render Step 329 (2026-10-16) - the 2D pins validated (mutations A-D) + the releaseTexture gap closed, COMPLETE

The 328-recommended validation slice, executed. ALL FOUR mutations NON-VACUOUS,
each with the exact failing line naming its pin (the full failing-line lists
below - no first-line-only reporting).

MUTATION A (PIN 2, the collision override): renderer.h :599 `if (colliding[i])`
-> `if (false)`. THE FAILING LINES (both, from --output-on-failure):
"draw2d: the higher depth must win the overlap (the red-colliding A covers the
green B)" (PIN 1 - the CO-FAILURE: the same override produces the winner's red,
so bypassing it removes that red too - the shared-code mutation, expected) and
"draw2d: the colliding override must zero the green channel" (PIN 2 - the pin
under test, named exactly). Exit 8. Restored by pathspec; REBUILT; the focused
ctest green.

MUTATION B (PIN 4, the meshId 2D skip): renderer.h :519 `if (entity.meshId != 0)
continue;` -> `if (false && ...)`. THE FAILING LINE (exactly one):
"draw2d: meshId>0 must be skipped in the 2D pass" - names PIN 4 exactly. Exit 8.
Restored; REBUILT; green.

MUTATION C (PIN 3, the dead skip): renderer.h :515 `if (!entity.alive) continue;`
-> `if (false && ...)`. THE FAILING LINE (exactly one): "draw2d: dead entities
must draw nothing (the clear color survives)" - names PIN 3 exactly. Exit 8.
Restored; REBUILT; green.

MUTATION D (PIN 5, the blend restore): renderer.h :633 (the WORLD-pass
glDisable - disambiguated from the 2 other GL_BLEND sites by the Step 112
comment anchor; the first attempt aborted on the 3-match ambiguity BEFORE any
edit) -> commented out. THE FAILING LINE (exactly one): "draw2d: GL_BLEND must
be disabled after the world pass" - names PIN 5 exactly. Exit 8. Restored;
REBUILT; green.

THE RELEASETEXTURE GAP (the 328 row-9 Partial -> CLOSED): the checkTextureRegistry
body read FIRST per the self-review - THE CONFIRMED FINDING: the old body was a
PURE LOCAL-VECTOR SIMULATION (a std::vector<unsigned int> reg with the grow/OOB
logic on the LOCAL copy) touching NO renderer code - releaseTexture had ZERO
coverage. The single-slot releaseTexture refusals (the core id<5 :1202, the
negative/OOB :1202, the double-release :1203) are PRIVATE - not directly
pinnable; RECORDED (the 328 residual stands for the single-slot form). The
PUBLIC-API observable contracts ARE now pinned (commit `0aa719e`,
tests/hostile_data_test.cpp +44): after init textureCount()==6 (0..4 + the Step
89 proof slot 5); registerNonCoreTexture appends at the next slot; the FIRST
unloadNonCoreTextures releases EXACTLY the 2 non-core slots; the unload KEEPS
the slots (the ids stable); the SECOND unload releases NOTHING (the
double-release refuse via the zeroed slots, the :1176 guard); the re-register
after the unload appends a fresh slot. No stricter policy invented - the asserts
match :1173-1180 verbatim.

THE PIN 6/7 TEXTURE-ASSET RESOLUTION (the 328 item): VERIFIED - the assets
resolve under the ctest CWD because the Renderer init() SUCCEEDS in the test
(the 327/329 runs): init loads checker.png + tex_player/scenery/hostile/
hostile_alt/paddle_spritesheet via the 3-candidate probe (:199-227) - a missing
asset would fail init and the section would report "renderer init must succeed".
The probe method: the successful init + the non-black PIN 6/7 reads.

GATES: the build exit 0; ctest -C Release 8/8 100% (19.60s with the new section;
the focused runs 6.97-35.26s across the mutations); alive x3 True with the
$null-checked probe.

STATUS: Render migration IN PROGRESS (1 of N: the 2D pins VALIDATED by the
4 mutations + the release gap closed; the mesh3d split SPLIT-JUSTIFIED at
322/328 waits on Cyril; the live pixel-perfect smoke stays VERIFICATION
REQUIRED).
***STOP NOTE (Step 330, 2026-10-17): THE 296-CLASS LOCK IS BACK (the FOURTH
incident: 305, 311, 321, 330) - the LINK blocked on 4 exes (LNK1104) AND the
EXECUTION blocked (the game exes cannot start).*** The mesh3d OBJ-loader split
is CODED AND COMMITTED (`0509c06` + the CMake fixes `4806755`/`3c6cb74`/
`c84fe93`/`b665eee` - the CMake corruption incident disclosed in the tracker:
the fix script's undefined $CRLF collapsed the file to one line; recovered
from 0509c06 + the temp mesh_gold target removed). UNVERIFIED: the gates
(the build exit 0 NOT achieved - the compile succeeded, the LINK blocked),
ctest, alive, the golden compare (the baseline recorded: mesh_result 508B
sha256 4E700683...A814), the mutation. THE RECOVERY (the 321 pattern): the
human clears the lock, then a gate-recovery step reruns the gates, the golden
compare, the mutation (the triangles==0 reject drop -> checkMeshLoad FAIL),
and flips 330 stopped -> completed. THE MEASUREMENTS: the baseline recorded
(pe_core 194 include lines / ~939ms first-run; the probes unreliable in the
harness); the AFTER measurements DEFERRED.

***STEP 331 (2026-10-17): THE 330 GATE-RECOVERY - ALL GATES VERIFIED GREEN, BUT
THE COMPLETED-FLIP IS BLOCKED BY THE INCLUDE VERDICT (STOP per the 331
addition; the revert-or-keep decision is Cyril's).*** THE LOCK DIAGNOSIS (no
blind kills): Get-Process across the 11 target/tool names found NO holders and
ALL exes open ReadWrite+ShareNone (PureEngine, Pong, Platformer, PureEditor0,
core_link_probe, hostile_data_test, time_test, math_test; mesh_gold.exe absent
as expected) - the manual clear HELD, nothing killed. THE AFTER MEASUREMENT
(lock-independent, FIRST): pe_core 195 include lines vs the 194 baseline = +1;
the probes 338 lines captured post-split with NO valid pre-split baseline - the
330 probe measurement ran NONEXISTENT targets (probe_a/probe_b are TUs of
core_link_probe, not CMake targets; the 330 "0 lines" were failed invocations) -
disclosed as a 330 measurement error. THE +1 DIAGNOSED: the stream subtree still
reaches pe_core via 8 other header-only carriers (hostile_data.h :16-19, input.h
:63-65, shader.h :29-31, tilemap.h :40-44, animation_data.h :29-33, resources.h
:80, renderer.h :63, ui.h :50; audio.h :87) - the net delta = -0 + 1 (the new
mesh3d_obj.h node) = +1 exactly; the mesh3d split's include payoff for the
all-16 TUs is NIL. THE GATES: the full Release build exit 0 (62.4s, 20
artifacts, LNK1104 = 0, all 11 targets linked); the golden byte-identity EXACT
(mesh_gold from the repo root: 60 floats, size = 508, sha256
4E700683DA1AB1F6D6B3D88330CE0F07EE9D4D906DFE47EE34295324B6E5A814 = the 330
baseline; the fixture self-removed); ctest 8/8 (26.00s, and 8/8 again
post-restore 15.01s); the mutation NON-VACUOUS with the named asserts - but NOT
the planned one: the triangles==0 reject is pinned by NO test (checkMeshLoad
read in full + all 20 loader call sites: no zero-face fixture exists; the test
gap recorded) - the executed EQUIVALENT dropped the per-vertex UV push_back
pair from mesh3d_obj.cpp -> "4 faces must expand to 60 floats, got 36" + "one
good face must expand to 15 floats, got 9" (direct run exit 1) -> pathspec
restore -> REBUILD -> ctest 8/8; alive x3 True (the $null -ne $p probe, stopped
in finally, WaitForExit). TIMES: UNMEASURED (the one-shot touch medians are not
comparable across the rebuild scopes). 330 STAYS STOPPED until Cyril rules:
REVERT the split, or KEEP it as verified-correct decoupling with the honestly-
recorded nil measured payoff.

***STEP 332 (2026-10-17): THE 330 STOP RESOLVED - Cyril ruled KEEP; 330 is
COMPLETED AS DECOUPLING (the gates verified by 331; the include payoff NIL
(pe_core 194 -> 195); the compile time UNMEASURED).*** METHODOLOGY NOTE
(recorded for every future split decision): the include fan-out must be
measured TRANSITIVELY with /showIncludes (the full note tree of the touched
TU), never by counting direct includes - the direct-include reasoning alone
would have missed the 8 other carriers that keep the stream subtree alive
for pe_core. QUEUED (compile-only, NOT executed in 332): re-measure
pe_core /showIncludes vs a pre-323 baseline. THE 332 WORK: the zero-face
OBJ-rejection PIN (the 331-identified gap) - the smallest fixture (verts
only, zero usable faces) asserts false + out untouched (the sentinel size
preserved) with a fixture-writable guard; commit 2aea156 (tests-only, +17);
gates: the build exit 0, the direct run exit 0, ctest 8/8 (43.08s); the
mutation landed as the positions-form surgical disable -> the NEW assert
failed BY NAME and NO OTHER ("zero-face file must fail and leave out
untouched", exit 1) -> pathspec restore -> REBUILD -> ctest 8/8 (16.40s);
the golden re-check EXACT (508B, the 330/331 baseline sha256); alive x3
True. THE STRUCTURAL FINDING (the mutation evidence): the wholesale reject
drop co-fails BOTH missing-file pins - the probe loop's in.clear() after
every failed open neutralizes the :31 "if (!in) return false;" guard (DEAD
CODE) so the missing-file failure is IMPLEMENTED BY THE REJECT (a
never-opened stream zero-parses -> triangles==0); the eof() surgical form
also co-fails (the sentry's skipws peek sets eofbit on the unattached
filebuf too) - only the parse result (positions.size()) separates the
paths. THE CONTRACT IS UNCHANGED: false + out untouched for BOTH paths.

***STEP 333 (2026-10-18): THE PE_CORE COMPILE-TIME / INCLUDE AUDIT (measure +
docs only; the queued 332 item executed).*** THE BASELINE IDENTITY: CORE-323
= `8aa07e5` (the scene_persist split - the FIRST header migration touching
pe_core's include tree; then 324 prefab, 330 mesh3d); the BASELINE = its
parent `f3164aa` (DOCS-322). Exported with `git archive --format=zip` into
%TEMP%\pe_baseline333 (OUTSIDE the repo; no checkout/stash/worktree in the
live repo); the identity checks passed (no scene_persist.h/prefab_persist.h/
mesh3d_obj.h in the export); the baseline configured + pe_core built exit 0
on attempt 1 (the FETCHCONTENT_SOURCE_DIR_GLFW/_MINIAUDIO/_STB overrides
reuse the live _deps - no network). THE METHOD (stated before measuring):
the same machine, the same Release config + generator + compiler; the
pe_core-TARGET isolation (the static-lib archive only - no exe link, no
tests); the FORCED REBUILD per trial by deleting THAT TU's .obj (only that
TU recompiles, so every /showIncludes note belongs to it); N=5 per TU per
side; the same CL=/showIncludes on BOTH sides; src/ and CMakeLists.txt in
the live repo untouched. THE PER-TU TABLE (the samples ms; the median; the
range; the unique-note count, constant across all 5 trials per TU):
BASELINE pe_core.cpp 17202/10108/7483/7107/6236 med 7483 rng 10966 count
190; HEAD pe_core.cpp 19156/8199/9460/8135/14726 med 9460 rng 11021 count
195 (193 unique); BASELINE stb_impl.cpp med 4540 rng 381 count 40; HEAD
stb_impl.cpp med 7486 rng 8802 count 40; BASELINE glad_gl.c med 2701 rng
854 count 24; HEAD glad_gl.c med 9302 rng 13251 count 24; HEAD-only TUs:
scene_persist.cpp 157 notes, mesh3d_obj.cpp 128 notes. THE INCLUDE VERDICT
(exact): pe_core.cpp 190 -> 195 raw (+5) / 190 -> 193 unique (+3 = EXACTLY
scene_persist.h + prefab_persist.h + mesh3d_obj.h); the SYSTEM-header
subtree BIT-IDENTICAL between the sides (zero files added or removed - the
removed stream includes were REDUNDANT CARRIERS; the 331 structural finding
now proven at the file level); the controls stb_impl 40==40 and glad_gl
24==24 (the harness validated). THE TIME VERDICT: UNMEASURABLE - the
run-to-run ranges (381-13251ms) dwarf every median difference, and the
CONTROL TUs on IDENTICAL sources show median gaps up to 6.6s (glad_gl 2701
vs 9302; stb_impl 4540 vs 7486) - the machine noise dominates any code
effect; the first trial per TU trends high (17.2s/19.2s pe_core) and was
NOT discarded (all samples reported per the method). THE OVERALL VERDICT:
NO DEMONSTRATED IMPROVEMENT for pe_core - the split value is the structural
decoupling (mesh3d.h/scene.h/prefab.h no longer FORCE the I/O includes on
their consumers; the I/O cost now lives ONCE in the _persist/_obj TUs), not
a compile-cost win on this machine. No claims beyond the numbers.

***STEP 334 (2026-10-18): THE LIT OVERLOAD VS THE FALLBACK - DISTINGUISHABLE
AND NOW PINNED (PIN 8).*** THE READ (file:line): the lit overload
renderer.h :408-420 - the litProgram==0 guard :412-416 falls back to the
UNLIT path (the internal ignores the lights pointer :452 - the fallback is
light-INDEPENDENT); the lit path :417-419 runs setLightUniforms :423-446
(the ambient color/intensity + the 4-light pos/color/radius/intensity)
through the lit program; lit.frag: FragColor = texel.rgb * color *
clamp(ambient.color*ambient.intensity + Sum lightColor*(intensity/(1+d^2/
r^2)), 0, 1), d = distance(vWorldPos, u_lightPos.xy), vWorldPos =
u_entityWorldPos + aPos.xy (lit.vert; the per-entity uniform :616-627, the
faf1052 world-space fix); the shipped assets/shaders/lit.vert+lit.frag
LOAD headless (the ../assets/shaders probe resolves from the ctest CWD; no
load diagnostics in any direct run) - PIN 7's fallback-or-own ambiguity
(327) RESOLVED: the REAL lit path is what runs. THE 3D PINS ARE A DIFFERENT
MECHANISM: the CPU per-face directional formula 0.35 + 0.65*diffuse pinned
by the consumer check (:6368-6487), the Step 260 world-space regression
(:6638-6685), the expected-value checks (:6951-6972) and the measured
+X-face brightness (:8172-8240) - none touch the 2D lit shader. THE PIN
(commit a660656, tests-only +41): checkDraw2DContract PIN 8 - (a) the
unlit reference (the plain overload, the PIN 7 fixture) keeps texture
content at the shared grid (deterministic: PIN 1 proves texel.r > eps at
the same probes for the same position/scale/texture); (b) the lit-dark
contrast (ambient (0,0,0)/0.0 + no lights) must render EVERY probe black -
texel-free (rgb*0 = 0; any alpha lands black over the black clear).
THE MUTATION (the committed tree): the overload forced to the fallback
(:412 -> if (true); the anchor included the overload signature - the guard
text appears at BOTH :317 and :412) -> the NEW assert failed BY NAME and
NO OTHER: "draw2d: the lit pass with zero ambient and no lights must
render the entity black (the lighting math must differ from the unlit
path)" (the mutated direct run exit 1; the unlit reference, PIN 7 and the
PINs 1-6 all passed - the forced fallback still draws); pathspec restore ->
REBUILD -> ctest 8/8 (8.98s); alive x3 True. THE CONTRACT IS UNCHANGED: the
fallback semantics (:412-416) stay the documented degradation for a
missing lit program.

***STEP 335 (2026-10-18): THE RENDERER PARTIAL-INIT TEARDOWN (destroyAll)
AUDIT + PIN.*** (The SECOND Step-334 prompt; recorded as tracker id 335 -
id 334 = the lit-vs-fallback pin from the first 334 prompt.) THE READ
(file:line): init() acquires in order - the program (:149), the transform/
color uniforms (:158/:164), the world VAO/VBO (:181-182), the checker
(:202), the 5 entity textures (:218-222), the id5 register proof (:230),
the font atlas (:248), the text VAO/VBO (:262-263), the aabb VAO/VBO
(:288-289), the tex uniform check (:305), the lit program (:316, NON-fatal
- the degradation warning :318); EVERY failure path runs destroyAll()
before return false (:151/:161/:167/:205/:225/:233/:251/:308);
destroyAll() (:1218-1241) frees textVAO/VBO, aabbVAO/VBO, worldVAO/VBO,
shaderProgram, litProgram, fontTexture, every entityTexture,
checkerTexture - then zeroes EVERY name (+ the lit locs to -1 +
entityTextures.clear()). THE CONTRACT (in-source, no ambiguity): :1214-
1217 "All names default to 0 and GL delete calls on 0 are no-ops, so this
is safe at ANY point of a partial init" + the TEARDOWN line :1162-1165
("safe at ANY point of a partial init (with a live context...)"). THE GAP
(2): the Renderer has NO destructor (the success-path objects rely on the
context teardown), and NO test reached ANY failure path - destroyAll was
NEVER executed by the suite (the 328 classification was read-based). THE
PIN (commit c674b59, tests-only +73): checkPartialInitTeardown - the
fixture is a 3-LEVEL-DEEP temp CWD (<tmp>/pe_partial_teardown/d1/d2/d3)
whose assets/ holds ONLY shaders/ + checker.png, so the loader 3-candidate
list (assets/, ../assets/, ../../assets/) can NEVER escape the fixture
tree; init acquires the program + the world VAO/VBO + the checker, pushes
FIVE zeroed entity slots, fails at the :223 quality check, runs
destroyAll (:225), returns false. The asserts: init must fail;
textureCount() == 0 after the failed init (the registry reset - the public
observable of the teardown); the SAME object re-initializes cleanly with
the 6 core textures once the CWD is restored (the no-corruption clause
:1166-1168 neighbor). THE MUTATION (the committed tree): the :225
destroyAll() call removed -> textureCount()==5 (the zeroed slots stay) ->
the NEW assert failed BY NAME and NO OTHER: "partialinit: after a failed
init the texture registry must be empty (the failure path must run
destroyAll)" (the mutated direct run exit 1; the init-failure assert, the
re-init asserts and the whole suite passed); pathspec restore -> REBUILD
-> ctest 8/8 (9.73s); alive x3 True. THE LIMITS (honest): the GL-name
freeing itself is NOT publicly observable (the handles are private; the GL
tolerates both) - the pin observes the REGISTRY-RESET consequence through
textureCount(); the other 7 failure paths stay read-verified only (the
shader/uniform/font failures are not injectable without touching src/ or
the committed assets); the never-inited case exercises NOTHING (no
destructor). THE CONTRACT IS UNCHANGED.

***STEP 336 (2026-10-18): THE FONT/DIGIT RENDERING AUDIT + THE DRAW-SIDE
LAYOUT PIN.*** THE READ (file:line): drawDigitString (renderer.h :649-725)
- the glyph quad 0.7 units, the advance 0.52 (:654-655), the digit-only
inline mapping (:675-683: the digits 0-9 -> 0-9, '.' -> 10, anything else
SKIPPED - the draw skipped, the slot kept via the index), the Step-243 UV
divisor cell/37 (:689-697), the position originX + c*0.52 with
projection*model and NO VIEW (:711-717); drawTextString (:1026-1069) -
fontCellFor consumption (:1046), the SAME skip-keep-slot rule (:1047-1049),
the alignment via alignOffsetX (:1040-1041), the same UV/position math
(:1052-1066); font.h (the boundary doc: "Anything without a glyph yields
-1: the CALLER skips the draw but still consumes the advance slot - the
established drawDigitString rule, so spaces gap correctly"); the call
sites: main.cpp (drawTextString for the stats/pause/game-over/menu
:2145-2177), ui.h (the score formatting -> drawDigitString per the Step
21 note :646-648). THE EXISTING COVERAGE (quoted): checkFontCells
(:548-578, the PURE mapping: the frozen digits/'.' cells, A-Z 11-36, the
lowercase folds, the glyph-less -> -1) and checkFontMetrics (:581-597, the
PURE width/alignment: textWidth counts EVERY character's advance,
alignOffsetX left/center) - BOTH PURE-LOGIC ONLY. THE GAP: the renderer's
x-PLACEMENT (the index-slot rule at :715/:1065) had NO pin - font.h calls
the draw path the REFERENCE for its own rule, unpinned. THE PIN (commit
ad7b779, tests-only +70): checkFontDrawLayout - draw "11" vs "1X2" ('X' =
glyph-less in the digit path: the draw skipped, the slot kept) at the same
origin; ONE glReadPixels over the glyph band; the drawn-ink CENTROID must
shift right by ~one advance (10.4 px at 40 px/unit and the 0.52 advance;
the threshold 5 px). THE MUTATION (the committed tree): the drawn-only
counter in drawDigitString (the skipped chars stop consuming their slots;
the 3 anchored edits - the guard text sits at BOTH :672 and :1042 so the
anchor carried the digit-path-unique comment, and the renderer.h has MIXED
line endings - the anchors went \r?\n-tolerant) -> '2' collapses onto slot
1 -> the centroid shift ~0 -> the NEW assert failed BY NAME and NO OTHER:
"fontdraw: a glyph-less character must consume its advance slot (the ink
centroid must shift right)" (the mutated direct run exit 1; the
reference-ink asserts, the pure font checks and the whole suite passed);
pathspec restore -> REBUILD -> ctest 8/8 (13.79s); alive x3 True. THE
STEP-243 QUIRK (the digit UV divisor 11 -> 37, 9167e14): DOCUMENTED-AND-
RESTORED in-source (:689-695 states the intent) - recorded as the known
intent per the 336 additions; its pixel discriminator (the ink-extent
comparison) is deferred as fragile, NOT pinned this step. NO font
redesign; NO new API.

***STEP 337 (2026-10-19): THE DIGIT UV CELL MAPPING (37 VS THE STALE 11) -
PINNED VIA THE CONTROLLED ATLAS.*** THE READ (file:line): the Step-243 UV
math in drawDigitString :696-697 (u0/u1 = cell/37, cell+1/37; the IDENTICAL
lines in drawTextString :1052-1053); FONT_TEXT_CELL_COUNT = 37 (renderer.h
:1264) and the ATLAS ON DISK = 592x16 = EXACTLY 37x16px cells (the PNG IHDR
read directly); the loader = stbi (resources.h: the declarations in
<stb_image.h>, the ONE implementation in stb_impl.cpp - the test TU already
compiles resources.h, so stbi_load is callable); the sampling CLAMP_TO_EDGE
+ LINEAR. THE REAL-ATLAS COVERAGE DESIGN WAS PRE-COMPUTED AND REJECTED
HONESTLY (the addition's fallback): decoding font_digits.png and comparing
each digit's correct-cell ink density vs its stale-divisor strip density
gives the BEST ratio 1.66 (digit 1) across all 10 digits - the 5x7 glyphs
have similar ink densities, NO digit clears a 2x margin -> per the addition
that design would be BLOCKED. THE CONTROLLED ATLAS (the addition's
preferred design) IS FULLY VIABLE: the fixture CWD (the 335 pattern,
3-level-deep so the 3-candidate probe cannot escape) provides a GENERATED
font_digits.png - TGA BYTES written under the .png name (stbi sniffs the
file CONTENT, not the extension): 592x16, the cells 0-9 AND 25 SOLID white
BGRA, the rest fully transparent. THE DISCRIMINATOR: under the CORRECT
divisor digit 7 samples its OWN cell 7 (solid) -> the quad renders ~784 of
784 px; under the STALE divisor digit 7 samples [7/11,8/11] x 592 =
[376.7,430.5] = the mostly-EMPTY cells 23.5-26.9 (only the 16 source px of
the solid cell 25 inside) -> ~233 px. THE MARGIN IS ASSERTED FIRST from the
atlas arithmetic (784 >= 2x 233 = 3.36x) - the fixture proves it can tell
the mappings apart before it tests anything. THE PIN (commit 569cbe0,
tests-only +123): checkDigitUvAtlas - (a) the digit-0 CONTROL must render
its solid cell (>= 700 px) under BOTH mappings (its stale strip [0,54] is
all-solid cells - the mutation cannot co-fail it); (b) THE PIN: digit 7
must fill the quad (>= 700). THE MUTATION (the committed tree): the divisor
37 -> 11 at BOTH draw paths (the u0/u1 lines are identical pairs, one
contract) -> digit 7's quad = the mostly-empty strip -> the NEW assert
failed BY NAME and NO OTHER: "fontatlas: digit 7 must sample its own
37-cell slot (the solid cell must fill the quad)" (the mutated direct run
exit 1; the digit-0 control, the margin pre-assert and the whole suite
passed); pathspec restore -> REBUILD -> ctest 8/8 (28.89s); alive x3 True.
NO font redesign; NO new API; the committed assets untouched (the
controlled atlas lives in the temp fixture CWD only).

***STEP 338 (2026-10-19): THE RENDER CATEGORY ACCEPTANCE AUDIT (evidence +
recommendation only; docs; the tracker id 338).*** THE CRITERIA SOURCE: the
326 draft A-I (V3 :4028-4044, recovered verbatim from disk) - PROPOSED,
NEVER approved by Cyril. Per the 338 additions: the status vocabulary is
NOT AUDITED / IN PROGRESS / BLOCKED / VERIFICATION REQUIRED / ACCEPTED;
this audit maps the EVIDENCE onto the proposed criteria and proposes
wording edits; it does NOT self-approve anything.

THE A-I EVIDENCE TABLE (the criterion: the evidence grade + the citations):
A. the build exit 0 (staged) - PROVEN: the full Release build exit 0 at
   every slice gate (327-337); the 338-turn build exit 0 (338.2s, LNK1104=0,
   19 artifacts). WORDING PROPOSAL: "(staged)" is ambiguous - the gates ran
   against the COMMITS; propose "(against the committed tree, before the
   docs record)".
B. ctest green, the count grows additively - PROVEN IN INTENT, WORDING
   DRIFT: ctest 8/8 at every gate; the render-contract coverage grew +189/
   +44/+41/+73/+70/+123 test lines (327/329/334/335/336/337) INSIDE the
   existing hostile_data_test - NO new "render-contract target" was created
   (the draft literal wording). The intent (the additive headless coverage
   of drawWorld/drawEntity3D) is met; propose "(inside the existing harness
   or a new target)". NO timing asserts anywhere in the Render pins.
C. the live smoke: alive x3 + the Cyril visual checklist - PARTIAL (the
   ONLY criterion not fully evidenced): alive x3 True at every gate (the
   automated half); the CYRIL VISUAL CHECKLIST for the Render migration is
   NOT RECORDED - the recorded 318 smoke line (HEAD 8137a23) predates ALL
   Render pins (327+). The SMOKE_TEST section 7 rows 7.4-7.12 (the GL-only
   gate: the OOB checker visual, the blend, the tint, the debug3d mesh/depth/
   drop, nohostiles) remain HUMAN-ONLY. THIS CRITERION HOLDS THE CATEGORY
   AT VERIFICATION REQUIRED.
D. the structure - PROVEN: mesh3d = the ONLY SPLIT-JUSTIFIED boundary and
   the ONLY split (mesh3d_obj.h/.cpp, mesh3d.h 196 -> 108, 330); the ODR
   grep 0 (the 326 sweep) held - core_link_probe green at every gate; the
   renderer API: ZERO changes beyond the declared moves (the 327-337 src
   diffs = the mutation cycles only, all restored by pathspec).
E. the include hygiene - PROVEN: mesh3d.h LOST its 3 stream includes to
   mesh3d_obj.cpp (330); no render header gained a heavy include (the
   327-337 src diffs = the tests); the 333 audit: pe_core's system-header
   subtree BIT-IDENTICAL across all the migrations.
F. the consumer linkage - PROVEN: mesh3d_obj.cpp linked into PureEngine,
   hostile_data_test, pe_core (the loader callers); the 330 temp mesh_gold
   needed the same link (the 323 LNK2019 lesson re-applied at the temp
   target).
G. the contracts pinned before any move - PROVEN: the mesh contracts
   (checkMeshLoad :7608 etc.) predate the 330 move; every NEW Render pin
   landed tests-first with the gates against the test commit
   (327/334/335/336/337).
H. the behavior proof - PROVEN: the moved loader's golden (mesh_gold,
   508B sha256 4E700683...A814) re-verified byte-identical at 331/332/337;
   checkMeshLoad's exact asserts over the moved body. WORDING PROPOSAL:
   the golden is a DATA dump, not a frame compare - propose "(the data-dump
   goldens count; no pixel golden unless Cyril asks)".
I. measurement not budget - PROVEN: no timing asserts in any Render test;
   the bench runs recorded as DATA (the 333 audit: the times UNMEASURABLE
   per the noise rule - recorded, not acted on).

THE COVERAGE ROWS (the 338 prompt's minimum set, beyond A-I):
- the 2D draw/order - PROVEN: PIN 1 (the depth sort; the mutation 327),
  PIN 3 (the dead skip; the mutation 329-C), PIN 4 (the meshId skip; the
  mutation 329-B), PIN 5 (the blend/depth restore; the mutation 329-D).
- depth/occlusion - PROVEN: checkDebugFrameDepth/checkDepthState/
  checkMeshDepthState/checkMeshOcclusion (the pre-327 pins the 326 audit
  cited at :4007-4012).
- lit vs unlit - PROVEN: PIN 7 (the callable; the 334 resolution - the
  REAL lit path runs, the shipped assets/shaders load headless) + PIN 8
  (the lit-dark vs the unlit contrast; the 334 mutation: the forced
  fallback -> single-assert by name).
- mesh registry/UV/textureId - PROVEN: checkMeshRegistry/checkMeshHandle/
  checkMeshIdParse/checkMeshStateLeak (the registry), checkMeshLoad (the
  UV bounds + the per-face planar scheme), PIN 6 (the OOB fallback) + the
  329 renderer-backed textureRegistry (the release/refuse contracts).
- partial-init destroyAll - PROVEN WITH LIMITS (the 335 pin: the checker-
  only fixture -> init false + textureCount()==0 + the re-init recovery;
  the mutation single-assert; THE LIMITS: 7 of 8 failure paths read-only;
  the GL-name freeing unobservable through the public API).
- font layout + digit UV - PROVEN: checkFontDrawLayout (the 336 centroid-
  shift skip rule) + checkDigitUvAtlas (the 337 controlled-atlas UV
  mapping; the margin asserted first).
- resource/load contracts - PROVEN: resources_test (the dedicated target)
  + checkTextureRegistry/checkResourceSystem (the 326-audit classification
  held).

THE LIMITS TABLE (each with its evidence; NONE approved - the approval
lines stay blank for Cyril):
L1. The live pixel-perfect smoke is HUMAN-ONLY (SMOKE_TEST 7.4-7.12; the
    criterion C) - the build/ctest can never close it.
    Evidence: SMOKE_TEST.md section 7; the 327/336 STATUS lines.
    Approved by Cyril: __
L2. 7 of 8 init failure paths are READ-ONLY verified (not injectable
    without touching src/ or the committed assets).
    Evidence: the 335 record; the init failure map :151-:308.
    Approved by Cyril: __
L3. The destroyAll GL-name freeing is not publicly observable (the private
    handles; the GL tolerates both) - the 335 pin observes the
    registry-reset consequence through textureCount().
    Evidence: the 335 record; the destroyAll map :1218-1241.
    Approved by Cyril: __
L4. PIN 6 (the OOB textureId) is "something drew" BY DESIGN - the
    diagnostic is NDEBUG-gated (silent in Release, no flake).
    Evidence: the 327 record, PIN 6.
    Approved by Cyril: __
L5. The compile-time: NO DEMONSTRATED IMPROVEMENT for pe_core (the times
    UNMEASURABLE per the noise rule; the include delta +3 unique files =
    the 3 shims).
    Evidence: the 333 audit (the per-TU table + the control TUs).
    Approved by Cyril: __
L6. The mesh3d include payoff is nil for the all-16-header TUs (the stream
    subtree survives via 8 other carriers; the system subtree
    bit-identical).
    Evidence: the 331 include verdict; the 333 file-level proof.
    Approved by Cyril: __
L7. The point-light attenuation geometry (the d^2/r^2 formula, the light
    positions) is unpinned headless - the zero-light and callable
    contracts are pinned (334); the light math runs only in the live game.
    Evidence: the 334 record; assets/shaders/lit.frag.
    Approved by Cyril: __
L8. The renderer.h carries MIXED line endings (CRLF/LF per-region) - a
    maintenance hazard for the literal anchors (the 336 mutation needed
    the tolerant regexes).
    Evidence: the 336 mutation record (the :719-722 LF region).
    Approved by Cyril: __

THE RECOMMENDATION: **VERIFICATION REQUIRED** (the 338 addition mandates
this status until the human smoke passes). THE REMAINING BLOCKERS to a
future ACCEPTED: (1) the Cyril visual checklist recorded (the criterion C:
the 2D frame, the 3D depth discipline, the HUD digits - the SMOKE_TEST 7
rows); (2) the A-I criteria formally APPROVED by Cyril (with the wording
edits proposed at A/B/H above). THE EVIDENCE BASE IS COMPLETE for every
criterion except C; NO criterion is UNVERIFIED; the limits L1-L8 carry
explicitly. The Render category status moves: IN PROGRESS -> VERIFICATION
REQUIRED. The next Render work (if any) opens only on Cyril's prompt.

***STEP 339 (2026-10-19): THE DEBUG3D DROP "HUGE BOX" CLASSIFIED (inspect
only; NO code fix).*** THE TRIGGER (Cyril smoke 2026-10-19): debug3d drop ->
the lower/ground box becomes huge and covers almost the whole screen; the
falling-box interaction hard to judge. THE TRACE (file:line): the drop
spawner = main.cpp :1144-1180 (dropSandbox, the Step 231 shared lambda) -
ground: position (0,-3,0), scale (1,1,1), halfExtents (4,0.5,4),
isStatic=true, meshId=1, Sandbox role, restitution 0; box: position
(0,6,0), scale (1,1,1), halfExtents (0.5,0.5,0.5), meshId=1, restitution
0.85, gravityScale 1; both queueSpawn'd. The per-frame path: applyPhysics
(main :1865) -> resolveSandboxPairs (:1870) -> the draw. resolveSandboxPairs
(physics.h :283-307) delegates to resolveCollision for the Sandbox pairs -
the writes are POSITIONS/VELOCITIES only; physics.h touches scale/
halfExtents ONLY as reads (the collision math :107-110/:342-355/:410-463).
drawEntity3D (renderer.h :776-780): the model = translation * rotationY *
scale(halfExtents * scale * 2) - the drawn size = the entity spec
(the Step 261 sizing); the mesh = meshId 1 -> unitCubeVertices (:796-799).
THE CLASSIFICATION: **A) DRAW/CAMERA GEOMETRY - the entities never change
size.** THE EXECUTION EVIDENCE (the temporary instrumentation, restored by
pathspec before any commit; the tree verified clean): the drop replicated
headlessly (the exact dropSandbox entities + applyPhysics +
resolveSandboxPairs, 180 frames) - sizesStable=1 (the scale/halfExtents
bitwise-unchanged across every frame); the box falls 6.0 -> the bounces
(vel +8.4 at f=90) -> the oscillation; the ground never moves. THE
GEOMETRY: the DEFAULT debug camera = eye (0,0,5), target origin, FOV 60
(main :1205-1208, the Step 216 reset) - the ground's +Z face (z=4) sits at
the view depth 1.0; its half-width 4.0 vs the frustum half-width
tan(30)*1*4/3 = 0.77 -> the face is 5.2x the frustum half-width (~10.4x the
full screen width) - the 8x1x8 ground box rendered CORRECTLY at a
camera-hostile distance fills and overflows the screen; the resting box
(the top y=-2.0) = 17.3% of the frustum half-height, just above the
ground's top face - the occlusion judgment is hard because the ground's
near face fills the lower screen. THE CRITICAL QUESTION ANSWERED (from
source, pending the visual retest): the 2D sprites of the sandbox entities
CANNOT grow - meshId>0 entities are SKIPPED in the 2D batch (the 327 PIN 4);
the ONLY 2D thing that scales with them = the F1 drawAABBs overlay
(:1085-1128, the line loops, the collision-exact halfExtents*scale*2, the
player orange / the others yellow) - ONLY when F1 is toggled; the player 2D
sprite is unaffected. THE MESH IDS: meshId 1 = the HARDCODED cube (both
sandbox entities; :796-799), meshId 2 = the hardcoded pyramid (NOT set by
any engine code - only the user meshid command), meshId 13 = an
UNREGISTERED registry id -> the CUBE FALLBACK (:789-799: the registry miss
-> unitCubeVertices); NO mesh path draws the HUD (the HUD digits =
drawDigitString, the 2D path; the meshId>0 2D skip separates them) and the
2D path never draws a mesh. THE FACES: the bright/dark = the CPU per-face
directional shading (renderer.h :868-880: the fixed lightDir
(0.32,0.74,0.42) normalized, ambient 0.35 + diffuse 0.65*max(0,n.L)) - the
top faces bright, the undersides darker; NO shadow pass exists (the grep:
zero matches in renderer.h/main.cpp/physics.h/collision.h). THE BOUNDED
FIX RECOMMENDATION (ONE step, no redesign): keep the entities; make the
DROP discoverable at the default camera - EITHER the dropSandbox reply
appends the camera tip ("drop: ground@N box@N (tip: debug3d cam 0 4 14 0
-2 0)") - the least invasive (the documented cam default stays), OR the
Step 216 cam-reset default moves to a drop-suitable eye (0,4,14) /
target (0,-2,0) - a DOCUMENTED-DEFAULT change needing Cyril's call. THE
EXACT PIN for either: the headless frustum-fit assert - compute
projection*view over the 8 ground corners + the 8 box corners at the
DOCUMENTED default cam; assert every NDC z in [-1,1] and the x/y extents
within [-1,1] (the ground face must not overflow the frustum); the
mutation: the cam default reverted to (0,0,5) -> the x extents exceed 1 ->
the named assert fails; plus the reply-string assert for the tip variant.
RENDER STAYS VERIFICATION REQUIRED - the blocker moves from "the drop
unjudgeable" to "the drop camera default mis-scaled for the sandbox
geometry" (the fix = ONE bounded step on Cyril's go).

***STEP 340 (2026-10-20): THE DROP CAMERA TIP + THE FRUSTUM-FIT PIN (the
339 bounded fix, NO physics/draw/default-camera change).*** THE
IMPLEMENTATION (commit 832e807, CORE-340): the drop camera defined ONCE in
main.cpp near dropSandbox - const float dropCam[6] = { 0, 8, 18, 0, 0, 0 }
(the eye xyz + the target xyz, exactly the 6 floats the cam command
consumes; the FOV stays the Step-215 default, not part of the tip) - and
the drop reply appends the tip BUILT FROM those constants via
ostringstream: "drop: ground@N box@N (session-transient, never saved)
(tip: debug3d cam 0 8 18 0 0 0)". THE CAMERA VALUES ARE THE FIT-REFINED
339 RECOMMENDATION: the 339 draft (0,4,14)/(0,-2,0) left the box START-pose
top corners OUTSIDE the frustum (the NDC y ~1.16 - the box spawns at y=6,
ABOVE the eye 4); (0,8,18)/(0,0,0) fits all 16 corners at all 3 poses (the
max |NDC| ~0.6). THE PINS (checkDropCamFit): (1) the constants PARSED from
main.cpp source (the grep-verified-contract pattern - a constant drift
fails the build gate, not only the smoke); (2) the convention validator
(the manual column-major clip must equal transformPoint for the same
input); (3) THE FIT: every NDC of the 8 ground corners + the 8 box corners
at the start (y=6)/a mid-bounce (y=3)/the rest (y=-2) within [-1,1] with
the explicit w-divide (transformPoint assumes w=1 and is not used);
(4) THE TIP: the reply must contain the tip built from the constants
(the source-level wiring pin; the runtime text = the manual smoke).
THE MUTATIONS (on the committed tree): M1 the target y 0->3 - VACUOUS
(TWO attempts: the eye z 18->11 and the target y 0->3 both left every pose
inside - the hand-math depth errors caught by the vacuous-run discipline;
the DIAG2 empirical sweep measured the ground min-NDC-y vs the target y:
0 -> -0.452, 3 -> -0.723, 6 -> -1.058 (EXITS), 9 -> -1.481); M1 retry the
target y 0->6 (the DIAG2-measured constant) -> NON-VACUOUS. THE
CONSOLIDATION (commit f231993, CORE-340b): the 4 per-pose FIT asserts
merged into ONE named assert - the shared camera cannot break one pose
without the geometric neighbors, so the per-pose split made the literal
single-assert mutation requirement unachievable; the combined assert:
"dropcam: the sandbox geometry must fit the frustum at the documented drop
camera (the ground + the box at the start/mid-bounce/rest)". M1 retry
result: exit 1, THE FIT assert the only casualty (the TIP + the suite
passed); M2 the tip removed from the reply -> the TIP assert failed BY
NAME and NO OTHER: "dropcam: the drop reply must contain the camera tip
built from the constants" (exit 1; the FIT passed - the constants remain
defined). Each restored by pathspec -> REBUILD -> ctest 8/8 (34.87s post-
M1, 21.01s post-M2); alive x3 True. THE SMOKE_TEST 7.11 reply text synced
(the tip appended). RENDER STAYS VERIFICATION REQUIRED until Cyril
re-smokes the drop (the tip makes the re-smoke one command: type the tip
line after drop).

***STEP 341 (2026-10-20): THE RENDER CATEGORY ACCEPTED (docs only; HEAD
c748c5c).*** CYRIL'S SMOKE (Render, 2026-10-20, the Release PureEngine.exe,
recorded verbatim in the 341 prompt): all items PASS; 2D lit vs unlit = N/A
(could not judge the lighting difference in normal play); the drop used
with debug3d cam 0 8 18 0 0 0 (the Step-340 tip); bounce/occlusion OK.
THE APPROVALS (granted by sending the 341 prompt; Cyril may strike lines):
the A/B/H wording edits APPROVED - A "against the committed tree"; B
"inside the existing harness or a new target"; H "the data-dump goldens
count"; the limits L1-L8 APPROVED AS KNOWN LIMITS - each keeps its
evidence line, no stronger guarantees implied.

THE CATEGORY COMPLETION REPORT:
THE ORIGINAL GAPS: (1) the 2D drawWorld path (~190 lines) had NO headless
test - only the F-04 cassert (the 326 audit); (2) the 2D pins unvalidated
(no mutation evidence); (3) the texture-release contracts were a
local-vector simulation (checkTextureRegistry never touched the
renderer); (4) the lit overload fallback-or-own ambiguity (PIN 7 could
not tell the paths); (5) the destroyAll partial-init contract documented
but never executed; (6) the font draw-side layout unpinned (the pure
checks covered only the mapping/width math); (7) the digit UV cell
mapping (the Step-243 fix) documented, not execution-proven; (8) the
zero-face OBJ rejection unpinned (the 331 discovery); (9) the debug3d
drop sandbox unjudgeable at the default camera (the 339/340 blocker).

THE CHANGES MADE (the commits): 327 the 2D drawWorld pins PINs 1-7
(46d97c6, the texel-free channel isolation); 329 the mutations A-D
validated PINs 2/4/3/5 + the releaseTexture gap closed through the public
API (0aa719e); 330 the mesh3d OBJ-loader split (0509c06 + the fixes);
331 the gate recovery (the lock cleared + the golden byte-identity
verified); 332 the zero-face OBJ pin + the equivalent mutation (2aea156);
333 the compile-time audit (the NO DEMONSTRATED IMPROVEMENT verdict, the
limits recorded as data) (4adf367-era); 334 the lit-vs-fallback
distinguishability pin (a660656, PIN 8); 335 the partial-init destroyAll
pin (c674b59); 336 the font draw-side layout pin (ad7b779); 337 the digit
UV controlled-atlas pin (569cbe0); 338 the acceptance audit (4adf367);
339 the drop huge-box classified (5bc3ff6); 340 the drop camera tip + the
frustum-fit pin (832e807/f231993/c748c5c).

THE FILES AFFECTED: tests/hostile_data_test.cpp (checkDraw2DContract
PINs 1-8, checkTextureRegistry's renderer-backed section, checkPartial
InitTeardown, checkFontDrawLayout, checkDigitUvAtlas, checkDropCamFit);
src/mesh3d.h (196 -> 108) + src/mesh3d_obj.h/.cpp (the split); src/main.cpp
(the drop camera constants + the reply tip); Blueprint/SMOKE_TEST.md (the
7.11 reply synced).

THE PINS WITH THE MUTATION EVIDENCE (the commit + the failing line):
- PIN 1 the depth sort (327, 46d97c6): the comparator flip -> "draw2d:
  the higher depth must win the overlap (the red-colliding A covers the
  green B)".
- PINs 2/3/4/5 (329, 0aa719e): the mutations A-D - the collision-override
  bypass -> PIN 2 (+ the co-failing PIN 1 as designed); the meshId-skip
  bypass -> PIN 4; the dead-skip bypass -> PIN 3; the blend/depth-restore
  bypass -> PIN 5 (the exact lines quoted in the 329 record).
- PIN 6 the OOB fallback (327): the something-drew assert BY DESIGN (the
  L4 limit; no mutation run - the diagnostic is NDEBUG-gated, the mutation
  would be vacuous by construction).
- PINs 7/8 the lit callable + the distinguishability (334, a660656): the
  forced-fallback mutation -> "draw2d: the lit pass with zero ambient and
  no lights must render the entity black (the lighting math must differ
  from the unlit path)".
- the texture release (329, 0aa719e): the renderer-backed release/refuse
  contracts (the counts) - exercised by the same-draw-path mutations A-D.
- the partial-init destroyAll (335, c674b59): the destroyAll removed from
  the :225 failure path -> "partialinit: after a failed init the texture
  registry must be empty (the failure path must run destroyAll)".
- the font layout (336, ad7b779): the drawn-only counter -> "fontdraw: a
  glyph-less character must consume its advance slot (the ink centroid
  must shift right)".
- the digit UV (337, 569cbe0): the divisor 37 -> 11 -> "fontatlas: digit 7
  must sample its own 37-cell slot (the solid cell must fill the quad)".
- the zero-face OBJ (332, 2aea156): the UV-pair drop -> "4 faces must
  expand to 60 floats, got 36" + "one good face must expand to 15 floats,
  got 9".
- the drop camera fit/tip (340, 832e807/f231993): the target y 0->6 ->
  "dropcam: the sandbox geometry must fit the frustum at the documented
  drop camera (the ground + the box at the start/mid-bounce/rest)"; the
  tip removed -> "dropcam: the drop reply must contain the camera tip
  built from the constants".

THE REMAINING LIMITS (the approved L1-L8): L1 the live smoke HUMAN-ONLY
(now PASS, 2026-10-20 - the evidence recorded above); L2 7 of 8 init
failure paths read-verified only; L3 the destroyAll GL-name freeing
unobservable through the public API; L4 PIN 6 something-drew by design;
L5 the compile-time NO DEMONSTRATED IMPROVEMENT for pe_core; L6 the
mesh3d include payoff nil for the all-16-header TUs; L7 the point-light
attenuation geometry unpinned headless; L8 the renderer.h mixed line
endings.

THE STATUS: **Render ACCEPTED (2026-10-20, HEAD c748c5c)** with the
approved limits L1-L8 listed above. THE FREEZE RULE: Render reopens only
on a measured regression (the same as Core Foundation and Entity/Scene/
Persistence). The next category opens only on Cyril's explicit prompt.

### Category backlog brief (Step 342, 2026-10-20, docs only - INSPECT, no
### implementation, no category ACCEPTED this step)

THE ACCEPTED BASELINE (frozen; the reopen only on a measured regression):
Core Foundation - ACCEPTED (319/325). Entity/Scene/Persistence - ACCEPTED
(325). Render (the draw path) - ACCEPTED (341, 2026-10-20, HEAD c748c5c;
the limits L1-L8 approved; the smoke PASS after the tip camera).

TRACK A - SIMULATION (physics.h 656, collision.h 374, simulation.h 162,
input.h 430, audio.h 470, engine_time.h 112, events.h 157):
| Area | Key headers | Tests / pins | Live consumer | Gaps | Risk |
| --- | --- | --- | --- | --- | --- |
| gravity/force/integrate | physics.h :41-55 | checkApplyForceMath, checkApplyPhysicsSemantics, checkApplyPhysicsFixedFreeMovers | main :1865 applyPhysics | TWO integration variants (applyPhysics vs applyPhysicsFixed :632) - the FIXED variant is test-only today (no production caller); the drift risk between them | medium |
| AABB/resolve | collision.h :83-180, physics.h resolveCollision | checkStaticFloorLanding..checkResolveEdgeTouch (7), checkRestitution, checkBounceRestHeights, checkMassWeighting | the arcade loop, the sandbox | RESTING_VEL scope: 0.5f threshold lives ONLY in resolveSandboxPairs (physics.h :304); Pong's default call passes 0 (the documented split) | low |
| swept/character | physics.h sweptMoveAndCollide :333, the character controller | checkSweptAABBContract, checkSweptControllerContract, checkSweptFixedContract, checkSweptMoveAndCollide, checkKinematicCarry/Resolve, checkFixedJump/Substep | platformer | the swept contract fully pinned; the platformer consumes | low |
| broadphase | physics.h (the grid pairs) | checkBroadphaseGridPairs, checkFixedTimestepNoTunnel | main (the Step-52 gate) | the Step-52 ruling: fine at <=2000 entities; the reopen ONLY on a measured superlinear breach | low (frozen by measurement) |
| sandbox resolve | physics.h :283-307 | checkSandboxResolve, checkBounceTunneling, checkDropPhysics | the debug3d drop (main :1144-1180) | the RESTING_VEL magic constant documented in-place (the 238 note) | low |
| input | input.h :83-404 | checkInputEdges, checkMouseInput, checkGamepadDeadzone/Buttons/Edge/PollSafety, checkActionMap, checkKeyNames, checkKeysForAllActions, checkGamepadActions, checkInputAdoptionHelper, checkInputBindings | main (the actions + the mouse + the pad) | the GLFW window paths need a live window (the headless checks use the Input adoption helper + the parse contracts); the gamepad DEVICE polling human-only | medium |
| audio | audio.h :124-422 | checkVolumeClamp, checkMuteToggle, checkPerSoundVolume, checkMusicVolumeIndependence, checkPreInitGuards, checkAudioVolumeMatrix, checkAudioDeviceLifecycle, checkAudioPoolRotation | main (the sfx + the music loop) | the miniaudio DEVICE playback = human-only (the volume matrix + the lifecycle headless-pinned); the music loop end-to-end unproven in CI | medium |
| time | engine_time.h :68-99 | checkTimeContract, checkTimeScale | main :1512 tick | the dt clamp/scale/pause pinned (318) | low |
| events | events.h :65-143 | checkEventsOrderAndPayload, checkEventsUnsubscribe, checkEventsEdgeCases, checkEventThrowAndOnce, checkEventReentrantOnceGaps | main (the collision events) | the reentrancy + the once-gaps pinned | low |

TRACK B - WORLD/CONTENT (tilemap.h 358, scene.h 279, prefab.h 88,
lifecycle.h 158, hostile_data.h 187):
| Area | Key headers | Tests / pins | Live consumer | Gaps | Risk |
| --- | --- | --- | --- | --- | --- |
| tilemap load/collide | tilemap.h :70-299 | checkTilemapValidLoad, checkTilemapMalformedFallsBack, checkTilemapCollidePushesOut, checkTileResolve | the arcade level (the tilemap default) | the tilemap is 2D-only - no 3D interaction contract; the malformed fallback pinned | low |
| scene/manager | scene.h :67-261 (the persistence in scene_persist) | checkSceneLifecycle, checkSceneNoCurrentNoOps, checkSceneByName, checkSpawnQueue, checkSceneDumpReload, checkSceneManagerSave/RoundTrip | main (the two scenes) | the Entity/Scene/Persistence ACCEPTED (325) - the format pins held | low (frozen) |
| prefab | prefab.h :56 (+ prefab_persist) | checkPrefabSystem, checkPrefabSceneSpawn, checkPrefabLiveSpawn, checkPrefabMultiSpawnPersist, checkPrefabSceneFailureMatrix | the spawn command | the strict v1 header check pinned; the versioning = reject-only (no migration path by design) | low |
| lifecycle/roles | lifecycle.h :76-152 | checkEntityLifecycle, checkSpawnAfterKill, checkNohostilesParse/Kill, checkHierarchy* | main (the role scoping) | the role IDs as ints (the ArcadeRole cast) - the type safety is the accepted design | low |
| hostile data | hostile_data.h :31-73 | checkCaseValidData..checkCaseMissingFile (5), the defaults load | the hostile defaults | the malformed warn paths pinned; the stress files (500/2000/5000) exercised via the stress test only | low |
| platformer/pong (CONSUMERS ONLY) | games/ | checkPlatformerLevels/Landing/LevelSwitch/Climb/GoalEvent, checkPongScore, checkPongSustainedContact | the games | the games = the consumers; no expansion per the standing priority | low |

TRACK C - GRAPHICS ASSETS (mesh3d.h 108, mesh3d_obj.h/.cpp, the renderer
mesh registry, the texture slots, resources.h 197) - the DRAW PATH IS
RENDER-ACCEPTED; this track = the asset-load/registry surfaces ONLY:
| Area | Key headers | Tests / pins | Live consumer | Gaps | Risk |
| --- | --- | --- | --- | --- | --- |
| OBJ loader | mesh3d.h :46-73 (the builders), mesh3d_obj.cpp (the loader) | checkMeshLoad (the 60-float expand, the UV bounds, the zero-face), checkWedgeMesh (the registered real asset) | meshid 3/5 (the debug3d binds) | the OBJ subset: the vn/vt tags partially supported, the quads unsupported (the 328 note) - the loader is a triangle-soup subset BY DESIGN; the bigger models untested | medium |
| mesh registry | renderer.h meshRegistry_ :751 | checkMeshRegistry, checkMeshHandle, checkTexturedMesh, checkMeshTextureId | meshid >= 3 | no eviction (the growable map by design); the unregistered ids fall back to the cube | low |
| texture slots | renderer.h (the registry, the release) | checkTextureRegistry (the renderer-backed section), checkMeshStateLeak | the 6 core + the non-core | Render-ACCEPTED (the release/refuse pinned); the asset-API view = the same contracts | low (frozen) |
| blob/pack/cache | resources.h :151-185 | checkBinaryBlob, checkResourceSystem, resources_test (the dedicated target) | the audio + the future asset packs | the pack v1 hit/fallback pinned; the cache = the process-lifetime single-threaded (the documented limit) | low |

TRACK D - EDITOR/TOOLS (tools/editor0/):
| Area | Key headers | Tests / pins | Live consumer | Gaps | Risk |
| --- | --- | --- | --- | --- | --- |
| Editor-0 | editor0_core.h (1398), main.cpp (3674), FINDINGS.md | checkEditorLiteKill/Spawn/Safety/Success (4 - the editor-lite logic mirrored in the harness) | PureEditor0 (its own CMake target) | the Phase-D findings live in tools/editor0/FINDINGS.md; the save-as write-safety documented; the engine src/ read-only during the Tools steps (the 284 rule) | medium |
| status | the Phase Tools option (c) per the 292 map - ALLOWED (the 284 amendment) | - | - | no editor feature work in 342; the Tools steps remain a CONSUMER phase | low |

THE ORDERED RECOMMENDATION: **A (Simulation) -> B (World/content) -> C
(Graphics assets) -> D (Editor/tools)**. WHY A FIRST: the largest
not-yet-audited surface (7 headers, ~2360 lines) with the highest
gameplay criticality (every interactive frame runs it), and the existing
check density (30+ physics/input/audio/time/event checks) makes the audit
cheap and evidence-rich; the TWO integration variants (applyPhysics vs
applyPhysicsFixed) and the audio device paths are the honest unknowns.
B second: the persistence-adjacent surfaces are already ACCEPTED-frozen,
so the audit narrows to the tilemap 3D interaction and the loader
versions. C third: mostly covered by the Render ACCEPTED contracts - the
audit narrows to the OBJ subset limits and the asset producers. D last:
the Tools phase needs the CONSUMER rules (the 284) and the human-gated
editor work. MUST NOT START until the A audit is ACCEPTED (or explicitly
deferred by Cyril): any B/C/D implement step (the pins/fixes/features).
**PARK: all four tracks sit in NOT AUDITED | INVENTORIED - no feature
steps, no pins, no fixes until Cyril picks the track for the next full
audit.**

### Simulation audit brief (Step 343, 2026-10-21, docs only - Track A of
### the 342 brief; INSPECT, no fixes, no pins; the status after: IN
### PROGRESS, NOT ACCEPTED)

THE SCOPE: physics.h 656, collision.h 374, simulation.h 162, input.h 430,
audio.h 470, engine_time.h 112, events.h 157 + the entity sim fields
(mass/restitution/friction/isStatic/isKinematic/velocity) as used. THE
EXCLUSIONS: the renderer draw (Render ACCEPTED), the mesh load (Track C),
the scene save format (Entity/Scene ACCEPTED), editor0.

THE INVENTORY (file:line; the API + the default contract + the tests + the
consumer + the human-only):
- gravity/force/integrate: physics.h :41-55 (applyGravity v += G*scale*dt;
  applyForce; integrate pos += v*dt). The tests: checkApplyForceMath,
  checkApplyPhysicsSemantics. The consumer: main :1879 (applyPhysics).
- applyPhysics (simulation.h :117-129, the VARIABLE-step): the gravity
  conditional on gravityScale > 0; the integration only for the nonzero
  velocity; isStatic never written. THE CONSUMER: main :1879 - the ONLY
  production integration path.
- applyPhysicsFixed (physics.h :632-656, the FIXED-substep): the entities
  batch + dt + the substep; the free movers integrated per substep.
  THE CALLERS: tests ONLY (checkApplyPhysicsFixedFreeMovers :2492-2543).
  NO production caller - see G1.
- resolveCollision (collision.h :107+, the writes = the positions/
  velocities; the scale/halfExtents read-only): the restitution/mass/
  friction handled; the tests: checkResolveRestitutionClamp,
  checkResolveBounceVsStick, checkResolveApproachingGuard,
  checkResolveEdgeTouch, checkMassWeighting, checkFriction.
- resolveSandboxPairs (physics.h :283-307): the Sandbox-role pairs ->
  resolveCollision(a, b, 0.5f, RESTING_VEL) - RESTING_VEL = 0.5f
  (constexpr :86) flows ONLY through this path (Pong's default call
  passes restingVel 0.0: any approaching speed bounces). The tests:
  checkSandboxResolve, checkBounceTunneling, checkDropPhysics,
  checkBounceRestHeights. The consumer: main :1870 (the debug3d drop).
- swept: sweptMoveAndCollide (physics.h :333+), sweptAABB - the swept
  contract (the earliest-hit clamp, the normal-axis zero, the start-inside
  delegation). useSwept = the OPT-IN PARAMETER (default false) on
  updateCharacterController (:478) and updateCharacterControllerFixed
  (:617) - NOT a global. The tests: checkSweptAABBContract,
  checkSweptControllerContract, checkSweptFixedContract,
  checkSweptMoveAndCollide (incl. the dt <= 0 guards).
- the character controller: updateCharacterController (:474, the variable)
  vs updateCharacterControllerFixed (:613, the substep loop forwarding
  useSwept per substep :627). THE CALLERS: tests ONLY (30+ call sites
  :1514-9124). NO production consumer - see G1.
- input: input.h :83-115 (the action map + keysForAction/keysForAll
  Actions), :140 (the gamepad buttons), :154 (keyNameToGLFW), :196
  (loadInputBindings + resetActionOverrides :263), :273-404 (the mouse
  state + isDown/isEdge + pollMouse + update). The tests:
  checkInputEdges, checkMouseInput, checkGamepadDeadzone/Buttons/Edge/
  PollSafety, checkActionMap, checkKeyNames, checkKeysForAllActions,
  checkGamepadActions, checkInputAdoptionHelper, checkInputBindings. The
  consumer: main (the actions + the mouse + the pad). HUMAN-ONLY: the
  DEVICE feel (the keyboard/gamepad/mouse polling needs a live window).
- audio: audio.h :124 (init/device), :211-253 (the event sounds),
  :276-333 (the volumes + mute), :352-373 (playMusicLoop/stopMusic/
  shutdown), :400-422 (clampVolume01/applyVolumes/loadNamedSound). The
  tests: checkVolumeClamp, checkMuteToggle, checkPerSoundVolume,
  checkMusicVolumeIndependence, checkPreInitGuards, checkAudioVolume
  Matrix, checkAudioDeviceLifecycle, checkAudioPoolRotation. The
  consumer: main :593+ (the music loop) + the sfx. HUMAN-ONLY: the
  DEVICE playback (the miniaudio output).
- time: engine_time.h :68-99 (start/tick/scaledTick; the 150ms stall ->
  the 0.1 clamp; the negative-scale guard). The tests: checkTimeContract,
  checkTimeScale. The consumer: main :1512.
- events: events.h :65-143 (subscribe/unsubscribe/emit/clear/once/
  handlerCount). The tests: checkEventsOrderAndPayload,
  checkEventsUnsubscribe, checkEventsEdgeCases, checkEventThrowAndOnce,
  checkEventReentrantOnceGaps. The consumer: main (the collision events).

THE EXPLICIT COVERAGES:
- applyPhysics vs applyPhysicsFixed: the variable = the ONLY production
  path (main :1879); the fixed = test-proven (the substep semantics, the
  free movers, the statics) with NO production caller. THE DRIFT RISK:
  the two integrators can diverge silently (the fixed was built for the
  platformer-era character work that never shipped).
- resolveCollision / resolveSandboxPairs / RESTING_VEL: the writes =
  the positions/velocities only (the scale/halfExtents read-only - the
  339-proven); RESTING_VEL 0.5f = the sandbox-only resting threshold
  (Pong 0.0); the mass weighting + the friction + the restitution clamp
  pinned.
- sweptAABB + sweptMoveAndCollide + useSwept: the contract pinned (the
  earliest-hit clamp; the dt <= 0 no-op; the start-inside delegation);
  useSwept = the per-controller opt-in (default false) - the swept path
  runs ONLY when a caller opts in; no production caller opts in.
- the character controller fixed vs variable: BOTH test-proven (the
  coyote/jump-consumed/substep-clamp contracts); NEITHER has a production
  consumer.
- input: the actions/bindings/overrides + the reset idempotence + the
  mouse state + the gamepad bridge pinned headless; the DEVICE polling
  human-only.
- audio: the volume/mute/lifecycle/pool-rotation state machine pinned
  headless; the DEVICE output human-only.
- time: the clamp/scale/pause pinned (318).
- events: the order/payload/unsubscribe/once/reentrancy pinned.

THE GAP LIST (classified):
G1 MISSING: a production consumer for the swept/character/fixed machinery
   (physics.h :333-656, ~320 lines = HALF the file; test-proven, zero
   production callers - the arcade uses applyPhysics only).
G2 THIN: the z-axis pair resolve (resolveCollision is 2D-AABB-focused;
   the sandbox drop resolves on Y; the Z-axis penetration/bounce behavior
   is exercised only incidentally by the 3D drop).
G3 HUMAN-ONLY: the audio device playback end-to-end (the output, the
   device loss, the real latency) - the state machine is pinned, the
   sound is not.
G4 HUMAN-ONLY: the input DEVICE feel (the polling under a real window/
   gamepad; the headless covers the contracts).
G5 BY-DESIGN LIMIT: the broadphase grid (the Step-52: fine at <=2000
   entities; the reopen ONLY on a measured superlinear breach).
G6 BY-DESIGN LIMIT: the prefab/scene versioning is reject-only (no
   migration path - the Entity/Scene ACCEPTED contract).

THE CANDIDATE PINS (at most 5, for a LATER phase - NOT implemented here):
P1. The fixed-vs-variable drift pin: the same scenario (a falling body
    with the bounces) through applyPhysics AND applyPhysicsFixed over N
    frames; the assert: the fixed substeps approximate the variable
    within a stated epsilon at the matching wall-clock points; the
    mutation: the substep count 0.
P2. The z-axis sandbox resolve pin: two Sandbox boxes overlapping on Z
    only; the assert: the separation along Z with the normal-axis
    velocity zero (no tunneling); the mutation: the z-normal zeroed in
    resolveCollision.
P3. The RESTING_VEL parameter semantics pin: the same pair resolved with
    restingVel 0.5 (the sandbox: the slow contact STICKS) vs 0.0 (Pong:
    any approach bounces); the assert: the velocity below the threshold
    zeroes ONLY when the threshold is nonzero; the mutation: the
    threshold comparison flipped.
P4. The audio applyVolumes state pin: the volumes/mute matrix -> the
    internal applied state (the device-independent core) for every
    combination in the existing matrix; the mutation: the mute bypasses
    applyVolumes.
P5. The input bindings reload idempotence pin: load -> override -> reset
    -> load again; the assert: the same key set as the first load (no
    accumulation); the mutation: the reset loses the defaults.

THE ACCEPTANCE DRAFT (NOT granted - the mandatory checklist for
Simulation ACCEPTED, the same shape as Render):
A. the build exit 0 against the committed tree.
B. ctest green; the count grows additively (inside the existing harness
   or a new target; no timing asserts).
C. the live smoke: alive x3 + the Cyril items (the arcade feel: the
   collisions, the hostiles; the platformer jumping IF G1 gains a
   consumer; the audio levels audible; the input responsiveness).
D. the structure: no physics.h split unless the audit justifies one; the
   ODR grep 0 held.
E. the include hygiene: no new heavy includes in the sim headers.
F. the consumer linkage: any moved body links the new .cpp (the 323
   pattern).
G. the contracts pinned before any move (the tests-first).
H. the behavior proof: the mutation evidence per new pin.
I. measurement not budget (no timing asserts; the bench = DATA).
THE LIMITS to carry: G1-G6 above (the consumer gap, the z-thinness, the
human-only audio/input, the broadphase and versioning by-design limits).

***STEP 344 (2026-10-22): THE SIMULATION PIN P3 (the RESTING_VEL parameter
semantics) - ALREADY PINNED (the mutation-first evidence; NO new test).***
THE DECISION RECORDED (the 344 prompt): G1 (the swept/character/fixed
machinery has no production consumer) = A KNOWN LIMIT, option A (no game
wiring). THE INSPECT (file:line): RESTING_VEL = 0.5f constexpr (physics.h
:86); resolveSandboxPairs :283-307 -> resolveCollision(a, b, 0.5f,
RESTING_VEL) :304; Pong's call site = the PURE DEFAULTS (pong.cpp :210/
:219: resolveCollision(entities[0], entities[2]) -> restitution 0.5 ->
max(a.rest, b.rest), restingVel 0.0); THE COMPARISON INSIDE
resolveCollision (physics.h :89-269): the signature :89-90 (restitution
0.5, restingVel 0.0 defaults), the two resting branches :194-206 (the
static-first: rel < -restingVel bounces; ELSE IF restingVel > 0.0f AND
rel <= 0.0f -> the normal velocity ZEROED - "the 0.0 default never
zeroes", the Step-238 note) and :219-233 (the dynamic-first: rel >
restingVel bounces; ELSE IF restingVel > 0.0f AND rel >= 0.0f -> zeroed).
THE MUTATION-FIRST PROBE (per the 344 prompt): the resting-branch guard
flipped restingVel > 0.0f -> restingVel > 1.0f in BOTH branches (physics.h
:199/:226) -> the build exit 0 -> the direct run exit 1 -> **an EXISTING
check failed: checkBounceRestHeights (tests/hostile_data_test.cpp :8846),
the failing assert :8936: "the box must settle with |vy| below the
threshold (no jitter)"** -> P3 IS ALREADY PINNED by checkBounceRestHeights.
PER THE 344 PROMPT: named, NO new test added. THE RESTORE: physics.h by
pathspec -> the src diff clean -> REBUILD -> ctest 8/8 -> alive x3.
SIMULATION STAYS IN PROGRESS; G1-G6 UNCHANGED (G1 now the recorded known
limit, option A).

## Kill criteria

***STOP NOTE (Step 284, 2026-10-03): THE PUSH FAILED (RESOLVED 2026-10-03: the human switched origin to the SSH alias remote with a repo-scoped deploy key; the 284 record was pushed and Steps 285-286 completed and pushed). The original failure text is kept below for history.*** The Step 284 record commit `64fb5a3` (the governance
amendment + the Phase Tools rules) is LOCAL ONLY: `git push` fails
with exit 128 and no surfaced error message, while the remote IS
reachable (`git ls-remote origin HEAD` returns ad4e8ce, exit 0) —
consistent with a credential/expiry issue in this non-interactive
shell; multiple retries consumed the retry-once budget. Step 285 (the
Editor-0 brief) was NOT attempted (the next-step rule: only if green
AND pushed). The human: push `64fb5a3` when the credentials recover,
then resume at Step 285 (reload first: expect HEAD `64fb5a3`, highest
284, Tools phase OPEN).
If any step's scope keeps expanding instead of shrinking, stop, cut scope, and re-record a smaller definition_of_done before continuing. Do not introduce an abstraction, manager, registry, or subsystem unless the current implementation demonstrates a concrete need for it.
