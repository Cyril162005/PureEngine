/**
 * PureEngine — Step 22: World/Game Separation (simulation mechanics)
 * File: simulation.h
 *
 * The engine's ninth SYSTEM boundary — pure MECHANICS only. Step 22
 * extracts exactly three operations from main.cpp's PLAYING branch,
 * each relocated byte-equivalent from its Steps 7/12/8 form:
 *
 *   - advanceRotations(): the per-entity rotation update — one loop,
 *     each entity advancing by its own speed via Entity::update(dt).
 *   - chasePlayer(): the hostile pursuit loop — indices 3 to the end
 *     of the vector, direction = player - hostile normalized, scaled
 *     by THIS hostile's base speed (hostileSpeeds[h - 3]) TIMES the
 *     frame's difficulty scale TIMES deltaTime, with the zero-length
 *     guard that keeps a hostile sitting exactly on the player still
 *     (the catch test ends the run that frame, not the chase).
 *   - scanSceneryCollisions(): the Steps 8-11 scenery system — every
 *     unique pair among the ORIGINAL THREE entities tested once, both
 *     flags set on overlap. The bound is the literal 3 on purpose:
 *     hostiles pass through scenery and their only interaction is the
 *     catch test, exactly as every step since Step 12 preserved.
 *
 * What this boundary is NOT — and the ruling that says so (B6):
 *   - no scheduler, no system registry, no execution pipeline. There
 *     is no object here, no run()/step()/tick() — three free
 *     functions in namespace pe, same pattern as lifecycle.h.
 *   - no ownership of entities or flags. main.cpp owns the vectors;
 *     these helpers mutate positions and flags handed to them.
 *
 * THE FRAME-ORDER CONTRACT (and it stays main.cpp's alone):
 *
 *   timer -> rotation -> difficulty -> chase -> collision rebuild ->
 *   collision scan -> edge detection -> catch
 *
 * main.cpp runs that sequence in that order every PLAYING frame and
 * keeps everything that carries MEANING: the timer accumulation, the
 * difficulty calculation, the colliding-vector rebuild from zero, the
 * edge detection (wasColliding comparison), the audio trigger, the
 * catch decision and every side effect of being caught, the state
 * transition, and the high-score write. The helpers above know none
 * of those things exist — they take data, move it, return nothing.
 *
 * Header-only, same discipline as every project module: no
 * simulation.cpp, no CMakeLists.txt change.
 */
#ifndef PUREENGINE_SIMULATION_H
#define PUREENGINE_SIMULATION_H
// Include guard, same pattern as every other project header.

#include <cstddef>     // std::size_t — loop counters
#include <array>       // std::array — fixed hostile speed data
#include <vector>      // the entity/flag/speed containers (caller-owned)

#include "entity.h"    // the pure data type the mechanics advance
#include "collision.h" // pe::aabbOverlap — the scenery scan's one test

namespace pe {

// --- The rotation update (Step 7's loop, relocated whole) ---
// Each entity carries its OWN speed (and direction), so each
// advances at its own rate — the state += rate * deltaTime pattern
// from Steps 2/5, applied per entity. This replaces the old global
// rotationAngle update; it has never been anything but this loop.
inline void advanceRotations(std::vector<Entity>& entities, float dt) {
    for (pe::Entity& entity : entities) {
        entity.update(dt);
    }
}

// --- The hostile chase (Steps 12/Phase 1's loop, relocated whole) ---
// Pure pursuers: each hostile recomputes its own pursuit vector
// every frame (no prediction, no flanking; difficulty comes from the
// numbers — the base speeds and the shared Phase 2 ramp, both handed
// in as data). Indices 3..end are the hostile range; speeds is the
// parallel hostileSpeeds array addressed as speeds[h - 3], exactly as
// main.cpp declares it. The fixed-size reference preserves the
// three-hostile contract without exposing a raw pointer.
// guard keeps intent honest per hostile: zero distance means no
// direction to move in.
inline void chasePlayer(std::vector<Entity>& entities,
                        const std::array<float, 3>& speeds,
                        float difficultyScale, float dt) {
    for (size_t h = 3; h < entities.size(); ++h) {
        pe::Entity& hostile = entities[h];
        const pe::Vec3 toPlayer = entities[0].position - hostile.position;
        if (toPlayer.length() > 0.0f) {
            hostile.position = hostile.position + toPlayer.normalized() * (speeds[h - 3] * difficultyScale) * dt;
        }
    }
}

// --- The scenery collision scan (Steps 8-11's loop, relocated whole) ---
// Every UNIQUE pair among the original three, tested exactly once
// (i runs each entity, j only the ones AFTER it — N*(N-1)/2 tests,
// 3 for N = 3). Overlap is symmetric, so BOTH flags are set. The
// caller owns the flag vector and must have rebuilt it from zero
// first (main.cpp does: collision state is derived fresh every
// frame, never remembered). The bound is the literal 3 — the
// hostiles are deliberately excluded; this loop IS the scenery
// system and nothing else.
inline void scanSceneryCollisions(const std::vector<Entity>& entities,
                                  std::vector<char>& colliding) {
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = i + 1; j < 3; ++j) {
            if (pe::aabbOverlap(entities[i], entities[j])) {
                colliding[i] = 1;
                colliding[j] = 1;
            }
        }
    }
}

} // namespace pe

#endif // PUREENGINE_SIMULATION_H
