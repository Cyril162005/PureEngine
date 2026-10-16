# PureEngine

A learning project: a minimal game engine written from scratch in C++ and
OpenGL, plus a small arcade game built entirely on top of it. No engine
framework, no game library — every engine layer was written as part of the
project itself.

> **v1.1 toolkit FROZEN** (Step 141 confirmation). Scope guardrails are
> binding: no ECS, no editor IN THE ENGINE, no networking, no 3D (the 3D
> contracts landed via the frozen v3/v4/v5 versions). AMENDED Step 284
> (2026-10-03): Editor/tools phase ALLOWED (explicit allow by Cyril) —
> an editor/tool is a CONSUMER of the frozen engine (own directory, own
> CMake target, documented APIs only, engine src/ read-only); the engine
> itself still ships no editing tools. The human release gate
> is [`Blueprint/SMOKE_TEST.md`](Blueprint/SMOKE_TEST.md); the authoritative
> freeze scope (capability map + OUT list) lives in
> [`Blueprint/PURE_ENGINE_V3.md`](Blueprint/PURE_ENGINE_V3.md). New
> capabilities land only as additive, opt-in, engine-pure steps with honest
> verification — frozen behavior is never changed or unfrozen.

- **Engine** (tracked steps 25–286; the 3D v4/v5/v6 versions
  COMPLETE/FROZEN (Steps 263/269/283); **Phase Tools/Editor OPEN at
  284** (governance: an editor/tool is a CONSUMER of the frozen engine;
  the engine-side bans stand; the engine still ships no editing tools);
  the **unattended-phase protocol** is a standing order in AGENTS.md
  (Step 285); the **Editor-0 brief** recorded (Step 286, no option
  chosen, no editor code; **Step 288** Tools editor0 step 1 of 5: the PureEditor0 tool target + loadSceneForEditor + --selftest green in tools/editor0/; **Step 289** editor0 step 2: the window viewer (entities + debug AABBs + bitmap-font status line; load failure keeps running) + the hidden-window GL frame, selftest 27 checks green, visual confirmation PENDING; **Step 290** editor0 step 3: the sample generator (--make-sample, refuses to overwrite) + mouse-drag pan + +/- zoom (clamped 0.25-4.0, editor-documented limits; NO wheel input in the engine), selftest 41 checks green, visual confirmation PENDING; **Step 291** editor0 step 4: the R reload (scene replaced exactly, no leftovers/spawns) + the hostile loads (12+7 case types, all clean rejects, the scene unchanged every time, NO crash/hang; the scene format has no count field - observed), selftest 65 checks green, visual confirmation PENDING; **Step 292** editor0 step 5 (docs only): the Editor-0 slice complete - findings classified (10 existing capability, 2 missing capability: camera zoom API + wheel/scroll input; 0 engine defect), option B (pick + read-only inspector) NOT blocked - all its APIs exist and are verified; **Step 293** docs only: the **PureEngine Direction Specification** (the product goal = a general-purpose 2D + 3D engine, the human decision 2026-10-08; the 5-category map; the 9 phases all NOT OPEN with entry/first/test/exit criteria; the agent rules) in PURE_ENGINE_V3.md + the AGENTS.md pointer; **Step 294** editor0 option B step 1: the click selection (click selects, empty-space clears, drag still pans; classifyPointerGesture threshold 4px) + the zoomed pick + the role-group highlight + the R reload clears/keeps the selection + the print sink (one report per attempt, test-proven), selftest 82 checks green; **Step 295** option B step 2: the live-path wiring (EditorState + stepEditorFrame, GLFW-free) + the coordinate ratio fix (the cursor window->fb; the no-selection live defect found and fixed editor-side) + the status feedback (reloaded/reload failed/no selection) + --diag-input; 102 checks; confirmed by Cyril; **Step 296** option B step 2: the read-only inspector panel (the 36-line list = the 24 saver fields in the 4dp saver style, the "+N more" overflow, no selection -> "no selection"; the panel never edits; a click inside the panel rect keeps the selection; the panel is immune to pan/zoom) + the Direction Spec rotation correction (same field, axis per mode: z-spin 2D, yaw +Y 3D); 113 checks; recovered from the lock blocker via the git stash; **Step 297** option B step 4: Tab = next / Shift+Tab = previous navigation (wrapping, the dead skipped, the empty scene a safe no-op) + the inspector line cap 40 with '...' + the status tag cap + buildSampleSceneN (--make-sample [count], 0..10000) + --diag-input prints one line per nav key press; 151 checks; the 296 inspector visual CONFIRMED by Cyril (the polish: the big font + the crowded status line = cosmetic, recorded); the prompt's two-versions conflict reported (the scroll/toggle + the 2000-entity scene + the second mutation NOT implemented - a candidate for a later step if confirmed); **Step 298** docs only: the Editor-0/Tools slice **CLASSIFIED AND CLOSED** (the 26 FINDINGS entries = 17 existing capability, 6 fixed defect - all editor-side, 2 missing capability: the camera zoom API + the wheel/scroll, 1 cosmetic; the closed-slice summary in PURE_ENGINE_V3.md: what works / what is out / honest limits; the next lane is the human's choice; **Step 299** option C step 1: the **save-as** (Ctrl+S; the editor's first write path; a NEW file path under savedata/ via the auto-increment counter; the write-safety refusals: empty path, the source, an existing destination - the source NEVER overwritten, the round-trip proven first; save->load->save byte-identical; the status feedback saved/save failed); 167 checks; the save-as visual confirmation PENDING; **Step 300** option C step 2: the **nudge + modified state + reload guard** (the arrow keys move the selected entity 0.1 world units, Shift 1.0, world-space, pan/zoom-independent; the modified marker on the status line; the R guard: the first R asks, the second R within 120 frames discards and reloads, any other key clears; Ctrl+S after nudging writes the edited scene to the next savedata/_editN file, the source untouched; --diag-input prints one line per nudge press); 241 PASS selftest lines; the 299 save-as + the 300 nudge visual confirmations PENDING; **Step 301** option C step 3: the **bounded undo/redo** (Ctrl+Z/Ctrl+Y, Ctrl+Shift+Z; positions only, one entry per applied nudge, the cap 32 with the oldest falling off, the redo cleared by a new nudge, the stacks cleared by a reload and surviving a save, undo/redo never touch files; the modified flag honest vs the baseline captured at every successful load/save); 280 PASS selftest lines; the 299 + 300 + 301 visual confirmations PENDING; **Step 302** docs only: the **option-C slice (299-301) CLASSIFIED AND CLOSED** (the save-as NEW-file-only + the nudge + the modified/baseline + the R guard + the bounded undo/redo verified by the selftest (280 PASS) + the non-vacuous mutations; what is out: overwrite-source, path-switch-after-save, multi-field edit, gizmos, the panel scroll, the 2000 stress, the engine zoom/wheel; the 299/300/301 visuals PENDING; the next lane was the human's choice; **Step 303** (AUTHORIZED): the **path-switch-after-save** (on a successful Ctrl+S the editor follows the saved file - the loaded/source path becomes the NEW _editN file; a failed save changes nothing; the FLAT naming rule _edit1 -> _edit2 (no nesting); the write-safety preserved (the current path = the source refusal, the original = the existing-file refusal); the undo/redo stacks kept, cleared on reload); 315 PASS selftest lines; the 299/300/301 + 303 visual confirmations PENDING; **Step 304**: the **inspector panel scroll** (PgUp/PgDn; the clamped 12-line window; the '+N more' hint at scroll 0; the resets on pick/nav/reload; the wheel stays a missing capability); 345 PASS selftest lines; the 299-304 visual confirmations PENDING; **Step 305** (recovered): the **always-on key legend** (5 compact bottom lines, every live key token; the 296-class lock STOP recovered by the human and reproduced); **Step 306** docs only: the tools **mini-slice (304-305) CLASSIFIED AND CLOSED** (the scroll + the legend verified by the selftest 349 PASS + the non-vacuous mutations; out: overwrite-source, multi-field edit, gizmos, the wheel/zoom engine capabilities, MCP/pools; the visuals 299-305 PENDING; **Step 307** docs only: the tools milestone declared **COMPLETE with a RECOMMENDED PAUSE** (further Tools steps only on measured need or explicit authorization; the phase stays OPEN) + the visual status set exactly as reported (the 307 checklist every row unfilled -> the 299-305 visuals ALL NOT TRIED; the selftest 349 PASS + the mutations remain the recorded verification). Step 308 (docs only): the **Vertical slices** documented in V3 (the 2D and the 3D slice workflows, every row grep-verified with file:line + a status + a category + the first capability to close; the honest corrections (the packaging CORRECTED 309: scripts/package.ps1 EXISTS - FINDINGS 44); the editor play mode, the entity create/delete wiring, the asset browsing, the export all MISSING; the 3D camera/lighting VERIFIED) + the text dependency graph + the 11-candidate list ordered ONLY by dependency and evidence (nothing chosen, nothing opened); the human visual line arrived unfilled -> the NOT-TRIED status stands (FINDINGS 43); **Step 309** (entity editing 1 of 3): the **entity delete** (the Delete key; the alive-flag kill - the saved file excludes the deleted entity, the undo restores it field-exact at the same index, the redo re-deletes, the same bounded stack as the nudges, the honest alive-aware baseline, the visible counts everywhere); 385 PASS selftest lines; the 299-305 visuals NOT TRIED + the 309 visual PENDING; **Step 310** (entity editing 2 of 3): the **entity create** (N instantiates the current prefab at the view center; [/] cycles the sorted editor-owned listing with the index+name in the status line; the create appends a NEW index, the undo kills it via the alive flag, the redo revives at the same index field-exact; the same bounded stack as the nudges/deletes; the growth-aware honest baseline; the saved file contains the created-and-alive and omits the dead; no list / a bad prefab = the reason, nothing changes); 441 PASS selftest lines; the 299-309 visuals NOT TRIED + the 310 visual PENDING; **Step 311** (RECOVERED): the **dual-view + 3D draw + meshId + play foundation** (V toggles the 2D ortho / the 3D perspective draw via drawEntity3D with the proven depth-clear; the meshId keys 0/1/2 - NOT serialized, documented; P = the play foundation: the snapshot, the fixed-dt tick via advanceRotations+applyPhysics only, the edits disabled, the field-exact restore, the source never written; the status VIEW:2D/3D + PLAYING); 462 PASS selftest lines; the 299-309 visuals NOT TRIED + the 310/311 on-screen visuals PENDING; **Step 312** (measurement only): the **--gl-info + --bench harness** (the GL/GLSL strings + COMPATIBLE/RISK; the median/p95/max/mean ms + the RSS + the machine info over N frames of the editor's own draw path with glFinish; draw_calls/bytes_uploaded = UNAVAILABLE; NO timing assertions - the numbers are data, 'indicative, this machine, not a budget'; .gitignore gains editor0_tmp/); 489 PASS selftest lines; the visuals 299-311 PENDING; Cyril to run the harness on every machine (the weakest first); **Step 313** (docs only): the **T0 measurement record** (Cyril's machine verbatim in V3/FINDINGS 52: i3-3120M, Intel HD 4000, GL 3.3.0 Build 10.18.10.4885, COMPATIBLE; bench200 2D 600f median 1.0709ms / p95 4.0543ms inside the indicative targets; bench200 3D 600f median 32.5299ms / p95 54.7805ms ABOVE the 16.6ms line - the 3D path expensive on HD 4000, DEFERRED until a measured need or an explicit open; the CSVs untracked under savedata/); **Step 314** (docs only, CATEGORY AUDIT): the **Core Foundation audit** (vec3/mat4/engine_time/window_guard header-only, pe_core.cpp the only core .cpp and linked by nobody, the bootstrap not a boundary; gaps: the bare GLFW includes + ordering luck, the pe_core instantiation list omits math, no math unit test, mat4.h:236 near/far; acceptance standard + 5 slices PROPOSED, the split starts only after Cyril approves; Entity/Scene untouched); **Step 315** (CORE, slice 1): the **hygiene defines** - GLFW_INCLUDE_NONE at every src/ GLFW site (6 grep-verified), mat4 near/far renamed zNear/zFar (API-preserving, the Step 312 clobber class dead), the engine_time.h banner fixed; build exit 0 / ctest 5/5 / alive x3 / the mutation non-vacuous; Core Foundation IN PROGRESS, slices 2-5 not started; **Step 316** (CORE, slice 2): the **pe_core contract + the ODR link probe** (pe_core.cpp 22 -> 31 includes - the 16-header list, math first; core_link_probe = 2 full-list TUs linked against pe_core, ctest 6/6 TIMEOUT 60; 0 ODR hazards, 0 inline fixes; the mutation LNK2005/LNK1169 caught + restored; the Step 102 header-only rule superseded incrementally, Cyril 2026-10-14; the slice-1 smoke not tried); **Step 317** (CORE, slice 3): the **math unit test + the STAY-INLINE decision** (tests/math_test.cpp: the Vec3 ops + all Mat4 builders with hand-computed deterministic asserts, ctest 6->7 TIMEOUT 60; math STAYS header-inline - small hot bodies, a .cpp would fan out to 6-8 targets; one test-bug of mine disclosed + fixed (the I*T expected value), the product code correct; the mutation non-vacuous; the smoke not tried again); **Step 318** (CORE, slice 4): the **FrameTime contract test + the engine_time STAY-INLINE decision** (tests/time_test.cpp: the deterministic dt clamp - a 150ms stall returns EXACTLY 0.1f bit-exact, the scale/pause state machine, ctest 7->8 TIMEOUT 60; engine_time STAYS header-inline - 1-5-line bodies, a 5+-consumer fan-out; the mutation non-vacuous; Cyril's smoke: PASS - the live row ACCEPTED; Core Foundation 4 of 5, slice 5 waits on go); **Step 319** (CORE, slice 5): the **window bootstrap extraction** (the paired src/window.h + src/window.cpp - pe::bootWindow owns init/hints/create/context/glad with the failure cleanup inside, the first Core Foundation .h/.cpp split; PureEngine + Pong + Platformer wired, the titles/sizes byte-identical, the exit codes caller-side; the checkWindowBoot negative check in hostile_data_test, ctest 8/8; the mutation non-vacuous; the H bench re-run as data (2D 1.1379ms, 3D 30.5809ms); the editor + pe_core residuals recorded; Core Foundation ACCEPTED 5 of 5); **Step 320** (AUDIT, docs only): the **Entity/Scene/Persistence category audit** (the 6-header inventory, ALL header-only; the honest contracts with file:line - the alive drop-on-save scene.h:420, the 4-decimal floats, the index semantics, meshId non-serialization; the ODR baseline 0 real hazards; the draft acceptance standard A-I; 5 migration slices PROPOSED tests-first, nothing started; Core Foundation remains ACCEPTED); **Step 321** (CORE, Entity/Scene slice 1): the **persistence format/contract pins** (tests-first, hostile_data_test: the 4-decimal writer bound + the rounded reload, meshId NOT serialized per the documented contract, the file-level dead-drop, the unknown-prefix strict reject, no .tmp left; ctest 8->8) - **RECOVERED + COMPLETED after the lock clear** (the gates re-run: the build exit 0, ctest 8/8, the direct pins run clean; the mutation NON-VACUOUS + restored + rebuilt; alive x3; the recovery: Cyril cleared the lock, the gate-recovery step flipped 321; FINDINGS 60); **Step 322** (INSPECT+DECIDE, docs only): the **numbers-driven split verdicts** (scene.h SPLIT-JUSTIFIED - the persistence bodies 436-542 lines, the I/O includes paid by 6 TUs of which 3 never persist; prefab.h SPLIT-JUSTIFIED - ~144-line bodies, 3+ of 6 TUs never load; hostile_data.h/entity.h/components.h/lifecycle.h STAY-INLINE with the numbers; the ODR pain none; the .cpp WAITS for Cyril; Entity/Scene/Persistence IN PROGRESS 1/5); **Step 323** (CORE, Entity/Scene slice 2): the **scene persistence split** (the paired src/scene_persist.h + src/scene_persist.cpp - the 4 bodies + the 7 file-local helpers moved VERBATIM from scene.h :277-816; scene.h 820 -> 279 lines with ZERO I/O includes; linked into 5 targets incl. LevelPipelineConsumer which calls the persistence; the golden byte-identity proof ALL FOUR files vs the pre-split SHA256; the mutation on the moved code NON-VACUOUS x2; Entity/Scene IN PROGRESS 2/5, Slices 3-4 wait on go); **Step 324** (CORE, Entity/Scene slice 3): the **prefab I/O split** (the paired src/prefab_persist.h + src/prefab_persist.cpp - loadPrefab + the prefab_detail helpers moved VERBATIM from prefab.h :58-177; prefab.h 209 -> 88 lines with ZERO I/O includes, instantiatePrefab + the Prefab struct stay; linked into the 4 caller targets; the load contracts UNCHANGED; the mutation on the moved load path NON-VACUOUS x4 assertions; Entity/Scene IN PROGRESS 3/5, Slice 4 waits on go); **Step 325** (CLOSE, docs + gates): **Entity/Scene/Persistence CATEGORY ACCEPTED** (the 320 criteria A-I all met; the pins 321 + the scene split 323 with the golden ALL FOUR + the prefab split 324 + the STAY-INLINE decisions 322, Slice 4 superseded; the honest contracts UNCHANGED; the gates at close: the build exit 0, ctest 8/8 (14.42s), alive x3; the next category opens only on an explicit prompt); **Step 326** (AUDIT, docs only): the **Render category AUDITED / migration NOT STARTED** (the 7-header inventory, ALL header-only, the ODR 0; renderer.h 1269 = the largest header - contract+tests on the 3D half, GAP on the untested 2D draw path, STAY-INLINE; mesh3d.h SPLIT-JUSTIFIED candidate for the OBJ-loader half, AWAITING CYRIL; shader/lighting/resources/font/ui STAY-INLINE with the numbers; the draft acceptance criteria A-I; the gates: the build exit 0, ctest 8/8, alive x3); **Step 327** (RENDER, slice 1): the **headless 2D drawWorld contract checks** (tests-first per the 326 largest-gap finding; hostile_data_test checkDraw2DContract: the depth-sort winner via the TEXEL-FREE channel isolation, the collision override, the dead skip, the meshId 2D skip, the blend/depth state restore, the OOB fallback, the lit overload callable; the depth-sort mutation NON-VACUOUS restored + REBUILT + 8/8; Render migration IN PROGRESS 1/N, the mesh3d split waits on go; **Step 328** (AUDIT, docs only): the **3D contract coverage MATRIX** (17 rows, the exact test assertions read from the bodies - no coverage from names; the registry/lookup/fallback, the OBJ load + the failure paths, the depth discipline, the occlusion, the clearDepth color-preservation, the texture registry/release Partial, the destroyAll partial-init Partial, the lighting uniforms, the lit fallback Partial, the 2D sort/override/skip/blend Covered); the 327 PIN reassessment (only PIN 1 mutated; PIN 6 observes the checker-fallback output only, PIN 7 does not distinguish the fallback from the lit path); the top-5 gaps ranked (the PIN-2 collision mutation, the PIN-4 meshId-skip mutation, the releaseTexture refusals unquoted, the lit-fallback distinguishability, the destroyAll partial-init); the recommended SMALLEST 329 (the 2 mutations + the refusals, read-checkTextureRegistry-FIRST); the mesh3d OBJ-split verdict SPLIT-JUSTIFIED with numbers (the 91-line loader, 3-of-5 never-load); the self-review names the weakest inference; the gates: the build exit 0, ctest 8/8 (22.73s), alive x3)))):
  window/context, rendering pipeline,
  own math library (`Vec3`/`Mat4`), entity/collision/state systems,
  audio playback (miniaudio: SFX pool, event sounds, music loop, mute),
  file-based asset loading (stb_image PNG) + binary blob/pack + cache,
  scene structure, animation, physics (gravity, impulse, statics, character
  controller), tilemaps, scenes, transform hierarchy, bitmap text,
  event bus, debug console, gamepad input, particles, prefab templates,
  screen↔world conversion + entity picking, resize/viewport, mouse input,
  UI buttons, and full game loops with states.
- **Games**: arcade survival (dodge crimson hostiles; survival time is the
  score, persisted across runs; arena has tile walls, a companion satellite,
  catch-burst particles, debug console, gamepad support), Pong
  (`build\Release\Pong.exe`: paddles + ball, the second-game API proof),
  and Platformer (`build\Release\Platformer.exe`: TITLE→L1→L2→WIN across
  two tile levels with the character controller, goal events, particles,
  console, and gamepad path — the v1.0 proof game).

The full build history and design decisions live in
[`Blueprint/GAME_BUILD.md`](Blueprint/GAME_BUILD.md) (machine-readable twin:
[`Blueprint/game_build_steps.json`](Blueprint/game_build_steps.json)),
[`Blueprint/PURE_ENGINE_V3.md`](Blueprint/PURE_ENGINE_V3.md) (engine track,
machine-readable twin: [`Blueprint/pure_engine_v3_steps.json`](Blueprint/pure_engine_v3_steps.json)).
Steps 63–70 were built as tested foundations first and then adopted by the
arcade game (Step 71); Step 72 hardened physics; Steps 73–74 added camera
lerp and the platformer proof game (48 behavior cases green, 15/15 human
playtest items pass).

## Requirements

- Windows 10/11 (tested), OpenGL 3.3-capable GPU and driver
- Visual Studio 2022 with the **Desktop development with C++** workload
  (MSVC compiler + Windows SDK)
- CMake 3.15 or newer
- Git, and internet access during the first configure (dependencies are
  fetched via CMake `FetchContent`: GLFW 3.3.8, miniaudio 0.11.25,
  stb pinned at commit `2c980bb5`)

## Build from source

From a **Developer Command Prompt / Developer PowerShell for VS 2022** (or any
shell where `cl` and `cmake` are on the PATH):

```bat
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

These are the exact commands used for every engine step and game phase. The
first configure takes several minutes because it clones the three
dependencies; later configures reuse the cached sources.

All in-tree assets are generated by small PowerShell scripts (no art tools,
no downloaded binaries) — the generated files are committed, but you can
reproduce them byte-for-byte:

| Script | Output |
|---|---|
| `make_checker.ps1` | `assets/checker.png` (legacy world texture) |
| `make_font.ps1` | `assets/font_digits.png` (37-cell bitmap atlas: digits + A–Z) |
| `make_beep.ps1` | `assets/beep.wav` (collision/alert sound) |
| `make_textures.ps1` | `assets/tex_player.png`, `assets/tex_scenery.png`, `assets/tex_hostile.png` (entity textures) |
| `make_paddlesheet.ps1` | `assets/paddle_spritesheet.png` (Pong paddle frames) |
| `make_music.ps1` | `assets/music_loop.wav` (4 s ambient loop, Step 123) |
| — | `assets/tex_hostile_alt.png` (committed; no generator script) |
| — | `assets/hostile_default.txt`, `assets/hostile_alt.txt` (data-driven hostile configs) |
| — | `assets/animation_default.txt`, `assets/paddle_animations.txt` (clip definitions) |
| — | `assets/tilemap_default.txt`, `assets/arcade_arena.txt` (tilemaps) |
| — | `assets/win_sound.wav`, `assets/GAMEOVER.wav` (event audio, committed; no generator script) |
| `scripts/package.ps1` | `package/PureEngine-0.1.0-win64.zip` |

## Run

```bat
build\Release\PureEngine.exe
build\Release\Pong.exe
build\Release\Platformer.exe
```

Each executable resolves its assets through a 3-candidate probe
(`assets/`, `../assets/`, `../../assets/`), so it works when launched from
the repo root, from `build/`, or from `build/Release/`.

### Arcade controls

| Input | Effect |
|---|---|
| SPACE | Start from menu / return to menu from game over (gamepad A works too) |
| 2 | Start the alternate hostile scene from the menu |
| Arrow keys | Move the player (gamepad left stick works too) |
| ESC | Pause / resume; on the menu it quits (gamepad START works too) |
| F1 | Toggle collision-box debug overlay |
| ` (backtick) | Toggle the debug console (type `help`, `entities`, `reset`, `volume`, `mute`, `pick`, `blob`, `spawn_prefab`, `textures`; ENTER submits) |

The alternate scene uses `hostile_alt.txt` and demonstrates the orange hostile texture variant.
The camera follows the player (the old WASD free-pan is gone). Blue tile
walls block the player; the small green satellite is a hierarchy-attached
companion; states show centered text labels (PAUSED / GAME OVER / YOU WIN).

### Pong controls

W/S move the left paddle, Up/Down move the right paddle, ESC quits. No
score, pause, audio, or states yet — that is future work, not architecture.

### Platformer controls

Arrows or A/D move, SPACE/W jump, ESC pauses, ` opens the debug console
(`help`, `entities`, `pos`, `reset`). Gamepad left stick moves where a
mapped pad is present. Reach the goal zone to advance: L1 → L2 → WIN.

The high score is saved to `savedata/highscore.txt` (created automatically
on the first record; excluded from git). Delete it to start fresh at 0.0.

## Standalone package (no dev environment needed)

The executable is **fully self-contained**: GLFW, miniaudio, stb, and GLAD
are statically linked, and the MSVC C/C++ runtime is statically linked as
well (via the `/MD`→`/MT` flag-variable rewrite in `CMakeLists.txt` — the
modern `CMAKE_MSVC_RUNTIME_LIBRARY` property did not take effect because
the fetched dependencies reset the policy scope; see the comment there).
A working package is just:

```
PureEngine/
  PureEngine.exe        <- build\Release\PureEngine.exe
  assets/               <- the whole assets folder
```

No DLLs ship alongside, no Visual Studio or VC++ Redistributable is needed
on the target machine — only Windows' own system DLLs and an OpenGL 3.3
driver. The `savedata/` folder is intentionally NOT shipped: the game
creates it on the first new record, so a fresh install starts at 0.0.

## Repository layout

```
src/main.cpp            arcade game: window, audio, entities, state machine,
                         simulation, camera, timing, high score + adopted
                         tilemap/scenes/events/console/gamepad/particles/text
src/renderer.h          the renderer boundary: shader, VAO/VBOs, textures,
                         world draw loop, digit + full-text UI, debug overlay
src/shader.h            shader loading: file probe, compile, link (lit/default)
src/lighting.h          2D point lights: PointLight/LightingState, world-space
src/resources.h         the resource-loading boundary: texture load/upload
                         (3-candidate path probe, stb_image) + binary
                         blob/pack loading + tiny binary cache
src/camera.h            the camera boundary: follow, lookAt view,
                         orthographic projection
src/input.h             the input boundary: key-state polling, edge detection,
                         previous-frame snapshot
src/gamepad.h           gamepad snapshots: poll, deadzone, button edges
src/engine_time.h        the time boundary: frame-time, clamped delta
src/lifecycle.h         the entity lifecycle boundary: initial construction
                         (incl. hierarchy satellite), snapshot restore
src/gamestate.h         game-state enum + predicates and per-state lookups
src/audio.h             the audio boundary: miniaudio engine, beep pool,
                         dedicated event sounds
src/ui.h                the UI boundary: HUD number formatting + layout
src/simulation.h        the simulation boundary: rotations, chase, physics
                         integration, scenery collision scan
src/physics.h           physics: gravity, impulse resolve (static-aware),
                         character controller (grounded/jump/coyote)
src/tilemap.h           tilemaps: load, convert to entities, tile collision
src/scene.h             scenes: SceneManager own/load/switch/clear
src/font.h              bitmap-text logic: cell map, width, alignment
src/events.h            event bus: subscribe/emit, snapshot delivery
src/console.h           debug console: toggle/type/commands/draw
src/components.h        lightweight component helpers: health/tag/timer/velocity
src/prefab.h            prefab templates: loadPrefab/instantiatePrefab
src/particles.h         particles: pool, emitter, drawWorld converter
src/animation*.h        animation clips, file loading, frame UVs
src/math/               own math layer (Vec3, Mat4)
src/entity.h            entity data (+parentIndex, isStatic, coyote fields)
src/collision.h         AABB collision
src/stb_impl.cpp        stb_image implementation unit
games/pong/pong.cpp     Pong: second-game API proof (own CMake target)
games/platformer/       Platformer proof game: 2 tile levels + controller +
                         scenes/events/console/particles (own CMake target)
tests/                  hostile_data_test.cpp, 78 behavior cases (CTest)
assets/                 committed assets (scripts generate most, not all)
Blueprint/              blueprints + step trackers (source of step history)
make_*.ps1              in-tree asset generators
scripts/package.ps1     packaging script
```

## Where the project stands

v1.0 means a small code-first 2D engine proven by real games. Current
state: the arcade game exercises rendering, input (+gamepad), audio,
config, HUD, save, tilemaps, scenes, events, console, particles, text,
and hierarchy; the platformer proves physics (statics, controller),
tilemap levels, scene switching, goal events, and full TITLE→L1→L2→WIN
flow. 48 behavior cases green; 15/15 human playtest items pass (particles
and audio verified by ear/eye; Up-arrow jump shares the proven W/Space
path but this VM never delivers that key). Deliberately deferred:
sprite batching (measured: unneeded, see below), editor, ECS, 3D, networking.
Phase 2 (101–133) added cache/blob, pe_core, dump/load, input rebinding,
persistence v2, alpha blending, lifecycle (alive/spawn/kill), resize/viewport,
mouse input, UI Button, music loop + asset, prefabs, menu buttons, mute,
screen/world conversion, point picking, and console proofs (pick/blob/health/
textures) without changing the frozen v1.0 core. Steps 134–141 were the
docs/freeze pass (v1.1 toolkit frozen at 141); Steps 142–151 were OPTIONAL
post-freeze polish (guardrails, tests, hygiene) — not v1.1 debt;
Steps 152–153 docs; Steps 154–155 input-campaign; Step 156 the
scene/prefab/persistence test campaign.

## Performance note

Measured on the integration workload (Release, `/O2`): tilemap→entities
for the 14-tile arena plus a 24-particle update/convert costs ~0.003 ms
per frame; a 10k-tile conversion costs ~3.8 ms one-off (level load, not
per frame); tile collision queries cost ~70 ns; the live arcade scene
runs at several hundred fps. Step-52 evidence (1.21 ms at 50 entities →
12.09 ms at 5000) still bounds the entity path. Nothing here justifies
batching: it stays deferred until a measurement says otherwise.
