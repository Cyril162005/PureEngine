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

// --- Collision response (Step 62, new — no predecessor) ---
// Impulse-based AABB separation + bounce for two overlapping boxes.
// Pair with aabbOverlap (collision.h): the caller establishes overlap
// first; if the boxes turn out separated on any axis this returns
// without touching either body (consistent with detection's strict <).
// 2D only: the scene is flat (z extents are 0), so only X/Y resolve.
//   1. Overlap depth per axis (world-space half sizes: extents scale
//      with the entity — the Step 8 rule, same as detection).
//   2. Push-out along the min-penetration axis, split EQUALLY: the
//      engine is massless, so no weighting exists. Consequence the
//      caller must respect: BOTH bodies move. Never resolve a pair
//      where one side must stay put (e.g. scenery) until a static/
//      mass concept exists — that is platformer-proof work, not this.
//   3. Impulse along the axis normal, equal masses, scaled by
//      restitution (0 = stick together, 1 = elastic). Applied only
//      when approaching (relative normal velocity < 0); separating
//      pairs keep the positional fix with no velocity change.
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

} // namespace pe

#endif // PUREENGINE_PHYSICS_H
