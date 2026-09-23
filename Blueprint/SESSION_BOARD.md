# PureEngine — Session Board

Per-session rows; update your row when you start/stop. Claims need real
command output behind them (see AGENTS.md reporting expectations).
Feature history stays in pure_engine_v3_steps.json + PURE_ENGINE_V3.md —
this board never replaces it. Highest step id: 163 (129 entries, 35–163,
no dups).

## Focus (binding for all sessions)
- **ENGINE-FIRST**: work lands in `src/` + `tests/`; games only on explicit request.
- **Main is the sole git commit/push.** Feature sessions implement → report;
  main pathspec-stages only the reported files → commit → board/step/README →
  sanity → push. No whole-repo `git add .`.
- input/audio/physics/render/scene = **parked unless assigned**.
- Next active system: **resources/assets pipeline** (Step 163 landed the load
  system contract test; boundary verified, gaps were test gaps only).

| Session | Owns | Status | Last update | Notes |
|---------|------|--------|-------------|-------|
| PureEngine_main | board, trackers, packaging/docs, catch-up counts, ONLY committer | active | 2026-09-21 | Step 163 recorded (resource load system contract test, `bf45f1b` + record `fa568da`); tree clean at push. Report flow exercised: pathspec-stage → commit → record → sanity → push. |
| PureEngine_physics | physics.h, collision.h, simulation.h | parked | 2026-09-21 | P1–P6 landed (`df4ca52`, `c1919f6`, `3799fc9`) + P6-integration kinematic platform (`72af8f2`, lost content RESTORED as `8b8bfbd` statics-append — riders carried, controller treats platform as infinite mass). Kinematic carry complete at HEAD. Messages left as-is per no-rewrite ruling. |
| PureEngine_input | input.h, gamepad.h | parked | 2026-09-21 | Campaign complete + union fix landed: `keysForAllActions()` includes Action::Console (default 12→13, remap test 13→14) — `2cacf5f`. Steps 154–159 recorded (`32125e9`…`54a7918`); Step 160 adoption + union recorded. Action::Console rebindable — campaign closed/parked. |
| PureEngine_audio | audio.h | parked | 2026-09-21 | A1–A5 landed (`58b5b7c`, `20d8f05`, `a1d348b`, `2a5a181`); queryable load state + pool rotation proven. Resource load system contract (Step 163) verified the blob/pack/cache half headlessly; audio's 3-probe + unified failure line share the same contract by construction. |
| PureEngine_render | renderer.h, camera.h, shader.h, lighting.h | COMPLETE | 2026-09-21 | Campaign 1-3 complete. Inc1: lighting.h doc header + MAX_LIGHTS cap (`af81e15`). Inc2: calculateFrameUV + addLight cap tests (`af8475b`). Inc3: stale Step-13 checklist doc fix (`5780309`). Build/CTest/alive green each step. Disclosure: first inc3 commit briefly swept owner-staged platformer.cpp; fixed same-session (reset --soft + pathspec commit), owner state restored, bad commit never pushed. |
| PureEngine_scene | scene.h, prefab.h, tilemap.h (persistence paths) + related tests | parked | 2026-09-21 | CAMPAIGN COMPLETE: Step 156 (manager save/spawn-after-kill/multi-spawn persist), Step 161 (`c8e40fc` load + manager round-trip), Step 162 (`a1824af` save path symmetry + Windows re-save rename fix + test 5e). Fresh sanity green before commit. Path symmetry beyond round-trip still optional later. |

## Hot files
- `tests/hostile_data_test.cpp` — active test author this hour only; never
  whole-stage while others have uncommitted hunks.
- `src/main.cpp` — declare on this board before edit.
- `Blueprint/pure_engine_v3_steps.json` — record owner declares before edit;
  main does catch-up counts after a session lands a record.
