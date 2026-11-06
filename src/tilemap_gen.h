/**
 * PureEngine - Step 352: the noise->tilemap generator (the tilemap path,
 * ADDITIVE; header-only inline, mirroring tilemap.h's own header-only
 * discipline). No existing tilemap function is edited; the generator consumes
 * Tile/Tilemap and calls the seed/noise modules.
 *
 * THE RECORDED TILEMAP SEMANTICS (the 352 read of tilemap.h :56-125 and the
 * loadTilemap validation :139-251):
 *  - tileId 0 = EMPTY (skipped by draw + collide); nonzero = SOLID;
 *    textureId == tileId (the loader mapping :241-242).
 *  - The legal renderer range: [0, TEXTURE_SLOTS-1] = [0,4]
 *    (tilemap.h :53-54, TEXTURE_SLOTS = 5; the OOB warn + checker fallback
 *    :273-286). The generator emits ids 1 and 2 only (0 for the empty band).
 *  - The loader dimensions: width > 0, height > 0 (:171), rows == height
 *    (:222), row length == width (:227); NO upper bound. The generator adds
 *    the documented additive cap (kNoiseGenMaxDim = 256) per the 352 scope.
 *
 * THE GENERATOR: seed = deriveSeed(master, "tilemap") (the named stable id
 * from the 350 API); the noise coordinate is WORLD-ANCHORED:
 * (originX + col) / scale - so two regions sharing an origin are
 * byte-equal on the overlap (the region-consistency pin). The threshold
 * mapping is owned and named; the boundary rule is STRICT less-than (a value
 * exactly AT a threshold maps to the HIGHER band - pinned).
 *
 * UNWIRED except the ONE console command (main.cpp "tilemap noise <seed>",
 * session-transient, never saved). No terrain, no fbm, no game wiring.
 */
#ifndef PUREENGINE_TILEMAP_GEN_H
#define PUREENGINE_TILEMAP_GEN_H

#include <cstdint>
#include <iostream>

#include "tilemap.h"
#include "noise.h"
#include "seed.h"

namespace pe {

// --- The owned threshold mapping (named constants) ---
inline constexpr float kNoiseGenThresholdA  = 0.42f;  // below  -> EMPTY (0)
inline constexpr float kNoiseGenThresholdB  = 0.68f;  // A..B   -> tile 1; >= B -> tile 2
inline constexpr int   kNoiseGenTileEmpty   = 0;
inline constexpr int   kNoiseGenTileLow     = 1;
inline constexpr int   kNoiseGenTileHigh    = 2;
inline constexpr int   kNoiseGenMaxDim      = 256;    // the additive cap (the loader has none)
static_assert(kNoiseGenTileHigh < TEXTURE_SLOTS,
    "the generated tile ids must stay in the legal renderer range [0, TEXTURE_SLOTS-1]");

// The owned value->tile mapping. The boundary rule: STRICT less-than, so a
// value exactly AT a threshold maps to the HIGHER band (pinned).
inline int tileIdForNoiseValue(float v) {
    if (v < kNoiseGenThresholdA) {
        return kNoiseGenTileEmpty;
    }
    if (v < kNoiseGenThresholdB) {
        return kNoiseGenTileLow;
    }
    return kNoiseGenTileHigh;
}

/**
 * Generate a w x h tilemap from the 2D value noise. Pure in (masterSeed,
 * originX, originY, w, h, scale): the stream seed is derived ONCE via
 * deriveSeed(masterSeed, "tilemap"), and each cell samples the world-
 * anchored noise coordinate ((originX + col) / scale, (originY + row) /
 * scale). The same dimension validation as loadTilemap (dims > 0 -> else
 * the empty map + the warning discipline) plus the documented cap.
 */
inline Tilemap generateTilemapFromNoise(std::uint64_t masterSeed,
                                        float originX, float originY,
                                        int w, int h, float scale) {
    if (w <= 0 || h <= 0 || w > kNoiseGenMaxDim || h > kNoiseGenMaxDim ||
        !(scale > 0.0f)) {
        std::cerr << "Warning: noise tilemap generation got bad dimensions/scale; using empty map." << std::endl;
        return Tilemap{};
    }
    const std::uint64_t streamSeed = deriveSeed(masterSeed, "tilemap");
    Tilemap map;
    map.width = w;
    map.height = h;
    map.tileSize = 1.0f;
    map.tiles.reserve(static_cast<std::size_t>(w) * static_cast<std::size_t>(h));
    for (int row = 0; row < h; ++row) {
        for (int col = 0; col < w; ++col) {
            // WORLD-ANCHORED noise coordinates (the region-consistency pin:
            // the same origin + an overlapping region must reproduce the
            // same tiles byte-for-byte).
            const float nx = (originX + static_cast<float>(col)) / scale;
            const float ny = (originY + static_cast<float>(row)) / scale;
            const float v = valueNoise2D(streamSeed, nx, ny);
            Tile t;
            t.tileId = tileIdForNoiseValue(v);
            t.solid = (t.tileId != 0);   // the loader semantics (:241)
            t.textureId = t.tileId;      // the loader semantics (:242)
            map.tiles.push_back(t);
        }
    }
    return map;
}

// --- The Step 357 fbm generator constants (named, pinned) ---
// The scale/octaves/lacunarity/gain are the documented fbm defaults (the
// command passes the scale; the generator reads the rest). The thresholds
// are MEASURED (the fbm distribution concentrates near 0.5 - the 352
// thresholds (0.42/0.68) would give a degenerate occupancy; the measured
// percentiles and the band fractions are recorded in V3). STRICT
// less-than: exactly AT a threshold maps to the higher band (the 352
// rule). The band NAMES and TILE IDS are reused from the 352 generator
// EXACTLY (kNoiseGenTileEmpty/Low/High = ids 0/1/2) - no new names or ids.
inline constexpr float kFbmGenScale          = 16.0f;  // the documented default scale
inline constexpr int   kFbmGenOctaves        = 4;      // the default octave count (<= kFbmMaxOctaves)
inline constexpr float kFbmGenLacunarity     = 2.0f;
inline constexpr float kFbmGenGain           = 0.5f;
inline constexpr float kFbmGenThresholdLow   = 0.40f;  // MEASURED: the p25 across the 3 masters = 0.396-0.400
inline constexpr float kFbmGenThresholdHigh  = 0.60f;  // MEASURED: the p75 across the 3 masters = 0.597-0.605
static_assert(kFbmGenOctaves >= 1 && kFbmGenOctaves <= kFbmMaxOctaves,
    "the fbm generator octave count must be within the fbm2D contract");
static_assert(kFbmGenThresholdLow > 0.0f && kFbmGenThresholdLow < kFbmGenThresholdHigh &&
              kFbmGenThresholdHigh < 1.0f,
    "the fbm band thresholds must be ordered inside [0,1)");

// The owned fbm value->tile mapping (the same three band ids as the 352
// generator; its own thresholds because the fbm distribution differs).
// Deliberately NOT a parameterized refactor of tileIdForNoiseValue - the
// 352 ruling: the 352 function body stays untouched.
inline int tileIdForFbmValue(float v) {
    if (v < kFbmGenThresholdLow) {
        return kNoiseGenTileEmpty;
    }
    if (v < kFbmGenThresholdHigh) {
        return kNoiseGenTileLow;
    }
    return kNoiseGenTileHigh;
}

/**
 * Generate a w x h tilemap from the 2D fbm field (Step 357). The SAME
 * dimension validation and world-anchored coordinates as the 352
 * generator; the stream seed is deriveSeed(masterSeed, "tilemap-fbm")
 * (DISTINCT from the 352 "tilemap"); the field is fbm2D at the named
 * fbm constants (kFbmGenOctaves/Lacunarity/Gain). Pure in (masterSeed,
 * originX, originY, w, h, scale).
 */
inline Tilemap generateTilemapFromFbm(std::uint64_t masterSeed,
                                      float originX, float originY,
                                      int w, int h, float scale) {
    if (w <= 0 || h <= 0 || w > kNoiseGenMaxDim || h > kNoiseGenMaxDim ||
        !(scale > 0.0f)) {
        std::cerr << "Warning: fbm tilemap generation got bad dimensions/scale; using empty map." << std::endl;
        return Tilemap{};
    }
    const std::uint64_t streamSeed = deriveSeed(masterSeed, "tilemap-fbm");
    Tilemap map;
    map.width = w;
    map.height = h;
    map.tileSize = 1.0f;
    map.tiles.reserve(static_cast<std::size_t>(w) * static_cast<std::size_t>(h));
    for (int row = 0; row < h; ++row) {
        for (int col = 0; col < w; ++col) {
            // WORLD-ANCHORED fbm coordinates (the region-consistency pin).
            const float fbmNx = (originX + static_cast<float>(col)) / scale;
            const float fbmNy = (originY + static_cast<float>(row)) / scale;
            const float v = fbm2D(streamSeed, fbmNx, fbmNy, kFbmGenOctaves,
                                  kFbmGenLacunarity, kFbmGenGain);
            Tile t;
            t.tileId = tileIdForFbmValue(v);
            t.solid = (t.tileId != 0);   // the loader semantics (:241)
            t.textureId = t.tileId;      // the loader semantics (:242)
            map.tiles.push_back(t);
        }
    }
    return map;
}

} // namespace pe

#endif // PUREENGINE_TILEMAP_GEN_H
