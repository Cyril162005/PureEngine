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

int main() {
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

    // --- Pin 6: legacy - NO new assert here. The existing tilemap/scene/
    //     platformer pins (checkTilemapValidLoad, checkTilemapCollidePushesOut,
    //     the scene asserts, checkPlatformerLevels/Landing/LevelSwitch) are
    //     exercised UNCHANGED by the full ctest suite (recorded in V3).

    if (g_fail == 0) { std::printf("tilemap_gen_test: ALL PASS\n"); }
    else { std::printf("tilemap_gen_test: %d FAIL\n", g_fail); }
    return g_fail == 0 ? 0 : 1;
}
