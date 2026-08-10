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
**Status:** Not started
**Goal:** More than one hostile entity, spawned as data (push_back), not new code paths.
**Definition of done:** At least 2-3 hostiles simultaneously chase the player; losing to any one of them ends the run the same way v1's single hostile does.
**Notes:**
-

### Phase 2 — Difficulty Scaling
**Status:** Not started
**Goal:** The game gets harder the longer you survive — hostile speed increases over time, and/or new hostiles spawn periodically.
**Definition of done:** A run that lasts 30+ seconds is measurably harder than the first 5 seconds, driven by deltaTime-based state, not fixed per-frame constants.
**Notes:**
-

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
