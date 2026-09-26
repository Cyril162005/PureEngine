# PureEngine — Manual Smoke-Test Checklist

**Human verification only — NOT CI.** The automated suite (`ctest`,
2 test binaries, 126 behavior cases across 119 + 7 test functions)
proves engine logic headlessly; this checklist covers everything that
needs eyes, ears, and hands on a real machine. Every item lists the
exact command/key and the expected observation. Checks that cannot be
automated (audible audio, mouse feel, resize behavior) are honestly
marked HUMAN-ONLY.

Machine state used for the last automated pass: Release build zero
errors, CTest 2/2 Passed, arcade/platformer/pong alive probes True.

---

## 1. Build & run

| # | Step | Expected |
|---|------|----------|
| 1.1 | `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` | Configure succeeds (first run clones GLFW/miniaudio/stb) |
| 1.2 | `cmake --build build --config Release` | Zero errors; all 7 targets produced |
| 1.3 | `build\Release\PureEngine.exe` | Window opens: purple MENU, instruction text |
| 1.4 | `build\Release\Platformer.exe` | Window opens: TITLE screen |
| 1.5 | `build\Release\Pong.exe` | Window opens: paddles + ball |

## 2. Arcade MENU — keyboard + mouse (Steps 82/121)

| # | Step | Expected |
|---|------|----------|
| 2.1 | SPACE in MENU | Game starts (PLAYING, survival timer runs) — HUMAN |
| 2.2 | `2` in MENU | Alternate scene starts (orange hostile variant) — HUMAN |
| 2.3 | Click the **START** button (below the instruction text) | Same as SPACE: game starts — HUMAN (Step 121) |
| 2.4 | Click the **ALT** button | Same as key 2: alternate scene starts — HUMAN (Step 121) |
| 2.5 | Keyboard paths still work after the buttons exist | SPACE/2 fire exactly as before (buttons are additive) — HUMAN |

## 3. Audio — music, SFX, mute (Steps 20/36/75/80/98/119/123/124)

| # | Step | Expected |
|---|------|----------|
| 3.1 | Launch Arcade (MENU) | Quiet 4 s ambient loop starts and repeats seamlessly (no click at the loop point) — HUMAN-ONLY (automated proof is the absent `load failure` log line, Step 123) |
| 3.2 | Press **M** during PLAYING | ALL audio silences — HUMAN (Step 124) |
| 3.3 | Press **M** again | Previous volume restored — HUMAN (Step 124) |
| 3.4 | Get caught by a hostile | Collision beep + game-over cue; music stops on returning to MENU (Step 119 stop path) — HUMAN |
| 3.5 | Beat the high score | NEW HIGH SCORE cue + saved message — HUMAN |
| 3.6 | `volume 0.5` in console | Master gain drops (SFX quieter) — HUMAN |
| 3.7 | Non-interactive (no launch) | `ctest` covers the audio mechanics headlessly: volume clamp/composition, mute, pre-init guards, and — when a playback device exists — init/music/lifecycle and the 4-slot rotation. Games may also query `audio.isLoaded(Sound)`/`audio.isMusicLoaded()` instead of trusting startup |

## 4. Debug console (backtick opens; ENTER submits)

Game commands registered in the Arcade (main.cpp): `entities`, `reset`,
`spawn_prefab [file]`, `volume [0-1]`, `mute`, `pick`, `blob [file]`,
`blobclear`, `health [index]`, `damage <n> [index]`, `heal <n> [index]`,
`scene_dump`, `scene_reload`, `textures`. Built-ins: `help`, `clear`,
`echo`.

| # | Step | Expected |
|---|------|----------|
| 4.1 | `` ` `` then `help` | Command list prints — HUMAN |
| 4.2 | `entities` | Entity count (7 in the default arena) — HUMAN |
| 4.3 | `pick` (cursor over an entity) | `mouse (x, y) world (x, y) -> index N role R tag '...' aabb half (hX, hY)`; cursor over empty space → `none` — HUMAN (Steps 127–129 logic proven in CI; the typing path is HUMAN) |
| 4.4 | `blob` | `blob 'beep.wav': 13274 bytes (loaded)`; run again → `(cache hit)`; `blobclear` → cleared — HUMAN (Step 101/131) |
| 4.5 | `health` / `damage 30` / `heal 30` | `entity 0: health 100...` → `100 -> 70` → `70 -> 100` — HUMAN (Step 103/132) |
| 4.6 | `spawn_prefab` | `Spawned 'enemy' at (0,0,0)`; the spawned hostile animates (walk clip) and joins the chase — HUMAN (Steps 120/122) |
| 4.7 | `textures` | `textures: 5 -> registered id 5 -> 6 -> unloaded 1 non-core -> 5` — HUMAN (Steps 89/94/133) |
| 4.8 | `mute` in console | `muted` / `unmuted` + master value — HUMAN (Step 124) |
| 4.9 | `reset` | World restored (spawned entities gone, health back) — HUMAN |

## 5. Window resize (Step 116)

| # | Step | Expected |
|---|------|----------|
| 5.1 | Drag the Arcade window edge briefly | World stays centered, vertical extent locked (±4.5), horizontal scales with aspect — no stretching — HUMAN (automated proof: `checkScreenToWorld` resize numbers, Step 125) |
| 5.2 | Resize while PLAYING | Camera follow + HUD remain correct at the new size — HUMAN |
| 5.3 | Minimize/restore | No crash, no division-by-zero (degenerate size is a no-op) — HUMAN |

## 6. Platformer & Pong quick pass

| # | Step | Expected |
|---|------|----------|
| 6.1 | Platformer: move/jump/land, reach goal | L1 → L2 → WIN — HUMAN (48-case CI suite covers controller logic) |
| 6.2 | Platformer: `` ` `` console (`entities`, `pos`, `reset`) | Commands respond — HUMAN |
| 6.3 | Pong: W/S + Up/Down paddles, ESC quits | Paddles move, quit works — HUMAN |

## 7. Draw/submit GL-only gate (Step 179 renderer contract — HUMAN-ONLY)

The Step 179 contract splits verification honestly: these behaviors are
NOT headless-testable without a GL context — they must be claimed from a
real window, never from CI alone.

| # | Step | Expected |
|---|------|----------|
| 7.1 | Arcade PLAYING, hostile chase | Batch correctness: sprites share one upload per same-texture group, no seams or flicker (Step 78) — HUMAN-ONLY |
| 7.2 | Watch depth ordering | Entities draw back-to-front by depth; ties keep construction order; colliding[] highlight stays on the right entity (Step 49) — HUMAN-ONLY |
| 7.3 | Kill an entity (console `damage 100`) | Dead entity draws nothing immediately (Step 113) — HUMAN-ONLY |
| 7.4 | Console `textures` then give an entity an out-of-range textureId | Checker fallback visual + warn-once log in debug builds (Step 84) — HUMAN-ONLY |
| 7.5 | Runtime-unload a slot an entity still references (`textures` unload path) | Released slot (kept, GL name 0) also falls back to checker — no undefined texture content (Step 180) — HUMAN-ONLY |
| 7.6 | Blend visuals | Semi-transparent sprites blend over the scene; opaque sprites unaffected (Step 112) — HUMAN-ONLY |
| 7.7 | Tint visuals | Per-entity tint (white default); colliding flag overrides red (Step 86) — HUMAN-ONLY |
| 7.8 | F-04 assert | entities.size() == colliding.size() holds every frame — a mismatch would abort (asserted every drawWorld call; the absence of an abort IS the pass evidence) — HUMAN-ONLY |
| 7.9 | Opt-in 3D debug mesh (Step 194) | **HOW TO INVOKE (Step 202): launch Arcade → press backtick (`` ` ``) to open the console → type `debug3d on` → ENTER.** The unit cube appears over the 2D frame (perspective + depth composition); **bind an entity live (Step 212): `meshid <index> <id>` (e.g. `meshid 2 1`) → a cube appears at that entity's position (`drawEntity3D`)**; **move the camera (Step 213): `debug3d cam <ex> <ey> <ez> <tx> <ty> <tz>` (e.g. `debug3d cam 3 2 5 0 0 0`) → the cube renders from a new angle**; `debug3d off` → 2D-only rendering restored exactly (the Step 193/196 mode round-trip). The CPU-side data (36×5 vertex layout, unit bounds, translation builder) is CI-proven — the on-screen pixels are HUMAN-ONLY |
| 7.10 | 3D depth occlusion (Step 195) | **Invoke as in 7.9** (`debug3d on`): with two overlapping 3D debug draws, front faces hide back faces (depth test on during the 3D path). CI-proven: the depth-test enable state MATCHES the pre-call state after the draw and the 2D pass leaves depth OFF (hidden-window GL context, `checkDepthState`) — the on-screen occlusion itself is HUMAN-ONLY |
| 7.11 | Debug-frame depth clear (Step 201) | **Invoke as in 7.9** (`debug3d on`): the opt-in `clearDepth` clear touches ONLY the depth buffer — the 2D frame's color content survives underneath (a leak would erase every 2D frame). CI-proven: a corner pixel outside the cubes stays the seeded color after every depth-only clear and the wiped depth lets the farther cube pass (hidden-window GL, `checkDebugFrameDepth`) — on-screen occlusion under real draws is HUMAN-ONLY |

---

**Rule:** an item not performed is UNVERIFIED — never claimed. The
automated suite is the regression gate; this checklist is the release
gate.
