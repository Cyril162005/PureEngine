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

## Entry 4
- **what:** renderer.init()'s fatal asset set is large (2 shader files,
  checker.png, 5 entity textures, the 6th-texture proof, the font
  atlas) and every GL consumer covers it ONLY through the 3-candidate
  CWD probe: from a build/Release CWD the "../../assets/" candidate
  silently hits the REPO ROOT, so a consumer needs no fixture copy but
  carries an implicit repo-root dependency.
- **evidence:** src/renderer.h (the init() fatal list, each via the
  assets/, ../assets/, ../../assets/ probe); the PureEditor0 hidden-
  window frame ran under the ctest CWD (build/Release) with NO new
  asset copy and passed (glGetError() == 0).
- **class:** existing capability (the probe covers the need; the
  implicit repo-root dependency is a maintainability note, not a
  defect).
- **status:** recorded, not fixed (no engine fix inside a Tools step).

## Entry 5
- **what:** drawWorld's colliding flags are mandatory (the F-04
  assert: entities.size() == colliding.size()) even for a viewer with
  NO collision state - a stateless consumer must fabricate flags via
  pe::flagsForCount (zero-initialized) to draw at all.
- **evidence:** src/renderer.h:401-404 (the assert on every drawWorld
  call), src/lifecycle.h:152 (flagsForCount), tools/editor0/main.cpp
  (the fabricated all-zero flags in both the viewer loop and the
  hidden-window frame).
- **class:** existing capability (the assert is documented and the
  helper is the documented pattern; a viewer's all-zero flags are
  correct, not a defect).
- **status:** recorded, not fixed.

## Entry 6
- **what:** the GL-consumer include set is wide (glad/gl.h +
  GLFW/glfw3.h + renderer.h + camera.h) and easy to miss part of: the
  first 289 rewrite dropped renderer.h/camera.h and the build failed
  with C2039 'Renderer' is not a member of 'pe' (19 errors).
- **evidence:** the 289 dev build output (C2039/C2065 cascade at
  main.cpp(195), see-declaration pointing at window_guard.h's pe);
  the fix was editor-side (the two includes restored), rebuild exit 0.
  The pattern is documented in games/pong/pong.cpp (relative includes,
  never -Isrc).
- **class:** existing capability (a documented convention the editor
  author must copy whole; the friction is an editor-side compile
  error, not an engine issue).
- **status:** recorded, fixed editor-side.

## Entry 7
- **what:** the camera exposes NO zoom API. camera.h has no zoom; the
  vertical world span can NEVER change through the public API
  (onResize locks halfHeight at 4.5 and there is no half-extent
  setter; position/halfWidth/halfHeight are private); the only
  projection rebuild is onResize. The engine's Camera::screenToWorld
  also has no zoom knowledge, so a zoomed view cannot be converted
  with the engine conversion alone.
- **evidence:** src/camera.h (the private fields, the onResize
  halfHeight=4.5 lock, no zoom symbol - a "zoom" grep over camera.h
  returns zero hits); PURE_ENGINE_V3.md:1693 pre-classified the zoom
  as unmet if the camera has no zoom API. The editor implements zoom
  as an EDITOR-OWNED screen-center scale (pe::Mat4::scale composed
  into the caller's projection via zoomedProjection) + zoom-aware
  conversions (screenToWorldAtZoom/worldToScreenAtZoom feeding the
  documented pe::screenToUi/uiToScreen the zoomed half-extents);
  41 selftest checks green including the zoom clamps and the zoomed
  round-trip.
- **class:** **missing capability** (a camera zoom API - engine work
  for a later engine version).
- **status:** recorded, not fixed (no engine fix inside a Tools step).

## Entry 8
- **what:** input.h exposes NO wheel/scroll input. The MouseState
  snapshot carries only position + left/right buttons; the header
  documents "no scroll, no cursor management". A wheel zoom is not
  expressible through the documented input boundary.
- **evidence:** src/input.h:264-290 (the MouseState struct + the "no
  scroll" note); the editor uses GLFW_KEY_EQUAL/GLFW_KEY_MINUS edges
  (one step per press) for zoom instead. 41 selftest checks green
  (the clampedZoom contract).
- **class:** **missing capability** (a wheel/scroll input - engine
  work for a later engine version).
- **status:** recorded, not fixed (zoom is +/- keys in editor0).

## Entry 9
- **what:** the pe::-qualification convention bites again, with the
  header self-containment as the new part: unqualified Vec3/Mat4 in
  editor code fail to parse (C4430/C2146 'missing type specifier' at
  editor0_core.h(98/112) with C2440/C2660 cascades; TWO dev builds
  failed), and editor0_core.h must include camera.h itself (it now
  consumes pe::screenToUi/pe::uiToScreen) - it did not compile
  self-contained until the include was added.
- **evidence:** the 290 dev build outputs (the bogus 'editor0::Vec3'
  declarations + the argument-list cascades); the fix = pe::Vec3/
  pe::Mat4 everywhere + the camera.h include; rebuild exit 0. The
  same convention as Entry 3 (288 recorded Scene; 290 hit Vec3/Mat4).
- **class:** existing capability (a documented convention the editor
  author must copy whole; the friction is an editor-side compile
  error, not an engine issue).
- **status:** recorded, fixed editor-side.

## Entry 10
- **what:** the hostile-load outcomes (Step 291, all inputs generated
  programmatically, nothing committed): every case is a clean reject
  with a non-empty err and the previous scene UNCHANGED (verified by
  the full field comparison against the snapshot). No crash, no hang,
  no huge allocation. The observed outcome of each case: the empty
  file rejected; all 7 truncation points rejected (0 valid-prefix
  loads - no line-boundary alignment in this sample); the wrong
  version (v99) and the negative version (v-1) rejected with the
  engine's stderr warn ("unknown scene version ... expected v1 or
  v2"); 4 KB of garbage bytes rejected; the 1 MB entity line rejected
  with NO hang; ALL FOUR injected count-field lines (0, -1, 2^31,
  5-mismatching) rejected as UNKNOWN keys - the SCENE (v2) format has
  NO count field (the count lives in the manager format), so the
  step's "count fields" are unknown-key strict rejects, observed and
  recorded; a directory path rejected (see Entry 11); a missing path
  rejected.
- **evidence:** tools/editor0/main.cpp (checkHostileLoads; the
  OBSERVED lines in the 291 selftest output); 65 selftest checks
  green.
- **class:** existing capability (the strict whole-file load contract
  held for every hostile input; the failures are safe).
- **status:** recorded, not fixed (nothing to fix - no defect found).

## Entry 11
- **what:** a directory path is classified "file not found" by the
  editor probe (the std::ifstream open on a directory FAILED in the
  291 run) - imprecise (the path exists; it is not a file) - and the
  ifstream-on-directory behavior DIFFERS from the 278-era consumer
  observation ("the open may succeed on Windows; the getline fails ->
  the header check -> false"). Both observations end in a safe
  reject; the classification wording differs.
- **evidence:** tools/editor0/editor0_core.h (the probe; the OBSERVED
  err "file not found: editor0_tmp") vs consumers/level_pipeline
  (the 278-era note "the open may succeed on Windows").
- **class:** existing capability (an API-clarity gap in the
  editor-side classification; both outcomes are safe rejects, not a
  defect).
- **status:** recorded, not fixed.

## Entry 12
- **what:** the mutation check nuance: under the
  replace-before-parse mutation, the 288 missing-file check still
  PASSED - its "current untouched" property is protected by the
  editor probe's EARLY RETURN (the missing-file path returns before
  any scene mutation could run), not by the parse-replace discipline.
  The parse-path discipline is proven by the 18 hostile/reload
  checks that DID fail under the mutation.
- **evidence:** the 291 mutation run: 18 checks FAILED (every
  "leaves the scene unchanged" assertion for the parse-reaching
  cases) while "selftest: `current` untouched on failure" passed
  (the probe path).
- **class:** existing capability (the documented wrapper design: the
  probe guard is the missing-file protection; a test-coverage note,
  not a defect).
- **status:** recorded, not fixed.
