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
#include "camera.h"     // Step 128: screen-space pick wrapper (acyclic — camera.h is math/state only)
#include <unordered_map> // Step 97: grid buckets
#include <utility>      // pair
#include <vector>       // candidate list

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

// ------------------------------------------------------------------
// Step 129: world-space AABB of one entity (pure).
// center = the entity's world position; halfExtents = its PRE-SCALE
// half-extents multiplied by its per-axis scale — the SAME scale rule
// aabbOverlap's entity overload and pickEntity's containment both
// apply, so this box matches what is RENDERED and what a pick hits.
// Z half-extent is FORCED to 0: the scene is flat (same rule as the
// overload above). constexpr: pure arithmetic, same culture as the
// math layer and aabbOverlap.
//
// Optional refactors were considered and DECLINED: pickEntity's inline
// two-line math is already minimal (a struct per scan adds nothing),
// and the proven constexpr overload above stays byte-identical.
// ------------------------------------------------------------------
struct WorldAABB {
    Vec3 center;
    Vec3 halfExtents;
};

constexpr WorldAABB entityWorldAABB(const Entity& e) {
    return WorldAABB{
        e.position,
        Vec3(e.halfExtents.x * e.scale.x,
             e.halfExtents.y * e.scale.y,
             0.0f)
    };
}

// Step 97: minimal grid helper — optional broadphase foundation.
// Buckets entities by position/cellSize, returns pairs sharing a cell.
// Does NOT replace existing O(n) scan; available for future use only.
inline std::vector<std::pair<std::size_t, std::size_t>> broadphaseGrid(
    const std::vector<Entity>& entities, float cellSize = 2.0f) {
    if (cellSize <= 0.0f) cellSize = 2.0f;
    std::unordered_map<long long, std::vector<std::size_t>> buckets;
    auto key = [&](int cx, int cy) -> long long {
        return (static_cast<long long>(cx) << 32) ^ (static_cast<unsigned int>(cy));
    };
    for (std::size_t i = 0; i < entities.size(); ++i) {
        int cx = static_cast<int>(std::floor(entities[i].position.x / cellSize));
        int cy = static_cast<int>(std::floor(entities[i].position.y / cellSize));
        buckets[key(cx, cy)].push_back(i);
    }
    std::vector<std::pair<std::size_t, std::size_t>> out;
    for (auto& kv : buckets) {
        auto& v = kv.second;
        for (std::size_t a = 0; a < v.size(); ++a)
            for (std::size_t b = a + 1; b < v.size(); ++b)
                out.emplace_back(v[a], v[b]);
    }
    return out;
}

// ------------------------------------------------------------------
// Step 127: point pick against entity AABBs (pure, O(n) scan).
// Returns the index of the entity whose scaled AABB contains the world
// point, or -1 when nothing is picked.
//
// Containment rule (the separating-axis principle against a zero-size
// box): |point - center| < scaledHalf on BOTH axes — strict '<', so a
// point exactly ON an edge is NOT a hit, the same touching-is-not-
// collision rule aabbOverlap applies. Half-extents are scaled by the
// entity's per-axis scale — the box matches what is RENDERED, the same
// no-invisible-fat-collider rule the entity overload above follows.
//
// Overlap rule when several AABBs contain the point: the HIGHEST depth
// wins — the foreground entity is what the player sees under the
// cursor, so it is what a pick must return. Ties break to the LOWEST
// index (first-constructed wins) — stable and deterministic.
//
// alive: entities flagged dead (Step 113) are skipped entirely — a
// logically-removed entity cannot be picked.
//
// Pure: no GLFW, no camera dependency. A caller that picks from the
// SCREEN converts first with Camera::screenToWorld (Steps 125/126),
// then calls this — composition, not coupling.
// ------------------------------------------------------------------
inline int pickEntity(const std::vector<Entity>& entities,
                      float worldX, float worldY) {
    int best = -1;
    int bestDepth = 0;
    for (std::size_t i = 0; i < entities.size(); ++i) {
        const Entity& e = entities[i];
        if (!e.alive) {
            continue;
        }
        const float hx = e.halfExtents.x * e.scale.x;
        const float hy = e.halfExtents.y * e.scale.y;
        const float dx = worldX > e.position.x ? worldX - e.position.x
                                               : e.position.x - worldX;
        const float dy = worldY > e.position.y ? worldY - e.position.y
                                               : e.position.y - worldY;
        if (dx < hx && dy < hy) {
            if (best == -1 || e.depth > bestDepth) {
                best = static_cast<int>(i);
                bestDepth = e.depth;
            }
            // e.depth == bestDepth keeps the earlier index: first-constructed wins.
        }
    }
    return best;
}

// ------------------------------------------------------------------
// Step 128: screen-space pick convenience (thin composition wrapper).
// Mouse framebuffer PIXELS + camera + framebuffer size + entity list ->
// the picked entity index (pickEntity rules) or -1.
//
// Conversion choice: Camera::screenToWorld — the WORLD-space inverse
// (adds the camera position). Entities live in world space and the
// camera pans them, so the pick must run in world coordinates: a world
// entity under the window center converts to world-space via position,
// which screenToWorld does and screenToWorldUi deliberately does not
// (UI-space conversion is for screen-space UI rects, if ever needed).
//
// No AABB math duplicated: one conversion + one pickEntity call. The
// include of camera.h is acyclic — camera.h is math/state only.
// ------------------------------------------------------------------
inline int pickEntityAtScreen(const std::vector<Entity>& entities,
                              const Camera& cam,
                              float mouseX, float mouseY,
                              float fbWidth, float fbHeight) {
    const Vec3 world = cam.screenToWorld(mouseX, mouseY, fbWidth, fbHeight);
    return pickEntity(entities, world.x, world.y);
}

} // namespace pe

#endif // PUREENGINE_COLLISION_H
