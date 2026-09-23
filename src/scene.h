/**
 * PureEngine — Step 64: Scene management foundation
 * File: scene.h
 *
 * A Scene owns a named entity list plus an optional Tilemap; a
 * SceneManager owns the set of scenes and which one is current. This is
 * ownership bookkeeping only — no simulation, no rendering, no game
 * logic. The games keep driving their own vectors until they choose to
 * adopt this (Arcade and Pong do not include this header yet).
 *
 * Conventions (same discipline as hostile_data.h / tilemap.h):
 *  - Header-only: every function is inline, no scene.cpp, no CMake change.
 *  - std::string file/identity plumbing, linear search by name (no <map>).
 *  - All failure paths are safe no-ops / false / nullptr — never crash.
 *  - "No tilemap" is Tilemap{width == 0}, reusing loadTilemap's empty-map
 *    fallback convention — no extra flag field.
 *
 * What this file does NOT do (later steps, not here):
 *  - No GameState coupling (gamestate.h untouched).
 *  - No renderer calls (renderer.h untouched).
 *  - No physics/input changes.
 *  - No automatic entity conversion: loadTilemapIntoScene stores DATA
 *    only; the game still calls tilemapToEntities itself when it wants
 *    drawable entities (Step 63 ruling stands).
 *
 * POINTER INVALIDATION: loadScene/addEntity/currentScene return
 * references/pointers into SceneManager-owned vectors. Any later
 * structural change (loadScene adding a new scene, addEntity pushing)
 * may reallocate and invalidate them — re-take after any structural
 * change (F-01). Mitigate with reserve() where growth is known
 * (e.g. scenes.reserve(4)). Same caveat class as tileAt() in tilemap.h.
 */
#ifndef PUREENGINE_SCENE_H
#define PUREENGINE_SCENE_H

#include <cstddef>  // std::size_t
#include <cstdio>    // std::rename for atomic save
#include <filesystem>
#include <fstream>   // scene file I/O
#include <iomanip>   // fixed setprecision for save
#include <iostream>  // Step 111: version-mismatch warning on load
#include <sstream>   // line parsing
#include <string>
#include <vector>

#include "entity.h"   // Scene::entities element type
#include "tilemap.h"  // Scene::tilemap type + loadTilemap()

namespace pe {

// A named entity collection with an optional tilemap payload.
struct Scene {
    std::string name;
    std::vector<Entity> entities;
    Tilemap tilemap;  // width == 0 means "no tilemap"
    std::string tilemapFile; // optional source file for serialization (basename)

    // Empty the scene in place. The name is identity, not content, so it
    // survives: the slot stays addressable by switchTo() afterwards.
    void clear() {
        entities.clear();
        tilemap = Tilemap();
        tilemapFile.clear();
    }
};

// Owns every scene and which one is current. current is an INDEX (not a
// pointer) so growing scenes/addEntity traffic cannot silently dangle it;
// -1 means "no current scene" (fresh manager, or nothing switched to yet).
// POINTER DISCIPLINE (Step 107): activeScene* is a raw pointer into scenes vector.
// Call scenes.reserve(N) before loadScene() to prevent reallocation.
// Re-take activeScene* after any structural change (loadScene, switchTo, etc.).
struct SceneManager {
    std::vector<Scene> scenes;
    int current = -1;
};

// Find-or-create: a repeat call with an existing name returns THAT scene
// (no silent duplicates). Does NOT change current — switchTo() does that.
inline Scene& loadScene(SceneManager& manager, const std::string& name) {
    for (std::size_t i = 0; i < manager.scenes.size(); ++i) {
        if (manager.scenes[i].name == name) {
            return manager.scenes[i];
        }
    }
    Scene fresh;
    fresh.name = name;
    manager.scenes.push_back(fresh);
    return manager.scenes.back();
}

// Current scene access. nullptr when there is none (fresh manager, or
// current out of range — belt and braces, never crash). See the
// invalidation note at the top of this file before storing the result.
inline Scene* currentScene(SceneManager& manager) {
    if (manager.current < 0) {
        return nullptr;
    }
    const std::size_t index = static_cast<std::size_t>(manager.current);
    if (index >= manager.scenes.size()) {
        return nullptr;
    }
    return &manager.scenes[index];
}

inline const Scene* currentScene(const SceneManager& manager) {
    if (manager.current < 0) {
        return nullptr;
    }
    const std::size_t index = static_cast<std::size_t>(manager.current);
    if (index >= manager.scenes.size()) {
        return nullptr;
    }
    return &manager.scenes[index];
}

// Find a scene by name WITHOUT creating it. nullptr when absent. Prefer
// this over holding a Scene& across loadScene calls: each creation may
// reallocate the vector, dangling earlier references into moved-from
// storage (observed live: a stale alias read a moved-from empty map).
// Re-find after any structural change instead of storing.
inline Scene* sceneByName(SceneManager& manager, const std::string& name) {
    for (std::size_t i = 0; i < manager.scenes.size(); ++i) {
        if (manager.scenes[i].name == name) {
            return &manager.scenes[i];
        }
    }
    return nullptr;
}

inline const Scene* sceneByName(const SceneManager& manager,
                                const std::string& name) {
    for (std::size_t i = 0; i < manager.scenes.size(); ++i) {
        if (manager.scenes[i].name == name) {
            return &manager.scenes[i];
        }
    }
    return nullptr;
}

// Make the named scene current. Unknown name: returns false, current
// UNCHANGED (the caller keeps a valid scene, never a void).
inline bool switchTo(SceneManager& manager, const std::string& name) {
    for (std::size_t i = 0; i < manager.scenes.size(); ++i) {
        if (manager.scenes[i].name == name) {
            manager.current = static_cast<int>(i);
            return true;
        }
    }
    return false;
}

// Empty the current scene in place (name survives, see Scene::clear).
// No current scene: safe no-op.
inline void clearCurrent(SceneManager& manager) {
    Scene* current = currentScene(manager);
    if (current == nullptr) {
        return;
    }
    current->clear();
}

// Append a copy of entity to the CURRENT scene only. No current scene:
// nullptr, nothing stored. See the invalidation note at the top.
inline Entity* addEntity(SceneManager& manager, const Entity& entity) {
    Scene* current = currentScene(manager);
    if (current == nullptr) {
        return nullptr;
    }
    current->entities.push_back(entity);
    return &current->entities.back();
}

// Erase entities[index] of the CURRENT scene only. No current scene or
// out-of-range index: false, nothing erased.
inline bool removeEntity(SceneManager& manager, std::size_t index) {
    Scene* current = currentScene(manager);
    if (current == nullptr) {
        return false;
    }
    if (index >= current->entities.size()) {
        return false;
    }
    current->entities.erase(
        current->entities.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

// --- Step 113: runtime entity lifecycle (index-stable, no erase) ---
// spawnEntity appends a live copy and returns its index. killEntity marks
// the slot dead WITHOUT erasing, so every index-addressed structure
// (colliding[] flags, parentIndex links, renderer drawOrder) keeps
// addressing the same slots. Out-of-range kill: safe no-op. Dead slots
// are skipped by rendering/collision/physics loops and dropped on save.
inline std::size_t spawnEntity(Scene& scene, const Entity& e) {
    scene.entities.push_back(e);
    scene.entities.back().alive = true;
    return scene.entities.size() - 1;
}

inline void killEntity(Scene& scene, std::size_t index) {
    if (index < scene.entities.size()) {
        scene.entities[index].alive = false;
    }
}

// Load tilemap DATA into the scene. Data only — does NOT call
// tilemapToEntities (the game decides if/when tiles become entities).
// Missing/malformed file: returns false and leaves scene.tilemap
// UNTOUCHED (whatever was there, including a previous good map, stays).
inline bool loadTilemapIntoScene(Scene& scene, const std::string& fileName) {
    const Tilemap loaded = loadTilemap(fileName);
    if (loaded.width <= 0 || loaded.height <= 0 || loaded.tiles.empty()) {
        return false;
    }
    scene.tilemap = loaded;
    scene.tilemapFile = fileName;
    return true;
}

// --- Step 81: scene serialization (header-only, text, strict whole-file) ---
// Helpers: trim whitespace (space, tab, CR)
inline std::string sceneTrim(const std::string& s) {
    std::size_t a = 0;
    while (a < s.size() && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '\n')) ++a;
    std::size_t b = s.size();
    while (b > a && (s[b-1] == ' ' || s[b-1] == '\t' || s[b-1] == '\r' || s[b-1] == '\n')) --b;
    return s.substr(a, b - a);
}
inline bool sceneParseFloat(const std::string& s, float& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    const char* c = s.c_str();
    double v = std::strtod(c, &end);
    if (end == c || *end != '\0') return false;
    out = static_cast<float>(v);
    return true;
}
inline bool sceneParseInt(const std::string& s, int& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    const char* c = s.c_str();
    long v = std::strtol(c, &end, 10);
    if (end == c || *end != '\0') return false;
    out = static_cast<int>(v);
    return true;
}
inline std::vector<std::string> sceneSplitComma(const std::string& s) {
    std::vector<std::string> parts;
    std::string cur;
    for (char ch : s) {
        if (ch == ',') { parts.push_back(sceneTrim(cur)); cur.clear(); }
        else cur.push_back(ch);
    }
    parts.push_back(sceneTrim(cur));
    return parts;
}

// --- Step 111: persistence v2 string helpers (header-only, quoted fields) ---
// sceneQuote wraps a string in double quotes, escaping '\' and '"'.
// sceneUnquote reverses it (strict: missing quotes or bad escape = false).
// sceneSplitCommaQuoted splits on commas OUTSIDE quoted spans so tags like
// "enemy, fast" survive as one field.
inline std::string sceneQuote(const std::string& s) {
    std::string q = "\"";
    for (char ch : s) {
        if (ch == '\\' || ch == '"') q.push_back('\\');
        q.push_back(ch);
    }
    q.push_back('"');
    return q;
}
inline bool sceneUnquote(const std::string& s, std::string& out) {
    std::string t = sceneTrim(s);
    if (t.size() < 2 || t.front() != '"' || t.back() != '"') return false;
    std::string inner = t.substr(1, t.size() - 2);
    std::string r;
    for (std::size_t i = 0; i < inner.size(); ++i) {
        if (inner[i] == '\\' && i + 1 < inner.size() &&
            (inner[i + 1] == '"' || inner[i + 1] == '\\')) {
            r.push_back(inner[i + 1]);
            ++i;
        } else if (inner[i] == '\\') {
            return false;  // dangling/bad escape -> strict fail
        } else {
            r.push_back(inner[i]);
        }
    }
    out = r;
    return true;
}
inline std::vector<std::string> sceneSplitCommaQuoted(const std::string& s) {
    std::vector<std::string> parts;
    std::string cur;
    bool inQuotes = false;
    for (std::size_t i = 0; i < s.size(); ++i) {
        char ch = s[i];
        if (inQuotes) {
            cur.push_back(ch);
            if (ch == '\\' && i + 1 < s.size()) { cur.push_back(s[++i]); }
            else if (ch == '"') inQuotes = false;
        } else if (ch == '"') {
            inQuotes = true;
            cur.push_back(ch);
        } else if (ch == ',') {
            parts.push_back(sceneTrim(cur));
            cur.clear();
        } else {
            cur.push_back(ch);
        }
    }
    parts.push_back(sceneTrim(cur));
    return parts;
}

// Save Scene to assets/<fileName> atomically via tmp+rename. Format v2
// (Step 111; v1 wrote the first 14 of these fields with a "# scene v1"
// header — see loadSceneFromFile for backward compatibility):
// # scene v2
// scene=<name>
// tilemap=<basename>   (optional, only if tilemapFile not empty)
// entity=px,py,pz,rotAngle,rotSpeed,sx,sy,sz,hx,hy,hz,texId,depth,roleId,moveSpeed,vx,vy,vz,gravityScale,isStatic,coyoteTime,jumpImpulse,maxFallSpeed,tintR,tintG,tintB,cols,rows,health,timer,tag,parentIndex,animationSpeed,currentClipName  (repeated)
//   - isStatic is 0/1; tag and currentClipName are double-quoted strings
//     ("" when empty); 34 comma-separated fields total.
inline bool saveSceneToFile(const Scene& s, const std::string& fileName) {
    if (fileName.empty() || s.name.empty()) return false;
    // Handle explicit paths (savedata/...) directly; else probe assets/
    bool explicitPath = fileName.find('/') != std::string::npos || fileName.find('\\') != std::string::npos || (fileName.size() > 1 && fileName[1] == ':');
    std::string writePath;
    std::string tmpPath;
    std::ofstream out;
    if (explicitPath) {
        // Ensure directory exists for savedata/ etc.
        std::size_t slash = fileName.find_last_of("/\\");
        if (slash != std::string::npos) {
            std::string dir = fileName.substr(0, slash);
            std::error_code ec;
            std::filesystem::create_directories(dir, ec);
        }
        writePath = fileName;
        tmpPath = writePath + ".tmp";
        out.open(tmpPath, std::ios::binary | std::ios::trunc);
        if (!out) return false;
    } else {
        const std::string dirs[3] = {"assets/", "../assets/", "../../assets/"};
        for (int i = 0; i < 3; ++i) {
            writePath = dirs[i] + fileName;
            tmpPath = writePath + ".tmp";
            out.open(tmpPath, std::ios::binary | std::ios::trunc);
            if (out) break;
            out.clear();
        }
        if (!out) return false;
    }
    out << "# scene v2\n";
    out << "scene=" << s.name << "\n";
    if (!s.tilemapFile.empty()) {
        out << "tilemap=" << s.tilemapFile << "\n";
    } else if (s.tilemap.width > 0) {
        // Fallback: if tilemap exists but no file name, omit (strict fallback would lose tilemap, but keep file valid)
    }
    out << std::fixed << std::setprecision(4);
    for (const Entity& e : s.entities) {
        if (!e.alive) continue;  // Step 113: dead entities don't exist in saved state
        out << "entity=" << e.position.x << "," << e.position.y << "," << e.position.z << ","
            << e.rotationAngle << "," << e.rotationSpeed << ","
            << e.scale.x << "," << e.scale.y << "," << e.scale.z << ","
            << e.halfExtents.x << "," << e.halfExtents.y << "," << e.halfExtents.z << ","
            << e.textureId << "," << e.depth << "," << e.roleId << "," << e.moveSpeed << ","
            << e.velocity.x << "," << e.velocity.y << "," << e.velocity.z << ","
            << e.gravityScale << "," << (e.isStatic ? 1 : 0) << ","
            << e.coyoteTime << "," << e.jumpImpulse << "," << e.maxFallSpeed << ","
            << e.tint.x << "," << e.tint.y << "," << e.tint.z << ","
            << e.cols << "," << e.rows << ","
            << e.health << "," << e.timer << ","
            << sceneQuote(e.tag) << "," << e.parentIndex << ","
            << e.animationSpeed << "," << sceneQuote(e.currentClipName) << "\n";
    }
    out.close();
    if (!out) { std::remove(tmpPath.c_str()); return false; }
    if (std::rename(tmpPath.c_str(), writePath.c_str()) != 0) {
        std::remove(tmpPath.c_str());
        return false;
    }
    return true;
}

// Load Scene from assets/<fileName> via 3-candidate probe. Strict whole-file:
// any malformed line or missing scene= causes false and leaves out untouched.
// Explicit paths (savedata/...) are tried directly first.
// Step 111: versioned. "# scene v1" (or no header) parses the legacy 14-field
// entity lines with defaults for newer state; "# scene v2" parses the full
// 34-field lines. Unknown versions (e.g. "# scene v99") warn and return false
// without touching out.
inline bool loadSceneFromFile(const std::string& fileName, Scene& out) {
    if (fileName.empty()) return false;
    bool explicitPath = fileName.find('/') != std::string::npos || fileName.find('\\') != std::string::npos || (fileName.size() > 1 && fileName[1] == ':');
    std::string candidates[4];
    int candCount = 0;
    if (explicitPath) {
        candidates[candCount++] = fileName;
        candidates[candCount++] = std::string("../") + fileName;
        candidates[candCount++] = std::string("../../") + fileName;
    } else {
        candidates[0] = std::string("assets/") + fileName;
        candidates[1] = std::string("../assets/") + fileName;
        candidates[2] = std::string("../../assets/") + fileName;
        candCount = 3;
    }
    std::ifstream in;
    std::string used;
    for (int i = 0; i < candCount; ++i) {
        in.open(candidates[i]);
        if (in) { used = candidates[i]; break; }
        in.clear();
    }
    if (!in) return false;
    Scene tmp;
    bool haveScene = false;
    bool haveTilemap = false;
    int fileVersion = 1;      // default when no "# scene vN" header is present
    bool versionSeen = false;
    std::string line;
    int lineNo = 0;
    while (std::getline(in, line)) {
        ++lineNo;
        std::string t = sceneTrim(line);
        if (t.empty()) continue;
        if (t.rfind("#", 0) == 0) {
            // Step 111: version header ("# scene v1" / "# scene v2").
            // Anything else starting with '#' stays a plain comment.
            if (t.rfind("# scene v", 0) == 0) {
                int v = 0;
                if (sceneParseInt(sceneTrim(t.substr(9)), v)) {
                    fileVersion = v;
                    versionSeen = true;
                }
            }
            continue;
        }
        if (t.rfind("scene=", 0) == 0) {
            if (haveScene) return false;
            std::string v = sceneTrim(t.substr(6));
            if (v.empty()) return false;
            tmp.name = v;
            haveScene = true;
        } else if (t.rfind("tilemap=", 0) == 0) {
            if (haveTilemap) return false;
            std::string v = sceneTrim(t.substr(8));
            if (v.empty()) return false;
            tmp.tilemapFile = v;
            haveTilemap = true;
        } else if (t.rfind("entity=", 0) == 0) {
            if (!haveScene) return false;
            std::string v = sceneTrim(t.substr(7));
            auto parts = sceneSplitCommaQuoted(v);
            if (parts.size() == 14) {
                // Legacy v1 line: 14 fields, defaults for all v2-only state.
                float px, py, pz, rot, sx, sy, sz, hx, hy, hz;
                int texId, depth, roleId;
                float moveSpeed;
                if (!sceneParseFloat(parts[0], px)) return false;
                if (!sceneParseFloat(parts[1], py)) return false;
                if (!sceneParseFloat(parts[2], pz)) return false;
                if (!sceneParseFloat(parts[3], rot)) return false;
                if (!sceneParseFloat(parts[4], sx)) return false;
                if (!sceneParseFloat(parts[5], sy)) return false;
                if (!sceneParseFloat(parts[6], sz)) return false;
                if (!sceneParseFloat(parts[7], hx)) return false;
                if (!sceneParseFloat(parts[8], hy)) return false;
                if (!sceneParseFloat(parts[9], hz)) return false;
                if (!sceneParseInt(parts[10], texId)) return false;
                if (!sceneParseInt(parts[11], depth)) return false;
                if (!sceneParseInt(parts[12], roleId)) return false;
                if (!sceneParseFloat(parts[13], moveSpeed)) return false;
                Entity e(Vec3(px, py, pz), rot, Vec3(sx, sy, sz), Vec3(hx, hy, hz), texId);
                e.depth = depth;
                e.roleId = roleId;
                e.moveSpeed = moveSpeed;
                // v1 defaults for v2-only state (Step 111 contract)
                e.rotationAngle = 0.0f;
                e.velocity = Vec3(0.0f, 0.0f, 0.0f);
                e.gravityScale = 0.0f;
                e.isStatic = false;
                e.coyoteTime = 0.1f;
                e.jumpImpulse = 7.0f;
                e.maxFallSpeed = 25.0f;
                e.tint = Vec3(1.0f, 1.0f, 1.0f);
                e.cols = 1;
                e.rows = 1;
                e.health = 100.0f;
                e.timer = 0.0f;
                e.tag.clear();
                e.parentIndex = -1;
                e.animationSpeed = 1.0f;
                e.currentClipName.clear();
                tmp.entities.push_back(e);
            } else if (parts.size() == 34) {
                // Full v2 line: 14 legacy fields + 20 extended fields.
                float px, py, pz, rotAngle, rot, sx, sy, sz, hx, hy, hz;
                int texId, depth, roleId;
                float moveSpeed, vx, vy, vz, grav;
                int isStaticInt;
                float coyote, jump, maxFall, tr, tg, tb;
                int cols, rows;
                float health, timer;
                std::string tagStr, clipStr;
                int parentIndex;
                float animSpeed;
                if (!sceneParseFloat(parts[0], px)) return false;
                if (!sceneParseFloat(parts[1], py)) return false;
                if (!sceneParseFloat(parts[2], pz)) return false;
                if (!sceneParseFloat(parts[3], rotAngle)) return false;
                if (!sceneParseFloat(parts[4], rot)) return false;
                if (!sceneParseFloat(parts[5], sx)) return false;
                if (!sceneParseFloat(parts[6], sy)) return false;
                if (!sceneParseFloat(parts[7], sz)) return false;
                if (!sceneParseFloat(parts[8], hx)) return false;
                if (!sceneParseFloat(parts[9], hy)) return false;
                if (!sceneParseFloat(parts[10], hz)) return false;
                if (!sceneParseInt(parts[11], texId)) return false;
                if (!sceneParseInt(parts[12], depth)) return false;
                if (!sceneParseInt(parts[13], roleId)) return false;
                if (!sceneParseFloat(parts[14], moveSpeed)) return false;
                if (!sceneParseFloat(parts[15], vx)) return false;
                if (!sceneParseFloat(parts[16], vy)) return false;
                if (!sceneParseFloat(parts[17], vz)) return false;
                if (!sceneParseFloat(parts[18], grav)) return false;
                if (!sceneParseInt(parts[19], isStaticInt)) return false;
                if (!sceneParseFloat(parts[20], coyote)) return false;
                if (!sceneParseFloat(parts[21], jump)) return false;
                if (!sceneParseFloat(parts[22], maxFall)) return false;
                if (!sceneParseFloat(parts[23], tr)) return false;
                if (!sceneParseFloat(parts[24], tg)) return false;
                if (!sceneParseFloat(parts[25], tb)) return false;
                if (!sceneParseInt(parts[26], cols)) return false;
                if (!sceneParseInt(parts[27], rows)) return false;
                if (!sceneParseFloat(parts[28], health)) return false;
                if (!sceneParseFloat(parts[29], timer)) return false;
                if (!sceneUnquote(parts[30], tagStr)) return false;
                if (!sceneParseInt(parts[31], parentIndex)) return false;
                if (!sceneParseFloat(parts[32], animSpeed)) return false;
                if (!sceneUnquote(parts[33], clipStr)) return false;
                Entity e(Vec3(px, py, pz), rot, Vec3(sx, sy, sz), Vec3(hx, hy, hz), texId);
                e.rotationAngle = rotAngle;
                e.depth = depth;
                e.roleId = roleId;
                e.moveSpeed = moveSpeed;
                e.velocity = Vec3(vx, vy, vz);
                e.gravityScale = grav;
                e.isStatic = (isStaticInt != 0);
                e.coyoteTime = coyote;
                e.jumpImpulse = jump;
                e.maxFallSpeed = maxFall;
                e.tint = Vec3(tr, tg, tb);
                e.cols = cols;
                e.rows = rows;
                e.health = health;
                e.timer = timer;
                e.tag = tagStr;
                e.parentIndex = parentIndex;
                e.animationSpeed = animSpeed;
                e.currentClipName = clipStr;
                tmp.entities.push_back(e);
            } else {
                return false;  // neither v1 (14) nor v2 (34) shape -> strict fail
            }
        } else {
            return false; // unknown prefix -> strict fail
        }
    }
    if (versionSeen && fileVersion != 1 && fileVersion != 2) {
        std::cerr << "loadSceneFromFile: unknown scene version v"
                  << fileVersion << " in " << fileName << " (expected v1 or v2)\n";
        return false;
    }
    if (!haveScene) return false;
    if (haveTilemap) {
        Tilemap tm = loadTilemap(tmp.tilemapFile);
        if (tm.width <= 0 || tm.tiles.empty()) return false;
        tmp.tilemap = tm;
    }
    out = tmp;
    return true;
}

inline bool saveSceneManagerToFile(const SceneManager& m, const std::string& fileName) {
    if (fileName.empty()) return false;
    // Simple index file: list scene files, one per line
    const std::string writePath = std::string("assets/") + fileName;
    const std::string tmpPath = writePath + ".tmp";
    std::ofstream out(tmpPath, std::ios::binary | std::ios::trunc);
    if (!out) return false;
    out << "# scene manager v1\n";
    out << "scenes=" << m.scenes.size() << "\n";
    out << "current=" << m.current << "\n";
    for (const Scene& s : m.scenes) {
        out << "scene_file=scene_" << s.name << ".txt\n";
        if (!saveSceneToFile(s, std::string("scene_") + s.name + ".txt")) {
            out.close();
            std::remove(tmpPath.c_str());
            return false;
        }
    }
    out.close();
    if (!out) { std::remove(tmpPath.c_str()); return false; }
    if (std::rename(tmpPath.c_str(), writePath.c_str()) != 0) {
        std::remove(tmpPath.c_str());
        return false;
    }
    return true;
}

// --- Increment (Scene/Persistence campaign): manager index loader ---
// Read side of saveSceneManagerToFile's "# scene manager v1" index file:
// scenes=<count>, current=<index>, one scene_file=<basename> line per
// scene. Each scene_file is loaded through the real loadSceneFromFile
// (its 3-candidate probe, so the manager round-trip works from any CWD
// the per-scene files reach). Strict whole-file: duplicate scenes=/
// current=, scenes-count mismatch, bad current index, any failed scene
// load, or an unknown prefix -> false, out untouched. Missing header
// defaults to v1; a seen-but-unknown version warns and fails (same
// convention as loadSceneFromFile). No current scene is current=-1,
// reusing the SceneManager field's own convention.
inline bool loadSceneManagerFromFile(const std::string& fileName, SceneManager& out) {
    if (fileName.empty()) return false;
    const std::string candidates[3] = {"assets/" + fileName,
                                       "../assets/" + fileName,
                                       "../../assets/" + fileName};
    std::ifstream in;
    for (int i = 0; i < 3; ++i) {
        in.open(candidates[i]);
        if (in) break;
        in.clear();
    }
    if (!in) return false;
    SceneManager tmp;
    int sceneCount = -1;  // -1 = scenes= not seen yet
    int current = -2;     // -2 = current= not seen yet (current=-1 is valid data)
    std::vector<std::string> sceneFiles;
    int fileVersion = 1;
    bool versionSeen = false;
    std::string line;
    while (std::getline(in, line)) {
        const std::string t = sceneTrim(line);
        if (t.empty()) continue;
        if (t.rfind("#", 0) == 0) {
            if (t.rfind("# scene manager v", 0) == 0) {
                int v = 0;
                if (sceneParseInt(sceneTrim(t.substr(17)), v)) {
                    fileVersion = v;
                    versionSeen = true;
                }
            }
            continue;  // anything else starting with '#' stays a plain comment
        }
        if (t.rfind("scenes=", 0) == 0) {
            if (sceneCount != -1) return false;  // duplicate
            int v = 0;
            if (!sceneParseInt(sceneTrim(t.substr(7)), v) || v <= 0) return false;
            sceneCount = v;
        } else if (t.rfind("current=", 0) == 0) {
            if (current != -2) return false;  // duplicate
            int v = 0;
            if (!sceneParseInt(sceneTrim(t.substr(8)), v)) return false;
            current = v;
        } else if (t.rfind("scene_file=", 0) == 0) {
            const std::string v = sceneTrim(t.substr(11));
            if (v.empty()) return false;
            sceneFiles.push_back(v);
        } else {
            return false;  // unknown prefix -> strict fail
        }
    }
    if (versionSeen && fileVersion != 1) {
        std::cerr << "loadSceneManagerFromFile: unknown scene manager version v"
                  << fileVersion << " in " << fileName << " (expected v1)\n";
        return false;
    }
    if (sceneCount == -1) return false;   // missing scenes=
    if (current == -2) return false;      // missing current=
    if (static_cast<int>(sceneFiles.size()) != sceneCount) return false;
    if (current != -1 && (current < 0 || current >= sceneCount)) return false;
    tmp.scenes.reserve(static_cast<std::size_t>(sceneCount));
    for (const std::string& sceneFileName : sceneFiles) {
        Scene loaded;
        if (!loadSceneFromFile(sceneFileName, loaded)) {
            return false;  // strict: any failed scene load rejects the manager
        }
        tmp.scenes.push_back(loaded);
    }
    tmp.current = current;
    out = tmp;
    return true;
}

}  // namespace pe

#endif  // PUREENGINE_SCENE_H
