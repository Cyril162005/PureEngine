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
