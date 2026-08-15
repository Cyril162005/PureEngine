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

#include "entity.h"  // the pure data type — untouched by Step 18

namespace pe {

// --- Build the initial world: the six entity constructions,
// relocated whole from main.cpp (Step 7 / Step 12 / Phase 1 /
// balance tuning). Order is CONTRACT: 0 = player, 1-2 = scenery,
// 3+ = hostiles — chased at hostileSpeeds[h - 3], caught by the
// h >= 3 loop, textured by index, collision-bounded by the literal 3.
inline std::vector<Entity> buildInitialEntities() {
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
    // Instance 4 — Step 12: the HOSTILE. Same pe::Entity type, same
    // vector, added purely as DATA (Step 7's ruling): the existing
    // update loop spins it (1.8 rad/s CCW), the existing draw loop
    // renders it. Balance tuning: its hitbox is NO LONGER the Step 8
    // default — see the explicit halfExtents argument below.
    // Starts at (0, -2) — bottom-center, away from the player's start
    // at (-1.5, 0) — unit scale like entities 1 & 2. It ignores the
    // spinning triangles (scenery is not solid in v1) and hunts only
    // the player — that chase and its catch test live in the
    // simulation section below, deliberately OUTSIDE the pair loop so
    // the Step 8 scenery-collision system stays byte-identical in
    // behavior.
    entities.push_back(Entity(pe::Vec3(0.0f, -2.0f, 0.0f), 1.8f,
                              pe::Vec3(1.0f, 1.0f, 1.0f),
                              // BALANCE TUNING (disclosed deviation from Step 8):
                              // 0.5 is the triangle's base half-width — the kill box
                              // matches the hostile's dominant footprint instead of the
                              // 0.7071 rotation-safe bound. Step 8's bound exists for
                              // FAIRNESS on rotating scenery (no blind spot at any spin
                              // angle); a chasing hostile does not need that guarantee,
                              // and its overreach reads as an invisible fat collider.
                              // The tighter box makes visual contact and actual death
                              // agree. Player and scenery keep 0.7071.
                              pe::Vec3(0.5f, 0.5f, 0.0f)));
    // Instances 5 & 6 — Game Build Phase 1: TWO MORE hostiles, added
    // the only way this project adds behavior — as DATA (two push_backs,
    // Step 7's ruling). Each carries its OWN personality through the
    // exact same fields Step 7 gave every triangle: position = spawn
    // point, rotationSpeed = visual spin, scale = size (and collider).
    // Spawn points split the compass around the player's (-1.5, 0)
    // start — bottom (existing hostile), top-right, top-left — so the
    // opening seconds are a genuine three-direction read, not an
    // instant surround. Spins differ (one clockwise, like entity 3;
    // one faster CCW) so the three threats are visually distinct.
    // Both sit BEFORE the initialEntities snapshot below, which is the
    // entire reason resetGame() needs NO changes: the snapshot simply
    // contains all six entities at their start positions.
    entities.push_back(Entity(pe::Vec3(3.0f, 2.0f, 0.0f), -1.2f,
                              pe::Vec3(1.0f, 1.0f, 1.0f),
                              pe::Vec3(0.5f, 0.5f, 0.0f)));   // tuning: tighter hitbox — see instance 4
    entities.push_back(Entity(pe::Vec3(-3.0f, 2.0f, 0.0f), 2.2f,
                              pe::Vec3(1.0f, 1.0f, 1.0f),
                              pe::Vec3(0.5f, 0.5f, 0.0f)));   // tuning: tighter hitbox — see instance 4
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
