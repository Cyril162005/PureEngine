/**
 * PureEngine — Step 120: Prefab/Template System
 * File: prefab.h
 *
 * Data-driven entity configuration: a Prefab holds only configuration fields
 * (scale, halfExtents, textureId, depth, roleId, moveSpeed, gravityScale,
 * isStatic, health, coyoteTime, jumpImpulse, maxFallSpeed, cols, rows,
 * tag, currentClipName, tint). Runtime state (position, velocity,
 * rotationAngle, rotationSpeed, parentIndex, alive, coyoteTimer,
 * wasGrounded, animationState, timer) is NOT stored — the caller supplies
 * position at instantiate time, and Entity defaults handle the rest.
 *
 * Parse discipline mirrors hostile_data.h exactly: local trim, 3-candidate
 * probe (assets/prefabs/, ../assets/prefabs/, ../../assets/prefabs/),
 * absolute-path guard, line-by-line key=value, comments (#), empty skip,
 * malformed→warn+skip (valid fields kept), no exceptions, no throw.
 * Step 324: the file-I/O half (loadPrefab + the prefab_detail parse helpers) moved to the paired prefab_persist.h/.cpp; the data + instantiatePrefab stay header-only.
 */

#ifndef PUREENGINE_PREFAB_H
#define PUREENGINE_PREFAB_H

#include "entity.h"
#include "math/vec3.h"
#include <string>

namespace pe {

struct Prefab {
    std::string name;
    // Configuration fields only (NOT runtime state)
    Vec3 scale         = Vec3(1.0f, 1.0f, 1.0f);
    Vec3 halfExtents   = Vec3(0.5f, 0.5f, 0.5f);
    Vec3 tint          = Vec3(1.0f, 1.0f, 1.0f);
    int  textureId     = 0;
    int  depth         = 0;
    int  roleId        = 0;
    float moveSpeed    = 0.0f;
    float gravityScale = 0.0f;
    bool  isStatic     = false;
    float health       = 100.0f;
    float coyoteTime   = 0.1f;
    float jumpImpulse  = 12.0f;
    float maxFallSpeed = 25.0f;
    int   cols         = 1;
    int   rows         = 1;
    std::string tag;
    std::string currentClipName;
    // NOT included: position, velocity, rotationAngle, rotationSpeed,
    //               parentIndex, alive, coyoteTimer, wasGrounded,
    //               animationState, timer
};

// Create a live Entity from a prefab at the given world position.
// All runtime state fields are left at Entity defaults.
inline Entity instantiatePrefab(const Prefab& p, const Vec3& position) {
    Entity e;
    e.position       = position;
    e.scale          = p.scale;
    e.halfExtents    = p.halfExtents;
    e.tint           = p.tint;
    e.textureId      = p.textureId;
    e.depth          = p.depth;
    e.roleId         = p.roleId;
    e.moveSpeed      = p.moveSpeed;
    e.gravityScale   = p.gravityScale;
    e.isStatic       = p.isStatic;
    e.health         = p.health;
    e.coyoteTime     = p.coyoteTime;
    e.jumpImpulse    = p.jumpImpulse;
    e.maxFallSpeed   = p.maxFallSpeed;
    e.cols           = p.cols;
    e.rows           = p.rows;
    e.tag            = p.tag;
    e.currentClipName = p.currentClipName;
    e.alive          = true;
    // NOT set: velocity, rotationAngle, rotationSpeed, parentIndex,
    //          coyoteTimer, wasGrounded, animationState, timer
    return e;
}

} // namespace pe

// --- Step 324: the file-I/O half moved to prefab_persist.h/.cpp (the load
// contracts unchanged; the include sits AFTER the namespace close, the
// same guarded-include pattern as the scene_persist pair). ---
#include "prefab_persist.h"
#endif // PUREENGINE_PREFAB_H