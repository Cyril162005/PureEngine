/**
 * PureEngine - Step 316: the ODR link probe, translation unit B (slice 2).
 * The SAME include set as TU A (the Step 314 Core Foundation list); TU B
 * calls Vec3::lengthSquared (math/vec3.h) - a different header than TU A's
 * Mat4::translation call. No main() here: probe_a.cpp owns the entry point
 * and calls probe::b_entry() so this object must link too.
 */
#include "math/vec3.h"
#include "math/mat4.h"
#include "entity.h"
#include "collision.h"
#include "gamestate.h"
#include "events.h"
#include "engine_time.h"
#include "lifecycle.h"
#include "input.h"
#include "gamepad.h"
#include "window_guard.h"
#include "mesh3d.h"
#include "components.h"
#include "hostile_data.h"
#include "prefab.h"
#include "animation.h"

namespace probe {
int b_entry() {
    const pe::Vec3 v(3.0f, 4.0f, 0.0f);
    return (v.lengthSquared() == 25.0f) ? 7 : 1;
}
}
