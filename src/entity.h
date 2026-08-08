/**
 * PureEngine — Step 7: Basic Object System
 * File: entity.h
 *
 * One game object, represented as plain DATA: a world position, a Z-axis
 * rotation (angle + speed), and a per-axis scale. Nothing more.
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

    // Default constructor: at the origin, unrotated, unscaled — an entity
    // that transforms nothing until configured. Every member initialized
    // in the initializer list: no garbage state possible, same standard
    // as the math types.
    Entity()
        : position(0.0f, 0.0f, 0.0f),
          rotationAngle(0.0f),
          rotationSpeed(0.0f),
          scale(1.0f, 1.0f, 1.0f) {}

    // Configured constructor: the three things that differ per instance.
    // rotationAngle always STARTS at 0 — instances begin unrotated and
    // accumulate angle from their own speed every frame.
    Entity(const Vec3& position, float rotationSpeed, const Vec3& scale)
        : position(position),
          rotationAngle(0.0f),
          rotationSpeed(rotationSpeed),
          scale(scale) {}

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
