/**
 * PureEngine — Step 8: Collision Detection (AABB)
 * File: collision.h
 *
 * An AABB — AXIS-ALIGNED BOUNDING BOX — is a rectangle (in our 2D scene)
 * that encloses an object, whose edges are locked parallel to the world
 * X and Y axes. It stores only a center point and HALF-EXTENTS: the
 * distance from the center to each edge. That is the entire shape.
 *
 * Why AABB and not the alternatives?
 *  - CIRCLE collision is even cheaper and rotation-proof, but it models
 *    round objects well and boxy ones poorly; AABB is the canonical
 *    first collision primitive in engines and pairs naturally with the
 *    axis-aligned math we already have. Circles come later as a second
 *    shape type if a game object wants one.
 *  - PRECISE per-triangle collision (edge-vs-edge tests) is exact but
 *    costs O(edges x edges) per pair and must re-test every frame for
 *    spinning geometry. It is the wrong tool at this stage and — real
 *    engines do this too — even exact engines run the cheap AABB test
 *    FIRST and only spend exact tests on pairs whose AABBs already
 *    overlap. AABB is not a placeholder; it is the permanent first gate.
 *
 * THE OVERLAP TEST itself — the separating axis principle:
 * Two axis-aligned boxes DO NOT overlap if, and only if, at least ONE
 * axis has a gap between them. Flip that with De Morgan and the
 * overlap condition is:
 *     |ax - bx| < ha.x + hb.x   AND   |ay - by| < ha.y + hb.y
 * AND across BOTH axes — never OR. OR is the classic subtle bug: it
 * reports a collision when two boxes merely share an X range while
 * sitting meters apart vertically. Strict '<' means exactly-touching
 * edges count as NOT colliding; switch to '<=' if the game ever wants
 * touch-as-collision.
 *
 * Why a separate file and not more code in entity.h?
 * entity.h is DATA — what an object IS. Collision is an ALGORITHM over
 * that data — what we DO with objects. Keeping them apart is the same
 * data/system split Step 7 justified, at file level. Header-only like
 * everything else so CMakeLists.txt stays untouched.
 *
 * constexpr: the test is pure arithmetic, so the compiler can prove it
 * with static_assert in main() before the program ever runs — same
 * culture as the math layer. We hand-roll the absolute value with a
 * ternary because std::fabs is not constexpr on MSVC.
 */
#ifndef PUREENGINE_COLLISION_H
#define PUREENGINE_COLLISION_H
// Include guard, same pattern as every other PureEngine header.

#include "math/vec3.h"  // centers and half-extents are Vec3s
#include "entity.h"     // the entity overload reads Entity members

namespace pe {

// ------------------------------------------------------------------
// Generic AABB overlap test: two boxes given as center + half-extents.
// Returns true when the boxes' INTERIORS overlap (strict inequality —
// edge-touching is not a collision here). Z is ignored entirely: our
// scene is flat, all three entities share z = 0, and testing an axis
// that can never differ would be dead code dressed up as generality.
// ------------------------------------------------------------------
constexpr bool aabbOverlap(const Vec3& centerA, const Vec3& halfA,
                           const Vec3& centerB, const Vec3& halfB) {
    // Manual |a - b| for each axis (constexpr-safe, no std::fabs).
    float dx = centerA.x > centerB.x ? centerA.x - centerB.x
                                     : centerB.x - centerA.x;
    float dy = centerA.y > centerB.y ? centerA.y - centerB.y
                                     : centerB.y - centerA.y;
    // Overlap iff NO separating gap exists on EITHER axis — AND, not OR.
    return dx < halfA.x + halfB.x && dy < halfA.y + halfB.y;
}

// ------------------------------------------------------------------
// Entity overload: the collision query the game loop actually calls.
// Reads each entity's world position as the box center, and scales its
// half-extents by its per-axis scale — a 0.6-scale entity occupies a
// 0.6-size box, exactly matching what is RENDERED. Forgetting the
// scale multiply is the classic "invisible fat collider" bug.
// ------------------------------------------------------------------
constexpr bool aabbOverlap(const Entity& a, const Entity& b) {
    return aabbOverlap(a.position,
                       Vec3(a.halfExtents.x * a.scale.x,
                            a.halfExtents.y * a.scale.y,
                            0.0f),
                       b.position,
                       Vec3(b.halfExtents.x * b.scale.x,
                            b.halfExtents.y * b.scale.y,
                            0.0f));
}

} // namespace pe

#endif // PUREENGINE_COLLISION_H
