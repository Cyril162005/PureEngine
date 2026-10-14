/**
 * PureEngine - Step 316: the ODR link probe, translation unit A (slice 2).
 * Includes ALL Core Foundation headers (the Step 314 list) and links against
 * pe_core together with probe_b.cpp - a multiple-definition (LNK2005-style)
 * failure at link time IS the ODR finding this probe exists to catch.
 * TU A calls Mat4::translation (math/mat4.h) and drives TU B's entry, so both
 * objects (and every symbol they pull from the headers) must resolve exactly
 * once across probe_a.obj + probe_b.obj + pe_core.lib.
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
int b_entry();
}

int main() {
    const pe::Mat4 m = pe::Mat4::translation(pe::Vec3(1.0f, 2.0f, 3.0f));
    const bool ok = (m.m[3][0] == 1.0f) && (m.m[3][1] == 2.0f) && (m.m[3][2] == 3.0f)
        && (probe::b_entry() == 7);
    return ok ? 0 : 1;
}
