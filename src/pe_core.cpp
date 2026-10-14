/**
 * PureEngine — Step 102: pe_core static library boundary
 * Optional compiled core. STEP 316 SUPERSESSION (Cyril, 2026-10-14): the
 * "header-only path remains primary" rule is superseded INCREMENTALLY by the
 * migration objective (header-first -> .h/.cpp, category by category; a module
 * stays header-only until its slice moves it) - see PURE_ENGINE_V3.md, the
 * Core Foundation audit + slice sections.
 * This translation unit forces instantiation of core headers into a static lib.
 * No behaviour change — same headers, same inline functions.
 */
#include "math/vec3.h"
#include "math/mat4.h"
#include "animation_data.h"
#include "audio.h"
#include "camera.h"
#include "collision.h"
#include "console.h"
#include "entity.h"
#include "events.h"
#include "font.h"
#include "gamepad.h"
#include "input.h"
#include "lifecycle.h"
#include "lighting.h"
#include "particles.h"
#include "physics.h"
#include "renderer.h"
#include "resources.h"
#include "scene.h"
#include "shader.h"
#include "simulation.h"
#include "tilemap.h"
#include "engine_time.h"
#include "ui.h"
#include "window_guard.h"
#include "mesh3d.h"
#include "components.h"
#include "gamestate.h"
#include "hostile_data.h"
#include "prefab.h"
#include "animation.h"

namespace pe {
int pe_core_version() { return 102; }
}
