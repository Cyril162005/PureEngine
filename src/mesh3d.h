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
 *   - no loader: vertices are built by hand here, not parsed.
 *
 * Header-only, like every project module: no mesh3d.cpp, no
 * CMakeLists.txt change.
 */
#ifndef PUREENGINE_MESH3D_H
#define PUREENGINE_MESH3D_H

#include <vector>

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

} // namespace pe

#endif // PUREENGINE_MESH3D_H
