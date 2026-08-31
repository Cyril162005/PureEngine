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
- `src/resources.h`: resource loading boundary
- `src/camera.h`: camera boundary
- `src/input.h`: input boundary
- `src/time.h`: timing boundary
- `src/lifecycle.h`: entity lifecycle boundary
- `src/audio.h`: audio boundary
- `src/ui.h`: HUD/UI boundary
- `src/simulation.h`: simulation logic boundary
- `src/entity.h`, `src/collision.h`, `src/gamestate.h`: core data and state logic
- `src/math/`: custom math layer
- `src/stb_impl.cpp`: stb implementation unit

## Reporting expectations

- State failures first, plainly, before summarizing any success.
- If a fix attempt failed, say so on that attempt instead of silently continuing.
- "Complete" means the work was actually executed and inspected, not merely written.
- If a check cannot be run, say "unverified" rather than guessing.
- If the request is ambiguous, state the interpretation in one line and continue with that assumption.

## Guardrails for agents

- Keep the project architecture small and coherent with the existing engine step model.
- Use existing patterns before inventing new ones.
- Favor incremental feature work over broad refactors.
- Do not make out-of-scope changes without flagging them clearly.
- No "bulletproof" or "production-ready" language unless a claim is backed by real verification.
