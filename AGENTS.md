# PureEngine

This repository is a compact, single-developer C++ OpenGL game engine and arcade survival game. The goal is to keep the project simple, incremental, and verifiable rather than expanding into a larger engine framework.

## Working rules

- One step per turn. Stop and report before starting the next step.
- Search for the relevant subsystem before writing new code.
- Prefer the smallest, existing boundary that owns the behavior you are changing.
- Do not trust `Blueprint/*.md` or `Blueprint/*.json` without checking the actual source and build output.
- Do not introduce new frameworks, architecture layers, or tracker schemas unless the current task truly requires them.
- Keep changes local, explicit, and consistent with the current step-based organization.

## Status and source of truth

- Engine work is tracked across the `Blueprint/` documents, but source and build output are the authoritative truth.
- The current implementation is organized as a set of engine boundaries in `src/` rather than a monolithic main file.
- The project is still in active development; treat tracker files as hints, not guarantees.

## Build and verification

Run everything from a Visual Studio 2022 Developer PowerShell or Developer Command Prompt:

```bat
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
build\Release\PureEngine.exe
```

For every claim such as "works," "passes," or "fixed," include the actual command and the relevant output or exit code. No evidence means "unverified."

## Project conventions

- This project is Windows-first and CMake-driven.
- Dependencies are fetched in `CMakeLists.txt` (GLFW, miniaudio, stb) and are expected to stay minimal.
- The game builds as a standalone executable; do not add unnecessary runtime dependencies.
- Asset-generation scripts in the repo root produce committed assets in `assets/` and should be respected if an asset change is involved.
- `savedata/` is runtime output and may be created or updated during testing; do not treat it as source code.

## Expected code structure

- `src/main.cpp`: orchestration and game loop
- `src/renderer.h`: rendering boundary
- `src/shader.h`: shader loading boundary
- `src/lighting.h`: 2D lighting state boundary
- `src/resources.h`: resource loading boundary
- `src/camera.h`: camera boundary
- `src/input.h`: input boundary
- `src/engine_time.h`: timing boundary
- `src/lifecycle.h`: entity lifecycle boundary
- `src/audio.h`: audio boundary
- `src/ui.h`: HUD/UI boundary
- `src/simulation.h`: simulation logic boundary
- `src/entity.h`, `src/collision.h`, `src/gamestate.h`: core data and state logic
- `src/hostile_data.h`: hostile definition data boundary
- `src/physics.h`: physics (gravity, impulse resolve, statics, character controller)
- `src/tilemap.h`: tilemap load/convert/collide boundary
- `src/scene.h`: scene + SceneManager boundary
- `src/font.h`: bitmap-text logic (cell map, metrics)
- `src/events.h`: event bus boundary
- `src/console.h`: debug console boundary
- `src/gamepad.h`: gamepad snapshot boundary
- `src/particles.h`: particle pool/emitter boundary
- `src/components.h`: lightweight component helpers boundary (health/tag/timer/velocity)
- `src/prefab.h`: prefab/template boundary (loadPrefab/instantiatePrefab)
- `src/animation.h`, `src/animation_data.h`: animation clips + file loading
- `src/math/`: custom math layer
- `src/stb_impl.cpp`: stb implementation unit
- `games/pong/pong.cpp`, `games/platformer/platformer.cpp`: second/third games (own CMake targets)

## Reporting expectations

- State failures first, plainly, before summarizing any success.
- If a fix attempt failed, say so on that attempt instead of silently continuing.
- "Complete" means the work was actually executed and inspected, not merely written.
- If a check cannot be run, say "unverified" rather than guessing.
- If the request is ambiguous, state the interpretation in one line and continue with that assumption.

## Standing priority (set Step 45 onward)

Focus is on **engine internals**, not gameplay features or content. Gameplay work is only justified when it is explicitly testing whether the engine holds up under real conditions (e.g. stress-testing entity count, verifying a module boundary actually holds under load). Do not propose new game content, difficulty tiers, or gameplay mechanics unless the current session explicitly requests one and cites a concrete engine-validation reason.

## v1.1 freeze guardrails (Step 142 — binding on all future sessions)

PureEngine v1.1 is FROZEN (Step 141 confirmation). The engine owns mechanisms; the game owns the loop. The following are **OUT — do not plan, do not start, do not "accidentally" grow**:

- **No ECS** — no archetype storage, no component scheduler, no query language. Entities stay plain structs (Step 7/103 ruling).
- **No editor** — no in-game editing tools in the ENGINE. AMENDED Step
  284 (2026-10-03): an editor/tool is a CONSUMER of the frozen engine
  (own directory, own CMake target, documented APIs only, engine src/
  read-only during any Tools step) — Editor/tools phase ALLOWED,
  explicit allow by Cyril; the engine itself still ships no editing
  tools and no hot-reload framework.
- **No networking** — no sockets, no multiplayer, no sync layers.
- **No 3D** — flat 2D only (see PURE_ENGINE_V3.md 2D/3D architecture notes; the 3D contracts landed via the v3/v4/v5 versions — this line is superseded by those frozen contracts, kept for history).
- **No retained-mode UI system** — no widget tree, no layout engine, no focus system IN THE ENGINE. Buttons/HUD stay plain data + pure helpers (Step 21/118 ruling). AMENDED Step 284 (2026-10-03): a tool/editor may build its OWN immediate-mode panels in its own target; a UI need the engine cannot meet is a classified finding for a later engine version.
- **No mixer graph** — audio stays pe::Audio's existing paths (Step 20/75/98/124 ruling).
- **No broadphase replacement** — deferred by measurement (Step 52: fine ≤2000 entities; re-open ONLY on a measured superlinear breach).
- **No materials system**, no generational handles, no container-ownership transfers.

New capabilities land only as additive, opt-in, engine-pure steps with honest verification — frozen behavior is never changed or unfrozen. Editor/tools work is a CONSUMER phase (see PURE_ENGINE_V3.md Phase Tools), allowed Step 284 by explicit Cyril decision. The authoritative scope statements live in `Blueprint/PURE_ENGINE_V3.md` (v1.1 toolkit freeze section) and the release gate in `Blueprint/SMOKE_TEST.md`.

## Guardrails for agents

- Keep the project architecture small and coherent with the existing engine step model.
- Use existing patterns before inventing new ones.
- Favor incremental feature work over broad refactors.
- Do not make out-of-scope changes without flagging them clearly.
- No "bulletproof" or "production-ready" language unless a claim is backed by real verification.

## Unattended phase protocol (Step 285 — binding on unattended runs)

- A phase is a numbered list of steps written by a human in the prompt. The
  agent executes only those steps, never invents steps, never opens a
  version or phase, never marks anything CONFIRMED that a human must
  confirm, and never changes scope.
- Preflight at the start of every phase: `git status -sb` (tree clean
  except `assets/hostile_default.txt` and `Testing/`), `git remote -v`
  (origin is the SSH alias below), `ssh -T git@github-pureengine`
  (judge by the message; the non-zero exit is normal), `git fetch origin`
  (local HEAD equals origin/main), `git push --dry-run origin main`.
  A failing preflight ends the phase before any work.
- Per step: reload the trackers, report the four engine-first lines, do
  the work, run gates (`cmake --build build --config Release`,
  `ctest -C Release`, alive x3, `git diff --check`,
  `git diff --stat`), record, commit, push, verify `git log -2` and
  `git status`, and continue ONLY if green AND pushed.
- STOP conditions: failed gate, tracker/prompt discrepancy, UNVERIFIED
  required item, hang, ambiguity, or a failed push after one retry. On
  STOP: write a STOP note in the tracker, commit it, push if possible,
  end the phase, and do not attempt later steps or fix anything outside
  the step's scope.
- Mutation checks: commit implementation and tests first, mutate only the
  committed tree, restore by pathspec, never commit a mutation, confirm
  `git diff --stat -- src/` is empty afterward. Expected values come from
  the contract or inputs, never from re-measured output.
- Consumer and Tools steps: engine src/ read-only; findings logged with
  one class each (existing capability / engine defect / missing
  capability / game-specific behavior); no engine fix inside a consumer
  step.
- Docs: edit tool only; scan U+FFFD; leave the SESSION_BOARD mojibake alone;
  dates from the system clock; confirm the step's entry exists in
  pure_engine_v3_steps.json and in git diff --stat before committing.
- Push route and credentials: origin is the SSH alias remote
  `git@github-pureengine:Cyril162005/PureEngine.git`, authenticated by a
  repo-scoped deploy key stored in the user's profile (.ssh). The agent
  never reads, prints, copies, or commits key material, tokens, or
  credentials; never edits ~/.ssh or the git credential configuration;
  never changes the remote URL; never uses --force, --no-verify, or
  --amend on pushed commits. A failed push is retried once, then STOP.
- Human-only decisions: opening a version or phase, choosing scope or a
  consumer, visual confirmations (v5 stays "pending"), any change to the
  frozen contracts, any change to these standing orders, and any
  credential or remote change.

