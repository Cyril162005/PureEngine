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

}  // namespace editor0

#endif  // EDITOR0_CORE_H
