/**
 * PureEngine — Step 14: Resource Loading Boundary
 * File: resources.h
 *
 * The engine's second SYSTEM boundary. Step 13 moved every GPU
 * concern into pe::Renderer — including texture LOADING, which the
 * renderer's own header flagged as the seam this step splits out.
 * Before Step 14, file I/O and stb_image decoding lived inside the
 * renderer's init; after it, the renderer OBTAINS its textures
 * through this small boundary and never touches a file itself.
 *
 * What this boundary is (and nothing more):
 *   - two free functions that turn an asset file into a GL texture
 *     name, using the EXACT load/upload pattern the game has run
 *     since Step 10 (checker) and Phase 3 (font atlas);
 *   - the same 3-candidate CWD probe every asset uses (assets/,
 *     ../assets/, ../../assets/), so the exe keeps working from the
 *     repo root, build/, or build/Release/;
 *   - the same forced-channel stbi_load calls, the same
 *     CLAMP_TO_EDGE + LINEAR sampling parameters, the same RGB /
 *     RGBA uploads. Relocated byte-compatible, not redesigned.
 *
 * What it deliberately is NOT (the blueprint's explicit constraint):
 *   - no resource cache, no asset database, no registry, no handles
 *     table, no async loading. One call = one load = one texture
 *     name. A cache is justified only when the project gains more
 *     assets than it currently has — it does not.
 *
 * Ownership rule (unchanged): the functions hand back a GL texture
 * name and forget it. The CALLER owns the name and deletes it — for
 * the six loaded textures that is still pe::Renderer::destroyAll(),
 * exactly as Step 13 left it. Returning 0 on failure keeps working
 * with that pattern because deleting GL name 0 is a safe no-op.
 *
 * Header-only, same discipline as src/math/, entity.h, collision.h,
 * gamestate.h, and renderer.h: no resources.cpp, no CMakeLists.txt
 * change. stb_image follows the Step 10 declare-everywhere /
 * define-once pattern — declarations here, the ONE implementation in
 * src/stb_impl.cpp.
 */
#ifndef PUREENGINE_RESOURCES_H
#define PUREENGINE_RESOURCES_H
// Include guard, same pattern as every other project header.

#include <glad/gl.h>     // glGenTextures / upload calls go through GLAD
#include <stb_image.h>   // declarations only — implementation lives in
                         // src/stb_impl.cpp (Step 10's pattern), so PNG
                         // decoding compiles exactly once in the program.

namespace pe {

// --- The shared RGB load/upload pattern (Step 10 checker, extracted
// in Phase 5, relocated whole from pe::Renderer in Step 14) ---
// Probes the three candidate paths in order, forces 3-channel decode,
// uploads RGB with CLAMP_TO_EDGE + LINEAR sampling. Returns the GL
// texture name, or 0 on failure (0 is never a valid texture name, and
// deleting it is a safe no-op — which is exactly what the caller's
// cleanup path relies on).
inline GLuint loadRgbTexture(const char* candidates[3]) {
    int w = 0, h = 0, c = 0;
    unsigned char* px = NULL;
    for (int k = 0; k < 3; ++k) {
        px = stbi_load(candidates[k], &w, &h, &c, 3);
        if (px) {
            break;
        }
    }
    if (!px) {
        return 0;
    }
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, px);
    stbi_image_free(px);
    return id;
}

// --- The RGBA atlas load/upload pattern (Phase 3 font, relocated
// whole from pe::Renderer in Step 14) ---
// Same 3-candidate probe, but FOUR forced channels: the alpha is what
// lets the digit glyphs blend over the scene instead of painting solid
// backing rectangles. Same CLAMP_TO_EDGE + LINEAR sampling as every
// other game texture. Returns the GL texture name, or 0 on failure.
inline GLuint loadRgbaTexture(const char* candidates[3]) {
    int w = 0, h = 0, c = 0;
    unsigned char* px = NULL;
    for (int k = 0; k < 3; ++k) {
        px = stbi_load(candidates[k], &w, &h, &c, 4);
        if (px) {
            break;
        }
    }
    if (!px) {
        return 0;
    }
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // RGBA, not RGB: the alpha channel is the point of this texture.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
    stbi_image_free(px);
    return id;
}

} // namespace pe

#endif // PUREENGINE_RESOURCES_H
