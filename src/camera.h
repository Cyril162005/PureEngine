/**
 * PureEngine — Step 15: Camera Module Boundary
 * File: camera.h
 *
 * The engine's third SYSTEM boundary. Before this step, camera
 * responsibility was scattered across TWO owners:
 *   - main.cpp held the camera STATE (a bare Vec3 position), the
 *     movement SPEED constant, the inline WASD arithmetic, the
 *     reset-to-origin line inside resetGame(), and the once-built
 *     orthographic PROJECTION matrix;
 *   - renderer.h performed the camera MATH — drawWorld rebuilt the
 *     lookAt VIEW matrix from the position it was handed.
 * Step 13's renderer header named this exact seam; Step 15 splits it
 * out. State, movement, and BOTH matrices now live here, and nothing
 * else.
 *
 * What moved in (relocated, not redesigned — every value is the one
 * the game has run with since Step 6 and the balance tuning):
 *   - position: world space, starts at the origin (0, 0, 0)
 *   - movement: speed 3.0 world units per second, applied as
 *     direction * speed * deltaTime — frame-rate independent, same
 *     pattern as Step 5's rotation. The KEY MEANING (which key is
 *     which direction) stays in main.cpp — polling keys is Step 16's
 *     future input boundary, so this class does NOT include or call
 *     GLFW; movement arrives as plain data.
 *   - view: Mat4::lookAt from the position, aiming at
 *     position + (0, 0, -1) with up (0, 1, 0) — the exact Step 6
 *     math, previously built inside the renderer.
 *   - projection: Mat4::orthographic(-6, 6, -4.5, 4.5, -1, 1) — the
 *     balance-tuned 12 x 9 box (4:3, 66.7 px per world unit at
 *     800x600), built ONCE; nothing about it changes per frame.
 *
 * What this class deliberately does NOT introduce:
 *   - no key polling, no GLFW dependency (Step 16's territory)
 *   - no follow/lerp/shake logic — the camera has never had any
 *   - no projection switching or viewport management
 * One concrete orthographic camera, relocated whole.
 *
 * Header-only, same discipline as src/math/, entity.h, collision.h,
 * gamestate.h, renderer.h, and resources.h: no camera.cpp, no
 * CMakeLists.txt change.
 */
#ifndef PUREENGINE_CAMERA_H
#define PUREENGINE_CAMERA_H
// Include guard, same pattern as every other project header.

#include "math/vec3.h"   // position state, lookAt inputs
#include "math/mat4.h"   // lookAt / orthographic builders

namespace pe {

class Camera {
public:
    // --- Step 11's reset line, relocated: back to the world origin ---
    // resetGame() calls this; a new run always starts with the camera
    // where the world was framed at launch.
    void reset() {
        position = Vec3(0.0f, 0.0f, 0.0f);
    }

    // --- Step 6's WASD arithmetic, relocated as DATA-IN ---
    // directionX/directionY are -1, 0, or +1 — decided by main.cpp's
    // key polling (the PLAYING-only gate stays there too). Multiplied
    // by the speed and the frame's deltaTime, exactly the expression
    // the inline code used: axis += direction * speed * dt.
    void move(float directionX, float directionY, float deltaTime) {
        position.x += directionX * moveSpeed * deltaTime;
        position.y += directionY * moveSpeed * deltaTime;
    }

    // The world-space position. Owned HERE now — main.cpp reads it
    // only if it ever needs the raw value; the renderer never sees it.
    const Vec3& getPosition() const {
        return position;
    }

    // --- Step 6's view matrix, relocated OUT of the renderer ---
    // lookAt builds the camera's INVERSE transform: camera at
    // 'position', aiming down -Z (target = position + (0,0,-1)),
    // world +Y up. Shifting the position moves every rendered vertex
    // by the OPPOSITE amount — the camera pans, the geometry stays
    // put. Rebuilt per frame, exactly as drawWorld used to do it.
    Mat4 view() const {
        return Mat4::lookAt(position,
                            position + Vec3(0.0f, 0.0f, -1.0f),
                            Vec3(0.0f, 1.0f, 0.0f));
    }

    // --- Step 6's projection, relocated ---
    // Built ONCE at construction; orthographic means apparent size
    // never changes with depth — right for this flat scene.
    const Mat4& projection() const {
        return proj;
    }

private:
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);   // Step 6: world-space camera position

    // Camera speed in WORLD UNITS PER SECOND. Multiplied by deltaTime
    // per frame, so panning is frame-rate independent.
    const float moveSpeed = 3.0f;

    // The PROJECTION matrix: maps the visible slice of world space
    // onto the clip cube. BALANCE TUNING: widened from Step 6's 8 x 6
    // box to a 12 x 9 box — more arena, more reaction time. The 4:3
    // aspect ratio is preserved (12:9 = 800:600), so shapes keep
    // their proportions (no stretching); a world unit simply covers
    // fewer pixels (66.7 instead of 100), which is why everything
    // renders visually smaller.
    const Mat4 proj = Mat4::orthographic(-6.0f, 6.0f, -4.5f, 4.5f, -1.0f, 1.0f);
};

} // namespace pe

#endif // PUREENGINE_CAMERA_H
