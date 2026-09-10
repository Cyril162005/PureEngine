/**
 * PureEngine — Step 60: Physics Foundation (data + free functions)
 * File: physics.h
 *
 * Minimal 2D physics vocabulary: a world gravity constant and three
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

} // namespace pe

#endif // PUREENGINE_PHYSICS_H
