/**
 * PureEngine - Step 352: the noise->tilemap generator test (World/content).
 * The pins, each with its quoted assert string:
 *   1. determinism   - "determinism: the same arguments must produce
 *                       byte-identical tiles across two runs"
 *   2. region        - "region consistency: the 8x8 and 16x16 generations
 *                       from the same origin must be byte-equal on the
 *                       overlap"
 *   3. validity      - "validity: all tile ids must lie in the legal
 *                       renderer range [0,4] with the loader solid/texture
 *                       semantics, and bad dimensions (zero, negative, over
 *                       the cap) must yield the empty map"
 *   4. threshold     - "threshold boundary: a noise value exactly at a
 *                       threshold must map to the documented higher band
 *                       (strict less-than)"
 *   5. independence  - "independence: two seeds must differ in >= 1 tile of
 *                       a fixed region"
 *   6. legacy        - NO new assert: the existing tilemap/scene/platformer
 *                       pins stay green unchanged (the full ctest suite is
 *                       the gate; recorded in V3/the tracker).
 * Deterministic only - no window, no GL, no files.
 */
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <vector>
#include "tilemap_gen.h"

static int g_fail = 0;
static void expect(bool ok, const char* name) {
    if (ok) { std::printf("PASS: %s\n", name); }
    else { std::printf("FAIL: %s\n", name); ++g_fail; }
}

namespace {
bool sameTiles(const pe::Tilemap& a, const pe::Tilemap& b) {
    if (a.width != b.width || a.height != b.height) { return false; }
    if (a.tiles.size() != b.tiles.size()) { return false; }
    if (a.tiles.empty()) { return true; }
    return std::memcmp(a.tiles.data(), b.tiles.data(),
                       a.tiles.size() * sizeof(pe::Tile)) == 0;
}
} // namespace

int main(int argc, char** argv) {
    if (argc > 1) {
        // Utility: measure the fbm band distribution at the generator
        // parameters (used once to choose the thresholds; ctest never
        // passes the flag).
        const std::uint64_t masters[3] = { 7ull, 8ull, 0x3561356135613561ull };
        const int R = 1024;
        for (int mi = 0; mi < 3; ++mi) {
            const std::uint64_t streamSeed = pe::deriveSeed(masters[mi], "tilemap-fbm");
            std::vector<float> vals;
            vals.reserve(static_cast<std::size_t>(R) * R);
            for (int row = 0; row < R; ++row) {
                for (int col = 0; col < R; ++col) {
                    const float nx = static_cast<float>(col) / pe::kFbmGenScale;
                    const float ny = static_cast<float>(row) / pe::kFbmGenScale;
                    vals.push_back(pe::fbm2D(streamSeed, nx, ny, pe::kFbmGenOctaves,
                                             pe::kFbmGenLacunarity, pe::kFbmGenGain));
                }
            }
            std::sort(vals.begin(), vals.end());
            double mean = 0.0;
            for (float v : vals) { mean += static_cast<double>(v); }
            mean /= static_cast<double>(vals.size());
            std::printf("master %llu: n=%zu min=%g max=%g mean=%g\n",
                static_cast<unsigned long long>(masters[mi]), vals.size(),
                static_cast<double>(vals.front()), static_cast<double>(vals.back()), mean);
            const float ps[7] = {0.05f, 0.10f, 0.25f, 0.50f, 0.75f, 0.90f, 0.95f};
            for (int pi = 0; pi < 7; ++pi) {
                const std::size_t idx = static_cast<std::size_t>(ps[pi] * static_cast<float>(vals.size() - 1));
                std::printf("  p%02d: %g\n", static_cast<int>(ps[pi] * 100.0f),
                    static_cast<double>(vals[idx]));
            }
            std::size_t cE = 0, cL = 0, cH = 0;
            for (float v : vals) {
                const int id = pe::tileIdForFbmValue(v);
                if (id == pe::kNoiseGenTileEmpty) { ++cE; }
                else if (id == pe::kNoiseGenTileLow) { ++cL; }
                else { ++cH; }
            }
            std::printf("  bands at the current thresholds: EMPTY=%zu (%.1f%%) LOW=%zu (%.1f%%) HIGH=%zu (%.1f%%)\n",
                cE, 100.0 * static_cast<double>(cE) / static_cast<double>(vals.size()),
                cL, 100.0 * static_cast<double>(cL) / static_cast<double>(vals.size()),
                cH, 100.0 * static_cast<double>(cH) / static_cast<double>(vals.size()));
        }
        for (int mi = 0; mi < 3; ++mi) {
            pe::Tilemap g = pe::generateTilemapFromFbm(masters[mi], 0.0f, 0.0f, 64, 64, pe::kFbmGenScale);
            std::size_t cE = 0, cL = 0, cH = 0;
            for (const pe::Tile& t : g.tiles) {
                if (t.tileId == pe::kNoiseGenTileEmpty) { ++cE; }
                else if (t.tileId == pe::kNoiseGenTileLow) { ++cL; }
                else { ++cH; }
            }
            std::printf("64x64 master %llu: EMPTY=%zu LOW=%zu HIGH=%zu of %zu\n",
                static_cast<unsigned long long>(masters[mi]), cE, cL, cH, g.tiles.size());
        }
        {
            pe::Tilemap g4 = pe::generateTilemapFromFbm(7ull, 0.0f, 0.0f, 64, 64, pe::kFbmGenScale);
            const std::uint64_t streamSeed = pe::deriveSeed(7ull, "tilemap-fbm");
            int diff = 0;
            for (int row = 0; row < 64; ++row) {
                for (int col = 0; col < 64; ++col) {
                    const float nx = static_cast<float>(col) / pe::kFbmGenScale;
                    const float ny = static_cast<float>(row) / pe::kFbmGenScale;
                    const float v1 = pe::fbm2D(streamSeed, nx, ny, 1, pe::kFbmGenLacunarity, pe::kFbmGenGain);
                    const pe::Tile& t = g4.tiles[static_cast<std::size_t>(row) * 64 + static_cast<std::size_t>(col)];
                    if (pe::tileIdForFbmValue(v1) != t.tileId) { ++diff; }
                }
            }
            std::printf("detail: the fbm(4-octave) tiles differ from the octaves=1 tiles at %d of 4096 cells\n", diff);
        }
        return 0;
    }
    const std::uint64_t m = 0x3520352035203520ull;
    const float ox = 2.0f;
    const float oy = -3.0f;
    const float scale = 16.0f;

    // --- Pin 1: determinism (two runs, byte-identical) ---
    {
        pe::Tilemap a = pe::generateTilemapFromNoise(m, ox, oy, 16, 16, scale);
        pe::Tilemap b = pe::generateTilemapFromNoise(m, ox, oy, 16, 16, scale);
        expect(sameTiles(a, b),
            "determinism: the same arguments must produce byte-identical tiles across two runs");
    }

    // --- Pin 2: region consistency (the same origin, byte-equal overlap) ---
    {
        pe::Tilemap small = pe::generateTilemapFromNoise(m, ox, oy, 8, 8, scale);
        pe::Tilemap big = pe::generateTilemapFromNoise(m, ox, oy, 16, 16, scale);
        bool overlap = (small.width == 8 && big.width == 16 &&
                        small.tiles.size() == 64 && big.tiles.size() == 256);
        for (int r = 0; r < 8 && overlap; ++r) {
            for (int c = 0; c < 8 && overlap; ++c) {
                const pe::Tile& ts = small.tiles[static_cast<std::size_t>(r) * 8 + c];
                const pe::Tile& tb = big.tiles[static_cast<std::size_t>(r) * 16 + c];
                if (ts.tileId != tb.tileId || ts.solid != tb.solid ||
                    ts.textureId != tb.textureId) {
                    overlap = false;
                }
            }
        }
        expect(overlap,
            "region consistency: the 8x8 and 16x16 generations from the same origin must be byte-equal on the overlap");
    }

    // --- Pin 3: validity (the legal id range + the dimension validation) ---
    {
        pe::Tilemap g = pe::generateTilemapFromNoise(m, ox, oy, 16, 16, scale);
        bool legal = (g.width == 16 && g.height == 16 && g.tiles.size() == 256);
        for (const pe::Tile& t : g.tiles) {
            if (t.tileId < 0 || t.tileId >= pe::TEXTURE_SLOTS) { legal = false; break; }
            if (t.solid != (t.tileId != 0)) { legal = false; break; }
            if (t.textureId != t.tileId) { legal = false; break; }
        }
        pe::Tilemap z = pe::generateTilemapFromNoise(m, ox, oy, 0, 8, scale);
        pe::Tilemap n = pe::generateTilemapFromNoise(m, ox, oy, 8, -1, scale);
        pe::Tilemap cap = pe::generateTilemapFromNoise(m, ox, oy, pe::kNoiseGenMaxDim + 1, 8, scale);
        pe::Tilemap badScale = pe::generateTilemapFromNoise(m, ox, oy, 8, 8, 0.0f);
        const bool dims = (z.width == 0 && z.tiles.empty() &&
                           n.width == 0 && n.tiles.empty() &&
                           cap.width == 0 && cap.tiles.empty() &&
                           badScale.width == 0 && badScale.tiles.empty());
        expect(legal && dims,
            "validity: all tile ids must lie in the legal renderer range [0,4] with the loader solid/texture semantics, and bad dimensions (zero, negative, over the cap) must yield the empty map");
    }

    // --- Pin 4: threshold boundary (exactly AT a threshold -> the higher band) ---
    {
        const bool atA = pe::tileIdForNoiseValue(pe::kNoiseGenThresholdA) == pe::kNoiseGenTileLow;
        const bool atB = pe::tileIdForNoiseValue(pe::kNoiseGenThresholdB) == pe::kNoiseGenTileHigh;
        const bool belowA = pe::tileIdForNoiseValue(pe::kNoiseGenThresholdA - 1e-6f) == pe::kNoiseGenTileEmpty;
        const bool mid = pe::tileIdForNoiseValue(0.5f * (pe::kNoiseGenThresholdA + pe::kNoiseGenThresholdB)) == pe::kNoiseGenTileLow;
        const bool zero = pe::tileIdForNoiseValue(0.0f) == pe::kNoiseGenTileEmpty;
        const bool aboveB = pe::tileIdForNoiseValue(pe::kNoiseGenThresholdB + 1e-6f) == pe::kNoiseGenTileHigh;
        expect(atA && atB && belowA && mid && zero && aboveB,
            "threshold boundary: a noise value exactly at a threshold must map to the documented higher band (strict less-than)");
    }

    // --- Pin 5: independence (two seeds, >= 1 differing tile in a fixed region) ---
    {
        pe::Tilemap a = pe::generateTilemapFromNoise(1ull, ox, oy, 8, 8, scale);
        pe::Tilemap b = pe::generateTilemapFromNoise(2ull, ox, oy, 8, 8, scale);
        int diff = 0;
        if (a.tiles.size() == b.tiles.size()) {
            for (std::size_t i = 0; i < a.tiles.size(); ++i) {
                if (a.tiles[i].tileId != b.tiles[i].tileId) { ++diff; }
            }
        }
        expect(diff >= 1,
            "independence: two seeds must differ in >= 1 tile of a fixed region");
    }

    // --- The Step 357 fbm generator pins ---
    // fbm-1 determinism
    {
        pe::Tilemap fa = pe::generateTilemapFromFbm(m, ox, oy, 16, 16, pe::kFbmGenScale);
        pe::Tilemap fb = pe::generateTilemapFromFbm(m, ox, oy, 16, 16, pe::kFbmGenScale);
        expect(sameTiles(fa, fb),
            "fbm determinism: the same arguments must produce byte-identical tiles across two runs");
    }
    // fbm-2 region consistency
    {
        pe::Tilemap fs = pe::generateTilemapFromFbm(m, ox, oy, 8, 8, pe::kFbmGenScale);
        pe::Tilemap fl = pe::generateTilemapFromFbm(m, ox, oy, 16, 16, pe::kFbmGenScale);
        bool foverlap = (fs.width == 8 && fl.width == 16 &&
                         fs.tiles.size() == 64 && fl.tiles.size() == 256);
        for (int r = 0; r < 8 && foverlap; ++r) {
            for (int c = 0; c < 8 && foverlap; ++c) {
                const pe::Tile& ts = fs.tiles[static_cast<std::size_t>(r) * 8 + c];
                const pe::Tile& tb = fl.tiles[static_cast<std::size_t>(r) * 16 + c];
                if (ts.tileId != tb.tileId || ts.solid != tb.solid ||
                    ts.textureId != tb.textureId) {
                    foverlap = false;
                }
            }
        }
        expect(foverlap,
            "fbm region consistency: the 8x8 and 16x16 generations from the same origin must be byte-equal on the overlap");
    }
    // fbm-3 validity
    {
        pe::Tilemap g = pe::generateTilemapFromFbm(m, ox, oy, 16, 16, pe::kFbmGenScale);
        bool legal = (g.width == 16 && g.height == 16 && g.tiles.size() == 256);
        for (const pe::Tile& t : g.tiles) {
            if (t.tileId < 0 || t.tileId >= pe::TEXTURE_SLOTS) { legal = false; break; }
            if (t.solid != (t.tileId != 0)) { legal = false; break; }
            if (t.textureId != t.tileId) { legal = false; break; }
        }
        pe::Tilemap z = pe::generateTilemapFromFbm(m, ox, oy, 0, 8, pe::kFbmGenScale);
        pe::Tilemap n = pe::generateTilemapFromFbm(m, ox, oy, 8, -1, pe::kFbmGenScale);
        pe::Tilemap cap = pe::generateTilemapFromFbm(m, ox, oy, pe::kNoiseGenMaxDim + 1, 8, pe::kFbmGenScale);
        const bool dims = (z.width == 0 && z.tiles.empty() &&
                           n.width == 0 && n.tiles.empty() &&
                           cap.width == 0 && cap.tiles.empty());
        expect(legal && dims,
            "fbm validity: all tile ids must lie in the legal renderer range [0,4] with the loader solid/texture semantics, and bad dimensions (zero, negative, over the cap) must yield the empty map");
    }
    // fbm-4 threshold boundary (exactly AT a threshold -> the higher band)
    {
        const bool atL = pe::tileIdForFbmValue(pe::kFbmGenThresholdLow) == pe::kNoiseGenTileLow;
        const bool atH = pe::tileIdForFbmValue(pe::kFbmGenThresholdHigh) == pe::kNoiseGenTileHigh;
        const bool belowL = pe::tileIdForFbmValue(pe::kFbmGenThresholdLow - 1e-6f) == pe::kNoiseGenTileEmpty;
        const bool zero = pe::tileIdForFbmValue(0.0f) == pe::kNoiseGenTileEmpty;
        const bool aboveH = pe::tileIdForFbmValue(pe::kFbmGenThresholdHigh + 1e-6f) == pe::kNoiseGenTileHigh;
        expect(atL && atH && belowL && zero && aboveH,
            "fbm threshold boundary: an fbm value exactly at each threshold must map to the documented band (strict less-than)");
    }
    // fbm-5 band occupancy (the fixed seed set over 64x64, every band >= 5%)
    {
        const std::uint64_t seedSet[3] = { 7ull, 8ull, 0x3561356135613561ull };
        bool occ = true;
        for (int i = 0; i < 3 && occ; ++i) {
            pe::Tilemap g = pe::generateTilemapFromFbm(seedSet[i], 0.0f, 0.0f, 64, 64, pe::kFbmGenScale);
            std::size_t cE = 0, cL = 0, cH = 0;
            for (const pe::Tile& t : g.tiles) {
                if (t.tileId == pe::kNoiseGenTileEmpty) { ++cE; }
                else if (t.tileId == pe::kNoiseGenTileLow) { ++cL; }
                else { ++cH; }
            }
            const double total = static_cast<double>(g.tiles.size());
            if (static_cast<double>(cE) / total < 0.05 ||
                static_cast<double>(cL) / total < 0.05 ||
                static_cast<double>(cH) / total < 0.05) {
                occ = false;
            }
        }
        expect(occ,
            "fbm band occupancy: for the fixed seed set over the 64x64 region every band must cover >= 5% of tiles");
    }
    // fbm-6 independence (two seeds; and the fbm tiles vs the same-master
    // 352-STREAM fbm reference - the seed ids "tilemap-fbm" vs "tilemap"
    // must produce distinct fields; mutation (d) collapses them)
    {
        pe::Tilemap a = pe::generateTilemapFromFbm(1ull, ox, oy, 8, 8, pe::kFbmGenScale);
        pe::Tilemap b = pe::generateTilemapFromFbm(2ull, ox, oy, 8, 8, pe::kFbmGenScale);
        int diff = 0;
        if (a.tiles.size() == b.tiles.size()) {
            for (std::size_t i = 0; i < a.tiles.size(); ++i) {
                if (a.tiles[i].tileId != b.tiles[i].tileId) { ++diff; }
            }
        }
        expect(diff >= 1,
            "fbm independence: two seeds must differ in >= 1 tile of a fixed region");
        pe::Tilemap g = pe::generateTilemapFromFbm(m, ox, oy, 8, 8, pe::kFbmGenScale);
        const std::uint64_t s352 = pe::deriveSeed(m, "tilemap");
        int diff2 = 0;
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                const float nx = (ox + static_cast<float>(c)) / pe::kFbmGenScale;
                const float ny = (oy + static_cast<float>(r)) / pe::kFbmGenScale;
                const float v = pe::fbm2D(s352, nx, ny, pe::kFbmGenOctaves,
                                          pe::kFbmGenLacunarity, pe::kFbmGenGain);
                if (pe::tileIdForFbmValue(v) != g.tiles[static_cast<std::size_t>(r) * 8 + static_cast<std::size_t>(c)].tileId) {
                    ++diff2;
                }
            }
        }
        expect(diff2 >= 1,
            "fbm independence: the fbm tilemap must differ from the same-master 352-stream fbm reference in >= 1 tile (the seed ids are distinct)");
    }
    // fbm-7 detail (the fbm tiles vs the octaves=1-equivalent tiles)
    {
        pe::Tilemap g = pe::generateTilemapFromFbm(m, 0.0f, 0.0f, 64, 64, pe::kFbmGenScale);
        const std::uint64_t streamSeed = pe::deriveSeed(m, "tilemap-fbm");
        int diff = 0;
        for (int r = 0; r < 64; ++r) {
            for (int c = 0; c < 64; ++c) {
                const float nx = static_cast<float>(c) / pe::kFbmGenScale;
                const float ny = static_cast<float>(r) / pe::kFbmGenScale;
                const float v1 = pe::fbm2D(streamSeed, nx, ny, 1, pe::kFbmGenLacunarity, pe::kFbmGenGain);
                if (pe::tileIdForFbmValue(v1) != g.tiles[static_cast<std::size_t>(r) * 64 + static_cast<std::size_t>(c)].tileId) {
                    ++diff;
                }
            }
        }
        expect(diff >= 200,
            "fbm detail: the fbm-generated tilemap must differ from the octaves=1-equivalent tilemap over the fixed region (>= 200 of 4096 cells)");
    }
    // --- Pin 6: legacy - NO new assert here. The existing tilemap/scene/
    //     platformer pins (checkTilemapValidLoad, checkTilemapCollidePushesOut,
    //     the scene asserts, checkPlatformerLevels/Landing/LevelSwitch) are
    //     exercised UNCHANGED by the full ctest suite (recorded in V3).

    if (g_fail == 0) { std::printf("tilemap_gen_test: ALL PASS\n"); }
    else { std::printf("tilemap_gen_test: %d FAIL\n", g_fail); }
    return g_fail == 0 ? 0 : 1;
}
