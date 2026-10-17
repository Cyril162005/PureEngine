/**
 * PureEngine - Step 330: the mesh OBJ-loader split (Render slice 2).
 * File: mesh3d_obj.h
 *
 * What moved here (from mesh3d.h :102-192, the body VERBATIM, inline
 * stripped): loadMeshFromObj - the OBJ triangle-soup loader with the
 * 3-candidate probe, the 1-based bounds-checked indexing, the malformed
 * warn+skip, the per-face planar UV scheme (the Step 245 documented
 * behavior), out UNTOUCHED on failure, the triangles==0 reject. mesh3d.h
 * keeps the vertex-data builders (unitCubeVertices/pyramidVertices - pure
 * data, no I/O) and includes THIS header so the existing callers compile
 * unchanged. NO cycle: this header is standalone (math/vec3.h only).
 *
 * The contracts (UNCHANGED, pinned by checkMeshLoad's exact asserts):
 * the 4-face fixture expands to 60 floats, the UVs in [0,1], the missing
 * file -> false + out untouched, the malformed lines warn+skip, the
 * 0-triangle reject.
 */
#ifndef PUREENGINE_MESH3D_OBJ_H
#define PUREENGINE_MESH3D_OBJ_H

#include <string>
#include <vector>
#include "math/vec3.h"

namespace pe {

bool loadMeshFromObj(const std::string& fileName, std::vector<float>& out);

} // namespace pe

#endif // PUREENGINE_MESH3D_OBJ_H
