# PureEngine — Session Board

Per-session rows; update your row when you start/stop. Claims need real
command output behind them (see AGENTS.md reporting expectations).
Feature history stays in pure_engine_v3_steps.json + PURE_ENGINE_V3.md —
this board never replaces it. Highest step id: 180 (146 entries, 35–180,
no dups).

## Focus (binding for all sessions)
- **ENGINE-FIRST**: work lands in `src/` + `tests/`; games only on explicit request.
- **Main is the sole git commit/push.** Feature sessions implement → report;
  main pathspec-stages only the reported files → commit → board/step/README →
  sanity → push. No whole-repo `git add .`.
- input/audio/physics/render/scene = **parked unless assigned**.
- Secondary goal: **PureEditor-lite v0** (load/select/nudge/save — tooling on engine APIs; TOOLING CONTRACT in PURE_ENGINE_V3.md). Non-goals: hierarchy editor, animation studio, multiplayer, ECS.
- Goal2 Phase1: **3D math baseline LANDED** (Step 191: Mat4::perspective + lookAt already existed; 2D untouched) — editor-lite TOOL loop is next.
- Next active system: physics swept consumer on request — useSwept controller opt-in landed (Step 181); game wire still on request. WindowGuard bootstrap helper landed (Step 190, src/window_guard.h) — opt-in, no game adoption. Otherwise blank— all assigned campaigns COMPLETE:
  console Step 168, particles Step 169, time contract Step 170, lifecycle/init contract Step 171, gamepad-actions Step 172, core loop contract Step 174, swept-twin P7 Step 175, audio matrix 176, input adoption 177, scene matrix 178, render doc 179, resources lifetime 180).

| Session | Owns | Status | Last update | Notes |
|---------|------|--------|-------------|-------|
| PureEngine_main | board, trackers, packaging/docs, catch-up counts, ONLY committer + CONSOLE contract (assigned) | active | 2026-09-21 | Steps 164–167 processed (animation 2e11c78, followLerp feb7db3, resources 4df2842, events e4b9bbf); split-stages disclosed; followLerpOk chain gate fixed in 167. Now: CONSOLE contract (Step 168) assigned to main. |
| PureEngine_physics | physics.h, collision.h, simulation.h | parked | 2026-09-24 | P1–P6 + kinematic platform + SWEPT AABB PACKAGE COMPLETE (Step 173: SweepHit/sweptAABB slabs + chain gate, commit e24f2dc). API-only — no game wire yet. Disclosure: Step 172 commit 244c9b1 swept the physics test function+registration (concurrent-write race); completed honestly in 173. |
| PureEngine_input | input.h, gamepad.h | parked | 2026-09-21 | Campaign complete + union fix landed: `keysForAllActions()` includes Action::Console (default 12→13, remap test 13→14) — `2cacf5f`. Steps 154–159 recorded (`32125e9`…`54a7918`); Step 160 adoption + union recorded. Action::Console rebindable — campaign closed/parked. |
| PureEngine_resources | resources.h, resources_test.cpp, CMake test target | parked | 2026-09-22 | Campaign COMPLETE: Step 163 (hostile_data_test contract), Step 166 (CONTRACT block in resources.h + dedicated headless resources_test target, 41 assertions) — commit 4df2842. |
| PureEngine_audio | audio.h | parked | 2026-09-21 | A1–A5 landed (`58b5b7c`, `20d8f05`, `a1d348b`, `2a5a181`); queryable load state + pool rotation proven. Resource load system contract (Step 163) verified the blob/pack/cache half headlessly; audio's 3-probe + unified failure line share the same contract by construction. |
| PureEngine_render | renderer.h, camera.h, shader.h, lighting.h | COMPLETE | 2026-09-21 | Campaign 1-3 complete. Inc1: lighting.h doc header + MAX_LIGHTS cap (`af81e15`). Inc2: calculateFrameUV + addLight cap tests (`af8475b`). Inc3: stale Step-13 checklist doc fix (`5780309`). Build/CTest/alive green each step. Disclosure: first inc3 commit briefly swept owner-staged platformer.cpp; fixed same-session (reset --soft + pathspec commit), owner state restored, bad commit never pushed. Post-campaign gap CLOSED as Step 165: checkFollowLerp camera contract — commit feb7db3 (split-staged around events in-flight hunks; followLerpOk registered, chain-gating pending events commit). Render/camera campaign COMPLETE. Particles contract lock LANDED as Step 169: checkParticleContract (spawn guard, accumulator carry, life conversion, swap-with-back) — commit 67cc2c2, fresh sanity green before commit, no src changes. Particles contract COMPLETE under Render. Draw/submit contract locked (UNCOMMITTED, awaiting main commit): consolidated doc block before drawWorld in renderer.h (+37/-0) — submission split, F-04 flags 1:1, Step 49 stable depth sort, 113 dead-skip, 84 OOB fallback, 78 batching, 86 tint, 112 blend, plus honest headless-vs-GL-only verification split (GL-only items listed for the human/SMOKE gate); doc-only, zero behavior; build/CTest/alive green. |
| PureEngine_scene | scene.h, prefab.h, tilemap.h (persistence paths) + related tests | parked | 2026-09-21 | CAMPAIGN COMPLETE: Step 156 (manager save/spawn-after-kill/multi-spawn persist), Step 161 (`c8e40fc` load + manager round-trip), Step 162 (`a1824af` save path symmetry + Windows re-save rename fix + test 5e). Fresh sanity green before commit. Path symmetry beyond round-trip still optional later. |
| PureEngine_animation | animation.h, animation_data.h + animation tests | parked | 2026-09-21 | Campaign COMPLETE: Step 164 checkAnimationSystem (load/bind/update/switch/loop/end, 12 cases) — commit 2e11c78, no engine source changes. |
| PureEngine_events | events.h + event tests | parked | 2026-09-21 | Campaign COMPLETE: Step 167 checkEventReentrantOnceGaps (reentrant depth-3, mid-dispatch removal, once refusals/auto-removal/throw) + followLerpOk chain gate fixed — commit e4b9bbf. The Step 165 chain-gating gap is closed. |

## Animation later-not-started (Step 69 of the 80-step run)
- Multi-track clips and cross-clip blending: NOT started, not planned —
  additive engine-pure steps only if a session explicitly requests one.

## Input residual (Step 59 of the 80-step run)
- A runtime rebind (loadInputBindings AFTER Input construction) does NOT
  update the Input's tracked keys: level-only or newly-mapped keys need
  RE-ADOPT (reconstruct Input from keysForAllActions(), Step 177 helper).

## Double-init safety (Step 171 contract)
- SAFE: audio.init() (idempotent guard), audio.shutdown() (idempotent),
  renderer.destroyAll() (members zeroed -> deleting 0 is a no-op),
  frameTime.start()/tick() (stateless clock semantics).
- UNSAFE — call-site contract, call ONCE: renderer.init() (no guard;
  re-init leaks the shader program + textures), glfwInit() without a
  matching glfwTerminate().

## Hot files
- `tests/hostile_data_test.cpp` — active test author this hour only; never
  whole-stage while others have uncommitted hunks.
- `src/main.cpp` — declare on this board before edit.
- `Blueprint/pure_engine_v3_steps.json` — record owner declares before edit;
  main does catch-up counts after a session lands a record.
