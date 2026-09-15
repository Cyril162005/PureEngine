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
 * Header-only, no CMakeLists.txt change.
 */

#ifndef PUREENGINE_PREFAB_H
#define PUREENGINE_PREFAB_H

#include "entity.h"
#include "math/vec3.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

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

// --- Internal helpers (local, not exported) ---
namespace prefab_detail {

inline std::string prefabTrim(const std::string& s) {
    const char* ws = " \t\r\n";
    std::size_t a = s.find_first_not_of(ws);
    if (a == std::string::npos) return {};
    std::size_t b = s.find_last_not_of(ws);
    return s.substr(a, b - a + 1);
}

inline std::vector<std::string> prefabCandidates(const std::string& filename) {
    std::vector<std::string> c;
    c.push_back("assets/prefabs/" + filename);
    c.push_back("../assets/prefabs/" + filename);
    c.push_back("../../assets/prefabs/" + filename);
    return c;
}

inline bool prefabParseFloat(const std::string& s, float& out) {
    std::istringstream ss(s);
    ss >> out;
    return !ss.fail();
}

inline bool prefabParseVec3(const std::string& s, Vec3& out) {
    std::istringstream ss(s);
    char comma;
    return (ss >> out.x >> comma >> out.y >> comma >> out.z)
           && !ss.fail();
}

inline bool prefabParseBool(const std::string& s, bool& out) {
    std::string t = prefabTrim(s);
    if (t == "true" || t == "1")  { out = true;  return true; }
    if (t == "false" || t == "0") { out = false; return true; }
    return false;
}

} // namespace prefab_detail

// Load a prefab from assets/prefabs/<filename>
// Returns true if file found and parsed (with at least name=).
// Returns false if file missing; partial parse on malformed lines
// (warns and skips bad lines, keeps valid ones).
inline bool loadPrefab(const std::string& filename, Prefab& out) {
    using namespace prefab_detail;

    // 3-candidate probe
    std::string resolved;
    for (auto& c : prefabCandidates(filename)) {
        if (std::filesystem::exists(c)) { resolved = c; break; }
    }
    // CWD-relative fallback (tests write temp files to the working
    // directory): a bare filename that exists resolves directly.
    if (resolved.empty() && std::filesystem::exists(filename)) {
        resolved = filename;
    }
    // Absolute path guard (for tests using temp files)
    if (resolved.empty()) {
        std::filesystem::path p(filename);
        if (p.is_absolute() && std::filesystem::exists(p))
            resolved = filename;
    }
    if (resolved.empty()) {
        std::cerr << "[prefab] File not found: " << filename << "\n";
        return false;
    }

    std::ifstream f(resolved);
    if (!f.is_open()) {
        std::cerr << "[prefab] Cannot open: " << resolved << "\n";
        return false;
    }

    // Check header
    std::string line;
    if (!std::getline(f, line) || prefabTrim(line) != "# PureEngine prefab v1") {
        std::cerr << "[prefab] Missing or wrong header in: " << resolved << "\n";
        return false;
    }

    Prefab result; // start from defaults
    while (std::getline(f, line)) {
        std::string t = prefabTrim(line);
        if (t.empty() || t[0] == '#') continue;

        auto eq = t.find('=');
        if (eq == std::string::npos) {
            std::cerr << "[prefab] Malformed line (no '='): " << t << "\n";
            continue;
        }
        std::string key = prefabTrim(t.substr(0, eq));
        std::string val = prefabTrim(t.substr(eq + 1));

        float f32; int i32; bool b; Vec3 v3;
        if      (key == "name")          result.name = val;
        else if (key == "scale"          && prefabParseVec3(val, v3))   result.scale = v3;
        else if (key == "halfExtents"    && prefabParseVec3(val, v3))   result.halfExtents = v3;
        else if (key == "tint"           && prefabParseVec3(val, v3))   result.tint = v3;
        else if (key == "textureId"      && (std::istringstream(val)>>i32)) result.textureId = i32;
        else if (key == "depth"          && (std::istringstream(val)>>i32)) result.depth = i32;
        else if (key == "roleId"         && (std::istringstream(val)>>i32)) result.roleId = i32;
        else if (key == "cols"           && (std::istringstream(val)>>i32)) result.cols = i32;
        else if (key == "rows"           && (std::istringstream(val)>>i32)) result.rows = i32;
        else if (key == "moveSpeed"      && prefabParseFloat(val, f32)) result.moveSpeed = f32;
        else if (key == "gravityScale"   && prefabParseFloat(val, f32)) result.gravityScale = f32;
        else if (key == "health"         && prefabParseFloat(val, f32)) result.health = f32;
        else if (key == "coyoteTime"     && prefabParseFloat(val, f32)) result.coyoteTime = f32;
        else if (key == "jumpImpulse"    && prefabParseFloat(val, f32)) result.jumpImpulse = f32;
        else if (key == "maxFallSpeed"   && prefabParseFloat(val, f32)) result.maxFallSpeed = f32;
        else if (key == "isStatic"       && prefabParseBool(val, b))    result.isStatic = b;
        else if (key == "tag")           result.tag = val;
        else if (key == "clip")          result.currentClipName = val;
        else std::cerr << "[prefab] Unknown key '" << key << "' — skipped\n";
    }

    out = result;
    return true;
}

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

#endif // PUREENGINE_PREFAB_H