/**
 * PureEngine — Step 63: Tilemap System Foundation
 * File: tilemap.h
 *
 * Grid-based level data: Tile/Tilemap structs, a strict file loader,
 * a tile->entity converter (reuse of the existing drawWorld path —
 * NO new renderer code, NO new GL), and entity-vs-solid-tile push-out.
 *
 * Conventions (all documented here because the file format implies them):
 *   - tileId == 0 means EMPTY: skipped by both tilemapToEntities and
 *     the collide scan. Nonzero ids are solid with textureId = value.
 *   - tiles[] is row-major, row 0 = TOP of the map.
 *   - cell (col,row) center (world units):
 *       x = (col - width/2 + 0.5) * tileSize
 *       y = (height/2 - row - 0.5) * tileSize
 *     i.e. the map is centered on the world origin, row 0 on top.
 *   - Tile entities are DRAW data, not gameplay actors: the caller keeps
 *     them in a separate vector, draws them with a second drawWorld()
 *     call, and must NEVER feed them to chase/scan loops.
 *   - collideEntityWithTilemap moves ONLY the entity (full correction);
 *     tiles are immovable by construction. Returns the separation axis
 *     normal, or (0,0,0) on a miss (platformer landing detection reads it).
 *
 * Parsing mirrors hostile_data.h exactly: trimmed comment/blank
 * handling, strict exception-free number parsing, the same 3-candidate
 * asset probe plus the Step-34 absolute-path guard, stderr warnings,
 * whole-file fallback to an empty map. Local trim/parse/probe helpers
 * are duplicated on purpose (including hostile_data.h for them would
 * couple tile loading to hostile data — the worse evil).
 *
 * Header-only, same discipline as every project module: no
 * tilemap.cpp, no CMakeLists.txt change.
 */
#ifndef PUREENGINE_TILEMAP_H
#define PUREENGINE_TILEMAP_H
// Include guard, same pattern as every other project header.

#include <cmath>       // std::floor — overlapped cell-range computation
#include <cstddef>     // std::size_t — tile vector indexing
#include <filesystem>  // absolute-path guard (Step-34 pattern)
#include <fstream>     // the candidate files
#include <iostream>    // malformed-line / missing-file warnings
#include <sstream>     // strict token parsing without exceptions
#include <string>      // lines, keys, values
#include <vector>      // the tile grid + entity conversion output

#include "collision.h"  // aabbOverlap 4-arg overload for cell tests
#include "entity.h"     // Entity built by tilemapToEntities / moved by collide

namespace pe {

struct Tile {
    int tileId = 0;      // 0 = empty (skipped by draw + collide)
    bool solid = false;  // nonzero ids are solid (see loader mapping)
    int textureId = 0;   // renderer slot (Step 55 numbering)
};

struct Tilemap {
    int width = 0;
    int height = 0;
    float tileSize = 1.0f;
    std::vector<Tile> tiles;  // row-major, tiles[row * width + col], row 0 = TOP
};

// --- Local trim (mirrors hostile_data.h; duplicated deliberately) ---
inline std::string tileTrim(const std::string& value) {
    const std::size_t first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

// --- Candidate search paths (mirrors hostileDataCandidates) ---
inline std::vector<std::string> tileDataCandidates(const std::string& fileName) {
    const std::filesystem::path path(fileName);
    if (path.is_absolute()) {
        return { fileName };
    }
    return {
        "assets/" + fileName,
        "../assets/" + fileName,
        "../../assets/" + fileName
    };
}

// --- Strict int parse without exceptions (mirrors hostile_data.h) ---
inline bool tileParseInt(const std::string& text, int& out) {
    std::stringstream stream(text);
    int value = 0;
    stream >> value;
    if (stream.fail() || !stream.eof()) {
        return false;
    }
    out = value;
    return true;
}

// --- Strict float parse without exceptions (mirrors hostile_data.h) ---
inline bool tileParseFloat(const std::string& text, float& out) {
    std::stringstream stream(text);
    float value = 0.0f;
    stream >> value;
    if (stream.fail() || !stream.eof()) {
        return false;
    }
    out = value;
    return true;
}

// --- World position of a cell center (row 0 = TOP, map centered) ---
inline Vec3 tileCenter(const Tilemap& map, int col, int row) {
    const float s = map.tileSize;
    return Vec3((static_cast<float>(col) - static_cast<float>(map.width) * 0.5f + 0.5f) * s,
                (static_cast<float>(map.height) * 0.5f - static_cast<float>(row) - 0.5f) * s,
                0.0f);
}

// --- Bounds-checked cell lookup: nullptr outside the grid ---
inline const Tile* tileAt(const Tilemap& map, int col, int row) {
    if (col < 0 || row < 0 || col >= map.width || row >= map.height) {
        return nullptr;
    }
    const std::size_t index = static_cast<std::size_t>(row) * static_cast<std::size_t>(map.width) +
                              static_cast<std::size_t>(col);
    return &map.tiles[index];
}

// --- Load a tilemap: width/height/tileSize keys + row= lines ---
// Format: width=<int>, height=<int>, tileSize=<float>, then exactly
// `height` row= lines with exactly `width` comma-separated ints each,
// top-to-bottom. Any malformed line, unknown key, dimension mismatch,
// or missing file -> warning + empty map (hostile_data discipline).
inline Tilemap loadTilemap(const std::string& fileName = "tilemap_default.txt") {
    Tilemap map;

    for (const std::string& candidate : tileDataCandidates(fileName)) {
        std::ifstream in(candidate);
        if (!in) {
            continue;
        }

        int width = 0, height = 0;
        float tileSize = 1.0f;
        std::vector<std::vector<int>> rows;
        bool ok = true;

        std::string line;
        while (std::getline(in, line)) {
            const std::string trimmed = tileTrim(line);
            if (trimmed.empty() || trimmed[0] == '#') {
                continue;
            }

            const std::size_t equalsPos = trimmed.find('=');
            if (equalsPos == std::string::npos) {
                std::cerr << "Warning: tilemap file contained a malformed line; using empty map." << std::endl;
                ok = false;
                break;
            }
            const std::string key = tileTrim(trimmed.substr(0, equalsPos));
            const std::string value = tileTrim(trimmed.substr(equalsPos + 1));

            if (key == "width" || key == "height") {
                int parsed = 0;
                if (!tileParseInt(value, parsed) || parsed <= 0) {
                    std::cerr << "Warning: tilemap file has a bad dimension; using empty map." << std::endl;
                    ok = false;
                    break;
                }
                if (key == "width") {
                    width = parsed;
                } else {
                    height = parsed;
                }
            } else if (key == "tileSize") {
                float parsed = 0.0f;
                if (!tileParseFloat(value, parsed) || parsed <= 0.0f) {
                    std::cerr << "Warning: tilemap file has a bad tileSize; using empty map." << std::endl;
                    ok = false;
                    break;
                }
                tileSize = parsed;
            } else if (key == "row") {
                std::vector<int> cells;
                std::stringstream fields(value);
                std::string token;
                bool rowOk = true;
                while (std::getline(fields, token, ',')) {
                    int cell = 0;
                    if (!tileParseInt(tileTrim(token), cell)) {
                        rowOk = false;
                        break;
                    }
                    cells.push_back(cell);
                }
                if (!rowOk || cells.empty()) {
                    std::cerr << "Warning: tilemap file has a malformed row; using empty map." << std::endl;
                    ok = false;
                    break;
                }
                rows.push_back(cells);
            } else {
                std::cerr << "Warning: tilemap file contained an unknown key; using empty map." << std::endl;
                ok = false;
                break;
            }
        }

        if (!ok) {
            return Tilemap{};
        }
        if (width <= 0 || height <= 0) {
            std::cerr << "Warning: tilemap file is missing dimensions; using empty map." << std::endl;
            return Tilemap{};
        }
        if (static_cast<int>(rows.size()) != height) {
            std::cerr << "Warning: tilemap row count does not match height; using empty map." << std::endl;
            return Tilemap{};
        }
        for (const std::vector<int>& cells : rows) {
            if (static_cast<int>(cells.size()) != width) {
                std::cerr << "Warning: tilemap row width does not match width; using empty map." << std::endl;
                return Tilemap{};
            }
        }

        map.width = width;
        map.height = height;
        map.tileSize = tileSize;
        map.tiles.reserve(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
        for (const std::vector<int>& cells : rows) {
            for (int value : cells) {
                Tile tile;
                tile.tileId = value;
                tile.solid = (value != 0);
                tile.textureId = value;
                map.tiles.push_back(tile);
            }
        }
        return map;
    }

    std::cerr << "Warning: tilemap file not found; using empty map." << std::endl;
    return Tilemap{};
}

// --- Non-empty tiles -> draw-ready entities (drawWorld reuse, no GL here) ---
// depth/roleId are caller-supplied (game meaning); textureId comes from
// the tile; scale matches tileSize with exact (non-spinning) half extents.
// Caller keeps the result in its OWN vector and draws it with a separate
// drawWorld() call — never feed these to chase/scan loops.
inline std::vector<Entity> tilemapToEntities(const Tilemap& map, float depth, int roleId) {
    std::vector<Entity> out;
    for (int row = 0; row < map.height; ++row) {
        for (int col = 0; col < map.width; ++col) {
            const Tile* tile = tileAt(map, col, row);
            if (!tile || tile->tileId == 0) {
                continue;
            }
            Entity entity;
            entity.position = tileCenter(map, col, row);
            entity.scale = Vec3(map.tileSize, map.tileSize, 1.0f);
            entity.halfExtents = Vec3(0.5f * map.tileSize, 0.5f * map.tileSize, 0.0f);
            entity.textureId = tile->textureId;
            entity.depth = static_cast<int>(depth);
            entity.roleId = roleId;
            out.push_back(entity);
        }
    }
    return out;
}

// --- Entity vs solid tiles: push-out + axis normal, miss = (0,0,0) ---
// Only overlapped grid cells are tested (constant small set). Each
// overlapping solid cell pushes the entity out along its own
// min-penetration axis immediately; the returned normal is from the
// first resolved cell (single-cell overlaps — the tested case — have
// exactly one). Tiles never move: full correction lands on the entity.
inline Vec3 collideEntityWithTilemap(Entity& entity, const Tilemap& map) {
    if (map.width <= 0 || map.height <= 0 || map.tileSize <= 0.0f) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    const float s = map.tileSize;
    const float ehx = entity.halfExtents.x * entity.scale.x;
    const float ehy = entity.halfExtents.y * entity.scale.y;

    // Grid range overlapped by the entity AABB (mapping inverted).
    int c0 = static_cast<int>(std::floor(entity.position.x / s + static_cast<float>(map.width) * 0.5f - 0.5f - ehx / s));
    int c1 = static_cast<int>(std::floor(entity.position.x / s + static_cast<float>(map.width) * 0.5f - 0.5f + ehx / s));
    // Row 0 sits at TOP (max Y): the entity's top edge yields the SMALLEST
    // row, its bottom edge the largest — r0 <= r1 or the loop below is empty.
    int r0 = static_cast<int>(std::floor(static_cast<float>(map.height) * 0.5f - 0.5f - entity.position.y / s - ehy / s));
    int r1 = static_cast<int>(std::floor(static_cast<float>(map.height) * 0.5f - 0.5f - entity.position.y / s + ehy / s));
    if (c0 < 0) { c0 = 0; }
    if (r0 < 0) { r0 = 0; }
    if (c1 >= map.width) { c1 = map.width - 1; }
    if (r1 >= map.height) { r1 = map.height - 1; }

    Vec3 hitNormal(0.0f, 0.0f, 0.0f);
    bool hit = false;
    const Vec3 ehalf(ehx, ehy, 0.0f);
    for (int row = r0; row <= r1; ++row) {
        for (int col = c0; col <= c1; ++col) {
            const Tile* tile = tileAt(map, col, row);
            if (!tile || !tile->solid) {
                continue;
            }
            const Vec3 center = tileCenter(map, col, row);
            const Vec3 thalf(s * 0.5f, s * 0.5f, 0.0f);
            if (!aabbOverlap(entity.position, ehalf, center, thalf)) {
                continue;
            }
            const float dx = entity.position.x - center.x;
            const float dy = entity.position.y - center.y;
            const float adx = dx >= 0.0f ? dx : -dx;
            const float ady = dy >= 0.0f ? dy : -dy;
            const float overlapX = (ehx + s * 0.5f) - adx;
            const float overlapY = (ehy + s * 0.5f) - ady;
            Vec3 normal(0.0f, 0.0f, 0.0f);
            if (overlapX < overlapY) {
                normal = Vec3(dx >= 0.0f ? 1.0f : -1.0f, 0.0f, 0.0f);
                entity.position.x += normal.x * overlapX;
            } else {
                normal = Vec3(0.0f, dy >= 0.0f ? 1.0f : -1.0f, 0.0f);
                entity.position.y += normal.y * overlapY;
            }
            if (!hit) {
                hitNormal = normal;
                hit = true;
            }
        }
    }
    return hitNormal;
}

} // namespace pe

#endif // PUREENGINE_TILEMAP_H
