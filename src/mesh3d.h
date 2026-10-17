/**
 * PureEngine — Step 194: 3D debug mesh proof (opt-in)
 * File: mesh3d.h
 *
 * The smallest possible 3D proof for the 2D+3D goal's Phase 1: pure
 * CPU-side vertex data for ONE unit cube, in the EXACT vertex layout
 * the existing world shaders already expect (5 floats per vertex:
 * position xyz + uv st — the uv stays (0,0), a texture is not the
 * point of a debug mesh).
 *
 * What this header is (and nothing more):
 *   - unitCubeVertices(): 36 vertices (12 triangles, 3 per triangle,
 *     CCW winding when viewed from outside) of a unit cube centered at
 *     the origin, corners at ±0.5 on every axis. Pure data: no GL, no
 *     state, directly testable (count, bounds, uv zeros).
 *
 * What it deliberately is NOT:
 *   - no glTF, no index buffers, no materials system, no normals —
 *     the debug proof draws flat-colored triangles;
 *   - no draw call here: the GL path lives in renderer.h's opt-in
 *     drawDebugMesh3D (SMOKE-only — see SMOKE_TEST section 7.9); the
 *     games never call it, so the 2D pipeline is unchanged;
 *   - no loader (Step 194 constraint) — EXCEPT Step 237's minimal
 *     OBJ triangle-soup loader (loadMeshFromObj): the first loaded
 *     mesh, v + f lines only, the same 5-float layout. Step 245: the
 *     loader fills per-face planar UVs (the documented scheme) so the
 *     registered meshes SAMPLE a diffuse texture instead of the (0,0)
 *     one-texel flat color.
 *
 * Header-only, like every project module: no mesh3d.cpp, no
 * CMakeLists.txt change.
 */
#ifndef PUREENGINE_MESH3D_H
#define PUREENGINE_MESH3D_H

#include <vector>
#include <string>

#include "math/vec3.h"  // the loader's position type

namespace pe {

// 36 vertices x 5 floats (pos xyz + uv st = (0,0)); unit cube at the
// origin, corners ±0.5. CCW from outside; the GPU culls nothing here
// (no cull change — the caller owns state as everywhere).
inline std::vector<float> unitCubeVertices() {
    return {
        // -Z face (back)
        -0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f,-0.5f,  0,0,   0.5f, 0.5f,-0.5f,  0,0,
        -0.5f,-0.5f,-0.5f,  0,0,   0.5f, 0.5f,-0.5f,  0,0,  -0.5f, 0.5f,-0.5f,  0,0,
        // +Z face (front)
        -0.5f,-0.5f, 0.5f,  0,0,   0.5f,-0.5f, 0.5f,  0,0,   0.5f, 0.5f, 0.5f,  0,0,
        -0.5f,-0.5f, 0.5f,  0,0,   0.5f, 0.5f, 0.5f,  0,0,  -0.5f, 0.5f, 0.5f,  0,0,
        // -X face (left)
        -0.5f,-0.5f,-0.5f,  0,0,  -0.5f,-0.5f, 0.5f,  0,0,  -0.5f, 0.5f, 0.5f,  0,0,
        -0.5f,-0.5f,-0.5f,  0,0,  -0.5f, 0.5f, 0.5f,  0,0,  -0.5f, 0.5f,-0.5f,  0,0,
        // +X face (right)
         0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f, 0.5f,  0,0,   0.5f, 0.5f, 0.5f,  0,0,
         0.5f,-0.5f,-0.5f,  0,0,   0.5f, 0.5f, 0.5f,  0,0,   0.5f, 0.5f,-0.5f,  0,0,
        // -Y face (bottom)
        -0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f, 0.5f,  0,0,
        -0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f, 0.5f,  0,0,  -0.5f,-0.5f, 0.5f,  0,0,
        // +Y face (top)
        -0.5f, 0.5f,-0.5f,  0,0,   0.5f, 0.5f,-0.5f,  0,0,   0.5f, 0.5f, 0.5f,  0,0,
        -0.5f, 0.5f,-0.5f,  0,0,   0.5f, 0.5f, 0.5f,  0,0,  -0.5f, 0.5f, 0.5f,  0,0,
    };
}

// Step 230: the SECOND geometry (meshId 2): a 4-sided pyramid - a
// square base (2 triangles) + 4 side triangles = 18 vertices x 5
// floats, the apex at (0, 0.5, 0), the base at y = -0.5. Same layout
// as the cube (aPos + aTexCoord zeros).
inline std::vector<float> pyramidVertices() {
    return {
        // base (two triangles, at y = -0.5)
        -0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f, 0.5f,  0,0,
        -0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f, 0.5f,  0,0,  -0.5f,-0.5f, 0.5f,  0,0,
        // four side faces (each to the apex (0, 0.5, 0))
        -0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f,-0.5f,  0,0,   0.0f, 0.5f, 0.0f,  0,0,
         0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f, 0.5f,  0,0,   0.0f, 0.5f, 0.0f,  0,0,
         0.5f,-0.5f, 0.5f,  0,0,  -0.5f,-0.5f, 0.5f,  0,0,   0.0f, 0.5f, 0.0f,  0,0,
        -0.5f,-0.5f, 0.5f,  0,0,  -0.5f,-0.5f,-0.5f,  0,0,   0.0f, 0.5f, 0.0f,  0,0
    };
}

// --- Step 237: the OBJ triangle-soup loader (the first loaded mesh,
// minimal - NOT a glTF full stack) ---
// Loads ONE simple mesh from assets: v (position) + f (triangular
// face, 1-based) lines only — no vn/vt, no quads, no index buffers
// (the debug proof draws flat-colored triangles; uv stays (0,0), the
// same 5-float layout the world shaders already expect). The SAME
// 3-candidate CWD probe every asset uses (the resources.h pattern).
// Malformed v/f lines are SKIPPED with a stderr warning (the
// animation loader pattern); a missing file or zero parsed triangles
// returns false and leaves out UNTOUCHED (the failure-not-cached
// contract). One call = one load; the CALLER owns the vertex data.
// Honest limits: flat-colored output (uv-less meshes sample one
// texel * tint), no normals, no index buffers.

} // namespace pe

// --- Step 330: the OBJ loader moved to mesh3d_obj.h/.cpp (the loader
// contracts unchanged; this header keeps the vertex-data builders and
// includes the loader declarations so the existing callers compile
// unchanged - mesh3d_obj.h is standalone, no include cycle). ---
#include "mesh3d_obj.h"

#endif // PUREENGINE_MESH3D_H
