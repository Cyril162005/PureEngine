/**
 * PureEngine - Step 330: the mesh OBJ-loader BODIES (Render slice 2).
 * Moved VERBATIM from mesh3d.h :102-192 (inline stripped): loadMeshFromObj -
 * the 3-candidate probe, the 1-based bounds-checked indexing, the malformed
 * warn+skip, the per-face planar UV scheme (the Step 245 documented
 * behavior), out UNTOUCHED on failure, the triangles==0 reject. The
 * contracts are UNCHANGED and pinned by checkMeshLoad's exact asserts.
 */
#include "mesh3d_obj.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace pe {
bool loadMeshFromObj(const std::string& fileName,
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
