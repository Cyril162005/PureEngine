/**
 * PureEngine — Step 70: Particle system foundation
 * File: particles.h
 *
 * Minimal old-school particles: a Particle struct (position, velocity,
 * life, size, color), a rate-based Emitter, an integrate-and-age
 * update, and a converter into Entities for the EXISTING drawWorld
 * path (the Step-63 tilemap pattern — zero renderer changes, zero new
 * GL). Engine-only: no game logic, no wiring (same foundation-then-
 * adopt pattern as Steps 63-69, so Arcade and Pong are unaffected by
 * construction — neither includes this header yet).
 *
 * Honest rendering limits, stated up front:
 *  - Particles draw as TEXTURED TRIANGLES (drawWorld's shared geometry),
 *    not quads or points. Dedicated quad/point rendering is a later step.
 *  - Step 86: Particle::color now rendered via Entity.tint (drawWorld's
 *    per-entity tint, white by default, red on colliding). Burst path
 *    unchanged (white), emitter colors now visible.
 *  - Particles do not collide: halfExtents carries the truthful quad
 *    bound (size/2) but nothing reads it. The game supplies its own
 *    all-zero colliding vector at the draw call (documented below).
 *
 * Randomness: std::rand (stdlib — no new dependency) picks emission
 * direction (uniform in the XY disc) plus speed/life in the emitter's
 * ranges. FIRST rng use in src/: noted here so the day the engine wants
 * a real rng (seedable, deterministic) the call sites are these two
 * lines in emit(). Tests assert RANGES and COUNTS, never exact random
 * values, so no seeding is needed.
 *
 * Removal is swap-with-back (order NOT preserved — documented, classic
 * particle-pool behavior). dt <= 0 is a no-op everywhere (negative dt
 * must never grow life or spawn). mins are assumed <= maxes (garbage
 * in, garbage out — no enforcement, documented). A non-positive
 * maxParticles means "never spawn".
 *
 * Header-only, same discipline as every project module: no
 * particles.cpp, no CMakeLists.txt change.
 */
#ifndef PUREENGINE_PARTICLES_H
#define PUREENGINE_PARTICLES_H

#include <cmath>    // std::cos / std::sin for emission direction
#include <cstddef>  // std::size_t for the loops
#include <cstdlib>  // std::rand / RAND_MAX (see note above)
#include <vector>   // particle pool + entity output

#include "entity.h"  // particlesToEntities target type

namespace pe {

struct Particle {
    Vec3 position;   // current world position (no hierarchy in Step 70)
    Vec3 velocity;   // world units/second (initial-only: no forces)
    float life = 0.0f;     // seconds remaining; <= 0 = dead
    float maxLife = 1.0f;  // life value at spawn (future fade scaling)
    float size = 0.5f;     // rendered edge length (becomes Entity scale)
    Vec3 color =
        Vec3(1.0f, 1.0f, 1.0f);  // stored, NOT rendered (see header note)
};

struct Emitter {
    Vec3 position;
    float spawnRate = 10.0f;  // particles/second
    float speedMin = 1.0f, speedMax = 2.0f;
    float lifeMin = 0.5f, lifeMax = 1.0f;
    float size = 0.5f;
    Vec3 color = Vec3(1.0f, 1.0f, 1.0f);
    int maxParticles = 256;    // cap: emit skips while full (drop-newest)
    float accumulator = 0.0f;  // fractional spawn carry between emits
};

// Deterministic single spawn: stores everything as given (maxLife =
// life-at-spawn, even if odd). No cap enforced here — the cap belongs
// to emit(); the game owns pools it fills by hand.
inline void spawnParticle(std::vector<Particle>& out, const Vec3& pos,
                          const Vec3& vel, float life, float size,
                          const Vec3& color) {
    Particle p;
    p.position = pos;
    p.velocity = vel;
    p.life = life;
    p.maxLife = life;
    p.size = size;
    p.color = color;
    out.push_back(p);
}

// Rate-based emission: accumulator += rate*dt, each whole unit spawns
// one particle (uniform XY-disc direction, uniform speed/life ranges).
// Step 83: foundation-only — no caller yet (arcade uses spawnParticle
// burst directly; this continuous emitter remains for future games).
// dt <= 0 or non-positive rate: nothing. While out is at maxParticles
// the spawn is SKIPPED (drop-newest) and its budget unit is consumed —
// no backlog burst when room frees. See the header note for the rng.
inline void emit(Emitter& e, std::vector<Particle>& out, float dt) {
    if (dt <= 0.0f || e.spawnRate <= 0.0f) {
        return;
    }
    e.accumulator += e.spawnRate * dt;
    while (e.accumulator >= 1.0f) {
        e.accumulator -= 1.0f;
        if (static_cast<int>(out.size()) >= e.maxParticles) {
            continue;  // full: drop this spawn (see note above)
        }
        const float unit =
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        const float angle = unit * 2.0f * 3.14159265f;
        const float speedRange = e.speedMax - e.speedMin;
        const float lifeRange = e.lifeMax - e.lifeMin;
        const float speed = e.speedMin + speedRange * unit;
        // std::cos/std::sin: same MSVC non-constexpr regime as
        // Mat4::rotationZ (entity.h note) — runtime only, always was.
        const Vec3 vel(std::cos(angle) * speed, std::sin(angle) * speed,
                       0.0f);
        const float life = e.lifeMin + lifeRange * unit;
        spawnParticle(out, e.position, vel, life, e.size, e.color);
    }
}

// Advance the pool: integrate, age, swap-remove the dead. dt <= 0 is a
// no-op. The swapped-in particle is processed at the same index, so
// every live particle integrates exactly once per call; survivors may
// come out reordered (documented pool behavior).
inline void updateParticles(std::vector<Particle>& particles, float dt) {
    if (dt <= 0.0f) {
        return;
    }
    for (std::size_t i = 0; i < particles.size();) {
        Particle& p = particles[i];
        p.position = p.position + p.velocity * dt;
        p.life -= dt;
        if (p.life <= 0.0f) {
            p = particles.back();
            particles.pop_back();
        } else {
            ++i;
        }
    }
}

// Live particles (life > 0 ONLY — the dead are skipped) become Entities
// for the existing drawWorld(projection, view, entities, colliding):
// position through, scale = (size, size, 1), halfExtents = the truthful
// size/2 bound (inert — nothing collides particles), tint = Particle.color
// (Step 86), texture/depth/role passed through. Caller draws with its
// own all-zero colliding vector (same length as the return).
inline std::vector<Entity> particlesToEntities(
    const std::vector<Particle>& particles, int textureId, int depth,
    int roleId) {
    std::vector<Entity> entities;
    for (std::size_t i = 0; i < particles.size(); ++i) {
        const Particle& p = particles[i];
        if (p.life <= 0.0f) {
            continue;
        }
        Entity e;
        e.position = p.position;
        e.scale = Vec3(p.size, p.size, 1.0f);
        e.halfExtents = Vec3(p.size * 0.5f, p.size * 0.5f, 0.0f);
        e.textureId = textureId;
        e.depth = depth;
        e.roleId = roleId;
        e.tint = p.color;
        entities.push_back(e);
    }
    return entities;
}

}  // namespace pe

#endif  // PUREENGINE_PARTICLES_H
