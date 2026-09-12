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
#include <fstream>   // scene file I/O
#include <iomanip>   // fixed setprecision for save
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

// Save Scene to assets/<fileName> atomically via tmp+rename. Format v1:
// # scene v1
// scene=<name>
// tilemap=<basename>   (optional, only if tilemapFile not empty)
// entity=px,py,pz,rotSpeed,sx,sy,sz,hx,hy,hz,texId,depth,roleId,moveSpeed  (repeated)
inline bool saveSceneToFile(const Scene& s, const std::string& fileName) {
    if (fileName.empty() || s.name.empty()) return false;
    const std::string dirs[3] = {"assets/", "../assets/", "../../assets/"};
    std::string writePath;
    std::string tmpPath;
    std::ofstream out;
    for (int i = 0; i < 3; ++i) {
        writePath = dirs[i] + fileName;
        tmpPath = writePath + ".tmp";
        out.open(tmpPath, std::ios::binary | std::ios::trunc);
        if (out) break;
        out.clear();
    }
    if (!out) return false;
    out << "# scene v1\n";
    out << "scene=" << s.name << "\n";
    if (!s.tilemapFile.empty()) {
        out << "tilemap=" << s.tilemapFile << "\n";
    } else if (s.tilemap.width > 0) {
        // Fallback: if tilemap exists but no file name, omit (strict fallback would lose tilemap, but keep file valid)
    }
    out << std::fixed << std::setprecision(4);
    for (const Entity& e : s.entities) {
        out << "entity=" << e.position.x << "," << e.position.y << "," << e.position.z << ","
            << e.rotationSpeed << ","
            << e.scale.x << "," << e.scale.y << "," << e.scale.z << ","
            << e.halfExtents.x << "," << e.halfExtents.y << "," << e.halfExtents.z << ","
            << e.textureId << "," << e.depth << "," << e.roleId << "," << e.moveSpeed << "\n";
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
inline bool loadSceneFromFile(const std::string& fileName, Scene& out) {
    if (fileName.empty()) return false;
    const std::string candidates[3] = {std::string("assets/") + fileName, std::string("../assets/") + fileName, std::string("../../assets/") + fileName};
    std::ifstream in;
    std::string used;
    for (int i = 0; i < 3; ++i) {
        in.open(candidates[i]);
        if (in) { used = candidates[i]; break; }
    }
    if (!in) return false;
    Scene tmp;
    bool haveScene = false;
    bool haveTilemap = false;
    std::string line;
    int lineNo = 0;
    while (std::getline(in, line)) {
        ++lineNo;
        std::string t = sceneTrim(line);
        if (t.empty() || t.rfind("#", 0) == 0) continue;
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
            auto parts = sceneSplitComma(v);
            if (parts.size() != 14) return false;
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
            tmp.entities.push_back(e);
        } else {
            return false; // unknown prefix -> strict fail
        }
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

}  // namespace pe

#endif  // PUREENGINE_SCENE_H
