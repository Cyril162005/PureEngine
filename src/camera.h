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
 *     input boundary (Step 16), so this class does NOT include or call
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
 *   - no projection switching or viewport management
 * Contract freeze (Step 83): arcade uses follow() (snap) / followLerp() (smooth);
 * move() retained for future free-pan games, not wired to arcade. One concrete
 * orthographic camera, relocated whole.
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

// --- Step 125: screen-to-world conversion core (pure, testable) ---
// Converts one window/framebuffer PIXEL coordinate into the ortho box's
// WORLD-UNIT coordinates, given the box half-extents the projection was
// built from. The inverse of the ortho mapping the renderer applies:
//   ndcX = (pixelX / fbWidth)  * 2 - 1      worldX = ndcX * halfW
//   ndcY = 1 - (pixelY / fbHeight) * 2      worldY = ndcY * halfH
// The Y term FLIPS the axis: mouse pixels are (0,0) TOP-LEFT with y-down
// (the raw glfwGetCursorPos space), while world/UI space is origin-at-
// CENTER with y-up. Degenerate framebuffer (zero/negative size) yields
// (0,0,0) — never divide by zero. Pure: no GLFW, no state, directly
// testable with known projection numbers.
inline Vec3 screenToUi(float mouseX, float mouseY,
                       float fbWidth, float fbHeight,
                       float halfW, float halfH) {
    if (fbWidth <= 0.0f || fbHeight <= 0.0f) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    const float ndcX = (mouseX / fbWidth) * 2.0f - 1.0f;
    const float ndcY = 1.0f - (mouseY / fbHeight) * 2.0f;
    return Vec3(ndcX * halfW, ndcY * halfH, 0.0f);
}

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

    // --- Camera follow (snap-to, no lerp) ---
    // Sets camera position to match a target's world position.
    // Kept separate from move() so WASD free-pan can coexist if desired.
    void follow(const Vec3& target) {
        position.x = target.x;
        position.y = target.y;
    }

    // Smooth follow: exponential approach toward the target. Same
    // destination as follow(), but the camera trails behind motion so the
    // player visibly moves relative to the world before the camera catches
    // up. factor is clamped to [0,1] so low-fps frames converge without
    // overshooting. Additive: follow() behavior is untouched.
    void followLerp(const Vec3& target, float dt, float speed = 5.0f) {
        float blend = speed * dt;
        if (blend < 0.0f) {
            blend = 0.0f;
        }
        if (blend > 1.0f) {
            blend = 1.0f;
        }
        position.x += (target.x - position.x) * blend;
        position.y += (target.y - position.y) * blend;
    }

    // --- Step 116: aspect-correct resize ---
    // Called on framebuffer resize; vertical world size locked at ±4.5,
    // horizontal scales with aspect. Zero/negative size is a no-op (minimized).
    // Step 125: the half-extents are also STORED so the screen-to-world
    // conversion helpers read the same values the projection was built from.
    void onResize(int width, int height) {
        if (width <= 0 || height <= 0) return;
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        halfHeight = 4.5f;
        halfWidth = halfHeight * aspect;
        proj = Mat4::orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
    }

    // --- Step 125: screen-to-world conversion (thin wrappers) ---
    // COORDINATE CONTRACT (documented here, owned here):
    //   - mouse input: window/framebuffer PIXELS, (0,0) at the TOP-LEFT,
    //     y-down — the raw glfwGetCursorPos space (pe::MouseState, Step 117).
    //   - world/UI space: origin at the CENTER, y-up, in WORLD UNITS —
    //     the ortho box [-halfWidth, halfWidth] x [-halfHeight, halfHeight]
    //     that the projection maps onto the framebuffer.
    //   - resize: onResize keeps halfHeight locked at 4.5 and scales
    //     halfWidth with aspect, so conversion always matches the live
    //     projection (Step 116 behavior unchanged).
    //   - screenToWorldUi IGNORES camera position: it converts into the
    //     screen-space rectangle UI elements are drawn in (projection *
    //     model, NO VIEW — the Step 21/drawHud contract).
    //   - screenToWorld ADDS the camera position: this camera's view is
    //     a pure translation (identity basis, lookAt straight down -Z),
    //     so world = uiCoords + position — the inverse of the world MVP.
    Vec3 screenToWorldUi(float mouseX, float mouseY,
                         float fbWidth, float fbHeight) const {
        return screenToUi(mouseX, mouseY, fbWidth, fbHeight,
                          halfWidth, halfHeight);
    }
    Vec3 screenToWorld(float mouseX, float mouseY,
                       float fbWidth, float fbHeight) const {
        return screenToWorldUi(mouseX, mouseY, fbWidth, fbHeight) + position;
    }

private:
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);   // Step 6: world-space camera position

    // Camera speed in WORLD UNITS PER SECOND. Multiplied by deltaTime
    // per frame, so panning is frame-rate independent.
    const float moveSpeed = 3.0f;

    // Step 125: the visible ortho box's half-extents, mirrored from
    // whatever onResize last built (defaults = the 12x9 launch box, so
    // a camera that never resized converts correctly too).
    float halfWidth = 6.0f;
    float halfHeight = 4.5f;

    // The PROJECTION matrix: maps the visible slice of world space
    // onto the clip cube. BALANCE TUNING: widened from Step 6's 8 x 6
    // box to a 12 x 9 box — more arena, more reaction time. The 4:3
    // aspect ratio is preserved (12:9 = 800:600), so shapes keep
    // their proportions (no stretching); a world unit simply covers
    // fewer pixels (66.7 instead of 100), which is why everything
    // renders visually smaller.
    Mat4 proj = Mat4::orthographic(-6.0f, 6.0f, -4.5f, 4.5f, -1.0f, 1.0f);
};

} // namespace pe

#endif // PUREENGINE_CAMERA_H
