# PureEngine

Solo learning project. C/C++ engine + game, GLFW/OpenGL/GLAD, built from
scratch. Target: low-end hardware (i3, 8GB RAM).

## Status (verify before trusting — trackers have been wrong before)

- Steps 1-12 (engine): complete, tracked in `Blueprint/PureEngine.json`
- Steps 13-24 (boundary refactor): complete, tracked in
  `Blueprint/pure_engine_v2_steps.json` — this is the authoritative continuation
- Steps 25-34: in progress

## Rules — no exceptions

- One step per turn. Stop and report before starting the next step.
- Grep for existing capability before writing new code.
- Never trust `Blueprint/*.md` or `*.json` claims without checking them against
  actual files on disk first.
- No new architecture, frameworks, or tracker schemas beyond what a step
  needs. If something bigger seems necessary, say so and stop — don't build it.

## Reporting standard

- Every "works," "passes," or "succeeds" claim needs the actual command and
  its actual output/exit code attached. No attached evidence = unverified,
  not true. Quote the relevant line; don't paraphrase it.
- If a check is possible, run it — don't hedge with "should"/"might" instead.
  If it wasn't run, say "unverified."
- State failures first, plainly, before any summary of successes. If a fix
  attempt didn't work, say so on that attempt — don't retry silently and
  report only the last try.
- "Complete" = executed and output inspected. Written-but-unrun code is not
  done. If only part of a task was verified, say exactly which part.
- Never invent file contents, line numbers, error messages, or results not
  actually observed. If something can't be checked, say that plainly.
- If a request is ambiguous, state the interpretation being used in one
  line and proceed. Don't make out-of-scope changes without flagging them
  separately.
- No "bulletproof"/"production-ready" framing. Report what was verified,
  what wasn't, what's still open. Leave confidence judgments to me.

## Build

Run from a Visual Studio 2022 Developer PowerShell or Developer Command Prompt:

```bat
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
build\Release\PureEngine.exe
```
