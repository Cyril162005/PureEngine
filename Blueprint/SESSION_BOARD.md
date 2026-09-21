# PureEngine — Session Board

Per-session rows; update your row when you start/stop. Claims need real
command output behind them (see AGENTS.md reporting expectations).
Feature history stays in pure_engine_v3_steps.json + PURE_ENGINE_V3.md —
this board never replaces it. Highest step id: 159 (125 entries, 35–159,
no dups).

| Session | Owns | Status | Last update | Notes |
|---------|------|--------|-------------|-------|
| PureEngine_main | board, trackers, packaging/docs, catch-up counts | active | 2026-09-21 | This pass: input marked complete (154–159), README/V3 counts synced to 159, SMOKE_TEST case count 78→103. Sweep-in precedents recorded in the step JSON (122/156). |
| PureEngine_physics | physics.h, collision.h, simulation.h | parked | 2026-09-21 | P1–P6 landed (`df4ca52`, `c1919f6`, `3799fc9`); restitution/fixed-substep contracts locked. |
| PureEngine_input | input.h, gamepad.h | COMPLETE | 2026-09-21 | Campaign complete: Steps 154–159 landed and recorded (`32125e9`, `482324e`, `2d7be33`, `d5c40cc`, `7428d77`; record `54a7918`). Action::Console rebindable — campaign closed. |
| PureEngine_audio | audio.h | parked | 2026-09-21 | A1–A5 landed (`58b5b7c`, `20d8f05`, `a1d348b`, `2a5a181`); queryable load state + pool rotation proven. |
| PureEngine_render | renderer.h, camera.h, shader.h, lighting.h | inc1 done | 2026-09-21 | Increment 1: Step 79 boundary doc header + MAX_LIGHTS cap named, zero behavior change (`af81e15`). |
| PureEngine_scene | scene.h, prefab.h, tilemap.h (persistence paths) + related tests | active | 2026-09-21 | Step 156 campaign landed (manager save, spawn-after-kill, multi-spawn persist — `842214e`/`60ac7d6`; manager-save swept into `c1919f6`, disclosed). Session's own increment 1 (tilemap.h `#ifndef NDEBUG` brace) claimed in progress — NOT visible in tree at this pass. Its proposed increments 2/3 overlap Step 156's already-landed coverage (checkSceneManagerSave = manager-save/round-trip). |

## Hot files
- `tests/hostile_data_test.cpp` — active test author this hour only; never
  whole-stage while others have uncommitted hunks.
- `src/main.cpp` — declare on this board before edit.
- `Blueprint/pure_engine_v3_steps.json` — record owner declares before edit;
  main does catch-up counts after a session lands a record.
