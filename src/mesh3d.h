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
#include <fstream>
#include <sstream>
#include <iostream>

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
inline bool loadMeshFromObj(const std::string& fileName,
                            std::vector<float>& out) {
    std::vector<float> parsed;   // built locally; out is UNTOUCHED on failure
    const std::string candidates[3] = {
        std::string("assets/") + fileName,
        std::string("../assets/") + fileName,
        std::string("../../assets/") + fileName};
    std::ifstream in;
    for (int k = 0; k < 3; ++k) {
        in.open(candidates[k]);
        if (in) break;
        in.clear();
    }
    if (!in) return false;
    std::vector<Vec3> positions;   // 1-based OBJ indexing: slot 0 unused
    positions.push_back(Vec3(0.0f, 0.0f, 0.0f));
    std::size_t triangles = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ls(line);
        std::string tag;
        ls >> tag;
        if (tag == "v") {
            float x = 0.0f, y = 0.0f, z = 0.0f;
            if (ls >> x >> y >> z) {
                positions.push_back(Vec3(x, y, z));
            } else {
                std::cerr << "[mesh] Malformed vertex line skipped\n";
            }
            } else if (tag == "f") {
                int i = 0, j = 0, k2 = 0;
                if (ls >> i >> j >> k2 &&
                    i > 0 && j > 0 && k2 > 0 &&
                    i < static_cast<int>(positions.size()) &&
                    j < static_cast<int>(positions.size()) &&
                    k2 < static_cast<int>(positions.size())) {
                    const Vec3& a = positions[i];
                    const Vec3& b = positions[j];
                    const Vec3& c = positions[k2];
                    // Step 245: the DOCUMENTED UV SCHEME - per-face
                    // planar. Project THIS face's 3 vertices onto the
                    // face's dominant 2D plane (drop the axis of the
                    // largest |normal| component; manual |.|, the
                    // collision.h constexpr-safe rule) and normalize
                    // into [0,1]^2 across the face's bounding box.
                    // WHY: every face gets even texture density - the
                    // diffuse texture repeats evenly per face, no
                    // global shear (a global XY map would stretch the
                    // tetra's side faces).
                    const Vec3 nrm = (b - a).cross(c - a);
                    const float anx = nrm.x >= 0.0f ? nrm.x : -nrm.x;
                    const float any = nrm.y >= 0.0f ? nrm.y : -nrm.y;
                    const float anz = nrm.z >= 0.0f ? nrm.z : -nrm.z;
                    int kx = 0, ky = 0;   // the two KEPT axes
                    if (anx >= any && anx >= anz)      { kx = 1; ky = 2; }  // drop X
                    else if (any >= anz)               { kx = 0; ky = 2; }  // drop Y
                    else                               { kx = 0; ky = 1; }  // drop Z
                    const Vec3* tri[3] = { &a, &b, &c };
                    float cu1[3], cu2[3];
                    float min1 = 1e30f, max1 = -1e30f;
                    float min2 = 1e30f, max2 = -1e30f;
                    for (int v = 0; v < 3; ++v) {
                        cu1[v] = (kx == 0) ? tri[v]->x : (kx == 1) ? tri[v]->y : tri[v]->z;
                        cu2[v] = (ky == 0) ? tri[v]->x : (ky == 1) ? tri[v]->y : tri[v]->z;
                        if (cu1[v] < min1) min1 = cu1[v];
                        if (cu1[v] > max1) max1 = cu1[v];
                        if (cu2[v] < min2) min2 = cu2[v];
                        if (cu2[v] > max2) max2 = cu2[v];
                    }
                    const float span1 = (max1 - min1) > 0.0f ? (max1 - min1) : 1.0f;
                    const float span2 = (max2 - min2) > 0.0f ? (max2 - min2) : 1.0f;
                    for (int v = 0; v < 3; ++v) {
                        parsed.push_back(tri[v]->x); parsed.push_back(tri[v]->y); parsed.push_back(tri[v]->z);
                        parsed.push_back((cu1[v] - min1) / span1);
                        parsed.push_back((cu2[v] - min2) / span2);
                    }
                    ++triangles;
                } else {
                    std::cerr << "[mesh] Malformed/unknown face line skipped\n";
                }
            }
        // Any other tag (vn/vt/o/g/s/...): skipped silently - the
        // debug sample uses none of them.
    }
    if (triangles == 0) {
        return false;
    }
    out = std::move(parsed);   // success only: out replaced, failure untouched
    return true;
}

} // namespace pe

#endif // PUREENGINE_MESH3D_H
