# PureEngine — Game Build Log (v2)

## Context
Engine (PureEngine, 12/12 steps) is complete, verified, and pushed — see `PURE_ENGINE.md` /
`pure_engine_steps.json`. This tracker covers the actual GAME built on top of it, starting
from the v1 slice shipped in Step 12: one player, one hostile, one lose condition, a
survival timer, MENU/PLAYING/PAUSED/GAME_OVER states.

## Goal
Genre: top-down arena survival.
Core loop: avoid/outlast hostile entities in a bounded arena; survive as long as possible.
v1 proved the loop is mechanically sound. v2 makes it a real, replayable game.

## Rule
Same as the engine: no step starts until the previous one is verified by Cyril, committed,
and pushed. No feature added without checking whether it reuses an existing engine system
(entity vector, AABB collision, sound pool, state machine) before writing anything new.

## Phases

### Phase 1 — Multiple Hostiles
**Status:** Completed
**Goal:** More than one hostile entity, spawned as data (push_back), not new code paths.
**Definition of done:** At least 2-3 hostiles simultaneously chase the player; losing to any one of them ends the run the same way v1's single hostile does.
**Notes:**
- Count ruling: THREE hostiles total (2 added). Every hostile stays slower than the player (1.8/1.6/1.5 vs 2.5 units/s), so a straight-line escape wins against pure pursuers regardless of count — the Step 12 'avoidable by construction' guarantee holds; three creates a pincer problem without compressing escape corridors into a spawn lottery. Performance: six extra trivial ops/frame — unmeasurable.
- Variation via per-entity DATA only (Step 7 pattern): parallel const float hostileSpeeds[] {1.8, 1.6, 1.5} mapped to entities[3..5] by index h-3; individual spins (-1.2 rad/s CW, 2.2 rad/s CCW alongside the original 1.8 CCW); spawn points split the compass around the player — bottom (0,-2) unchanged, top-right (3,2), top-left (-3,2). The spread makes them straggle rather than arrive as a wall.
- Container/collision exactly as Step 12 established: same entity vector (push_backs BEFORE the initialEntities snapshot); the 3-entity scenery pair loop bound stayed literally untouched (hostiles at indices 4-5 pass through scenery and each other); catch test generalized to a loop over the hostile range OR-ing Step 8's aabbOverlap into one bool — touching ANY hostile ends the run through the IDENTICAL Step 12 death block (console print, pool beep, state flip last).
- resetGame() needed ZERO changes — the snapshot/restore pattern absorbed the two new entities for free (colliding/wasColliding are size-driven). Strongest demonstration yet of Step 7's 'entities as data' ruling.
- No CMakeLists.txt change (git diff proved it byte-identical) — pure logic; gamestate.h also untouched (no new states needed, which is evidence Step 11 got the design right).
- Build note: full build-folder delete + fresh configure (493.7 s, all deps re-cloned) + Release build clean FIRST TRY; forced main.cpp recompile confirmed zero warnings in engine code.
- VERIFIED by the user directly: three hostiles spawn at distinct points, track independently at different speeds, pass through scenery and each other without interaction, losing to any hostile works identically to v1 (console print, beep, dark red freeze), all Step 1-12 behavior intact including all six entities freezing correctly during PAUSED and full reset restoring all three hostiles to spawn points.

### Phase 2 — Difficulty Scaling
**Status:** Completed
**Goal:** The game gets harder the longer you survive — hostile speed increases over time, and/or new hostiles spawn periodically.
**Definition of done:** A run that lasts 30+ seconds is measurably harder than the first 5 seconds, driven by deltaTime-based state, not fixed per-frame constants.
**Notes:**
- Flavor ruling: the LOW-RISK variant — existing hostiles speed up; no new entities, no vector growth mid-run, no stress on the snapshot pattern. hostileSpeeds[] became a const BASE array; scaling is applied at usage time in the chase loop, never written back.
- Formula: effectiveSpeed = baseSpeed * min(1 + survivalTime * difficultyRate, maxDifficultyScale) with difficultyRate = 0.01 (+1%/s of PLAYED time) and maxDifficultyScale = 1.33. Ramp: 10 s -> +10%, cap binds at 33 s; fastest hostile tops out at 1.8 * 1.33 = 2.394 < the player's 2.5 — the Step 12 'avoidable by construction' guarantee holds at ANY run length (endgame = routing skill vs a constant ceiling, not a death spiral).
- Driver: Step 12's survivalTime reused — no new timer. Accumulated only inside the PLAYING gate, so pausing freezes difficulty exactly like the clock (no wall-clock leak); resetGame() needed ZERO changes (its existing survivalTime = 0 reset is the complete fix — the scale is a pure function recomputed each frame, no stored state).
- Only new include: <algorithm> for std::min. CMakeLists.txt, gamestate.h, entity.h, collision.h all unchanged (empty git diff proved it) — pure logic.
- BALANCE TUNING shipped in the same commit (not a separate phase — feel fixes to Phase 1, bundled into one build/verify cycle): (1) arena widened — ortho box (-4..4, -3..3) -> (-6..6, -4.5..4.5); still exactly 4:3 (12:9 = 800:600), no stretching, world unit drops 100 -> 66.7 px so everything renders at 2/3 pixel size. (2) hostile hitboxes tightened — the three hostiles pass explicit halfExtents (0.5, 0.5) (the triangle's base half-width) instead of the Step 8 default 0.7071: DISCLOSED DEVIATION — 0.7071 exists for fairness on rotating scenery (no blind spot at any spin angle), a guarantee chasing hostiles don't need; the tighter box makes visual contact and actual death agree (kill distance 1.4142 -> 1.2071, ~15% grace). Player and the 3 scenery hitboxes stay untouched at 0.7071 — scenery tint/beep confirmed unchanged. No hostile speed change, no proximity warning — deliberately deferred pending this test.
- Build note: full build-folder delete + fresh configure (495.1 s, all deps re-cloned) + Release build clean; zero warnings in engine code (single C4244 inside third-party miniaudio, known since Step 9).
- VERIFIED by the user directly: wider arena with correct 4:3 proportions and no stretching; tighter hostile hitboxes correlate visual near-misses with survival while the original 3 scenery hitboxes remain untouched (still tint/beep correctly); difficulty ramps hostile speed correctly over the first ~30 seconds and caps as designed; pausing does not leak extra difficulty from wall-clock time; a new game resets hostiles to base speed.

### Phase 3 — On-Screen Text (Score/Timer Display)
**Status:** Not started
**Goal:** Replace the console-only survival time with an actual on-screen number. This is the first genuinely new engine capability since Step 12 — no font/text rendering exists yet.
**Definition of done:** Survival time is visible on screen during PLAYING and on GAME_OVER, without needing to check the console.
**Notes:**
-

### Phase 4 — Scoring Beyond Survival Time
**Status:** Not started
**Goal:** Decide whether survival time alone is the score, or whether something else (hostiles avoided, near-misses, etc.) adds depth.
**Definition of done:** A defined, displayed score system beyond a raw timer — or a deliberate decision that the timer alone is enough, recorded here either way.
**Notes:**
-

### Phase 5 — Content Pass
**Status:** Not started
**Goal:** Replace placeholder checkerboard triangles with actual game-appropriate visuals (still simple — this is not an art-heavy project).
**Definition of done:** Player, hostiles, and arena are visually distinct from each other and from the Step 4-10 placeholder geometry.
**Notes:**
-

### Phase 6 — Packaging
**Status:** Not started
**Goal:** A build that runs on a clean machine without the dev environment (Visual Studio, CMake) installed.
**Definition of done:** The exe + assets can be zipped and run elsewhere, or a documented build process lets someone else compile it from a clean checkout.
**Notes:**
-

## Kill Criteria
If a phase's scope keeps expanding instead of shrinking, stop, cut scope, and re-record a
smaller definition of done here before continuing. Same discipline as the engine tracker —
this file is not exempt just because it's "the fun part."
