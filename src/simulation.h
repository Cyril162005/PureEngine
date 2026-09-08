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
 *   - chasePlayer(): the hostile pursuit loop — every entity whose
 *     roleId matches the caller-supplied hostile id, direction =
 *     player - hostile normalized, scaled
 *     by THIS hostile's base speed (entity.moveSpeed) TIMES the
 *     frame's difficulty scale TIMES deltaTime, with the zero-length
 *     guard that keeps a hostile sitting exactly on the player still
 *     (the catch test ends the run that frame, not the chase).
 *   - scanSceneryCollisions(): the Steps 8-11 scenery system — every
 *     unique pair among the Player/Scenery-role entities tested once,
 *     both flags set on overlap, wherever they sit in the vector.
 *     Hostiles pass through scenery and their only interaction is the
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

// --- The hostile chase (Steps 12/Phase 1's loop, Step 54 role ids) ---
// Pure pursuers: each hostile recomputes its own pursuit vector
// every frame (no prediction, no flanking; difficulty comes from the
// numbers — each hostile's Entity::moveSpeed and the shared Phase 2 ramp).
// Role identities arrive as caller-supplied ints (game-defined values);
// the engine compares roleId only, never naming roles itself.
// Guard keeps intent honest per hostile: zero distance means no
// direction to move in.
inline void chasePlayer(std::vector<Entity>& entities,
                        float difficultyScale, float dt,
                        int playerRoleId, int hostileRoleId) {
    const pe::Entity* player = nullptr;
    for (const pe::Entity& entity : entities) {
        if (entity.roleId == playerRoleId) {
            player = &entity;
            break;
        }
    }
    if (!player) {
        return;
    }

    for (pe::Entity& entity : entities) {
        if (entity.roleId == hostileRoleId) {
            const pe::Vec3 toPlayer = player->position - entity.position;
            if (toPlayer.length() > 0.0f) {
                entity.position = entity.position + toPlayer.normalized() * (entity.moveSpeed * difficultyScale) * dt;
            }
        }
    }
}

// --- The scenery collision scan (Steps 8-11's loop, role-based pairing) ---
// Every UNIQUE pair among the scenery pool (entities whose role is
// Player or Scenery, wherever they sit in the vector), tested exactly
// once — N*(N-1)/2 tests over the collected pool indices. Overlap is
// symmetric, so BOTH flags are set. The caller owns the flag vector
// and must have rebuilt it from zero first (main.cpp does: collision
// state is derived fresh every frame, never remembered). Hostiles
// (role Hostile) remain excluded wherever they sit: no
// index-position assumption.
inline void scanSceneryCollisions(const std::vector<Entity>& entities,
                                  std::vector<char>& colliding,
                                  int playerRoleId, int sceneryRoleId) {
    std::vector<size_t> pool;
    for (size_t i = 0; i < entities.size(); ++i) {
        if (entities[i].roleId == playerRoleId || entities[i].roleId == sceneryRoleId) {
            pool.push_back(i);
        }
    }
    for (size_t a = 0; a < pool.size(); ++a) {
        for (size_t b = a + 1; b < pool.size(); ++b) {
            if (pe::aabbOverlap(entities[pool[a]], entities[pool[b]])) {
                colliding[pool[a]] = 1;
                colliding[pool[b]] = 1;
            }
        }
    }
}

} // namespace pe

#endif // PUREENGINE_SIMULATION_H
