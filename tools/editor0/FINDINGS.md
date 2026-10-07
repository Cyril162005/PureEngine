# Phase Tools consumer: editor0 - FINDINGS.md (Step 288, Tools step 1 of 5)

Format: id | what happened | evidence | class | status.
Engine src/ was read-only for the step (git diff --stat -- src/ empty);
NO engine fix for any entry.

## Entry 1
- **what:** pe::loadSceneFromFile returns bare false with no reason
  code; an editor wrapper cannot distinguish "file not found" from
  "parse failed" without probing the file itself.
- **evidence:** tools/editor0/editor0_core.h (the editor-side
  std::ifstream existence probe that runs BEFORE the engine loader;
  the selftest OBSERVED output "file not found:
  editor0_tmp/no_such_scene_zz.txt" is the wrapper's err text, not an
  engine message). Same friction recorded at Step 277 (the loader
  silent false returns); re-observed here from the editor side.
- **class:** existing capability (the strict whole-file load contract
  works as documented; the reason-code absence is an API-clarity gap,
  not a defect).
- **status:** recorded, not fixed (no engine fix inside a Tools step).

## Entry 2
- **what:** each consumer duplicates the fixture-copy CMake block
  (add_custom_command copy_directory of consumers/level_pipeline) so
  the shared prefab files resolve from its own WORKING_DIRECTORY; the
  LevelPipelineConsumer's copy cannot be reused because
  $<TARGET_FILE_DIR> differs per target.
- **evidence:** CMakeLists.txt (the new PureEditor0 block mirrors the
  LevelPipelineConsumer block; both copy the same source dir into two
  target dirs).
- **class:** existing capability (the pattern works and is explicit;
  the per-consumer duplication is a maintainability note, not a
  defect).
- **status:** recorded, not fixed.

## Entry 3
- **what:** the engine types are namespace-qualified (pe::Scene);
  an unqualified `Scene&` in editor code fails to compile (C2061).
  An editor author must qualify every engine type at every use.
- **evidence:** the first PureEditor0 build FAILED (error C2061
  'Scene' at editor0_core.h(37,57) with C2660 cascades; exit code 1);
  the fix was editor-side qualification (pe::Scene& / pe::Scene tmp),
  the rebuild exit 0. The Phase D consumer already qualifies every
  pe:: type (consumer.cpp).
- **class:** existing capability (a documented convention every
  existing consumer follows; the friction is an editor-side compile
  error, not an engine issue).
- **status:** recorded, fixed editor-side.
