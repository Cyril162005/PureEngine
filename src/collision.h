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

// ------------------------------------------------------------------
// Step P7: swept segment-vs-AABB (the tunneling fix discrete cannot be).
// Casts the mover's center path (from -> to) against the target's AABB
// EXPANDED by the mover's half extents — the Minkowski expansion, the
// AABB equivalent of capsule-vs-box. A mover whose per-step displacement
// exceeds the target's thickness tunnels past the endpoint-only
// aabbOverlap test; the sweep catches the crossing and reports WHEN
// (entry fraction) and WHERE (face normal) it happens.
//
// The slab method, per axis (2D only — the scene is flat, z ignored):
//   - delta = to - from. If the segment is PARALLEL to the slab
//     (delta == 0) and outside it, there is no hit ever; inside it,
//     the axis constrains nothing.
//   - Otherwise t1/t2 are the slab-boundary crossings; the smaller is
//     the entry, the larger the exit. The axis whose entry is LATEST
//     decides the contact normal (the face the mover came through,
//     pointing back toward the mover).
//   - Hit iff entry <= exit and entry <= 1.0 (the crossing happens
//     within the segment). entry < 0 means the segment STARTS inside
//     the expanded box: reported as a hit at t = 0 with a ZERO normal
//     (no face-crossing happened — the caller treats it as overlap,
//     the discrete path's job).
//
// Edge policy: a segment STARTING exactly ON a slab boundary gives
// tEntry == 0 — the sweep reports contact at the start (a sweep is
// about the path, not the endpoint; the discrete strict-'<' rule
// still governs aabbOverlap itself). Inline, not constexpr: the
// zero-delta guards are runtime branches; pure arithmetic otherwise.
// ------------------------------------------------------------------
struct SweepHit {
    bool hit;
    float t;      // entry fraction along the segment, [0,1]
    Vec3 normal;  // axis-aligned face normal, pointing toward the mover
};

inline SweepHit sweptAABB(const Vec3& from, const Vec3& to,
                          const Vec3& halfExtents,
                          const Vec3& targetCenter,
                          const Vec3& targetHalfExtents) {
    SweepHit out;
    out.hit = false;
    out.t = 1.0f;
    out.normal = Vec3(0.0f, 0.0f, 0.0f);

    // Minkowski expansion: the target grows by the mover's half extents,
    // so the CENTER's segment is all the sweep needs.
    const float expandedHx = halfExtents.x + targetHalfExtents.x;
    const float expandedHy = halfExtents.y + targetHalfExtents.y;

    float tEntry = 0.0f;
    float tExit = 1.0f;
    int entryAxis = -1;  // -1: no axis constrained the entry (start-inside)
    float entrySign = 0.0f;

    // X slab
    const float dx = to.x - from.x;
    if (dx != 0.0f) {
        float t1 = ((targetCenter.x - expandedHx) - from.x) / dx;
        float t2 = ((targetCenter.x + expandedHx) - from.x) / dx;
        float sign = -1.0f;  // moving +x enters the min-x face: normal -x
        if (t1 > t2) {
            const float tmp = t1;
            t1 = t2;
            t2 = tmp;
            sign = 1.0f;     // moving -x enters the max-x face: normal +x
        }
        if (t1 > tEntry) {
            tEntry = t1;
            entryAxis = 0;
            entrySign = sign;
        }
        if (t2 < tExit) {
            tExit = t2;
        }
    } else if (from.x < targetCenter.x - expandedHx ||
               from.x > targetCenter.x + expandedHx) {
        return out;  // parallel to the slab and outside it: no hit ever
    }

    // Y slab (same structure)
    const float dy = to.y - from.y;
    if (dy != 0.0f) {
        float t1 = ((targetCenter.y - expandedHy) - from.y) / dy;
        float t2 = ((targetCenter.y + expandedHy) - from.y) / dy;
        float sign = -1.0f;
        if (t1 > t2) {
            const float tmp = t1;
            t1 = t2;
            t2 = tmp;
            sign = 1.0f;
        }
        if (t1 > tEntry) {
            tEntry = t1;
            entryAxis = 1;
            entrySign = sign;
        }
        if (t2 < tExit) {
            tExit = t2;
        }
    } else if (from.y < targetCenter.y - expandedHy ||
               from.y > targetCenter.y + expandedHy) {
        return out;
    }

    if (tEntry > tExit || tEntry > 1.0f) {
        return out;  // slabs never overlap in time, or the crossing is
                     // beyond the segment's end
    }
    if (tEntry < 0.0f) {
        // Segment starts inside the expanded box: overlap, not a crossing.
        out.hit = true;
        out.t = 0.0f;
        out.normal = Vec3(0.0f, 0.0f, 0.0f);
        return out;
    }
    out.hit = true;
    out.t = tEntry;
    if (entryAxis == 0) {
        out.normal = Vec3(entrySign, 0.0f, 0.0f);
    } else if (entryAxis == 1) {
        out.normal = Vec3(0.0f, entrySign, 0.0f);
    }
    return out;
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
