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
    AmbientLight ambient;
    std::vector<PointLight> lights;

    void addLight(const PointLight& l) {
        if ((int)lights.size() < 4) lights.push_back(l);
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
