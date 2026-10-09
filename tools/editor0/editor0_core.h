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
        s += " | selected " + std::to_string(selected) + " ("
             + entities[static_cast<std::size_t>(selected)].tag + ")";
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
            if (g == PointerGesture::Click) {
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

}  // namespace editor0

#endif  // EDITOR0_CORE_H
