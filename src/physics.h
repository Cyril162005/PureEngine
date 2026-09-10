/**
 * PureEngine — Step 60: Physics Foundation (data + free functions)
 * File: physics.h
 *
 * Minimal 2D physics vocabulary: a world gravity constant and four
 * stateless free functions (simulation.h precedent — no class, no
 * scheduler). All vectors are Vec3 with z = 0 (the math layer has no
 * Vec2, and every velocity in the codebase is already a Vec3).
 * Functions take Vec3 references, not Entities: they work on any
 * position/velocity pair and know nothing about entity structure.
 *
 * Honest physics model: massless. There is no mass, no force
 * accumulation, and no persistent acceleration field (nothing to
 * forget to clear) — "force" here means acceleration applied now.
 * gravityScale 0 = inert (the default every existing entity gets:
 * this step changes no behavior anywhere); 1 = full GRAVITY.
 *
 * Nothing calls these functions yet — adoption (bounce response,
 * platformer proof) is separately scoped work. This step is data
 * structures + verified compilation only.
 *
 * Header-only, same discipline as every project module: no
 * physics.cpp, no CMakeLists.txt change.
 */
#ifndef PUREENGINE_PHYSICS_H
#define PUREENGINE_PHYSICS_H
// Include guard, same pattern as every other project header.

#include "math/vec3.h"  // Vec3 arithmetic only
#include "entity.h"    // resolveCollision mutates Entity positions/velocities

namespace pe {

// World gravity, world-units/s^2. Tunable; negative Y (down on screen).
constexpr Vec3 GRAVITY(0.0f, -9.8f, 0.0f);

// Gravity as a velocity change: v += GRAVITY * scale * dt.
// Skipped entirely at scale 0 (the default) — inert entities pay a branch.
inline void applyGravity(Vec3& velocity, float gravityScale, float dt) {
    if (gravityScale > 0.0f) {
        velocity = velocity + (GRAVITY * gravityScale * dt);
    }
}

// Instant velocity change: v += force * dt. Massless — force IS the
// acceleration (see header note); callers pass pre-scaled values.
inline void applyForce(Vec3& velocity, const Vec3& force, float dt) {
    velocity = velocity + (force * dt);
}

// Semi-implicit Euler position step: p += v * dt. Call AFTER all
// velocity updates for the frame (gravity, forces, then integrate).
inline void integrate(Vec3& position, const Vec3& velocity, float dt) {
    position = position + (velocity * dt);
}

// --- Collision response (Step 62, extended Step 71) ---
// Impulse-based AABB separation + bounce for two overlapping boxes.
// Pair with aabbOverlap (collision.h): the caller establishes overlap
// first; if the boxes turn out separated on any axis this returns
// without touching either body (consistent with detection's strict <).
// 2D only: the scene is flat (z extents are 0), so only X/Y resolve.
//   1. Overlap depth per axis (world-space half sizes: extents scale
//      with the entity — the Step 8 rule, same as detection).
//   2. Push-out along the min-penetration axis. If NEITHER body is
//      static, split EQUALLY (massless, equal correction). If ONE body
//      is static, the static body does NOT move and the full correction
//      goes to the dynamic body. If BOTH are static, nothing moves.
//   3. Impulse along the axis normal, equal masses, scaled by
//      restitution (0 = stick together, 1 = elastic). Applied only
//      when approaching (relative normal velocity < 0); separating
//      pairs keep the positional fix with no velocity change.
// Static bodies never receive impulse and their velocity is not modified.
// Manual |.| throughout (collision.h's constexpr-safe rule: no fabs).
inline void resolveCollision(Entity& a, Entity& b, float restitution = 0.5f) {
    if (restitution < 0.0f) {
        restitution = 0.0f;
    }
    if (restitution > 1.0f) {
        restitution = 1.0f;
    }

    const float ahx = a.halfExtents.x * a.scale.x;
    const float ahy = a.halfExtents.y * a.scale.y;
    const float bhx = b.halfExtents.x * b.scale.x;
    const float bhy = b.halfExtents.y * b.scale.y;
    const float dx = b.position.x - a.position.x;
    const float dy = b.position.y - a.position.y;
    const float adx = dx >= 0.0f ? dx : -dx;
    const float ady = dy >= 0.0f ? dy : -dy;
    const float overlapX = (ahx + bhx) - adx;
    const float overlapY = (ahy + bhy) - ady;
    if (overlapX <= 0.0f || overlapY <= 0.0f) {
        return;
    }

    float nx = 0.0f, ny = 0.0f, pen = 0.0f;
    if (overlapX < overlapY) {
        nx = dx >= 0.0f ? 1.0f : -1.0f;
        pen = overlapX;
    } else {
        ny = dy >= 0.0f ? 1.0f : -1.0f;
        pen = overlapY;
    }

    // Static body handling: static bodies don't move from collisions
    const bool aStatic = a.isStatic;
    const bool bStatic = b.isStatic;

    if (aStatic && bStatic) {
        return;  // both static: no movement possible
    }

    if (aStatic || bStatic) {
        // One static, one dynamic: full correction on dynamic body
        if (aStatic) {
            b.position.x += nx * pen;
            b.position.y += ny * pen;
            // Only dynamic body gets impulse
            const float relNx = (b.velocity.x) * nx;
            const float relNy = (b.velocity.y) * ny;
            if (relNx + relNy < 0.0f) {
                const float impulse = -(1.0f + restitution) * (relNx + relNy);
                b.velocity.x += nx * impulse;
                b.velocity.y += ny * impulse;
            }
        } else {
            a.position.x -= nx * pen;
            a.position.y -= ny * pen;
            const float relNx = (a.velocity.x) * nx;
            const float relNy = (a.velocity.y) * ny;
            if (relNx + relNy < 0.0f) {
                const float impulse = -(1.0f + restitution) * (relNx + relNy);
                a.velocity.x -= nx * impulse;
                a.velocity.y -= ny * impulse;
            }
        }
        return;
    }

    // Neither static: equal split (original behavior)
    const float half = pen * 0.5f;
    a.position.x -= nx * half;
    a.position.y -= ny * half;
    b.position.x += nx * half;
    b.position.y += ny * half;

    const float relNx = (b.velocity.x - a.velocity.x) * nx;
    const float relNy = (b.velocity.y - a.velocity.y) * ny;
    if (relNx + relNy >= 0.0f) {
        return;
    }
    const float impulse = -(1.0f + restitution) * (relNx + relNy) * 0.5f;
    a.velocity.x -= nx * impulse;
    a.velocity.y -= ny * impulse;
    b.velocity.x += nx * impulse;
    b.velocity.y += ny * impulse;
}

// --- Character controller (Step 71: platformer hardening) ---
// Minimal platformer character controller built on the existing physics
// primitives. The controller:
//   - Tracks grounded state (was on floor last frame)
//   - Coyote time: brief window after leaving ground where jump is still
//     allowed (prevents "missed jump" frustration at frame boundaries)
//   - Jump: applies upward impulse only when grounded or within coyote
//     window; clears coyote timer
//   - Horizontal movement: applies force, then integrates; wall collisions
//     cancel horizontal velocity (no wall-sticking)
//   - Ceiling collision: cancels upward velocity on impact
//   - Requires the character entity to have gravityScale > 0
//   - Uses existing resolveCollision for horizontal/vertical separation
//   - The caller must call checkGrounded AFTER horizontal/vertical
//     collisions are resolved for the frame
// This is a free function set (no class), same discipline as the rest
// of physics.h. Controller state lives ON the Entity (coyoteTimer,
// jumpImpulse, maxFallSpeed, wasGrounded — entity.h Step 71 fields) so
// there is no parallel object to keep in sync; horizontal intent arrives
// via velocity.x, which the caller's input handling sets beforehand.

// Check if character is standing on a static body (floor).
// Scans all static entities below the character within a small vertical
// tolerance (0.05 world units). Returns true if any contact found.
// Intended to be called AFTER vertical collision resolution for the frame.
inline bool checkGrounded(const Entity& character,
                          const std::vector<Entity>& entities) {
    const float tolerance = 0.05f;
    const float chx = character.halfExtents.x * character.scale.x;
    for (const Entity& e : entities) {
        if (!e.isStatic) continue;
        // Quick AABB check: character's bottom edge near entity's top
        const float charBottom = character.position.y -
            (character.halfExtents.y * character.scale.y);
        const float entityTop = e.position.y +
            (e.halfExtents.y * e.scale.y);
        if (entityTop >= charBottom - tolerance &&
            entityTop <= charBottom + tolerance) {
            // Horizontal overlap check
            const float chx_s = character.halfExtents.x * character.scale.x;
            const float ehx_s = e.halfExtents.x * e.scale.x;
            const float dx = e.position.x - character.position.x;
            const float adx = dx >= 0.0f ? dx : -dx;
            if (adx < chx_s + ehx_s) {
                return true;
            }
        }
    }
    return false;
}

// Update character controller for one frame.
// Order: horizontal force -> gravity -> integrate -> horizontal collision
// resolution -> vertical collision resolution -> grounded check.
// Returns true if the character is grounded AFTER this frame's updates.
inline bool updateCharacterController(Entity& character,
                                      const std::vector<Entity>& staticEntities,
                                      float dt,
                                      bool jumpPressed) {
    if (dt <= 0.0f) return false;  // no-op for non-positive dt

    // --- Coyote timer ---
    const bool grounded = checkGrounded(character, staticEntities);
    if (grounded) {
        character.coyoteTimer = character.coyoteTime;
    } else if (character.coyoteTimer > 0.0f) {
        character.coyoteTimer -= dt;
        if (character.coyoteTimer < 0.0f) character.coyoteTimer = 0.0f;
    }

    // --- Horizontal force ---
    // (Caller sets character.velocity.x based on input before calling,
    //  or we could add moveLeft/moveRight helpers. For now we assume
    //  velocity.x is already set by the caller's input handling.)

    // --- Gravity + integration ---
    if (character.gravityScale > 0.0f) {
        applyGravity(character.velocity, character.gravityScale, dt);
    }
    // Clamp fall speed
    if (character.velocity.y < -character.maxFallSpeed) {
        character.velocity.y = -character.maxFallSpeed;
    }
    integrate(character.position, character.velocity, dt);

    // --- Static collision resolution (walls/floor/ceiling, one pass) ---
    // Each overlap resolves along its MIN-penetration axis: floor/ceiling
    // vs wall is decided PER PAIR. (A split horizontal-then-vertical pass
    // misfires here: floor contact also overlaps horizontally, and a pure
    // horizontal pass would shove the character sideways off the floor
    // instead of landing it. Single min-axis pass, full correction on the
    // character — statics never move. The resolved velocity component
    // zeroes, so walls stop slides and floors/ceilings stop falls/rises.)
    bool hitCeiling = false;
    for (const Entity& wall : staticEntities) {
        if (!wall.isStatic) continue;
        const float chx = character.halfExtents.x * character.scale.x;
        const float chy = character.halfExtents.y * character.scale.y;
        const float whx = wall.halfExtents.x * wall.scale.x;
        const float why = wall.halfExtents.y * wall.scale.y;
        const float dx = wall.position.x - character.position.x;
        const float dy = wall.position.y - character.position.y;
        const float adx = dx >= 0.0f ? dx : -dx;
        const float ady = dy >= 0.0f ? dy : -dy;
        const float overlapX = (chx + whx) - adx;
        const float overlapY = (chy + why) - ady;
        if (overlapX > 0.0f && overlapY > 0.0f) {
            if (overlapX < overlapY) {
                const float nx = dx >= 0.0f ? 1.0f : -1.0f;
                character.position.x -= nx * overlapX;
                character.velocity.x = 0.0f;
            } else {
                const float ny = dy >= 0.0f ? 1.0f : -1.0f;
                character.position.y -= ny * overlapY;
                if (ny > 0.0f) {
                    hitCeiling = true;  // wall above: ceiling
                }
                character.velocity.y = 0.0f;
            }
        }
    }

    // --- Jump ---
    if (jumpPressed && (character.coyoteTimer > 0.0f || checkGrounded(character, staticEntities))) {
        character.velocity.y = character.jumpImpulse;
        character.coyoteTimer = 0.0f;
    }

    // Ceiling hit cancels upward velocity
    if (hitCeiling && character.velocity.y > 0.0f) {
        character.velocity.y = 0.0f;
    }

    // Update wasGrounded for next frame
    character.wasGrounded = checkGrounded(character, staticEntities);
    return character.wasGrounded;
}

} // namespace pe

#endif // PUREENGINE_PHYSICS_H
