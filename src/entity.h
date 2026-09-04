/**
 * PureEngine — Step 7: Basic Object System
 * File: entity.h
 *
 * One game object, represented as plain DATA: a world position, a Z-axis
 * rotation (angle + speed), a per-axis scale, and (since Step 8) AABB
 * half-extents for collision. Nothing more.
 *
 * Why a plain struct and not a full ECS? At three instances and exactly
 * one behavior (spin, then draw), a full Entity-Component-System — entity
 * ID registry, component pools, a system scheduler — would be pure
 * indirection with no payoff. What this file keeps from the ECS idea is
 * the part that actually matters: entities are DATA in a contiguous
 * container, and generic loops process all of them. If the engine ever
 * grows heterogeneous components (some entities with sprites, some with
 * physics, some with neither), that is the day to introduce the full ECS
 * vocabulary — and this struct migrates cleanly into it as the transform
 * component.
 *
 * Header-only, same as src/math/: every function is defined here, no
 * entity.cpp exists, and CMakeLists.txt needs no change.
 */
#ifndef PUREENGINE_ENTITY_H
#define PUREENGINE_ENTITY_H
// Include guard, same pattern as the math headers: safe against
// double-inclusion in one translation unit.

#include "math/vec3.h"  // position and scale are Vec3s
#include "math/mat4.h"  // modelMatrix() returns a Mat4

namespace pe {

struct Entity {
    Vec3 position;        // world position of the entity's origin
    float rotationAngle;  // current Z rotation in RADIANS, accumulates
    float rotationSpeed;  // radians per second; negative = clockwise
    Vec3 scale;           // per-axis size multiplier, (1,1,1) = no change
    // --- Step 8: collision bounds (AABB half-extents) ---
    // Distance from the entity's origin to each bounding-box edge,
    // BEFORE scale is applied (collision code multiplies by scale, so
    // a 0.6-scale entity gets a 0.6-size box — collider matches what
    // renders). For the shared triangle geometry this is the distance
    // from the origin to its FARTHEST vertex: the corners at
    // (+/-0.5, -0.5) sit sqrt(0.5^2 + 0.5^2) = 0.7071... away. The
    // triangle SPINS, and a bounding box tighter than its farthest
    // vertex would be wrong at some angles — the 0.7071 square is the
    // tightest box that stays correct at EVERY rotation angle. Z is 0:
    // the scene is flat, collision is a 2D test.
    Vec3 halfExtents;
    int textureId = 0;
    // --- Step 45: explicit draw-layer field ---
    // Canonical draw order for the current entity types:
    //   0 = player (background — draws first)
    //   1 = scenery
    //   2 = hostile (foreground — draws last, on top)
    // The renderer's draw loop iterates the entity vector, which
    // buildInitialEntities() assembles in depth order (player first,
    // then scenery, then hostiles). As long as that construction order
    // holds, the vector is already depth-ordered and no runtime sort
    // is needed. This field makes the INTENT explicit in the data so
    // a future step that reorders the vector (e.g. dynamic spawn,
    // entity removal) knows to sort by depth before drawing.
    int depth = 0;

    // Default constructor: at the origin, unrotated, unscaled — an entity
    // that transforms nothing until configured. Every member initialized
    // in the initializer list: no garbage state possible, same standard
    // as the math types. Half-extents default to the shared triangle's
    // rotation-safe bound (see the member's comment).
    Entity()
        : position(0.0f, 0.0f, 0.0f),
          rotationAngle(0.0f),
          rotationSpeed(0.0f),
          scale(1.0f, 1.0f, 1.0f),
          halfExtents(0.7071f, 0.7071f, 0.0f),
          textureId(0),
          depth(0) {}

    // Configured constructor: the things that differ per instance.
    // rotationAngle always STARTS at 0 — instances begin unrotated and
    // accumulate angle from their own speed every frame.
    // halfExtents has a DEFAULT ARGUMENT: every entity so far shares
    // the same triangle geometry, so callers omit it; the day a second
    // mesh arrives, callers pass its real bounds — no existing call
    // site breaks. depth defaults to 0 — callers (lifecycle.h) set the
    // correct layer explicitly after construction.
    Entity(const Vec3& position, float rotationSpeed, const Vec3& scale,
           const Vec3& halfExtents = Vec3(0.7071f, 0.7071f, 0.0f),
           int textureId = 0)
        : position(position),
          rotationAngle(0.0f),
          rotationSpeed(rotationSpeed),
          scale(scale),
          halfExtents(halfExtents),
          textureId(textureId),
          depth(0) {}

    // Per-frame simulation: advance this entity's angle. This is the
    // universal state += rate * deltaTime pattern — the same one the
    // camera and the old global rotation used — now OWNED BY THE DATA,
    // so each entity advances at its own rate.
    void update(float deltaTime) {
        rotationAngle += rotationSpeed * deltaTime;
    }

    // Build this entity's MODEL matrix: local coordinates -> world
    // coordinates. The multiplication order acts on a vertex RIGHT-TO-
    // LEFT (Steps 5/6 rule):
    //   scale      first — resize around the entity's own origin,
    //   rotationZ  then  — spin around its own (now scaled) center,
    //   translation last — carry the result to its world position.
    // This order means "spin in place, where you stand". Swap the
    // translation to the right and the entity would ORBIT the world
    // origin instead. Order is meaning — same lesson, now inside data.
    // Not constexpr: rotationZ calls std::sin/std::cos, which MSVC
    // cannot evaluate at compile time (documented in mat4.h).
    Mat4 modelMatrix() const {
        return Mat4::translation(position)
             * Mat4::rotationZ(rotationAngle)
             * Mat4::scale(scale);
    }
};

} // namespace pe

#endif // PUREENGINE_ENTITY_H
