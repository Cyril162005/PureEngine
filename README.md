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
  chosen, no editor code; **Step 288** Tools editor0 step 1 of 5: the PureEditor0 tool target + loadSceneForEditor + --selftest green in tools/editor0/; **Step 289** editor0 step 2: the window viewer (entities + debug AABBs + bitmap-font status line; load failure keeps running) + the hidden-window GL frame, selftest 27 checks green, visual confirmation PENDING; **Step 290** editor0 step 3: the sample generator (--make-sample, refuses to overwrite) + mouse-drag pan + +/- zoom (clamped 0.25-4.0, editor-documented limits; NO wheel input in the engine), selftest 41 checks green, visual confirmation PENDING; **Step 291** editor0 step 4: the R reload (scene replaced exactly, no leftovers/spawns) + the hostile loads (12+7 case types, all clean rejects, the scene unchanged every time, NO crash/hang; the scene format has no count field - observed), selftest 65 checks green, visual confirmation PENDING; **Step 292** editor0 step 5 (docs only): the Editor-0 slice complete - findings classified (10 existing capability, 2 missing capability: camera zoom API + wheel/scroll input; 0 engine defect), option B (pick + read-only inspector) NOT blocked - all its APIs exist and are verified; **Step 293** docs only: the **PureEngine Direction Specification** (the product goal = a general-purpose 2D + 3D engine, the human decision 2026-10-08; the 5-category map; the 9 phases all NOT OPEN with entry/first/test/exit criteria; the agent rules) in PURE_ENGINE_V3.md + the AGENTS.md pointer; **Step 294** editor0 option B step 1: the click selection (click selects, empty-space clears, drag still pans; classifyPointerGesture threshold 4px) + the zoomed pick + the role-group highlight + the R reload clears/keeps the selection + the print sink (one report per attempt, test-proven), selftest 82 checks green; **Step 295** option B step 2: the live-path wiring (EditorState + stepEditorFrame, GLFW-free) + the coordinate ratio fix (the cursor window->fb; the no-selection live defect found and fixed editor-side) + the status feedback (reloaded/reload failed/no selection) + --diag-input; 102 checks; confirmed by Cyril; **Step 296** option B step 2: the read-only inspector panel (the 36-line list = the 24 saver fields in the 4dp saver style, the "+N more" overflow, no selection -> "no selection"; the panel never edits; a click inside the panel rect keeps the selection; the panel is immune to pan/zoom) + the Direction Spec rotation correction (same field, axis per mode: z-spin 2D, yaw +Y 3D); 113 checks; recovered from the lock blocker via the git stash; **Step 297** option B step 4: Tab = next / Shift+Tab = previous navigation (wrapping, the dead skipped, the empty scene a safe no-op) + the inspector line cap 40 with '...' + the status tag cap + buildSampleSceneN (--make-sample [count], 0..10000) + --diag-input prints one line per nav key press; 151 checks; the 296 inspector visual CONFIRMED by Cyril (the polish: the big font + the crowded status line = cosmetic, recorded); the prompt's two-versions conflict reported (the scroll/toggle + the 2000-entity scene + the second mutation NOT implemented - a candidate for a later step if confirmed); **Step 298** docs only: the Editor-0/Tools slice **CLASSIFIED AND CLOSED** (the 26 FINDINGS entries = 17 existing capability, 6 fixed defect - all editor-side, 2 missing capability: the camera zoom API + the wheel/scroll, 1 cosmetic; the closed-slice summary in PURE_ENGINE_V3.md: what works / what is out / honest limits; the next lane is the human's choice; **Step 299** option C step 1: the **save-as** (Ctrl+S; the editor's first write path; a NEW file path under savedata/ via the auto-increment counter; the write-safety refusals: empty path, the source, an existing destination - the source NEVER overwritten, the round-trip proven first; save->load->save byte-identical; the status feedback saved/save failed); 167 checks; the save-as visual confirmation PENDING; **Step 300** option C step 2: the **nudge + modified state + reload guard** (the arrow keys move the selected entity 0.1 world units, Shift 1.0, world-space, pan/zoom-independent; the modified marker on the status line; the R guard: the first R asks, the second R within 120 frames discards and reloads, any other key clears; Ctrl+S after nudging writes the edited scene to the next savedata/_editN file, the source untouched; --diag-input prints one line per nudge press); 241 PASS selftest lines; the 299 save-as + the 300 nudge visual confirmations PENDING; **Step 301** option C step 3: the **bounded undo/redo** (Ctrl+Z/Ctrl+Y, Ctrl+Shift+Z; positions only, one entry per applied nudge, the cap 32 with the oldest falling off, the redo cleared by a new nudge, the stacks cleared by a reload and surviving a save, undo/redo never touch files; the modified flag honest vs the baseline captured at every successful load/save); 280 PASS selftest lines; the 299 + 300 + 301 visual confirmations PENDING; **Step 302** docs only: the **option-C slice (299-301) CLASSIFIED AND CLOSED** (the save-as NEW-file-only + the nudge + the modified/baseline + the R guard + the bounded undo/redo verified by the selftest (280 PASS) + the non-vacuous mutations; what is out: overwrite-source, path-switch-after-save, multi-field edit, gizmos, the panel scroll, the 2000 stress, the engine zoom/wheel; the 299/300/301 visuals PENDING; the next lane was the human's choice; **Step 303** (AUTHORIZED): the **path-switch-after-save** (on a successful Ctrl+S the editor follows the saved file - the loaded/source path becomes the NEW _editN file; a failed save changes nothing; the FLAT naming rule _edit1 -> _edit2 (no nesting); the write-safety preserved (the current path = the source refusal, the original = the existing-file refusal); the undo/redo stacks kept, cleared on reload); 315 PASS selftest lines; the 299/300/301 + 303 visual confirmations PENDING)):
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
