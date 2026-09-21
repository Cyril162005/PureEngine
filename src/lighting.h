/**
 * PureEngine — Step 79: 2D Lighting State Boundary
 * File: lighting.h
 * Boundary: Lighting
 *
 * Plain-data light state: one ambient term plus a fixed-capacity vector
 * of point lights. No GL objects live here — the renderer's lit draw
 * path (Step 79, renderer.h setLightUniforms) reads this state and
 * uploads it as uniforms; this boundary owns only WHAT the light data
 * is and how it is capped.
 *
 * Capacity contract: addLight() silently ignores pushes beyond
 * MAX_LIGHTS (4) — the same bound the shader's uniform array and the
 * renderer's upload loop assume, so the cap is consistent at every
 * consumer. Named as a constant (rather than repeating the literal 4)
 * so a future shader-array change has exactly one place to edit.
 *
 * Header-only, same discipline as every project module: no
 * lighting.cpp, no CMakeLists.txt change.
 */
#ifndef PUREENGINE_LIGHTING_H
#define PUREENGINE_LIGHTING_H

#include "math/vec3.h"
#include <vector>

namespace pe {

struct PointLight {
    Vec3 position;
    Vec3 color;
    float radius = 5.0f;
    float intensity = 1.0f;
};

struct AmbientLight {
    Vec3 color = Vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
};

struct LightingState {
    // The one bound every consumer assumes: the uniform array size in
    // lit.frag, the renderer's upload loop, and addLight's cap.
    static constexpr int MAX_LIGHTS = 4;

    AmbientLight ambient;
    std::vector<PointLight> lights;

    void addLight(const PointLight& l) {
        if ((int)lights.size() < MAX_LIGHTS) lights.push_back(l);
    }

    int lightCount() const {
        return (int)lights.size();
    }

    static LightingState makeDefault() {
        LightingState s;
        s.ambient = {Vec3(1,1,1), 1.0f};
        return s;
    }
};

}  // namespace pe

#endif
