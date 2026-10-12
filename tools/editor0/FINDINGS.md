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

## Entry 18
- **what:** the live-path coordinate-space DEFECT (found and fixed
  editor-side): the editor's cursor came from
  pe::Input::pollMouse -> glfwGetCursorPos (src/input.h:374) in
  WINDOW (logical) coordinates while every size it passed to the
  conversions came from glfwGetFramebufferSize (main.cpp:724/:777/
  :829/:851) in FRAMEBUFFER (physical) pixels; with display scaling
  != 100% the spaces differ by the scale ratio and an unconverted
  cursor picked the WRONG world point (the reported live defect: the
  left click showed no clear selection, R showed no visible change).
- **evidence:** src/input.h:374 (the cursor), src/camera.h:60/:286
  (the documented raw glfwGetCursorPos space - the ENGINE documented
  the space correctly), main.cpp:724/:777/:829/:851 (the fb sizes);
  the fix = windowToFbX/Y (the ratio conversion) in stepEditorFrame;
  102 selftest checks green (the ratio tests at 1.0/1.25/1.5 + the
  negative controls: the raw cursor MISSES); the MUTATION (the ratio
  skipped in the pick path) made both ratio tests FAIL - NON-VACUOUS.
- **class:** existing capability (the engine's documented cursor
  space is correct; the defect was the EDITOR's wiring, found and
  fixed editor-side; no engine change).
- **status:** fixed editor-side (the Step 295 ratio conversion).

## Entry 19
- **what:** the HUMAN PASS on the live select (Cyril, verbatim,
  2026-10-09): click on entity deepens/strengthens box color (select
  works); empty space returns to original color (clear works); drag
  pans OK; zoom OK; R status/reload feedback changes, the scene
  looks the same if the file unchanged (expected) - accepted OK; ESC
  OK. Overall: all work well. Truncated load earlier: lines printed
  = 1 (the Step 294 one-report-per-attempt fix CONFIRMED by the
  human). Do not reopen Pong; the selection is not still broken.
- **evidence:** Cyril's hands-on run of
  build\Release\PureEditor0.exe savedata\sample_scene.txt from
  D:\PureEngine; the 295-STOP commit 95b81a8 (the wiring + the fix).
- **class:** existing capability (the live path confirmed by the
  human; the 294/295 visual confirmations CLOSED).
- **status:** recorded (the visual confirmations: CONFIRMED).

## Entry 20
- **what:** the read-only inspector panel (the Step 296): the 36-line
  list = the 24 scene-saver fields (position/rotationAngle/
  rotationSpeed/scale/halfExtents/textureId/depth/roleId/moveSpeed/
  velocity/gravityScale/isStatic/coyoteTime/jumpImpulse/maxFallSpeed/
  tint/cols/rows/health/timer/tag/parentIndex/animationSpeed/
  currentClipName) + the context lines (the rot.axis z(2d)/y(3d));
  the per-component lines so every line fits the 12x9 default view
  (<= 22 chars at 0.52 advance); the floats in the SAVER's 4-decimal
  style (the panel shows exactly what the file round-trips - the 4dp
  precision is the saver's documented view, the 276 finding); the
  "+N more" overflow; no selection -> ONE "no selection" line; the
  panel never edits.
- **evidence:** tools/editor0/editor0_core.h (makeInspectorLines/
  inspectorLinesForSelection); 113 selftest checks green (the 36
  exact lines, the coverage, the +N more boundary: maxLines 12 -> 11
  + "+25 more", maxLines 36/50 -> all 36).
- **class:** existing capability (the bitmap font + the documented
  pe::uiToScreen; no engine change).
- **status:** recorded (the panel landed; verified).

## Entry 21
- **what:** the panel-rect DEFECT (found and fixed in the same step):
  the FIRST pointInPanelRect used a FIXED 12-line extent regardless
  of the actual line count, so the startup ONE-LINE "no selection"
  strip suppressed the entity clicks behind it (the 3 selection
  tests FAILED: the ratio 1.0/1.25/1.5 clicks hit the y 0..380 rect).
  The fix: the panel's ACTUAL extent - the line count is
  deterministic (12 lines when selected, 1 when not) - a startup
  strip must never suppress the entity clicks below it.
- **evidence:** the 296 selftest runs (the 3-FAILED run before the
  fix; the 113-check green run after); the panel-suppression test
  (a click inside the grown rect KEEPS the selection; the
  outside-rect control CLEARS).
- **class:** existing capability (the editor's own defect, found and
  fixed editor-side; no engine change).
- **status:** fixed editor-side (verified).

## Entry 22
- **what:** the rotation-axis truth - the Direction Spec's "Z-only"
  (Step 293) was INCOMPLETE: rotationAngle is ONE field with the
  AXIS PER MODE - the 2D path reads it as a Z-SPIN in the XY plane
  (src/renderer.h:591 Mat4::rotationZ in modelMatrix), the 3D path
  (drawEntity3D, src/renderer.h:732-734/:777) REINTERPRETS the same
  field as YAW about +Y (Mat4::rotationY, Step 214, documented
  in-source as the explicit choice).
- **evidence:** src/entity.h:46-47 (the field's comment says "current
  Z rotation" - the 2D reading), src/renderer.h:591 (rotationZ),
  src/renderer.h:732-734/:777 (rotationY as yaw). The Direction Spec
  corrected in the record commit.
- **class:** existing capability (documented in-source; the doc-gap
  was the Direction Spec's ambiguous summary).
- **status:** corrected (the record commit).

## Entry 23
- **what:** the stash/lock incident (the 295-296 stop + the
  recovery): an exclusive file lock on build/Release/PureEditor0.exe
  blocked the link ~1 hour - the READ side was ALSO blocked, so it
  was NOT a running-instance image lock but an exclusive DATA handle
  (a stuck sync/copy tool, an AV scan, or a backup; the holder was
  invisible to the non-elevated shell). The 296 work was stashed
  (stash@{0}) with a backup patch (D:\PureEngine_wip\296_stash.patch,
  17934 bytes); the HUMAN deleted and rebuilt the exe (the lock
  cleared); the stash popped CLEANLY and was dropped (99793f80)
  only after the pop succeeded.
- **evidence:** the LNK1104 outputs (7 build attempts + del/ren/waits
  blocked); git stash list/pop; the recovery's 113-check green run.
- **class:** existing capability (the git stash recovery worked end
  to end; the blocker was external to the repository).
- **status:** recorded (recovered).

## Entry 24
- **what:** the key audit for the navigation (the Step 297): Tab is
  MAPPED ("TAB" -> GLFW_KEY_TAB, src/input.h:176) but was NOT
  registered in any edge-tracked list - the registration is the
  CONSUMER's ctor list (src/main.cpp:866-882 the Arcade's list; the
  doc: "untracked keys simply report no edge"). The Shift modifier
  as a LEVEL read is ENGINE-PROVEN: src/main.cpp:1609-1610 uses
  pe::Input::isDown(window, GLFW_KEY_LEFT_SHIFT) ||
  ...RIGHT_SHIFT. The final key choice: Tab = the next (the Tab edge,
  registered in the editor's ctor list), Shift+Tab = the previous
  (the Tab edge + the Shift level).
- **evidence:** src/input.h:160-177 (the mapping + the edge reads),
  src/main.cpp:866-882 (the registration pattern), src/main.cpp:
  1609-1610 (the Shift level in use); the editor's Input ctor now
  includes GLFW_KEY_TAB; 151 selftest checks green.
- **class:** existing capability (the mechanism supports any GLFW
  key; the only gap was the editor's own registration list, fixed).
- **status:** recorded (the keys documented).

## Entry 25
- **what:** the nav scan-direction DEFECT (found and fixed in-step by
  the test): the first navigateSelection scanned FORWARD for BOTH
  directions, so "previous from 6" with the dead 5 found ITSELF
  (index 6) instead of skipping back to 4 - the test caught it ("nav:
  the previous from 6 skips the dead 5 -> 4" FAILED). The fix: the
  scan direction matches (the next walks FORWARD, the previous walks
  BACKWARD; the wrap is always non-negative).
- **evidence:** the 297 selftest run (1 check FAILED before the fix;
  151 checks green after); tools/editor0/editor0_core.h
  (navigateSelection's direction-matched scan).
- **class:** existing capability (the editor's own defect, found and
  fixed editor-side by the test; no engine change).
- **status:** fixed editor-side (verified).

## Entry 26
- **what:** the HUMAN FEEDBACK on the 296 inspector (Cyril's photos,
  2026-10-09, verbatim summary): "no selection" shown when nothing is
  picked; selecting an entity shows tag, role, pos, scale, half,
  rotation and "+25 more"; the selected entity's outline turns
  orange; drag, zoom, R, ESC still work. THE 296 VISUAL
  CONFIRMATION: CONFIRMED. The polish finding (recorded, cosmetic):
  the big bitmap font and a crowded top status line.
- **evidence:** Cyril's photos of the running editor; the 296 commit
  f47f1aa.
- **class:** existing capability (the polish items are cosmetic
  notes, not defects).
- **status:** recorded (the visual confirmations: CONFIRMED; the
  polish: a candidate for a later cosmetic pass, not scheduled).

## Entry 28
- **what:** the save-as (the Step 299; the editor's FIRST write
  path): saveAsForEditor wraps the frozen pe::saveSceneToFile
  (scene.h:381) with the write-safety refusals (an empty path, the
  loaded SOURCE path, an EXISTING destination - the file untouched);
  the trigger is Ctrl+S (the S edge + the Ctrl level - the
  engine-proven combo pattern, main.cpp:1609-1610); the target is a
  derived NEW path savedata/<base>_edit<N>.txt (makeSaveAsPath,
  deterministic, no clock) with an auto-incrementing counter (the
  next free name); the status feedback 'saved <file> (<N> entities)'
  / 'save failed: <reason>' (exact-string tested). The round-trip is
  PROVEN: save -> load -> field-exact (all 24 checks per entity) and
  save -> load -> save BYTE-IDENTICAL (the 276 pattern).
- **evidence:** 167 selftest checks green; the counter bug (the
  break skipped the loop's increment - the counter stayed at the
  saved N) caught by the test and fixed (n + 1); the MUTATION (the
  source refusal skipped) made 'save-as: the source path refused'
  FAIL - NON-VACUOUS.
- **class:** existing capability (the engine's save/load contracts
  proven; the editor's write path landed with the refusals; the
  counter bug was an editor-side defect found and fixed in-step by
  the test).
- **status:** recorded (the write path landed; verified).

## Step 298 classification - the Editor-0 / Tools slice (288-297), CLOSED

Format: id | class | evidence (the fix commits cited for the fixed
defects). The optional Tab retest: NOT RUN (the placeholder unfilled).

| id | class |
|---|---|
| 1 loader bare false, no reason code | **existing capability** (the doc-gap; the editor's probe compensates) |
| 2 per-consumer fixture-copy duplication | **existing capability** (maintainability) |
| 3 unqualified Scene& (C2061) | **fixed defect** (editor-side, Step 288, commit a532f41) |
| 4 the renderer asset probe's implicit repo-root dependency | **existing capability** (the doc-gap) |
| 5 drawWorld's mandatory colliding flags | **existing capability** (documented) |
| 6 the wide GL-consumer include set | **fixed defect** (editor-side, Step 289, commit e799b90) |
| 7 the camera zoom API | **missing capability** (engine work for a later version) |
| 8 the wheel/scroll input | **missing capability** (engine work for a later version) |
| 9 unqualified Vec3/Mat4 + the header self-containment | **fixed defect** (editor-side, Step 290, commit 6ae05f5) |
| 10 the hostile-load outcomes (all clean rejects) | **existing capability** |
| 11 the directory classification + the ifstream inconsistency | **existing capability** (the doc-gap) |
| 12 the mutation nuance (the probe's early return) | **existing capability** |
| 13 the zoomed pick composition | **existing capability** |
| 14 the role-group highlight | **existing capability** (+ the per-entity-color parameter = a missing-capability note) |
| 15 the observed overlap pick order | **existing capability** (not asserted as a contract) |
| 16 the dead-entity skip | **existing capability** |
| 17 the print discipline (one report per attempt) | **existing capability** (CONFIRMED by Cyril: 1 line) |
| 18 the coordinate-space ratio defect | **fixed defect** (editor-side, Step 295, commit 95b81a8) |
| 19 the human PASS on the live select | **existing capability** (the confirmation) |
| 20 the inspector panel | **existing capability** |
| 21 the panel-rect fixed-extent defect | **fixed defect** (editor-side, Step 296, commit f47f1aa) |
| 22 the rotation-axis truth (the Direction Spec correction) | **existing capability** (the correction landed, Step 296, commit 8506a21) |
| 23 the stash/lock incident + recovery | **existing capability** (recovered) |
| 24 the key audit (Tab = next, Shift+Tab = previous) | **existing capability** |
| 25 the nav scan-direction defect | **fixed defect** (editor-side, Step 297, commit b1ca958) |
| 26 the 296 visual CONFIRMED + the polish | **existing capability** (the confirmation) + **cosmetic** (the polish: the big font + the crowded status - not scheduled unless the human asks) |

TALLY: 26 entries = 17 existing capability, 6 fixed defect (3/6/9/
18/21/25 - all editor-side, no engine defect), 2 missing capability
(7 the camera zoom API, 8 the wheel/scroll input), 1 cosmetic
(the polish in 26). DEFERRED (named, not this phase - from the open
board/V3 notes): the panel scroll + the visibility toggle, the
2000-entity stress, the second mutation (the scroll clamp), the
save-as (option C), the gizmos.

## Entry 13
- **what:** the engine's pickEntityAtScreen has NO zoom knowledge (it
  converts with the camera's stored half-extents), so a pick at
  zoom != 1 cannot use the engine wrapper alone; the editor composes
  a zoom-aware pick (the world point via screenToWorldAtZoom, then
  the DOCUMENTED pe::pickEntity with world coords).
- **evidence:** src/collision.h:364-369 (the Camera-based wrapper);
  tools/editor0/editor0_core.h (pickEntityAtScreenZoomed); 82
  selftest checks green (the pick at pan (2,1) zoom 1.5 selects the
  computed entity; the wrong-zoom negative control misses).
- **class:** existing capability (the composition works; the engine
  wrapper is documented as Camera-based - no engine change).
- **status:** recorded, not fixed (nothing to fix).

## Entry 14
- **what:** the selected-entity highlight is a ROLE-GROUP color: the
  engine's drawAABBs takes only a playerRoleId (the color is decided
  INSIDE by roleId: orange vs yellow) - no per-entity or
  selected-entity color parameter exists. The editor passes the
  selected entity's roleId, which highlights the selected entity
  AND its role-mates (the sample's roles are distinct, so exactly
  the selected entity highlights); a TRUE per-entity color would
  need an engine change.
- **evidence:** src/renderer.h:1085 (the signature) + the
  roleId-based coloring inside; tools/editor0/main.cpp (the
  highlight call). 82 selftest checks green.
- **class:** existing capability, with a doc-gap note (the
  per-entity color parameter is a MISSING CAPABILITY for a later
  engine version - the role-group highlight works without any
  engine change).
- **status:** recorded, not fixed.

## Entry 15
- **what:** the overlapping-entity pick order OBSERVED (recorded as
  a finding, NOT asserted as a contract per the step): two
  entities at the same position with depths 5 (index 0) and 2
  (index 1) - the pick returned index 0 (the HIGHEST depth wins;
  ties keep the earlier index, collision.h:341-343).
- **evidence:** src/collision.h:341-343 (the depth rule + the tie
  comment); the 294 selftest OBSERVED line ("the pick returned
  index 0").
- **class:** existing capability (documented in-source; the
  observation recorded).
- **status:** recorded, not asserted as a contract.

## Entry 16
- **what:** a dead entity is NOT selectable: pickEntity skips
  alive=false entities (collision.h:330-332), so a dead entity
  under the cursor returns -1 (observed).
- **evidence:** src/collision.h:330-332 (the alive skip); the 294
  selftest dead test (the pick returned -1 for a dead-only list).
- **class:** existing capability (documented; the dead entity is
  not selectable - correct for the editor).
- **status:** recorded, not fixed (nothing to fix).

## Entry 17
- **what:** the print discipline (the Step 294 investigation of
  Cyril's 5-identical-lines observation): the editor's load path
  has EXACTLY ONE stderr print site (main.cpp:663, the load site;
  the full grep: the reload site :688 per R press, the make-sample
  sites :749/:755, the init failure sites :631-644; editor0_core.h
  has ZERO print sites). The test sink (the editor-owned
  reportMessage(std::ostream&)) proves ONE failed load produces
  EXACTLY ONE report line. The 5x observation is NOT reproducible
  from the editor code - the cause is OUTSIDE the editor (the most
  likely: the accumulated console output across multiple opens in
  the same terminal; UNVERIFIED). All diagnostic sites now route
  through reportMessage - the once-per-attempt discipline is
  explicit and tested.
- **evidence:** the grep (the print sites); the 294 selftest sink
  test ("one failed load produced 1 report line(s)"); the 293
  observation entry.
- **class:** existing capability (the report discipline is correct;
  the 5x was the terminal, not the code; no duplicate-print defect
  existed in the editor).
- **status:** recorded (the discipline made explicit + tested; no
  behavior change).

## Entry 29
- **what:** the Step 300 human feedback, VERBATIM: `<PASTE your Ctrl+S
  result: filenames made, source unchanged True/False, status line
  text, or "not run">` - the template placeholder arrived UNFILLED,
  i.e. the "not run" branch: the Step 299 save-as visual confirmation
  was NOT provided.
- **evidence:** the prompt text (pasted unmodified); no filenames, no
  True/False, no status line text.
- **class:** existing capability (nothing to fix; the save-as code and
  its 299 selftest checks are unchanged and green this step).
- **status:** recorded; the 299 save-as visual confirmation stays
  PENDING (the final-report commands cover it again).

## Entry 30
- **what:** the arrow keys were NOT registered in the editor's
  edge-tracked list (the consumer's ctor list is the registration -
  the same shape as the 297 Tab finding): pe::Input::isEdge returns
  false for untracked keys, so the nudge edges did not exist until the
  editor added them.
- **evidence:** src/input.h:300-333 (the ctor + the untracked-false
  rule); src/main.cpp:870-881 (the GAME registers UP/DOWN - arrows are
  registrable); the editor list grew ESC/+/-/R/Tab/S + UP/DOWN/LEFT/
  RIGHT (kEditorKeys, 10 keys, selftest-checked).
- **class:** existing capability (the registration pattern works as
  documented; adding keys is a small editor-owned change).
- **status:** recorded, fixed editor-side (the arrows registered).

## Entry 31
- **what:** GLFW offers no programmatic key-event injection, so the
  mapping from a PHYSICAL key press to the EditorInput nudge edges
  cannot be proven by an automated test - only the registration (the
  kEditorKeys introspection) and the manual --diag-input run witness
  the live path.
- **evidence:** src/input.h (glfwGetKey polling only; no injection
  API); the selftest keys check ("the editor's tracked list registers
  all four arrows"); the --diag-input nudge line for the manual proof.
- **class:** missing capability (a test-harness/GLFW-level gap, NOT an
  engine defect; the engine's polling contract is unchanged).
- **status:** recorded; the live proof is Cyril's --diag-input run
  (the final-report commands).

## Entry 32
- **what:** the Step 301 human feedback, VERBATIM: `Step 300 ~d7d3302.
  Visual check: <PASS / notes / not run>.` - the template options
  pasted UNFILLED, i.e. the "not run" branch: the Step 300 nudge
  visual confirmation was NOT provided.
- **evidence:** the prompt text (the placeholder unmodified); no PASS,
  no notes.
- **class:** existing capability (nothing to fix; the 300 code and its
  checks are unchanged and green this step).
- **status:** recorded; the 299 save-as AND the 300 nudge visual
  confirmations stay PENDING (the final-report commands cover both).

## Entry 33
- **what:** the Z and Y keys were NOT registered in the editor's
  edge-tracked list (the consumer's ctor list is the registration -
  the same shape as Entries 30/297): pe::Input::isEdge returns false
  for untracked keys, so Ctrl+Z / Ctrl+Y had no edges until the
  editor added them.
- **evidence:** src/input.h:300-333 (the ctor + the untracked-false
  rule); the editor list grew to 12 keys (ESC/+/-/R/Tab/S + the four
  arrows + Z + Y, selftest-checked); the Ctrl LEVEL is the
  engine-proven static isDown.
- **class:** existing capability (the registration pattern works as
  documented; a small editor-owned change).
- **status:** recorded, fixed editor-side (Z and Y registered).

## Entry 34
- **what:** the definition-order rule: EditorState holds
  std::vector<UndoEntry>, so UndoEntry must be defined BEFORE the
  state struct - the first layout had it after and the build FAILED
  (C2065 'UndoEntry': undeclared identifier + vector template
  cascades).
- **evidence:** the first Step 301 build exit 1 (the C2065 at
  editor0_core.h(403) + the C2923/C2976/C2662 cascades); the fix
  moved the struct above EditorState, the rebuild exit 0. The same
  class as Entry 3 (the editor author owns ordering/qualification).
- **class:** existing capability (a compile-time rule; no engine
  issue).
- **status:** recorded, fixed editor-side.

## Entry 35
- **what:** the Step 302 human feedback, VERBATIM: `Step 301 ~1bf97b4.
  Visual nudge/undo/redo: <PASS / notes / not run>.` - the template
  options pasted UNFILLED, i.e. the "not run" branch: the Step 301
  undo/redo visual confirmation was NOT provided.
- **evidence:** the prompt text (the placeholder unmodified); no PASS,
  no notes.
- **class:** existing capability (nothing to fix; the 301 code and its
  checks are unchanged and green this step).
- **status:** recorded; the 299 save-as, the 300 nudge AND the 301
  undo/redo visual confirmations ALL stay PENDING (the commands were
  provided in the 299/300/301 final reports).

## Step 302 classification - the Editor-0 option C slice (299-301), CLOSED

Format: id | class | evidence (the fix commits cited). The human
visual confirmations for 299, 300 and 301: ALL NOT RUN (the three
unfilled placeholders, Entries 29/32/35) - the selftest + the
mutation evidence are the recorded verification.

| id | class |
|---|---|
| 28 the save-as (the editor's first write path) | **existing capability** (+ the counter bug = a fixed defect editor-side, Step 299, commit ae4d516, caught by the test) |
| 29 the 300 human feedback (the unfilled placeholder) | **existing capability** (the record; the 299 visual PENDING) |
| 30 the arrow-key registration (the editor list) | **existing capability** (fixed editor-side, Step 300, commit 38c40de) |
| 31 no programmatic key injection (GLFW) | **missing capability** (a test-harness gap, NOT an engine defect; the live proof = --diag-input) |
| 32 the 301 human feedback (the unfilled placeholder) | **existing capability** (the record; the 300 visual PENDING) |
| 33 the Z/Y key registration (the editor list) | **existing capability** (fixed editor-side, Step 301, commit 6937fb9) |
| 34 the UndoEntry definition order (C2065) | **existing capability** (a compile-time rule, fixed editor-side, Step 301, commit 6937fb9) |

TALLY: 7 entries (28-34) = 6 existing capability, 1 missing
capability (31, the test-harness key injection), 0 engine defect.
The in-step fixed defects already recorded stay as recorded (the 299
counter bug, commit ae4d516; the 300/301 test-setup fixes were not
findings entries - they were caught by the selftest before each
commit). DEFERRED (named, NOT implemented, each needs a
human-authorized later step): saving OVER the source or an existing
file (the write-safety rule stands), path-switch-after-save (the
editor keeps the ORIGINAL path as the reload source after Ctrl+S),
multi-field editing (positions only), gizmos / drag-to-move, the
panel scroll + the visibility toggle (the 297 extras), the
2000-entity stress, a second mutation per step, the engine-side
missing capabilities (the camera zoom API, the wheel/scroll input,
the key injection - docs-first version opens only).

## Entry 36
- **what:** the Step 303 human feedback, VERBATIM: `option C closed
  at 302 ~a828992. Path-switch authorized. Visuals for 299-301 still
  PENDING (do not block this step).` - FILLED this time: the
  EXPLICIT AUTHORIZATION for the path-switch-after-save (the 302
  deferred item) and the confirmation that the three visuals stay
  pending without blocking.
- **evidence:** the prompt text (quoted); the 302 deferred list
  (FINDINGS/the V3 closed-slice section) named path-switch-after-save
  as out.
- **class:** existing capability (the record + the authorization; the
  change is editor-owned, no engine surface).
- **status:** recorded; the authorization implemented this step (the
  success-branch switch + the flat naming rule); the 299/300/301
  visuals stay PENDING.

## Entry 37
- **what:** the deferred item path-switch-after-save RESOLVED: after
  a SUCCESSFUL Ctrl+S the editor's loaded/source path now becomes the
  saved file; a FAILED save changes NOTHING (the path, the baseline,
  the stacks - the negative controls). The naming needed a decision:
  the NESTED rule (sample_scene_edit1_edit1.txt, unbounded growth)
  vs the FLAT rule - the FLAT rule chosen and documented (a trailing
  _edit<digits> suffix is stripped before the new _edit<N> is
  appended; a strip that would empty the base or a non-digit tail is
  skipped).
- **evidence:** editor0_core.h (the success-branch switch; the flat
  strip in makeSaveAsPath); the selftest 315 PASS / 0 FAIL (the path
  + baseline + status updates, the failed-save negative controls,
  the flat-rule unit cases, the stacks kept across the switch, the
  ORIGINAL bytes unchanged); the mutation (the path updated even on a
  failed save) FAILED 6 checks NON-VACUOUS.
- **class:** existing capability (editor-owned; the write-safety
  preserved: the CURRENT path refused by the source rule, the
  ORIGINAL file by the existing-file rule - no existing file is ever
  overwritten).
- **status:** recorded, implemented editor-side.

## Entry 38
- **what:** the Step 304 session human note, VERBATIM: `Human visuals
  299-303 still PENDING - do not block these steps.` - the standing
  confirmation that the five unconfirmed visuals do not gate the
  follow-on steps.
- **evidence:** the prompt text (quoted); the five pending visuals
  (299 save-as, 300 nudge, 301 undo/redo, 303 path-switch + the
  304/305 ones to come).
- **class:** existing capability (the record; nothing to fix).
- **status:** recorded; the visuals stay PENDING.

## Entry 39
- **what:** the inspector panel scroll: the WHEEL is not available in
  the engine (the recorded missing capability, Entries 8/31), so the
  scroll is KEY-DRIVEN - PgUp/PgDn added to the editor's tracked list
  (14 keys; the audit: PAGE_ appears nowhere in the engine or the
  editor input paths - no mapping conflicts). The window keeps the
  12-line panel geometry (the pointInPanelRect contract); at scroll 0
  the last line shows the SAME '+N more' hint as the capped list (the
  discoverability kept); a selection change (pick or nav) and a
  successful reload reset the offset; an empty selection is a safe
  no-op.
- **evidence:** editor0_core.h (clampedPanelScroll,
  inspectorLinesScrolled, kEditorPanelLines, the scroll block + the
  resets); the selftest 345 PASS / 0 FAIL (the clamp cases, the exact
  slices, the 296-identical hint string, the resets, the
  registration); the mutation (the scroll never moves) FAILED 5
  checks NON-VACUOUS.
- **class:** existing capability (the keys are the documented
  substitute for the missing wheel; editor-owned, no engine change).
- **status:** recorded, implemented editor-side.

## Entry 40
- **what:** the Step 305 STOP: after the TOOLS-305 implementation
  commit (43eef8d) and the NON-VACUOUS mutation (a legend line dropped
  -> 3 checks FAILED), the RESTORE + REBUILD hit LNK1104 (cannot open
  PureEditor0.exe) and the lock PERSISTED: read OK, write BLOCKED,
  delete BLOCKED, no stray PureEditor0 process - the 296-class
  exclusive external holder (an AV scan or a stuck sync/index/copy
  tool), invisible to the non-elevated shell. ~7 minutes of waits
  (10/60/120/180 s) did not clear it; the 304 lock had cleared in
  ~70 s. The formal gates for 305 are BLOCKED (the rebuild fails; the
  exe cannot start under ctest).
- **evidence:** the build output (LINK : fatal error LNK1104, twice);
  the probes (READ_OK / WRITE_BLOCKED / DELETE_BLOCKED / no stray
  process); the 296 precedent (the human cleared it by deleting and
  rebuilding; resmon -> CPU -> Associated Handles -> search
  'PureEditor0' identifies the holder).
- **class:** existing capability (the environment lock, not a code or
  engine issue; the committed 43eef8d tree is clean and its selftest
  was 349 PASS / 0 FAIL before the mutation).
- **status:** STOP recorded per the protocol; the RECOVERY (the
  human): clear the lock (resmon/Associated Handles, or delete
  build/Release/PureEditor0.exe and rebuild), then: the build, the
  selftest (349 PASS / 0 FAIL must hold), ctest 5/5, alive x3, the
  diff checks, then the DOCS-305 record (the tracker/V3/board/README)
  - the same shape as the 296 recovery.

## Entry 41
- **what:** the Step 305 RECOVERY: the lock cleared and the HUMAN
  rebuilt and verified. The human evidence, VERBATIM: 'Rebuild
  PureEditor0 succeeded after lock cleared'; 'PureEditor0.exe
  --selftest -> all checks passed (includes scroll + help legend
  lines)'; 'ctest -C Release -> 5/5, 100% (PureEditor0 Passed
  0.35s)'. The agent reproduced: the selftest all checks passed,
  alive x3 (all three True), git diff --check clean,
  git diff --stat -- src/ EMPTY.
- **evidence:** the prompt text (the three quoted lines); the
  reproduced selftest run; the tracker entry 305 flipped
  stopped -> completed.
- **class:** existing capability (the environment lock resolved by
  the human; the committed 43eef8d code needed no change).
- **status:** recorded; the 305 gates are GREEN (the human-proven +
  the reproduced); the legend visual (the 5 bottom lines on screen)
  stays PENDING.

## Step 306 classification - the Editor-0 tools mini-slice (304-305), CLOSED

Format: id | class | evidence. The standing human note (the 304
session, verbatim): 'Human visuals 299-303 still PENDING - do not
block these steps.' The 305 recovery evidence (verbatim, Entry 41):
the rebuild succeeded; the selftest all checks passed; ctest 5/5 100%
(PureEditor0 0.35s).

| id | class |
|---|---|
| 38 the standing human note (the visuals pending, not blocking) | **existing capability** (the record) |
| 39 the panel scroll (the PgUp/PgDn substitute for the missing wheel) | **existing capability** (editor-owned; the wheel stays a missing capability, FINDINGS 8/31) |
| 40 the 296-class lock (the 305 STOP) | **existing capability** (the environment; recovered - the human cleared it) |
| 41 the recovery (the human evidence + the reproduced gates) | **existing capability** (the confirmation; no code change) |

TALLY: 4 entries (38-41) = 4 existing capability, 0 missing
capability, 0 engine defect. The slice verification: the selftest 349
PASS / 0 FAIL (29 scroll checks + 15 legend checks) + two NON-VACUOUS
mutations (304: the scroll never moves - 5 FAILs; 305: a legend line
dropped - 3 FAILs). DEFERRED (named, NOT implemented, each needs a
human-authorized later step): saving over the source, multi-field
editing (positions only), gizmos / drag-to-move, the panel
scroll-by-wheel (needs the engine wheel input - docs-first), the
engine camera zoom API, the 2000-entity stress, MCP / object pools /
ImGui / ECS / glTF / networking (the freezes stand).

## Entry 42
- **what:** the Step 307 visual checklist arrived with EVERY row an
  UNFILLED placeholder (the template pasted back unmodified). The
  rows, VERBATIM: '1 legend visible: <PASS/FAIL/not tried + what he
  saw>' / '2 nudge + modified marker (300): <...>' / '3 undo/redo:
  <...>' / '4 R once guard (300): <...>' / '5 R twice reload (300):
  <...>' / '6 Ctrl+S new _edit1 file (299): <...>' / '7 second Ctrl+S
  _edit2 flat naming (301): <...>' / '8 new file becomes the loaded
  scene (303): <...>' / '9 PgUp/PgDn inspector scroll (304): <...>' /
  '10 panel click keeps selection, empty click clears: <...>' /
  'source file hash unchanged: <True/False>; saved file position vs
  inspector value: <matches/differs/not compared>.' Per the step rule
  ('exactly as reported'; 'do not mark anything CONFIRMED that the
  rows do not support'): the visuals for 299, 300, 301, 303, 304 and
  305 are ALL **NOT TRIED**; no PASS is recorded; no FAIL exists (no
  observation was provided); nothing is marked CONFIRMED.
- **evidence:** the prompt text (the 12 placeholder rows); the
  recorded verification remains the selftest (349 PASS / 0 FAIL), the
  non-vacuous mutations and the human's 305 ctest run (5/5, 0.35s).
- **class:** existing capability (the record; nothing to fix; the
  code is unchanged this step).
- **status:** recorded; the NOT-TRIED status stands until a filled
  checklist arrives.

## Entry 43
- **what:** the Step 308 human visual line, VERBATIM: `<PASTE your
  one-line result, e.g. "visuals: all PASS, source unchanged: True" or
  "visuals: not tried">` - the template pasted back UNFILLED: no PASS
  rows, no FAIL rows, no "not tried" sentence. Per the 307 ruling the
  NOT-TRIED status of the 299-305 visuals stands UNCHANGED (nothing
  marked CONFIRMED, nothing marked FAILED).
- **evidence:** the prompt text (the unfilled placeholder); FINDINGS
  42 (the 307 checklist, also all unfilled).
- **class:** existing capability (the record; nothing to fix; no code
  this step).
- **status:** recorded; the NOT-TRIED status stands.

## Entry 44
- **what:** the DOCS CONTRADICTION resolved: Step 308's Vertical
  slices said "NO package.ps1 exists - packaging is MISSING" while
  the Direction Spec (293) listed package.ps1 as a verified name and
  the human reported scripts/package.ps1 as git-tracked. VERIFIED
  THIS STEP (the search rule applied: build/, .git/, .kilo/ and
  .goose/ excluded): git ls-files lists scripts/package.ps1; the
  recursive search finds EXACTLY ONE main-tree copy
  (D:/PureEngine/scripts/package.ps1, 3911 bytes; the
  .kilo/worktrees/*/scripts/package.ps1 copies are another tool's
  worktrees, excluded). THE 308 CLAIM WAS WRONG - the 308 search was
  NON-RECURSIVE (a root-level dir + Get-ChildItem *.ps1), so it saw
  only the 6 asset-generator PS1s and missed scripts/. What the
  script does (read this step): the arcade release ZIP packager
  (the F-08 re-fix, the Step 143 dry-run documented) - stages
  PureEngine.exe + 18 runtime assets + shaders/ + prefabs/ into
  package/PureEngine-0.1.0-win64.zip (v0.1.0).
- **class:** existing capability (the packaging EXISTS for the arcade
  game; the remaining gap: the EDITOR is not packaged and there is no
  installer/export beyond the zip).
- **status:** recorded; the 308 V3 rows, the 308 tracker note, the
  board and the README corrected this step with a dated pointer.

## Entry 45 (the process finding)
- **what:** .kilo/ and .goose/ contain another tool's WORKTREE COPIES
  inside the repo directory (e.g. .kilo/worktrees/southern-catshark
  24bec72, .kilo/worktrees/zesty-omelet 6013172, each with a full
  tree incl. scripts/package.ps1). A naive recursive search hits
  them and can report DUPLICATE or STALE hits (the copies may lag the
  main tree). The 308 search missed scripts/package.ps1 while a
  worktree-aware search would have found several copies.
- **evidence:** this step's search (the exclusions applied) found one
  main-tree copy; the .kilo/worktrees paths observed but NOT touched
  (per the step orders).
- **class:** existing capability (a search-hygiene/process note, not
  an engine or editor defect).
- **status:** recorded; the standing search rule: ALWAYS exclude
  build/, .git/, .kilo/ and .goose/ and say so in the evidence lines.

## Entry 46
- **what:** the entity DELETE (the editor's first structural edit):
  the DELETE key removes the selected entity via the ALIVE FLAG (not
  vector erasure - the evidence: entity.h:183-189 "kills never erase,
  so indices stay stable"; the saver DROPS dead entities on save,
  scene.h:419-420 "dead entities don't exist in saved state"), so the
  saved file excludes the deleted entity and the undo restores it at
  the SAME index with every serialized field untouched. The delete is
  an undoable command on the same bounded stack as the nudges (the
  typed UndoEntry: isDelete), in order with them; the redo
  re-deletes; the selection clears after a delete; the modified flag
  is honest (the baseline gained the alive dimension:
  captureAliveFlags + sceneDiffers). The key audit: Delete was NOT
  registered (the consumer ctor-list rule) - added (15 keys).
- **evidence:** editor0_core.h (the typed UndoEntry, the delete
  block, applyUndo/applyRedo alive branches, captureAliveFlags/
  aliveCount/sceneDiffers); the selftest 385 PASS / 0 FAIL (36 new
  checks: the N-1 saved file, the field-exact restore, the mixed
  delete/nudge undo/redo sequences, the no-selection no-op, the
  source bytes, the cap at 40 entities, the wrong-index negative
  controls); the mutation (the delete-undo does not restore) FAILED 8
  checks NON-VACUOUS.
- **class:** existing capability (the engine's alive-flag contract
  was already correct and documented; the wiring is editor-owned, no
  engine change).
- **status:** recorded, implemented editor-side.

## Entry 47
- **what:** the entity CREATE (the editor's first scene-growing
  command): N instantiates the CURRENT prefab (the [/ ] keys cycle
  the editor-owned listing) at the VIEW CENTER in world space (the
  camera position - zoom-independent by the conversion formula). The
  create appends a NEW index (the engine APIs: loadPrefab prefab.h:
  103-177 -> instantiatePrefab prefab.h:181-205 -> queueSpawn +
  flushSpawns scene.h:73/:81 - the new index = the old count, the
  vector GROWS); the undo KILLS it (the alive flag, never erasure -
  the index and the fields stay); the redo REVIVES it at the same
  index with the same fields; the saved file contains the
  created-and-alive entities and omits the dead ones (the saver's
  dead-skip, scene.h:419-420). The create is an undoable command on
  the same bounded stack, in order with the nudges and the deletes.
  The listing mirrors the engine probe (prefab.h:71-73: the first
  existing dir among assets/prefabs/, ../assets/prefabs/,
  ../../assets/prefabs/ - from the ctest CWD the FIRST resolves to
  the bundled copy). The keys: N, ], [ - NONE registered before
  (the consumer ctor-list rule) - all three added (18 keys).
  The modified flag is honest across creates (the growth-aware
  sceneDiffers: the entities BEYOND the baseline count as edited only
  while ALIVE - a created-and-undone entity is the clean state).
- **evidence:** editor0_core.h (listPrefabFiles, createEntityFromPrefab,
  the typed UndoEntry isCreate, the create block, the growth-aware
  sceneDiffers); the selftest 441 PASS / 0 FAIL (56 new checks: the
  listing, the field-exact create from enemy.txt, the two-creates
  independence, the undo/redo round, the mixed sequences, the
  save-as round-trip + the omitted undone create, the empty-list and
  malformed no-ops, the cycle wrap, the source bytes, the cap);
  the mutation (the redo of a create does not revive) FAILED 3 checks
  NON-VACUOUS.
- **class:** existing capability (the engine's prefab + spawn APIs
  were already correct and documented; the wiring is editor-owned, no
  engine change).
- **status:** recorded, implemented editor-side.

## Entry 48 (the process findings)
- **what:** three environment/process lessons from this step:
  (1) the git checkouts REWRITE the mixed-ending files to the
  config's CRLF conversion (the multi-line edit-tool anchors die
  after any checkout - the one-shot .ps1 route with the explicit
  CRLF/LF handling is the working substitute); (2) a PowerShell
  function that uses Write-Output for logging AND returns a value
  POLLUTES the return with the log lines (the 309/310 corruption:
  the log strings were concatenated INTO the file content - the
  fix: the script-level $script:raw variable, the functions return
  nothing); (3) the std::remove of a just-created file can LOSE to
  a transient AV lock (the idempotency pre-removes must cover a
  RANGE of derived names, and the asserts should read the ACTUAL
  saved path (the switched state.path) instead of a hard-coded one).
- **evidence:** the 310 in-step failures (the C4430/C2059 corruption
  at main.cpp(1) - repaired by stripping to the first /** header with
  the modifications intact; the 3 non-deterministic (b) failures).
- **class:** existing capability (the tooling/process notes; no
  engine or editor-code defect).
- **status:** recorded; the practices adopted this step.

## Entry 49
- **what:** the Step 311 STOP: the pack's code (the dual view, the 3D
  draw, the meshId keys, the play foundation) and its tests are
  COMMITTED (TOOLS-311 45e7be1 + the tests d593c8d), but the BUILD is
  blocked by the 296-class exclusive lock on PureEditor0.exe (LNK1104;
  write + delete blocked, read OK, no stray process) - ~12 minutes of
  waits (70/120/150/240 s) did not clear it. The gates (the build +
  the selftest + the mutation + ctest) are UNRUN; the tests are
  UNVERIFIED. One compile pass BEFORE the lock confirmed the core
  edits compile (the C2039 setPerspectiveMode -> the actual setter
  setPerspective(fovY, nearZ, farZ) camera.h:225 fixed; the proj
  scoping in the two draw loops fixed) - the FINAL compile+link state
  is UNKNOWN.
- **evidence:** the LNK1104 build outputs (x2); the probes
  (STILL_LOCKED / DELETE_BLOCKED x2 across ~12 min); the 296/305
  precedents.
- **class:** existing capability (the environment lock; not a code
  issue).
- **status:** STOP recorded per the protocol. THE RECOVERY (the
  human): clear the lock (resmon/Associated Handles, or delete
  build/Release/PureEditor0.exe and rebuild), then: the build (the
  compile errors, if any, are fixed editor-side), the selftest (441
  + the new ~14 = ~455 checks must pass - fix any test bug editor-
  side), THE MUTATION (the play-restore broken -> the restore test
  FAILS; restore by pathspec), ctest 5/5, alive x3, the diff checks,
  then the DOCS-311 record (the tracker 311 stopped -> completed,
  V3, the board, the README).

## Entry 50
- **what:** the Step 311 RECOVERY: the lock cleared (the HUMAN rebuilt
  PureEditor0 - the verbatim evidence: 'Build Release OK after lock
  clear'; the selftest 2 FAIL only - BOTH the view-toggle harness
  bug; ctest 4/5 PureEditor0 failed = the STALE exe from the human's
  run BEFORE the fix). THE DIAGNOSIS: the view-toggle test called the
  SECOND stepEditorFrame with keyViewToggle = FALSE (no edge -> no
  toggle) - the SAME harness-bug class as the 310 (g) missing second
  R edge; the PRODUCT code was correct (the enter-3D checks passed).
  THE FIX: the second V press now carries a REAL edge (main.cpp, the
  check (a)). THE POST-FIX GATES (all green): the selftest 462 PASS /
  0 FAIL (x2 runs); ctest 5/5 100% (10.27s; the PureEditor0 duration
  0.46s); alive x3 (all three True); git diff --check exit 0 clean;
  git diff --stat -- src/ EMPTY. THE MUTATION (the deferred
  play-restore broken) FAILED 1 check NON-VACUOUS ('play: the exit
  restored the snapshot FIELD-EXACT'); restored by pathspec.
- **class:** existing capability (a test-harness bug, fixed
  editor-side; the product code needed no change).
- **status:** recorded; the 311 gates are GREEN; the tracker 311
  flipped stopped -> completed. The visuals 299-311 stay NOT TRIED
  except the human's partial 311 evidence (the build + the selftest +
  ctest quoted above; the on-screen visuals remain PENDING).
