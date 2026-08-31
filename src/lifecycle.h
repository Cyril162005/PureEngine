/**
 * PureEngine — Step 18: Entity Lifecycle Boundary
 * File: lifecycle.h
 *
 * The engine's sixth SYSTEM boundary. Before Step 18 the entity
 * lifecycle lived inline in main.cpp: six push_backs assembled the
 * world, one assignment restored it on reset, and "removal" existed
 * only IMPLICITLY — the snapshot restore discards accumulated play
 * state. Step 18 makes those responsibilities EXPLICIT by relocating
 * the OPERATIONS into this header — without replacing the
 * vector-based entity representation, and without inventing the
 * lifecycle machinery this game has never had.
 *
 * What this boundary is (and nothing more):
 *   - buildInitialEntities(): the six entity constructions, in their
 *     EXACT pre-Step-18 order and with their EXACT values — relocated
 *     whole from main.cpp, comments included. Index conventions are
 *     load-bearing across the whole project (0 = player, 1-2 =
 *     scenery with the collision bound of 3, 3+ = hostiles chased at
 *     hostileSpeeds[h - 3], textures selected by index), so creation
 *     ORDER is part of the contract, not an accident.
 *   - resetEntities(entities, snapshot): the snapshot-restoration
 *     ASSIGNMENT — the ONLY place the explicit "removal" of the old
 *     run's state happens. The engine never erases or adds entities
 *     at runtime; restoration IS its removal semantics.
 *   - flagsForCount(n): the per-entity collision-flag sizing
 *     expression, so creation and flag sizing live under one named
 *     responsibility.
 *
 * What it deliberately is NOT (the blueprint's explicit constraint):
 *   - no ECS, no registry, no component scheduler, no object pool,
 *     no generational handles, no spawning system, no destruction
 *     machinery. The game has never added or removed an entity after
 *     startup, and this boundary does not pretend otherwise.
 *
 * Ownership rule (unchanged): main.cpp OWNS the collections —
 * entities, initialEntities, colliding — and keeps resetGame() as ONE
 * atomic block that calls resetEntities() alongside the camera,
 * clear-color, collision-history, and timer resets. The boundary
 * performs operations; it owns no state and makes no decisions about
 * when a reset happens or what an entity MEANS (player, scenery, or
 * hostile — that stays game meaning in main.cpp).
 *
 * Header-only, same discipline as src/math/, entity.h, collision.h,
 * gamestate.h, resources.h, camera.h, input.h, and time.h: no
 * lifecycle.cpp, no CMakeLists.txt change. Free functions in
 * namespace pe, the resources.h precedent — a stateless class would
 * add nothing.
 */
#ifndef PUREENGINE_LIFECYCLE_H
#define PUREENGINE_LIFECYCLE_H
// Include guard, same pattern as every other project header.

#include <cstddef>   // std::size_t — the flag-sizing parameter
#include <vector>    // the entity container type (owned by the CALLER)

#include "entity.h"       // the pure data type — untouched by Step 18
#include "hostile_data.h"  // the one-current-archetype defaults used at startup

namespace pe {

// --- Build the initial world: the six entity constructions,
// relocated whole from main.cpp (Step 7 / Step 12 / Phase 1 /
// balance tuning). Order is CONTRACT: 0 = player, 1-2 = scenery,
// 3+ = hostiles — chased at hostileSpeeds[h - 3], caught by the
// h >= 3 loop, textured by index, collision-bounded by the literal 3.
// The current hostile archetype values can now be loaded from a tiny
// text file at startup; the default profile is the original values as a
// fallback when the file is absent or malformed.
inline std::vector<Entity> buildInitialEntities(const HostileDefaults& hostile = HostileDefaults()) {
    std::vector<Entity> entities;
    // Instances 1 & 2 — the Step 6 pair, exactly preserved: same world
    // positions, same 0.9 rad/s counter-clockwise spin (~7 s/revolution),
    // same unit scale.
    entities.push_back(Entity(pe::Vec3(-1.5f, 0.0f, 0.0f), 0.9f,
                              pe::Vec3(1.0f, 1.0f, 1.0f)));
    entities.push_back(Entity(pe::Vec3( 1.5f, 0.0f, 0.0f), 0.9f,
                              pe::Vec3(1.0f, 1.0f, 1.0f)));
    // Instance 3 — the proof that the loop scales without code
    // duplication: new position, new speed AND DIRECTION (-1.4 rad/s =
    // clockwise, ~4.5 s/revolution), new scale (60% size). None of this
    // required a single new rendering line — behavior comes from DATA.
    entities.push_back(Entity(pe::Vec3(0.0f, 1.5f, 0.0f), -1.4f,
                              pe::Vec3(0.6f, 0.6f, 1.0f)));
    // The current hostile archetype is still one small fixed set of
    // values, but it now comes from the startup loader default/profile.
    entities.push_back(Entity(hostile.spawnPositions[0], hostile.rotationSpeeds[0],
                              pe::Vec3(1.0f, 1.0f, 1.0f),
                              pe::Vec3(0.5f, 0.5f, 0.0f)));
    entities.push_back(Entity(hostile.spawnPositions[1], hostile.rotationSpeeds[1],
                              pe::Vec3(1.0f, 1.0f, 1.0f),
                              pe::Vec3(0.5f, 0.5f, 0.0f)));
    entities.push_back(Entity(hostile.spawnPositions[2], hostile.rotationSpeeds[2],
                              pe::Vec3(1.0f, 1.0f, 1.0f),
                              pe::Vec3(0.5f, 0.5f, 0.0f)));
    return entities;
}

// --- Restore a run's entities to the initial snapshot ---
// The explicit form of what reset always WAS: one assignment that
// replaces the entire accumulated play state (positions, rotations)
// with the start configuration. This is the boundary's ONLY
// "removal" — nothing is ever erased element-by-element, and
// nothing is ever spawned at runtime. The CALLER keeps the
// collection and decides WHEN restoration happens (resetGame() in
// main.cpp stays one atomic block).
inline void resetEntities(std::vector<Entity>& entities,
                          const std::vector<Entity>& initialSnapshot) {
    entities = initialSnapshot;
}

// --- Size a per-entity collision-flag buffer ---
// The sizing expression the collision system has used since Step 10:
// one char per entity, zero-initialized. Kept here so creation and
// flag sizing share one named responsibility; the CALLER owns the
// buffer.
inline std::vector<char> flagsForCount(std::size_t count) {
    return std::vector<char>(count, 0);
}

} // namespace pe

#endif // PUREENGINE_LIFECYCLE_H
