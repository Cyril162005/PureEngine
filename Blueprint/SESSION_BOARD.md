# PureEngine — Session Board

Per-session rows; update your row when you start/stop. Claims need real
command output behind them (see AGENTS.md reporting expectations).
Feature history stays in pure_engine_v3_steps.json + PURE_ENGINE_V3.md —
this board never replaces it. Highest step id: 267 (267 entries, 25–264,
no dups).

## Focus (binding for all sessions)
- **ENGINE-FIRST**: work lands in `src/` + `tests/`; games only on explicit request.
- **Main is the sole git commit/push.** Feature sessions implement ? report;
  main pathspec-stages only the reported files ? commit ? board/step/README ?
  sanity ? push. No whole-repo `git add .`.
- input/audio/physics/render/scene = **parked unless assigned**.
- Secondary goal: **PureEditor-lite v0** (load/select/nudge/save — tooling on engine APIs; TOOLING CONTRACT in PURE_ENGINE_V3.md). Non-goals: hierarchy editor, animation studio, multiplayer, ECS.
**editor-lite v0 headless path LANDED** (Step 197: TOOL loop green) — Goal2 Phase1: **3D math baseline LANDED** (Step 191: Mat4::perspective + lookAt already existed; 2D untouched) + **camera 3D mode LANDED** (Step 193: setPerspective/setOrthographicMode, default ortho, games unchanged) — editor-lite TOOL loop is next.
- Next active system: physics swept consumer on request — useSwept controller opt-in landed (Step 181); game wire still on request. WindowGuard bootstrap helper landed (Step 190, src/window_guard.h) — opt-in, no game adoption. Otherwise blank— all assigned campaigns COMPLETE:
  console Step 168, particles Step 169, time contract Step 170, lifecycle/init contract Step 171, gamepad-actions Step 172, core loop contract Step 174, swept-twin P7 Step 175, audio matrix 176, input adoption 177, scene matrix 178, render doc 179, resources lifetime 180).

| Session | Owns | Status | Last update | Notes |
|---------|------|--------|-------------|-------|
| PureEngine_main | board, trackers, packaging/docs, catch-up counts, ONLY committer + CONSOLE contract (assigned) | active | 2026-10-02 | Steps 233–264 processed: sandbox resolve pass (e37c5ee), nohostiles debug toggle (aa161e0), resting-contact + inverted-approach fix + resolve-before-draw (63b1943), drop display tuning (a0f3240), docs freeze + tracker reconciliation (4f7e226/057fe33), first real 3D mesh load (fd4949b), Pong ball-stick fix (97faa9a), Pong grind fix (595af60), Pong scoring fix (c7e1ef1), Pong live goal bound (f70066c), Pong score diagnosis (3896ee2), font digit divisor 11->37 fix (9167e14: the HUD digits were 3.4-cell mashes since the Step-66 atlas extension - the Step-243 root cause of unreadable scores), mesh registry (82234d5: meshId -> loaded OBJ map, multiple slots, clearRegisteredMeshes, tetra under two ids), textured loaded mesh (2dbde7e: per-face planar UVs - the registered meshes sample the diffuse checker), textureId sampling on registered meshes (edd335c: valid -> the slot texture per the 2D rule, else checker; meshId 1/2 never read textureId), per-face directional lighting (ede9dda: outward normals + fixed light + ambient, CPU per-face, no shader change), depth status confirmed (248: verify-only, no code - the same 195/201 pattern; the always-clear deviation disclosed), single depth clear per 3D pass (a561448: the Step 201 opt-in, inter-entity occlusion proven), entity-mesh depth round-trip CI (a1dd308: checkMeshDepthState, the 248 disclosure closed), second real OBJ (1433a6f: the wedge loaded at init, meshId 5), texid command (7506a75: the meshid twin for the texture slot, parseIndexId reused), 3D capability map (253: docs-only - the map + the open findings classified + the 3D done-criteria), finding (b) reclassified (254: missing capability - no cross-path contract; the 3D scale-only convention is CI-proven; deferred to the next version; (c) blocks freeze until deferred), finding (c) resolved (255: checkMeshStateLeak - the state-leak CI, all five items green; the done-criteria blockers now (c) resolved + (a)/(b) deferred), 3D ENGINE VERSION COMPLETE/FROZEN (256: all six done-criteria VERIFIED, fresh gates exit 0; deferred candidates: (a) world-space lighting, (b) 3D sizing source of truth), Phase D consumer (257: the adversarial composition of the frozen APIs - the contract held end-to-end, no engine edits needed), mutation audit (258: verify-only - all four asserts NON-VACUOUS, the mutation table in the V3 findings log), ENGINE v4 OPEN (259: "Engine v4 - 3D correctness" - scope exactly (a) world-space lighting + (b) 3D sizing source of truth; the v3 contract amended only at those two), (a) world-space lighting RESOLVED (260: the inverse-transpose of R*S, model-scale, bit-identical at rotation 0; both mutations caught), (b) 3D sizing RESOLVED (261: the model scale = halfExtents*scale*2, the 2D AABB expression; the lit normals normalized (the diffuse size-invariant); both mutations caught), v4 COMPLETE/FROZEN (263: the done-criteria audit 10/10 VERIFIED; the pre-freeze mutation closure 262 closed the weak points; the 2D AABB debug-draw 2x overscale recorded as the ENGINE DEFECT, candidate v5), consumer refreshed on frozen v4 (264: the sized+rotated composition, all three mutations caught first-try), docs consolidated (265: the step counts, the known limits, the stale facts swept), health gate green + recommended stop (266), ENGINE v5 OPEN (267: "Engine v5 - 2D debug-draw correctness" - scope exactly (c) 2D AABB debug-draw size vs collision size; v4 stays frozen). ctest 2/2 + alive x3 each step. Resolved: drawn mesh size = collision extents (261, the (b) sizing source of truth). |
| PureEngine_physics | physics.h, collision.h, simulation.h | parked | 2026-09-24 | P1–P6 + kinematic platform + SWEPT AABB PACKAGE COMPLETE (Step 173: SweepHit/sweptAABB slabs + chain gate, commit e24f2dc). API-only — no game wire yet. Disclosure: Step 172 commit 244c9b1 swept the physics test function+registration (concurrent-write race); completed honestly in 173. |
| PureEngine_input | input.h, gamepad.h | parked | 2026-09-21 | Campaign complete + union fix landed: `keysForAllActions()` includes Action::Console (default 12?13, remap test 13?14) — `2cacf5f`. Steps 154–159 recorded (`32125e9`–`54a7918`); Step 160 adoption + union recorded. Action::Console rebindable — campaign closed/parked. |
| PureEngine_resources | resources.h, resources_test.cpp, CMake test target | parked | 2026-09-22 | Campaign COMPLETE: Step 163 (hostile_data_test contract), Step 166 (CONTRACT block in resources.h + dedicated headless resources_test target, 41 assertions) — commit 4df2842. |
| PureEngine_audio | audio.h | parked | 2026-09-21 | A1–A5 landed (`58b5b7c`, `20d8f05`, `a1d348b`, `2a5a181`); queryable load state + pool rotation proven. Resource load system contract (Step 163) verified the blob/pack/cache half headlessly; audio's 3-probe + unified failure line share the same contract by construction. |
| PureEngine_render | renderer.h, camera.h, shader.h, lighting.h | COMPLETE | 2026-09-21 | Campaign 1-3 complete. Inc1: lighting.h doc header + MAX_LIGHTS cap (`af81e15`). Inc2: calculateFrameUV + addLight cap tests (`af8475b`). Inc3: stale Step-13 checklist doc fix (`5780309`). Build/CTest/alive green each step. Disclosure: first inc3 commit briefly swept owner-staged platformer.cpp; fixed same-session (reset --soft + pathspec commit), owner state restored, bad commit never pushed. Post-campaign gap CLOSED as Step 165: checkFollowLerp camera contract — commit feb7db3 (split-staged around events in-flight hunks; followLerpOk registered, chain-gating pending events commit). Render/camera campaign COMPLETE. Particles contract lock LANDED as Step 169: checkParticleContract (spawn guard, accumulator carry, life conversion, swap-with-back) — commit 67cc2c2, fresh sanity green before commit, no src changes. Particles contract COMPLETE under Render. Draw/submit contract locked (UNCOMMITTED, awaiting main commit): consolidated doc block before drawWorld in renderer.h (+37/-0) — submission split, F-04 flags 1:1, Step 49 stable depth sort, 113 dead-skip, 84 OOB fallback, 78 batching, 86 tint, 112 blend, plus honest headless-vs-GL-only verification split (GL-only items listed for the human/SMOKE gate); doc-only, zero behavior; build/CTest/alive green. |
| PureEngine_scene | scene.h, prefab.h, tilemap.h (persistence paths) + related tests | parked | 2026-09-21 | CAMPAIGN COMPLETE: Step 156 (manager save/spawn-after-kill/multi-spawn persist), Step 161 (`c8e40fc` load + manager round-trip), Step 162 (`a1824af` save path symmetry + Windows re-save rename fix + test 5e). Fresh sanity green before commit. Path symmetry beyond round-trip still optional later. |
| PureEngine_animation | animation.h, animation_data.h + animation tests | parked | 2026-09-21 | Campaign COMPLETE: Step 164 checkAnimationSystem (load/bind/update/switch/loop/end, 12 cases) — commit 2e11c78, no engine source changes. |
| PureEngine_events | events.h + event tests | parked | 2026-09-21 | Campaign COMPLETE: Step 167 checkEventReentrantOnceGaps (reentrant depth-3, mid-dispatch removal, once refusals/auto-removal/throw) + followLerpOk chain gate fixed — commit e4b9bbf. The Step 165 chain-gating gap is closed. |

## Animation later-not-started (Step 69 of the 80-step run)
- Multi-track clips and cross-clip blending: NOT started, not planned —
  additive engine-pure steps only if a session explicitly requests one.

## Physics wiring (Step 208 finding RESOLVED: options A + B, Steps 209+220)
- DONE: Pong ball-paddle through resolveCollision (kinematic paddles,
  ball e=1.0).
- DONE: Platformer ice (cell 2, friction 0) + bouncy (cell 3,
  restitution 1) tiles through the controller resolve path (220).
- COMPLETION EVIDENCE (Step 236 reconciliation — 224's "208 CLOSED" was
  premature): Step 226 (Cyril's playtest verdict on the tiles) plus
  Step 235 (Cyril PASS on the drop resting-contact fix). The sandbox
  drop now bounces and settles (RESTING_VEL 0.5, resolve before draw).

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
