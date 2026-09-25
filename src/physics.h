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
#include "collision.h" // sweptAABB — the swept move's one detection test (no cycle)

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
    // Step 206: per-body restitution. EFFECTIVE VALUE: an explicit
    // parameter (any value != the 0.5 default) overrides; the default
    // call uses max(a.restitution, b.restitution) - Box2D-style (the
    // bouncier body wins). With both bodies unset (0.5) the default
    // path is byte-identical to the pre-206 behavior. Clamped to
    // [0,1] below like the parameter always was.
    if (restitution == 0.5f) {
        restitution = a.restitution > b.restitution ? a.restitution : b.restitution;
    }
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

    // Static/kinematic body handling: neither ever moves from collisions
    // (Step P6: isKinematic responds like isStatic — infinite mass, the
    // body never gets pushed and never receives impulse; its own motion
    // comes from the integration paths, not collision response).
    const bool aStatic = a.isStatic || a.isKinematic;
    const bool bStatic = b.isStatic || b.isKinematic;

    // Step 205 inverse masses, HOISTED here (Step 207): the friction
    // impulse needs the SAME values in BOTH branches — no second,
    // separate split. invMass = static||kinematic||mass<=0 ? 0 : 1/mass.
    const float invMassA = (aStatic || a.mass <= 0.0f) ? 0.0f : 1.0f / a.mass;
    const float invMassB = (bStatic || b.mass <= 0.0f) ? 0.0f : 1.0f / b.mass;
    const float invSum = invMassA + invMassB;

    // Step 207: TANGENTIAL FRICTION IMPULSE (Coulomb clamp, stated
    // exactly). COMBINED FRICTION = sqrt(fA * fB) - the GEOMETRIC MEAN
    // (both bodies contribute; a frictionless body (0) kills the
    // pair's friction). The normals are always AXIS-ALIGNED (AABB
    // resolve), so the tangent is the PERPENDICULAR axis - no general
    // 2D direction derivation. COULOMB CLAMP: |j_t| <= friction * j_n,
    // where j_n is the normal impulse magnitude from THIS contact.
    //   j_t = -relVelTangent / invSum, clamped to +-friction * j_n
    // Applied weighted by the SAME invMass values as the normal
    // impulse (no second split). jn <= 0 (no normal impulse) means no
    // friction - friction requires a normal force.
    auto applyFriction = [&](float jn) {
        const float friction = std::sqrt(a.friction * b.friction);
        if (friction <= 0.0f || jn <= 0.0f) {
            return;
        }
        float tvx = 0.0f, tvy = 0.0f;
        if (nx != 0.0f) { tvy = 1.0f; } else { tvx = 1.0f; }
        const float relTangent = (b.velocity.x - a.velocity.x) * tvx +
                                 (b.velocity.y - a.velocity.y) * tvy;
        if (relTangent == 0.0f) {
            return;
        }
        float jt = -relTangent / invSum;
        const float maxJt = friction * jn;
        if (jt > maxJt) {
            jt = maxJt;
        }
        if (jt < -maxJt) {
            jt = -maxJt;
        }
        a.velocity.x -= tvx * (jt * invMassA);
        a.velocity.y -= tvy * (jt * invMassA);
        b.velocity.x += tvx * (jt * invMassB);
        b.velocity.y += tvy * (jt * invMassB);
    };

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
                applyFriction(impulse);
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
                applyFriction(impulse);
            }
        }
        return;
    }

    // Neither static: INVERSE-MASS WEIGHTING (Step 205; the standard
    // impulse resolution). FORMULA (exact, stated):
    //   invMass = (isStatic || isKinematic || mass <= 0) ? 0 : 1/mass
    //   positional: each body moves along -n/+n by
    //               pen * invMass / (invMassA + invMassB)
    //   impulse:    j = -(1 + restitution) * relVelAlongNormal
    //                     / (invMassA + invMassB)
    //               a.velocity -= n * (j * invMassA)
    //               b.velocity += n * (j * invMassB)
    // Heavier bodies move less (a 1:2 pair splits the correction 1/3
    // vs 2/3 the OTHER way). With both masses 1.0 (the default) this
    // reduces EXACTLY to the original equal split (0.5/0.5) — the
    // behavior is byte-identical when mass is unset (non-regression).
    // (Step 207: invMassA/invMassB/invSum are computed ONCE above —
    // the same values feed the normal impulse and the friction impulse.)
    a.position.x -= nx * pen * (invMassA / invSum);
    a.position.y -= ny * pen * (invMassA / invSum);
    b.position.x += nx * pen * (invMassB / invSum);
    b.position.y += ny * pen * (invMassB / invSum);

    const float relNx = (b.velocity.x - a.velocity.x) * nx;
    const float relNy = (b.velocity.y - a.velocity.y) * ny;
    if (relNx + relNy >= 0.0f) {
        return;
    }
    const float impulse = -(1.0f + restitution) * (relNx + relNy) / invSum;
    a.velocity.x -= nx * (impulse * invMassA);
    a.velocity.y -= ny * (impulse * invMassA);
    b.velocity.x += nx * (impulse * invMassB);
    b.velocity.y += ny * (impulse * invMassB);
    applyFriction(impulse);
}

// --- Step P7: swept move + collide (the discrete resolve's swept twin) ---
// Moves the mover by velocity*dt along its center path, sweeping against
// every static/kinematic body's expanded AABB (sweptAABB, collision.h).
// Where the discrete endpoint test tunnels, the sweep clamps the move at
// the EARLIEST hit: position = from + delta*tBest, and the velocity
// component along the contact normal zeroes — the swept equivalent of
// the character controller's discrete axis-zeroing (walls stop slides,
// floors stop falls).
//
// Contract:
//   - No contact: the mover takes the FULL delta, returns false.
//   - Contact: clamps at the earliest hit (min t over all swept bodies),
//     zeroes only the normal-axis velocity, returns true. The mover's
//     edge lands exactly on the contacted face (the center path clamps
//     at the Minkowski-expanded box).
//   - Start-inside (sweptAABB's overlap report, t=0, zero normal):
//     contact with NO movement and NO velocity change — overlap
//     resolution stays the discrete path's job (updateCharacterController
//     / resolveCollision), exactly as sweptAABB's contract assigns.
//   - Statics/kinematics are never written; dead bodies collide with
//     nothing. dt <= 0 is a no-op returning false.
// API-only (no main/Platformer call in this slice). Free function, same
// discipline as the rest of physics.h. Defined BEFORE the character
// controller so the controller's swept opt-in can call it.
inline bool sweptMoveAndCollide(Entity& mover,
                                const std::vector<Entity>& staticEntities,
                                float dt) {
    if (dt <= 0.0f) {
        return false;
    }
    const Vec3 from = mover.position;
    const Vec3 delta = mover.velocity * dt;
    const Vec3 to = from + delta;
    const Vec3 half(mover.halfExtents.x * mover.scale.x,
                    mover.halfExtents.y * mover.scale.y,
                    0.0f);

    float bestT = 1.0f;
    Vec3 bestNormal(0.0f, 0.0f, 0.0f);
    bool contacted = false;
    for (const Entity& e : staticEntities) {
        if (!e.alive) continue;                       // dead bodies collide with nothing
        if (!e.isStatic && !e.isKinematic) continue;  // movers test statics/kinematics only
        const SweepHit h = sweptAABB(
            from, to, half, e.position,
            Vec3(e.halfExtents.x * e.scale.x,
                 e.halfExtents.y * e.scale.y,
                 0.0f));
        if (h.hit && h.t < bestT) {
            bestT = h.t;
            bestNormal = h.normal;
            contacted = true;
        }
    }

    if (!contacted) {
        mover.position = to;
        return false;
    }
    mover.position = from + delta * bestT;
    if (bestNormal.x != 0.0f) {
        mover.velocity.x = 0.0f;
    }
    if (bestNormal.y != 0.0f) {
        mover.velocity.y = 0.0f;
    }
    return true;
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
    for (const Entity& e : entities) {
        if (!e.isStatic && !e.isKinematic) continue;
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
//
// Step P8 opt-in: useSwept=true swaps the integrate step for the swept
// move (sweptMoveAndCollide) — the center path sweeps against every
// static/kinematic body, so a mover whose per-substep displacement
// exceeds the target's thickness clamps at the earliest crossing
// instead of tunneling past the endpoint test. The discrete resolve
// pass below still runs after the sweep: it no-ops when the sweep left
// exact contact, and resolves any residual overlap (start-inside
// reports, spawn penetration) with the usual min-axis push-out + 
// velocity zero. Default false preserves the discrete path byte-identical.
//
// WHEN TO ENABLE (Step P8 ruling): default OFF is correct for normal
// platformer speeds — maxFallSpeed (25) + 1/60 substeps bound the
// per-substep displacement to ~0.42, well inside a 1.0 tile's
// thickness, and the discrete path is proven there. Enable useSwept
// for movers whose per-substep displacement can exceed a target's
// thickness (fast projectiles, fast platforms) or when the 8-substep
// clamp is insufficient. Composes with the Fixed wrapper: substeps
// split dt, the sweep catches whatever a substep would still tunnel.
inline bool updateCharacterController(Entity& character,
                                      const std::vector<Entity>& staticEntities,
                                      float dt,
                                      bool jumpPressed,
                                      bool useSwept = false) {
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
    if (useSwept) {
        // Swept move: clamps tunnel crossings at the earliest hit and
        // zeroes the normal-axis velocity (including a boundary-start
        // rest contact — sweptAABB reports the face normal there, so a
        // resting character keeps velocity.y at 0 instead of
        // accumulating gravity while frozen at exact contact).
        sweptMoveAndCollide(character, staticEntities, dt);
    } else {
        integrate(character.position, character.velocity, dt);
    }

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
        if (!wall.isStatic && !wall.isKinematic) continue;
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

// --- Step 87: optional fixed-dt substeps (header-only, non-breaking) ---
// Splits a large dt into fixedDt chunks (default 1/60) so a fast mover cannot
// tunnel a 1-unit tile in one variable step. Existing callers keep variable-dt;
// platformer or future movers opt in via the *Fixed wrappers. No new systems,
// no broadphase, no change to resolve math — just substeps. Step P8: the
// useSwept flag forwards to every substep (swept move path, off by default) —
// substeps + sweep together are the fast-mover recipe.
inline bool updateCharacterControllerFixed(Entity& character,
                                            const std::vector<Entity>& staticEntities,
                                            float dt, bool jumpPressed,
                                            float fixedDt = 1.0f / 60.0f,
                                            bool useSwept = false) {
    if (dt <= 0.0f) return checkGrounded(character, staticEntities);
    if (fixedDt <= 0.0f) fixedDt = 1.0f / 60.0f;
    int steps = static_cast<int>(std::ceil(dt / fixedDt));
    if (steps < 1) steps = 1;
    if (steps > 8) steps = 8; // clamp — avoids spiral on huge dt
    float sub = dt / static_cast<float>(steps);
    bool grounded = false;
    for (int i = 0; i < steps; ++i) {
        bool jp = (i == 0) ? jumpPressed : false; // consume jump once
        grounded = updateCharacterController(character, staticEntities, sub, jp, useSwept);
    }
    return grounded;
}

inline void applyPhysicsFixed(std::vector<Entity>& entities, float dt,
                              float fixedDt = 1.0f / 60.0f) {
    if (dt <= 0.0f) return;
    if (fixedDt <= 0.0f) fixedDt = 1.0f / 60.0f;
    int steps = static_cast<int>(std::ceil(dt / fixedDt));
    if (steps < 1) steps = 1;
    if (steps > 8) steps = 8;
    float sub = dt / static_cast<float>(steps);
    for (int i = 0; i < steps; ++i) {
        for (Entity& e : entities) {
            if (e.isStatic) continue;  // Step P4a: static bodies never move (matches applyPhysics)
            if (e.gravityScale > 0.0f) applyGravity(e.velocity, e.gravityScale, sub);
            if (e.velocity.x != 0.0f || e.velocity.y != 0.0f || e.velocity.z != 0.0f) integrate(e.position, e.velocity, sub);
        }
        // Static resolve per substep would need statics list; caller can
        // loop updateCharacterControllerFixed for character cases. This
        // helper covers free-physics movers (no statics); the isStatic
        // guard makes the never-moves rule hold by construction, not
        // caller convention.
    }
}

} // namespace pe

#endif // PUREENGINE_PHYSICS_H
