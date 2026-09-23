# PureEngine — Session Board

Per-session rows; update your row when you start/stop. Claims need real
command output behind them (see AGENTS.md reporting expectations).
Feature history stays in pure_engine_v3_steps.json + PURE_ENGINE_V3.md —
this board never replaces it. Highest step id: 159 (125 entries, 35–159,
no dups).

| Session | Owns | Status | Last update | Notes |
|---------|------|--------|-------------|-------|
| PureEngine_main | board, trackers, packaging/docs, catch-up counts, ONLY committer | active | 2026-09-21 | Input FINISHED REPORT processed: union fix split-staged (only union hunks; scene's in-flight round-trip hunks left unstaged), committed `2cacf5f`, pushed. Step 160 recorded (126 entries, 35–160). Working-tree CTest fails ONLY in scene's unstaged checkSceneManagerRoundTrip — not in committed state, unrelated to input fix. |
| PureEngine_physics | physics.h, collision.h, simulation.h | parked | 2026-09-21 | P1–P6 landed (`df4ca52`, `c1919f6`, `3799fc9`) + P6-integration kinematic platform (`72af8f2`, lost content RESTORED as `8b8bfbd` statics-append — riders carried, controller treats platform as infinite mass). Kinematic carry complete at HEAD. Messages left as-is per no-rewrite ruling. |
| PureEngine_input | input.h, gamepad.h | parked | 2026-09-21 | Campaign complete + union fix landed: `keysForAllActions()` includes Action::Console (default 12→13, remap test 13→14) — `2cacf5f`. Steps 154–159 recorded (`32125e9`…`54a7918`); Step 160 adoption + union recorded. Action::Console rebindable — campaign closed/parked. |
| PureEngine_audio | audio.h | parked | 2026-09-21 | A1–A5 landed (`58b5b7c`, `20d8f05`, `a1d348b`, `2a5a181`); queryable load state + pool rotation proven. |
| PureEngine_render | renderer.h, camera.h, shader.h, lighting.h | COMPLETE | 2026-09-21 | Campaign 1-3 complete. Inc1: lighting.h doc header + MAX_LIGHTS cap (`af81e15`). Inc2: calculateFrameUV + addLight cap tests (`af8475b`). Inc3: stale Step-13 checklist doc fix (`5780309`). Build/CTest/alive green each step. Disclosure: first inc3 commit briefly swept owner-staged platformer.cpp; fixed same-session (reset --soft + pathspec commit), owner state restored, bad commit never pushed. |
| PureEngine_scene | scene.h, prefab.h, tilemap.h (persistence paths) + related tests | parked | 2026-09-21 | CAMPAIGN COMPLETE: Step 156 (manager save/spawn-after-kill/multi-spawn persist), Step 161 (`c8e40fc` load + manager round-trip), Step 162 (`a1824af` save path symmetry + Windows re-save rename fix + test 5e). Fresh sanity green before commit. Path symmetry beyond round-trip still optional later. |

## Hot files
- `tests/hostile_data_test.cpp` — active test author this hour only; never
  whole-stage while others have uncommitted hunks.
- `src/main.cpp` — declare on this board before edit.
- `Blueprint/pure_engine_v3_steps.json` — record owner declares before edit;
  main does catch-up counts after a session lands a record.
