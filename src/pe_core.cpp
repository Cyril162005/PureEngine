/**
 * PureEngine — Step 102: pe_core static library boundary
 * Optional compiled core: header-only path remains primary (games include headers directly).
 * This translation unit forces instantiation of core headers into a static lib.
 * No behaviour change — same headers, same inline functions.
 */
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
#include "time.h"
#include "ui.h"

namespace pe {
int pe_core_version() { return 102; }
}
