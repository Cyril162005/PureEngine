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

#include <cstddef>  // std::size_t for the Step 65 hierarchy functions
#include <vector>   // std::vector<Entity> for the Step 65 hierarchy functions

#include "math/vec3.h"  // position and scale are Vec3s
#include "math/mat4.h"  // modelMatrix() returns a Mat4
#include "animation.h"  // AnimationState member (Step 56, data only)

namespace pe {

// --- Step 54: opaque role identity (replaces Step 47's arcade enum) ---
// The engine stores an int it never interprets. The GAME assigns meaning
// (arcade mapping: 0 = player, 1 = scenery, 2 = hostile — see ArcadeRole
// in lifecycle.h). Engine filters compare roleId only against
// caller-supplied values, never against engine-side names.

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
    int roleId = 0;  // opaque game-defined identity (see above)
    float moveSpeed = 0.0f;
    // --- Step 56: animation playback (data only, no engine consumer yet) ---
    // animationState tracks the playing clip; animationSpeed scales dt at
    // the call site (update(dt * animationSpeed)). Both default to idle.
    AnimationState animationState;
    float animationSpeed = 1.0f;
    // --- Step 59B: per-entity sheet layout (full grids only) ---
    // cols*rows cells; total frame count = cols*rows. Defaults (1x1)
    // sample the full texture — identical to pre-animation behavior.
    int cols = 1;
    int rows = 1;
    // --- Step 60: physics state (inert by default) ---
    // velocity: world units/s. gravityScale: 0 = no physics (default,
    // non-breaking), 1 = full GRAVITY. See physics.h (free functions,
    // massless model, no persistent acceleration field).
    Vec3 velocity = Vec3(0.0f, 0.0f, 0.0f);
    float gravityScale = 0.0f;
    // --- Step 65: hierarchy link (inert by default) — Step 83 freeze ---
    // Contract: attachment only (translation); parent indices invalid after
    // erase/reorder — re-establish after any structural change (same
    // discipline as SceneManager indices, Step 64). parentIndex is an INDEX
    // into the caller's entity vector, not an owning pointer: -1 = root
    // (pre-Step-65 behavior for every existing entity). The engine never
    // follows it implicitly — only setParent()/worldPosition() read it — so
    // renderer, physics, collision, and both games are unaffected until a
    // caller opts in. No TRS composition (rotation/scale do not propagate).
    int parentIndex = -1;

    // --- Step 71: physics body type (platformer hardening) ---
    // isStatic = true means infinite mass: the body never moves from
    // collisions, and its velocity is never integrated. Intended for
    // tilemap geometry, platforms, and other immovable level geometry.
    // Default false preserves all existing behavior (arcade game,
    // Pong, etc.). Gravity/integration still respect gravityScale and
    // velocity as before; only resolveCollision treats the flag.
    bool isStatic = false;

    // --- Step 71: character controller fields (platformer hardening) ---
    // Coyote timer: remaining time in the coyote window (seconds)
    float coyoteTimer = 0.0f;
    // Coyote time: how long after leaving ground a jump is still allowed
    float coyoteTime = 0.1f;
    // Jump impulse: upward velocity applied on jump
    float jumpImpulse = 12.0f;
    // Maximum downward velocity (terminal velocity cap)
    float maxFallSpeed = 25.0f;
    // Was grounded last frame (for state tracking)
    bool wasGrounded = false;

    // --- Step 86: per-entity tint (particle color) ---
    // Default white (1,1,1) — existing entities unchanged; colliding tint
    // overrides to red (1,0,0) in renderer. Particles set this to Particle.color.
    Vec3 tint = Vec3(1.0f, 1.0f, 1.0f);

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
          depth(0),
          roleId(0),
          moveSpeed(0.0f),
          animationState(),
          animationSpeed(1.0f),
          cols(1),
          rows(1),
          velocity(0.0f, 0.0f, 0.0f),
gravityScale(0.0f),
            parentIndex(-1),
            isStatic(false),
            tint(1.0f, 1.0f, 1.0f) {}

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
          depth(0),
          roleId(0),
          moveSpeed(0.0f),
          animationState(),
          animationSpeed(1.0f),
           cols(1),
           rows(1),
           velocity(0.0f, 0.0f, 0.0f),
 gravityScale(0.0f),
            parentIndex(-1),
            isStatic(false),
            tint(1.0f, 1.0f, 1.0f) {}

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

// --- Step 65: parent-child transform foundation (free functions) ---
// TRANSLATION-ONLY: worldPosition() accumulates local positions up the
// parent chain. A parent's rotation/scale do NOT propagate to children
// yet — full TRS composition is future work once a consumer needs it.
// The hierarchy is OPT-IN: nothing in the engine calls these implicitly,
// so every pre-Step-65 entity (parentIndex == -1) behaves exactly as
// before. parentIndex values are indices into the CALLER's vector —
// reorder/resize the vector and re-establish parenting afterwards.
inline bool setParent(std::vector<Entity>& entities, std::size_t child,
                      int parent) {
    if (child >= entities.size()) {
        return false;
    }
    if (parent < -1) {
        return false;
    }
    if (parent != -1) {
        const std::size_t first = static_cast<std::size_t>(parent);
        if (first >= entities.size() || first == child) {
            return false;
        }
        // Walk the prospective parent's chain: reaching `child` means
        // this link would close a cycle. The walk is hard-capped at
        // entities.size() steps — a well-founded chain is always shorter
        // than that, so exhausting the budget means the ancestry never
        // resolves (a pre-existing detached loop, writable because the
        // field is public) and the link is refused too.
        int cursor = parent;
        for (std::size_t steps = 0; steps <= entities.size(); ++steps) {
            if (cursor < 0) {
                break;  // reached a root: well-founded, safe to link
            }
            const std::size_t here = static_cast<std::size_t>(cursor);
            if (here >= entities.size()) {
                break;  // hand-written garbage link: treat as root
            }
            if (here == child) {
                return false;  // cycle
            }
            if (steps == entities.size()) {
                return false;  // never resolved: detached loop, refuse
            }
            cursor = entities[here].parentIndex;
        }
    }
    entities[child].parentIndex = parent;
    return true;
}

// Sum local positions from the entity up through its ancestors.
// Out-of-range index: (0,0,0). The walk is hard-capped at
// entities.size() steps, so even a hand-written cycle (bypassing
// setParent — the field is public) always terminates instead of hanging.
inline Vec3 worldPosition(const std::vector<Entity>& entities,
                          std::size_t index) {
    if (index >= entities.size()) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    Vec3 world = entities[index].position;
    int cursor = entities[index].parentIndex;
    for (std::size_t steps = 0; steps < entities.size(); ++steps) {
        if (cursor < 0) {
            break;
        }
        const std::size_t here = static_cast<std::size_t>(cursor);
        if (here >= entities.size()) {
            break;
        }
        world = world + entities[here].position;
        cursor = entities[here].parentIndex;
    }
    return world;
}

} // namespace pe

#endif // PUREENGINE_ENTITY_H
