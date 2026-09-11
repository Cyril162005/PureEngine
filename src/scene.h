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
 * may reallocate and invalidate them — use promptly, do not store.
 * Same caveat class as tileAt() in tilemap.h.
 */
#ifndef PUREENGINE_SCENE_H
#define PUREENGINE_SCENE_H

#include <cstddef>  // std::size_t
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

    // Empty the scene in place. The name is identity, not content, so it
    // survives: the slot stays addressable by switchTo() afterwards.
    void clear() {
        entities.clear();
        tilemap = Tilemap();
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
    return true;
}

}  // namespace pe

#endif  // PUREENGINE_SCENE_H
