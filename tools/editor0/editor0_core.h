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

}  // namespace editor0

#endif  // EDITOR0_CORE_H
