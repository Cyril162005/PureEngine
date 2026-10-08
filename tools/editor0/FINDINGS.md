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

## Step 292 classification (docs only; no engine fix)

id | summary | class | evidence | impact | fix size

1 | pe::loadSceneFromFile returns bare false with no reason code; an
editor wrapper cannot distinguish not-found from parse-failed
without its own probe | **existing capability** (doc-gap: the
reason-code absence is undocumented; the strict whole-file contract
IS documented, src/scene.h:459-473) | tools/editor0/editor0_core.h
(the std::ifstream probe BEFORE the engine loader); the Step 277
silent-false finding | an editor adds a probe (done here); no crash
risk | one function (an engine reason-code out-param - a later
engine version)

2 | each consumer duplicates the fixture-copy CMake block
(consumers/level_pipeline copy_directory) | **existing capability**
| CMakeLists.txt (the LevelPipelineConsumer + PureEditor0 blocks,
two copies of the same source dir) | maintainability; a third
consumer duplicates again | mechanical (a shared CMake function -
build config, no engine)

3 | an unqualified `Scene&` in editor code fails to compile (C2061)
| **existing capability** (doc-gap: none - consumer.cpp models the
convention) | the 288 first build (error C2061 at
editor0_core.h(37,57), exit 1); editor0_core.h:41 (the qualified
signature) | the editor author must qualify every engine type at
every use | mechanical (convention compliance)

4 | renderer.init()'s fatal asset set (2 shaders, checker, 5
textures, the 6th-texture proof, the font) is covered ONLY by the
3-candidate CWD probe; from a build/Release CWD the ../../assets/
candidate silently hits the REPO ROOT | **existing capability**
(doc-gap: the implicit repo-root dependency is undocumented in
renderer.h) | src/renderer.h (the init() fatal list, each via the
assets/, ../assets/, ../../assets/ probe); the 289 hidden-window
frame ran with NO new asset copy and passed (glGetError() == 0) |
a consumer running from build/ silently depends on the repo root;
relocating the repo breaks it | mechanical (a fixture copy per
consumer) or design change (an asset-root API - engine)

5 | drawWorld's colliding flags are mandatory (the F-04 assert)
even for a viewer with NO collision state | **existing capability**
(documented: the assert + flagsForCount) | src/renderer.h:401-404;
src/lifecycle.h:152 | a stateless viewer fabricates all-zero flags
(the documented pattern) | mechanical (none needed - the helper
exists)

6 | the GL-consumer include set (glad/gl.h + GLFW/glfw3.h +
renderer.h + camera.h) is easy to miss part of | **existing
capability** (doc-gap: none - pong.cpp documents the relative-
includes rule) | the 289 dev build (C2039 'Renderer' is not a
member of 'pe', 19 errors, exit 1) | a compile error catches the
miss; no silent breakage | mechanical (convention compliance)

7 | the camera exposes NO zoom API (halfHeight locked at 4.5 by
onResize, no half-extent setter, position/halfWidth/halfHeight
private) | **missing capability** (pre-classified
PURE_ENGINE_V3.md:1693) | src/camera.h (no zoom symbol - a "zoom"
grep over camera.h returns zero hits); the editor's
zoomedProjection composition + zoom-aware conversions (41 checks
green, Step 290) | no true zoom through the public API; the editor
composes a screen-center scale into the caller's matrix | design
change (a camera zoom API - a later engine version)

8 | input.h exposes NO wheel/scroll input | **missing capability**
| src/input.h:264-290 ("no scroll, no cursor management";
MouseState carries only x/y/left/right) | a wheel zoom is not
expressible through the documented input boundary; the editor uses
+/- keys (one step per press, Step 290) | one function (a scroll
callback + a MouseState field - a later engine version)

9 | unqualified Vec3/Mat4 in editor code fail to parse; the header
must include what it uses (self-containment) | **existing
capability** (doc-gap: none - the convention is established by
Entries 3/6) | the two 290 dev builds (C4430/C2146 at
editor0_core.h(98/112) with C2440/C2660 cascades, exit 1); the
camera.h include added; rebuild exit 0 | headers must be
self-contained; the compile error catches the miss | mechanical
(convention compliance)

10 | the hostile loads: every case a clean reject with a non-empty
err and the previous scene UNCHANGED; no crash, no hang, no huge
allocation | **existing capability** (doc-gap: the SCENE (v2)
format has NO count field - the count lives in the manager format;
the injected count= lines are unknown-key strict rejects, observed)
| tools/editor0/main.cpp (checkHostileLoads); the 291 selftest (65
checks; all 7 truncations rejected, 0 valid-prefix loads; v99/v-1
rejected with the engine stderr warn; the 1 MB line rejected with
no hang) | the failures are safe; the loader is robust for the
editor's needs | none

11 | a directory path is classified "file not found" by the editor
probe; the ifstream-on-directory behavior differs across
observations (278: "may succeed"; 291: "failed") | **existing
capability** (doc-gap: the directory classification wording is
imprecise; both outcomes are safe rejects) | tools/editor0/
editor0_core.h (the probe; the OBSERVED err "file not found:
editor0_tmp") vs the 278-era consumer note | the wording misleads
slightly; the outcome is safe | mechanical (an editor-side stat
refinement - one function)

12 | the missing-file "current untouched" property is protected by
the editor probe's EARLY RETURN, not the parse-replace discipline |
**existing capability** (doc-gap: the probe guard's role in the 288
check is undocumented) | the 291 mutation run (18 checks FAILED for
the parse-reaching cases; the 288 missing-file check still passed)
| a test-coverage note: the parse-path discipline is proven by the
18 hostile/reload checks | none

TALLY: 12 entries = 10 existing capability (4 with doc-gap notes),
2 missing capability (7 camera zoom API, 8 wheel/scroll input),
0 engine defect, 0 game-specific behavior. NO engine fix for any
entry (the Tools rules); the two missing capabilities are engine
work for a later engine version, opened docs-first from these
findings if the human chooses.

## Step 293 observation: the repeated console print on a failed load

- **what (recorded VERBATIM from Cyril's hands-on run, 2026-10-08):**
  he ran `PureEditor0.exe savedata\sample_truncated.txt` and
  `PureEditor0.exe savedata\sample_garbage.txt` (made by truncating
  and by writing 4096 bytes of garbage). EACH printed
  "editor0 ERROR: parse failed: <path>" to the TERMINAL 5 times
  (identical lines) and the editor stayed running until ESC. He
  opened `savedata\sample_scene.txt` normally and reported that it
  works. --make-sample on an existing path refused to overwrite, as
  designed. He did NOT describe the on-screen error-line text, so
  the window error-line display (Step 289) stays UNCONFIRMED.
- **classification: OBSERVATION** (per the Step 293 prompt - not a
  defect and not one of the four classes; the outcome is safe: the
  editor stays running until ESC and the scene is untouched).
- **mechanism (UNVERIFIED):** the code path prints ONCE per failed
  load (tools/editor0/main.cpp runViewer: the single
  std::fprintf(stderr, ...) at the load site, BEFORE the loop); the
  5x count is NOT explained by the cited code path - the likely
  explanation is the accumulated console output across multiple
  opens in the same terminal; unverified, recorded as reported.
- **status:** recorded (an observation; no engine fix, no editor
  fix inside a docs step).
