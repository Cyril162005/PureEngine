# Phase D consumer: level pipeline — FINDINGS.md (Step 275, 1 of at most 5)

Format: id | what happened | evidence | class | status.
Engine src/ is read-only; NO engine fix for any entry.

## Entry 1
- **what:** a parse-FAIL numeric field (e.g. `health=1e39`, float
  overflow) is reported by loadPrefab's stderr as
  `[prefab] Unknown key 'health' — skipped` — the message says UNKNOWN
  KEY for a field whose key IS known; the parse failure falls through
  the else-if chain to the unknown-key else (src/prefab.h:153-172: the
  conditions are `key == "scale" && parseVec3(...)`, so a parse fail
  makes the WHOLE condition false and the chain reaches the final
  else).
- **evidence:** consumers/level_pipeline/consumer.cpp (the hostile
  case 6); the actual observed behavior: the field keeps its DEFAULT
  (100.0) and the warn message is the misleading "Unknown key".
- **class:** **existing capability, with a doc-gap note** — the parse
  discipline ("warn+skip") is documented (prefab.h:16) but the
  misleading message for a parse-fail KNOWN field is undocumented;
  the message friction is an API-clarity issue, not a crash.
- **status:** recorded, not fixed (no engine fix inside a consumer
  step).

## Entry 2
- **what:** a duplicate field in a prefab file is silently accepted;
  the LAST assignment wins (health 20.0, not 10.0) — no warning that a
  duplicate was seen.
- **evidence:** consumers/level_pipeline/consumer.cpp (the hostile
  case 5); src/prefab.h:154-171 (the plain assignments overwrite).
- **class:** **existing capability, with a doc-gap note** — silent
  last-wins is undocumented; the parse discipline doc (prefab.h:16)
  says nothing about duplicates.
- **status:** recorded, not fixed.

## Entry 3
- **what:** a malformed line (no '=') is warn-skipped and loadPrefab
  returns TRUE with the valid fields kept — the caller cannot
  distinguish "all fields parsed" from "some lines dropped" except by
  reading stderr.
- **evidence:** consumers/level_pipeline/consumer.cpp (the hostile
  case 3); prefab.h:101-102 (the documented "partial parse on
  malformed lines (warns and skips bad lines, keeps valid ones)").
- **class:** **existing capability** — the partial-parse contract is
  DOCUMENTED; the friction is that the return value carries no
  dropped-line count (an API-clarity note, not a defect).
- **status:** recorded, not fixed.

## Entry 4
- **what:** an empty prefab file (no header line at all) fails clean:
  loadPrefab returns false with the "[prefab] Missing or wrong
  header" note — the header requirement is enforced.
- **evidence:** consumers/level_pipeline/consumer.cpp (the hostile
  case 2); prefab.h:135-138.
- **class:** **existing capability** — the documented header contract
  works as documented.
- **status:** recorded (positive evidence; no action).

## Entry 5
- **what:** a missing prefab file fails clean: loadPrefab returns
  false with the "[prefab] File not found" note; no crash, no partial
  use of the out-prefab (the out stays default-constructed).
- **evidence:** consumers/level_pipeline/consumer.cpp (the hostile
  case 1); prefab.h:122-125.
- **class:** **existing capability** — the documented missing-file
  contract works.
- **status:** recorded (positive evidence; no action).

## Entry 6
- **what:** an unknown KEY is warn-skipped ("[prefab] Unknown key
  '<key>' — skipped") — distinct from Entry 1: the key truly is not in
  the parser's list.
- **evidence:** consumers/level_pipeline/consumer.cpp (the hostile
  case 4); prefab.h:172.
- **class:** **existing capability** — documented behavior.
- **status:** recorded (positive evidence; no action).

## Entry 7
- **what:** loadPrefab's 3-candidate probe (assets/prefabs/,
  ../assets/prefabs/, ../../assets/prefabs/) plus the CWD-relative
  fallback resolve a BARE filename against the process CWD root only —
  a consumer's own data SUBDIRECTORY (consumers/level_pipeline/) is
  never probed, so the first consumer run failed with "[prefab] File
  not found: prefab_tile.txt" for files that existed one level below
  the CWD. Worked around inside the consumer by referencing the subdir
  path explicitly.
- **evidence:** the first consumer run's output (the File not found
  notes); src/prefab.h:69-75/:111-115 (the probe candidates + the
  CWD-relative fallback); the consumer.cpp's kTileFile paths.
- **class:** **existing capability, with a doc-gap note** — the probe
  list is documented (prefab.h:14-15) but the doc does not say a data
  subdir is never probed; an API-clarity issue.
- **status:** recorded, worked around inside the consumer (no engine
  fix).

## Entry 8
- **what:** adding "${CMAKE_SOURCE_DIR}/src" as a consumer target's
  include dir makes MSVC resolve `<time.h>` (pulled in by the MSVC
  <ctime> through the standard headers) to the ENGINE's src/time.h,
  shadowing the standard header; ctime's `using ::clock_t` then fails
  ("clock_t is not a member of the global namespace"). The existing
  test targets avoid this by using RELATIVE includes (../src/...)
  instead of a src include dir. Worked around inside the consumer by
  using relative includes.
- **evidence:** the build error output (the ctime clock_t errors with
  the src include dir; clean build after switching to the relative
  includes); src/time.h:48-52 (the engine's guard is PUREENGINE_TIME_H,
  not a guard collision — the shadow is the /I-dir resolution order).
- **class:** **engine defect (candidate)** — a src include dir breaks
  any target that uses the standard <ctime>/<time.h>; the existing
  targets' relative-include pattern is the workaround; not fixed here.
- **status:** recorded, worked around inside the consumer (no engine
  fix).

## Entry 9 (Step 276)
- **what:** saveSceneToFile's fs::rename fails with a sharing violation
  when the destination file has an OPEN read handle (a std::ifstream
  left open by the caller). The FIRST consumer run of the save->load->
  save check failed ("the second save succeeds") because the s1
  ifstream was still in scope; the engine's documented error return
  worked correctly (saveSceneToFile returned false, the tmp removed) —
  the failure was the consumer's unclosed handle, not the engine.
- **evidence (exact repro):** a std::ifstream on <file> left in scope;
  pe::saveSceneToFile(scene, <file>) called again; the rename fails
  (the Windows sharing violation), saveSceneToFile returns false.
  src/scene.h:443-448 (the rename + the error return); the consumer's
  fixed code (the inner scope closes f1 before the second save).
- **class:** **existing capability** (the documented error return
  works) with a doc-gap note — saveSceneToFile's doc does not say the
  rename fails if the destination is open elsewhere on Windows.
- **status:** recorded, worked around inside the consumer (the handle
  closed); no engine fix.

## Entry 10 (Step 276)
- **what:** the 34-field v2 format serializes floats at 4dp fixed
  precision (scene.h:418) — the values with more than 4 decimals do
  NOT round-trip bit-exact (1e-6 -> 0.0000; 0.12345678 -> 0.1235);
  values at 4dp or coarser (0.1f, 0, -1.5, 1e30) round-trip exactly;
  save->load->save is byte-identical (the 4dp rounding is idempotent).
- **evidence:** the consumer's float-edge checks (all PASS as
  OBSERVED-recorded); src/scene.h:418 (std::fixed << setprecision(4)).
- **class:** **existing capability, with a doc-gap note** — the 4dp
  precision is UNDOCUMENTED in the format comment (scene.h:372-380);
  observed, not asserted as correct.
- **status:** recorded; no engine fix.

## Entry 11 (Step 277)
- **what:** saveSceneManagerToFile honors explicit paths (the /, the \,
  the drive letter; the dir auto-created, scene.h:658-674) but its load
  counterpart loadSceneManagerFromFile does NOT — the loader probes
  ONLY assets/, ../assets/, ../../assets/ (scene.h:726-728); a save
  written to any non-assets path (consumers/level_pipeline/
  rt_manager.txt) is NOT findable by the loader — the round-trip
  breaks for every non-assets path. loadSceneFromFile DOES honor
  explicit paths (scene.h:461-467), so the asymmetry is specific to
  the manager loader.
- **evidence (exact repro):** saveSceneManagerToFile(m,
  "consumers/level_pipeline/rt_manager.txt") returns true;
  loadSceneManagerFromFile("consumers/level_pipeline/rt_manager.txt")
  returns false silently; the file exists at that path.
- **class:** **missing capability** (the explicit-path load absent;
  the repro above) with a doc-gap note — the loader's doc (scene.h:
  714-723) does not state that explicit paths are not honored.
- **status:** recorded, worked around inside the consumer (the bare
  name rt_manager.txt, probe-reachable); no engine fix.

## Entry 12 (Step 277)
- **what:** loadScene returns a Scene& into the SceneManager's scenes
  vector; any loadScene call may REALLOCATE that vector, so a Scene&
  or Scene* taken before a later loadScene DANGLES. The consumer took
  s2 = &loadScene(m, "beta") and used it after loadScene(m, "gamma")
  — the access violation (0xC0000005), the process crashed with the
  stdout buffer lost. The documented rule (scene.h:108-109: "Call
  scenes.reserve(N) before loadScene() to prevent reallocation.
  Re-take activeScene* after any structural change") exists and was
  violated by the consumer (no reserve, no re-take).
- **evidence (exact repro):** two loadScene calls without reserve; the
  first's returned Scene& used after the second → 0xC0000005. The
  crash output: the stdout lost (buffered), the stderr survived.
- **class:** **existing capability** — the engine behaved as the
  documented rule says; the friction is that the raw-reference API
  makes the rule load-bearing (a crash results when the reserve/
  re-take is missed) — a safety/API-clarity note, not a defect.
- **status:** recorded, worked around inside the consumer (reserve(3)
  + the compared values copied before the later loadScene); no engine
  fix.

## Entry 13 (Step 277)
- **what:** loadSceneManagerFromFile's common failure paths return
  false SILENTLY — no stderr message for the count mismatch, the bad
  current index, the failed scene load, or the unknown prefix (only
  the unknown VERSION warns, scene.h:775-777). The consumer's first
  manager-load failure (Entry 11) produced no diagnostic at all — the
  failure reason had to be deduced by inspection.
- **evidence:** the consumer's first manager-load run (no stderr for
  the failed load); scene.h:724-794 (the silent false returns).
- **class:** **existing capability, with a doc-gap note** — the
  silent-failure behavior is undocumented; API-clarity friction.
- **status:** recorded; no engine fix.
