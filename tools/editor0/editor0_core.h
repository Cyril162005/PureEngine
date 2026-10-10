/**
 * PureEditor0 core (Step 288, Tools step 1 of 5) - EDITOR-OWNED FILE.
 *
 * The editor's scene-load boundary. Wraps the frozen engine API
 * pe::loadSceneFromFile (src/scene.h:459) with the editor contract:
 * load into a TEMPORARY Scene and replace `current` only on success;
 * on failure `current` is untouched and err is set with a short reason.
 *
 * Tools rules (PURE_ENGINE_V3, Step 284): documented APIs only; engine
 * src/ is read-only during any Tools step; no engine fix inside a
 * Tools step. Consumes ONLY: pe::loadSceneFromFile (Step 162; the
 * explicit-path + rename-overwrite save contract; the strict
 * whole-file load that builds into a temp Scene and writes out only
 * on success).
 */
#ifndef EDITOR0_CORE_H
#define EDITOR0_CORE_H

#include <cstdio>
#include <fstream>
#include <string>

#include "../../src/scene.h"
#include "../../src/camera.h"

namespace editor0 {

// Load a scene file for the editor.
//   success (true):  `current` has been REPLACED by the loaded scene.
//   failure (false): `current` is untouched; err holds a short reason:
//                    "empty path", "file not found: <path>", or
//                    "parse failed: <path>".
// Deliberate, documented divergence from the engine loader: the path
// as GIVEN must exist (an editor loads the file the user names); the
// engine's "../" and "../../" fallback probes are NOT followed here.
// The engine loader returns bare false with no reason on failure, so
// the not-found vs parse-failed distinction is editor-side (recorded
// in FINDINGS.md).
inline bool loadSceneForEditor(const std::string& path, pe::Scene& current, std::string& err) {
    err.clear();
    if (path.empty()) {
        err = "empty path";
        return false;
    }
    {   // existence probe so err can say WHY (the engine returns bare false)
        std::ifstream probe(path, std::ios::binary);
        if (!probe) {
            err = "file not found: " + path;
            return false;
        }
    }
    pe::Scene tmp;  // never touches `current` until the load has succeeded
    if (!pe::loadSceneFromFile(path, tmp)) {
        err = "parse failed: " + path;
        return false;
    }
    current = tmp;  // replace only on success
    return true;
}

// The editor's status line (pure text-building: no GL, no window, no
// engine call). Tested headless by --selftest with expected strings
// from the inputs.
//   ok:   "editor0: <path> | <count> entities"
//   err:  "editor0 ERROR: <err>"  (err takes precedence; count ignored)
inline std::string makeStatusLine(const std::string& path, std::size_t count, const std::string& err) {
    if (!err.empty()) {
        return "editor0 ERROR: " + err;
    }
    return "editor0: " + path + " | " + std::to_string(count) + " entities";
}

// --- Step 290: pan + zoom (pure math: no GL, no GLFW) ---
// The documented zoom limits (EDITOR-OWNED constants): the frozen
// camera exposes NO zoom API (camera.h has none; halfHeight is locked
// at 4.5 by onResize and there is no half-extent setter), so the
// editor implements zoom as a screen-center scale composed into the
// caller's projection matrix. The limits are documented HERE and in
// the step record; a camera zoom API is a classified missing
// capability for a later engine version.
inline constexpr float kEditorZoomMin = 0.25f;
inline constexpr float kEditorZoomMax = 4.0f;

// Clamp a zoom factor to the documented limits.
inline float clampedZoom(float zoom) {
    if (zoom < kEditorZoomMin) return kEditorZoomMin;
    if (zoom > kEditorZoomMax) return kEditorZoomMax;
    return zoom;
}

// The camera-position delta for a mouse drag of (dxPx, dyPx) pixels,
// computed from the camera formula (screenToUi's mapping):
//   worldPerPixelX = 2*halfW / (zoom*fbWidth)
//   worldPerPixelY = 2*halfH / (zoom*fbHeight)
// The grabbed world point follows the cursor:
//   position += ui(m0) - ui(m1) = (-dx*wppx, +dy*wppy)
// (the Y term flips because screen pixels are y-down while world/UI
// space is y-up). Degenerate inputs (zero fb or zoom) yield (0,0,0) -
// never divide by zero. Pure: no GLFW, no state, directly testable.
inline pe::Vec3 cameraPanDelta(float dxPx, float dyPx, int fbWidth, int fbHeight,
                               float halfW, float halfH, float zoom) {
    if (fbWidth <= 0 || fbHeight <= 0 || zoom <= 0.0f) {
        return pe::Vec3(0.0f, 0.0f, 0.0f);
    }
    const float wppx = 2.0f * halfW / (zoom * static_cast<float>(fbWidth));
    const float wppy = 2.0f * halfH / (zoom * static_cast<float>(fbHeight));
    return pe::Vec3(-dxPx * wppx, dyPx * wppy, 0.0f);
}

// The zoomed projection for drawing: the base projection composed with
// a screen-center scale (S applied LAST in clip space scales about the
// screen center). The engine API takes the caller's matrix unchanged -
// the editor owns the composition.
inline pe::Mat4 zoomedProjection(float zoom, const pe::Mat4& base) {
    return pe::Mat4::scale(pe::Vec3(zoom, zoom, 1.0f)) * base;
}

// The editor's screen<->world conversions at the current zoom: the same
// documented pure helpers (pe::screenToUi / pe::uiToScreen), fed the
// ZOOMED half-extents (the visible span is the base span scaled by
// 1/zoom) plus the camera position (the Camera::screenToWorld formula).
// A non-positive zoom yields (0,0,0) - the conversion is meaningless.
inline pe::Vec3 screenToWorldAtZoom(float mouseX, float mouseY, int fbWidth, int fbHeight,
                                    float halfW, float halfH, float zoom, const pe::Vec3& camPos) {
    if (zoom <= 0.0f) {
        return pe::Vec3(0.0f, 0.0f, 0.0f);
    }
    return pe::screenToUi(mouseX, mouseY, static_cast<float>(fbWidth), static_cast<float>(fbHeight),
                          halfW / zoom, halfH / zoom) + camPos;
}
inline pe::Vec3 worldToScreenAtZoom(const pe::Vec3& world, int fbWidth, int fbHeight,
                                    float halfW, float halfH, float zoom, const pe::Vec3& camPos) {
    if (zoom <= 0.0f) {
        return pe::Vec3(0.0f, 0.0f, 0.0f);
    }
    const pe::Vec3 rel(world.x - camPos.x, world.y - camPos.y, world.z - camPos.z);
    return pe::uiToScreen(rel.x, rel.y, static_cast<float>(fbWidth), static_cast<float>(fbHeight),
                          halfW / zoom, halfH / zoom);
}

// --- Step 294: selection + gesture + report sink (pure: no GL, no GLFW) ---
// The click-vs-drag threshold (EDITOR-OWNED, documented): a press and
// release within this many PIXELS (Euclidean distance) is a CLICK;
// anything further is a DRAG.
inline constexpr float kEditorClickThresholdPx = 4.0f;

enum class PointerGesture { Click, Drag };

// Classify a pointer gesture (pure, testable): the Euclidean pixel
// distance between the press and the release decides. dist == the
// threshold is a CLICK (the boundary is inclusive).
inline PointerGesture classifyPointerGesture(float downX, float downY,
                                             float upX, float upY,
                                             float thresholdPx) {
    const float dx = upX - downX;
    const float dy = upY - downY;
    const float distSq = dx * dx + dy * dy;
    return (distSq <= thresholdPx * thresholdPx)
               ? PointerGesture::Click : PointerGesture::Drag;
}

// The editor's report sink (the Step 294 print discipline): ALL editor
// diagnostics route through ONE function taking an ostream - a failed
// load reports ONCE per attempt, and a std::ostringstream sink can
// count the reports in tests.
inline void reportMessage(std::ostream& out, const std::string& message) {
    out << message << "\n";
}

// The zoom-aware pick (the editor's composition): the world point at
// the current pan+zoom via screenToWorldAtZoom, then the DOCUMENTED
// pe::pickEntity (world coords in, index out). The engine's
// pickEntityAtScreen has no zoom knowledge (it converts with the
// camera's stored half-extents), so the editor composes.
inline int pickEntityAtScreenZoomed(const std::vector<pe::Entity>& entities,
                                    float mouseX, float mouseY,
                                    int fbWidth, int fbHeight,
                                    float halfW, float halfH, float zoom,
                                    const pe::Vec3& camPos) {
    if (zoom <= 0.0f) {
        return -1;
    }
    const pe::Vec3 world = screenToWorldAtZoom(mouseX, mouseY, fbWidth, fbHeight,
                                               halfW, halfH, zoom, camPos);
    return pe::pickEntity(entities, world.x, world.y);
}

// The editor's reload WITH the selection lifecycle (the Step 294
// contract): on SUCCESS the scene is replaced AND the selection is
// CLEARED (the indices are stale after any structural change); on
// FAILURE both `current` and `selected` are UNCHANGED. Returns the
// load result.
inline bool reloadForEditor(const std::string& path, pe::Scene& current,
                            int& selected, std::string& err) {
    const bool ok = loadSceneForEditor(path, current, err);
    if (ok) {
        selected = -1;  // the successful reload clears the selection
    }
    return ok;  // the failed reload leaves the selection unchanged
}

// The status line WITH the selection (the Step 294 extension): the same
// ok format + " | selected <i> (<tag>)" when something is selected;
// selected < 0 = 'no selection' (the Step 295 state; the 3-param base unchanged). The err
// rule is unchanged: err takes precedence (no selection shown).
inline std::string makeStatusLine(const std::string& path, std::size_t count,
                                  const std::string& err,
                                  int selected,
                                  const std::vector<pe::Entity>& entities) {
    std::string s = makeStatusLine(path, count, err);
    if (!err.empty()) {
        return s;
    }
    if (selected >= 0 && selected < static_cast<int>(entities.size())) {
        std::string selTag = entities[static_cast<std::size_t>(selected)].tag;
        if (selTag.size() > 20) selTag = selTag.substr(0, 20) + "...";  // the status bound (the Step 297)
        s += " | selected " + std::to_string(selected) + " ("
             + selTag + ")";
    } else {
        s += " | no selection";   // the Step 295 startup/empty state
    }
    return s;
}

// --- Step 295: the per-frame editor update (the live-path wiring,
// GLFW-free: plain data in) ---
// COORDINATE TRUTH (the Step 295 audit, grep-verified 2026-10-09): the
// cursor comes from pe::Input::pollMouse -> glfwGetCursorPos
// (src/input.h:374) in WINDOW (logical) coordinates; the editor's
// sizes come from glfwGetFramebufferSize (main.cpp:724/:777/:829/:851)
// in FRAMEBUFFER (physical) pixels. With display scaling != 100% the
// two spaces differ by the scale ratio and an unconverted cursor
// picks the WRONG world point (the reported live-path defect: the
// left click showed no clear selection). The fix is editor-side:
// convert the cursor by the window/framebuffer ratio before any pan
// or pick.

// The selection navigation (the Step 297; pure): the next (+1) or the
// previous (-1) ALIVE entity in index order, WRAPPING; no current
// selection -> the first (next) / the last (previous) alive; no alive
// entities -> the current unchanged (a safe no-op; nothing crashes).
// The dead entities are skipped. Testable with any scene.
inline int navigateSelection(const pe::Scene& scene, int current, int direction) {
    const int n = static_cast<int>(scene.entities.size());
    if (n == 0) {
        return current;  // the empty scene: a safe no-op
    }
    int start;
    if (current < 0 || current >= n) {
        start = (direction >= 0) ? 0 : n - 1;  // no selection: the first / the last
    } else {
        start = current + ((direction >= 0) ? 1 : -1);
    }
    for (int step = 0; step < n; ++step) {
        // The scan direction matches: the next walks FORWARD, the
        // previous walks BACKWARD (the wrap is always non-negative).
        const int idx = (direction >= 0)
                            ? (((start + step) % n) + n) % n
                            : (((start - step) % n) + n) % n;
        if (scene.entities[static_cast<std::size_t>(idx)].alive) {
            return idx;
        }
    }
    return current;  // no alive entities: nothing happens
}

// The inspector line cap (the Step 297; documented): the extreme
// values (a 3.4e38 float at 4 decimals = 44 chars, a 100-char tag)
// are capped with a trailing "..." so no inspector line runs past
// this bound.
inline constexpr int kInspectorMaxLineChars = 40;

// --- Step 299: the save-as (the editor's WRITE path; the first one) ---
// The save-as target path (pure): the loaded file's basename +
// "_edit<N>.txt" under savedata/ (a NEW path; a bare source name also
// lands under savedata/; the engine's saver creates the directory for
// explicit paths). Deterministic, no clock.
inline std::string makeSaveAsPath(const std::string& sourcePath, int n) {
    std::string base = sourcePath;
    const std::size_t slash = base.find_last_of("/\\");
    if (slash != std::string::npos) base = base.substr(slash + 1);
    const std::size_t dot = base.find_last_of('.');
    if (dot != std::string::npos && dot > 0) base = base.substr(0, dot);
    return "savedata/" + base + "_edit" + std::to_string(n) + ".txt";
}

// The save-as (the Step 299; the write-safety rules, the V3 option C
// brief): a NEW file path ONLY. Wraps the frozen engine API
// pe::saveSceneToFile (src/scene.h:381, the explicit-path +
// rename-overwrite contract) with the editor refusals: an empty path,
// the currently loaded SOURCE path, and an EXISTING destination are
// REFUSED (the same spirit as the sample refusal) until the
// round-trip is proven. On failure err is set with a short reason.
// sourcePath defaults to empty (no source refusal) for direct calls.
inline bool saveAsForEditor(const std::string& path, const pe::Scene& current, std::string& err,
                            const std::string& sourcePath = std::string()) {
    err.clear();
    if (path.empty()) {
        err = "empty path";
        return false;
    }
    if (!sourcePath.empty() && path == sourcePath) {
        err = "refusing to save over the loaded source: " + path;
        return false;
    }
    {
        std::ifstream probe(path, std::ios::binary);
        if (probe) {
            err = "refusing to overwrite an existing file: " + path;
            return false;
        }
    }
    if (!pe::saveSceneToFile(current, path)) {
        err = "the save failed: " + path;
        return false;
    }
    return true;
}

// The save-as feedback (the Step 299; exact-string tested).
inline std::string makeSaveFeedback(bool ok, const std::string& path,
                                    std::size_t count, const std::string& reason) {
    if (ok) {
        return "saved " + path + " (" + std::to_string(count) + " entities)";
    }
    return "save failed: " + reason;
}

// The per-frame editor input (plain data, no GLFW): main.cpp fills it
// from the real input each frame.
struct EditorInput {
    float cursorX = 0.0f, cursorY = 0.0f;   // WINDOW coordinates (the raw glfwGetCursorPos space)
    int windowWidth = 0, windowHeight = 0;  // the window's logical size
    int fbWidth = 0, fbHeight = 0;          // the framebuffer's pixel size
    bool leftDown = false;                  // the left button level NOW
    bool keyR = false;                      // the R edge (reload)
    bool keyZoomIn = false;                 // the +/- edge (zoom in)
    bool keyZoomOut = false;                // the -/_ edge (zoom out)
    bool keyEsc = false;                    // the ESC edge (quit)
    bool keyNavNext = false;                // the Tab edge WITHOUT Shift (the next, the Step 297)
    bool keyNavPrev = false;                // the Tab edge WITH Shift (the previous, the Step 297)
    bool keySave = false;                   // the Ctrl+S edge (the save-as, the Step 299)
};

// The editor-owned state (everything the per-frame update touches).
struct EditorState {
    pe::Scene current;      // the loaded scene
    std::string path;       // the loaded path (for R + the status)
    pe::Camera camera;      // the editor's camera (main.cpp calls onResize)
    std::string status;     // the base + selection line
    std::string feedback;   // the transient reload feedback (empty = none)
    int selected = -1;      // -1 = nothing selected
    float zoom = 1.0f;
    bool dragging = false;
    bool quitRequested = false;
    float lastX = 0.0f, lastY = 0.0f;   // the last cursor (window coords)
    float downX = 0.0f, downY = 0.0f;   // the press position (window coords)
    // The diag/testability fields (the last click's record):
    int pickCount = 0;                  // how many CLICK picks happened
    int lastPickIndex = -2;             // the last click's pick result
    float lastClickWorldX = 0.0f, lastClickWorldY = 0.0f;  // the converted world point
    // The navigation diag fields (the Step 297):
    int navCount = 0;                   // how many navigation key presses fired
    int lastNavBefore = -2;             // the selection before the last nav
    int lastNavIndex = -2;              // the selection after the last nav
    bool lastNavWasNext = false;        // the last nav's direction
    // The save-as fields (the Step 299):
    int saveCounter = 1;                // the next free _edit<N> number
    int saveCount = 0;                  // how many saves happened (the diag)
    std::string lastSavePath;           // the last saved path
};

// The window->framebuffer ratio conversion (the Step 295 fix): the
// cursor is given in WINDOW coordinates; the conversions need
// FRAMEBUFFER pixels. A degenerate window size (0) passes the cursor
// through unchanged (never divide by zero).
inline float windowToFbX(float cursorX, int windowWidth, int fbWidth) {
    return (windowWidth > 0)
               ? cursorX * (static_cast<float>(fbWidth) / static_cast<float>(windowWidth))
               : cursorX;
}
inline float windowToFbY(float cursorY, int windowHeight, int fbHeight) {
    return (windowHeight > 0)
               ? cursorY * (static_cast<float>(fbHeight) / static_cast<float>(windowHeight))
               : cursorY;
}

// The transient reload feedback (the Step 295 contract): the status
// line shows this after an R, UNTIL THE NEXT EVENT (a click or
// another R); then the base + selection returns. Exact-string tested.
inline std::string makeReloadFeedback(bool ok, const std::string& path,
                                      std::size_t count, const std::string& reason) {
    if (ok) {
        return "reloaded " + path + " (" + std::to_string(count) + " entities)";
    }
    return "reload failed: " + reason;
}

// The per-frame editor update (the Step 295 extraction; editor-owned,
// GLFW-free): the cursor is given in WINDOW coordinates and converted
// to FRAMEBUFFER pixels by the ratio BEFORE any pan or pick. Pure: no
// GL, no GLFW; driven by synthetic EditorInput sequences in tests.
// The forward declaration: stepEditorFrame's click suppression uses
// pointInPanelRect (declared below, the Step 296 section).
inline bool pointInPanelRect(float px, float py, bool panelActive, float halfW, float halfH,
                             int fbWidth, int fbHeight);

inline void stepEditorFrame(EditorState& state, const EditorInput& in) {
    // The cursor in FRAMEBUFFER pixels (the ratio conversion).
    const float fbX = windowToFbX(in.cursorX, in.windowWidth, in.fbWidth);
    const float fbY = windowToFbY(in.cursorY, in.windowHeight, in.fbHeight);

    // The key edges: quit first.
    if (in.keyEsc) {
        state.quitRequested = true;
    }
    // The zoom steps (one per press).
    if (in.keyZoomIn)  state.zoom = clampedZoom(state.zoom * 1.25f);
    if (in.keyZoomOut) state.zoom = clampedZoom(state.zoom / 1.25f);
    // The reload (R): through reloadForEditor (the selection lifecycle);
    // the transient feedback lasts until the next event.
    if (in.keyR) {
        std::string err;
        const bool ok = reloadForEditor(state.path, state.current, state.selected, err);
        state.feedback = makeReloadFeedback(ok, state.path, state.current.entities.size(), err);
        state.status = makeStatusLine(state.path, state.current.entities.size(),
                                      std::string(), state.selected, state.current.entities);
    }
    // The selection navigation (the Step 297): the next/previous keys
    // cycle among the ALIVE entities, wrapping; no selection -> the
    // first (next) / the last (previous) alive; the empty scene is a
    // safe no-op. The nav is an event: the stale feedback clears.
    if (in.keyNavNext || in.keyNavPrev) {
        const int before = state.selected;
        state.selected = navigateSelection(state.current, state.selected,
                                           in.keyNavNext ? 1 : -1);
        state.status = makeStatusLine(state.path, state.current.entities.size(),
                                      std::string(), state.selected, state.current.entities);
        state.feedback.clear();
        ++state.navCount;
        state.lastNavBefore = before;
        state.lastNavIndex = state.selected;
        state.lastNavWasNext = in.keyNavNext;
    }
    // The save-as (the Step 299): Ctrl+S -> the first free
    // savedata/<base>_edit<N>.txt (a NEW path; NEVER the source). The
    // counter advances only on success; the existing-destination
    // refusal auto-advances the scan. The save is an event: the stale
    // feedback clears.
    if (in.keySave) {
        std::string saveErr;
        std::string target;
        bool saved = false;
        int n = state.saveCounter;
        for (int attempt = 0; attempt < 1000; ++attempt, ++n) {
            target = makeSaveAsPath(state.path, n);
            if (saveAsForEditor(target, state.current, saveErr, state.path)) { saved = true; break; }
            if (saveErr.rfind("refusing to overwrite", 0) != 0) break;  // a non-refusal failure: stop
        }
        if (saved) {
            state.saveCounter = n + 1;  // the next free name (the break skips the loop's ++n)
            state.feedback = makeSaveFeedback(true, target, state.current.entities.size(), "");
            ++state.saveCount;
            state.lastSavePath = target;
        } else {
            state.feedback = makeSaveFeedback(false, target, state.current.entities.size(), saveErr);
        }
        state.status = makeStatusLine(state.path, state.current.entities.size(),
                                      std::string(), state.selected, state.current.entities);
    }
    // The pointer: the gesture classification. The threshold is in
    // WINDOW pixels (the same space the press was captured in); the
    // PAN and PICK use the FB-converted cursor.
    if (in.leftDown) {
        if (state.dragging) {
            // The drag pan: the deltas in WINDOW coords, converted to FB
            // pixels by the ratio, then the cameraPanDelta formula.
            const float dxFb = windowToFbX(in.cursorX - state.lastX, in.windowWidth, in.fbWidth);
            const float dyFb = windowToFbY(in.cursorY - state.lastY, in.windowHeight, in.fbHeight);
            const pe::Vec3 pan = cameraPanDelta(dxFb, dyFb, in.fbWidth, in.fbHeight,
                                                state.camera.halfExtentX(), state.camera.halfExtentY(),
                                                state.zoom);
            state.camera.follow(pe::Vec3(state.camera.getPosition().x + pan.x,
                                         state.camera.getPosition().y + pan.y, 0.0f));
        } else {
            state.downX = in.cursorX;   // the press START (window coords)
            state.downY = in.cursorY;
        }
        state.dragging = true;
        state.lastX = in.cursorX;
        state.lastY = in.cursorY;
    } else {
        if (state.dragging) {
            // The release: classify the gesture (WINDOW pixels).
            const PointerGesture g = classifyPointerGesture(state.downX, state.downY,
                                                            in.cursorX, in.cursorY,
                                                            kEditorClickThresholdPx);
            // The inspector panel does not edit (the Step 296 contract):
            // a click INSIDE the panel rect NEVER changes the selection
            // (a complete no-op: no pick, no status change; the entities
            // behind the panel are not clickable - pan/zoom to reach
            // them; documented).
            if (g == PointerGesture::Click &&
                !pointInPanelRect(fbX, fbY, state.selected >= 0, state.camera.halfExtentX(), state.camera.halfExtentY(),
                                  in.fbWidth, in.fbHeight)) {
                // Select: the entity under the FB-converted cursor at the
                // current pan+zoom; empty space (-1) CLEARS.
                state.selected = pickEntityAtScreenZoomed(state.current.entities, fbX, fbY,
                                                          in.fbWidth, in.fbHeight,
                                                          state.camera.halfExtentX(), state.camera.halfExtentY(),
                                                          state.zoom, state.camera.getPosition());
                state.status = makeStatusLine(state.path, state.current.entities.size(),
                                              std::string(), state.selected, state.current.entities);
                state.feedback.clear();  // the next event: the feedback clears
                // The diag/testability record (the last click).
                const pe::Vec3 clickWorld = screenToWorldAtZoom(fbX, fbY, in.fbWidth, in.fbHeight,
                                                                state.camera.halfExtentX(), state.camera.halfExtentY(),
                                                                state.zoom, state.camera.getPosition());
                ++state.pickCount;
                state.lastPickIndex = state.selected;
                state.lastClickWorldX = clickWorld.x;
                state.lastClickWorldY = clickWorld.y;
            }
        }
        state.dragging = false;
    }
}

// --- Step 296: the read-only inspector (pure: no GL, no GLFW) ---
// The floats use the SCENE SAVER's 4-decimal style (std::fixed,
// setprecision(4) - the same "%.4f"), so the panel shows exactly what
// the file round-trips (the 4dp precision is the saver's documented
// view, the 276 finding).
inline std::string f4(float v) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.4f", v);
    return buf;
}

// The inspector lines for ONE entity (the Step 296 contract; pure):
// a FIXED field order - index, tag, role, position, scale, rotation
// (+units), halfExtents, health, textureId, tint, then EVERY field
// the scene saver writes (the saver's 24-field list, scene.h:381-458:
// position, rotationAngle, rotationSpeed, scale, halfExtents,
// textureId, depth, roleId, moveSpeed, velocity, gravityScale,
// isStatic, coyoteTime, jumpImpulse, maxFallSpeed, tint, cols, rows,
// health, timer, tag, parentIndex, animationSpeed, currentClipName).
// Multi-component fields split into per-component lines so every line
// fits the 12x9 default view (<= 22 chars at 0.52 advance). More
// lines than maxLines -> the final "+N more" line. The panel NEVER
// edits. The rotation is the SINGLE-AXIS rotationAngle; the axis is
// PER MODE (z-spin in the 2D path, yaw about +Y in the 3D path -
// renderer.h:732-734/:777), documented on the rot.axis line.
inline std::vector<std::string> makeInspectorLines(const pe::Entity& e, int index, int maxLines) {
    std::vector<std::string> all;
    all.reserve(36);
    all.push_back("entity " + std::to_string(index));
    all.push_back("tag " + e.tag);
    all.push_back("role " + std::to_string(e.roleId));
    all.push_back("pos.x " + f4(e.position.x));
    all.push_back("pos.y " + f4(e.position.y));
    all.push_back("pos.z " + f4(e.position.z));
    all.push_back("scale.x " + f4(e.scale.x));
    all.push_back("scale.y " + f4(e.scale.y));
    all.push_back("scale.z " + f4(e.scale.z));
    all.push_back("rotation " + f4(e.rotationAngle) + " rad");
    all.push_back("rot.speed " + f4(e.rotationSpeed) + " rad/s");
    all.push_back("rot.axis z(2d)/y(3d)");
    all.push_back("half.x " + f4(e.halfExtents.x));
    all.push_back("half.y " + f4(e.halfExtents.y));
    all.push_back("half.z " + f4(e.halfExtents.z));
    all.push_back("health " + f4(e.health));
    all.push_back("textureId " + std::to_string(e.textureId));
    all.push_back("tint.r " + f4(e.tint.x));
    all.push_back("tint.g " + f4(e.tint.y));
    all.push_back("tint.b " + f4(e.tint.z));
    all.push_back("depth " + std::to_string(e.depth));
    all.push_back("moveSpeed " + f4(e.moveSpeed));
    all.push_back("vel.x " + f4(e.velocity.x));
    all.push_back("vel.y " + f4(e.velocity.y));
    all.push_back("vel.z " + f4(e.velocity.z));
    all.push_back("gravityScale " + f4(e.gravityScale));
    all.push_back("isStatic " + std::string(e.isStatic ? "1" : "0"));
    all.push_back("coyoteTime " + f4(e.coyoteTime));
    all.push_back("jumpImpulse " + f4(e.jumpImpulse));
    all.push_back("maxFallSpeed " + f4(e.maxFallSpeed));
    all.push_back("cols " + std::to_string(e.cols));
    all.push_back("rows " + std::to_string(e.rows));
    all.push_back("timer " + f4(e.timer));
    all.push_back("parentIndex " + std::to_string(e.parentIndex));
    all.push_back("animSpeed " + f4(e.animationSpeed));
    all.push_back("clip " + e.currentClipName);
    // The inspector line cap (the Step 297): the extreme values are
    // truncated with the documented "..." rule before the overflow.
    for (std::string& l : all) {
        if (static_cast<int>(l.size()) > kInspectorMaxLineChars) {
            l = l.substr(0, static_cast<std::size_t>(kInspectorMaxLineChars) - 3) + "...";
        }
    }
    if (maxLines > 0 && static_cast<int>(all.size()) > maxLines) {
        const int overflow = static_cast<int>(all.size()) - (maxLines - 1);
        all.resize(maxLines - 1);
        all.push_back("+" + std::to_string(overflow) + " more");
    }
    return all;
}

// The panel lines for the CURRENT selection (the Step 296 contract):
// no selection (or an OOB index) -> ONE "no selection" line.
inline std::vector<std::string> inspectorLinesForSelection(const pe::Scene& scene,
                                                           int selected, int maxLines) {
    if (selected < 0 || selected >= static_cast<int>(scene.entities.size())) {
        return std::vector<std::string>{ "no selection" };
    }
    return makeInspectorLines(scene.entities[static_cast<std::size_t>(selected)], selected, maxLines);
}

// The inspector panel's click rect (the Step 296 contract): the
// panel's drawn extent in UI space (x -6.0..5.7, y -1.2..4.0 - the
// 22-char lines from x -5.8, 12 lines from y 3.6), converted to
// framebuffer pixels with the DOCUMENTED pe::uiToScreen. A click
// inside NEVER changes the selection (the panel does not edit; the
// entities behind the panel are not clickable - pan/zoom to reach
// them; documented).
inline bool pointInPanelRect(float px, float py, bool panelActive, float halfW, float halfH,
                             int fbWidth, int fbHeight) {
    // The panel's ACTUAL drawn extent: the 12-line inspector when a
    // selection exists (panelActive), the one-line "no selection" strip
    // otherwise - the line count is deterministic, so the rect matches
    // what is on screen (a startup strip must never suppress the
    // entity clicks below it).
    const int lineCount = panelActive ? 12 : 1;
    const float yBottomUi = 3.6f - static_cast<float>(lineCount - 1) * 0.42f - 0.35f;
    const pe::Vec3 tl = pe::uiToScreen(-6.0f, 4.0f, static_cast<float>(fbWidth),
                                       static_cast<float>(fbHeight), halfW, halfH);
    const pe::Vec3 br = pe::uiToScreen(5.7f, yBottomUi, static_cast<float>(fbWidth),
                                       static_cast<float>(fbHeight), halfW, halfH);
    return px >= tl.x && px <= br.x && py >= tl.y && py <= br.y;
}

}  // namespace editor0

#endif  // EDITOR0_CORE_H
