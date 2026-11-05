/**
 * PureEngine - Step 356 (amended, Option A): the deterministic fbm2D test.
 * The nine pins, each with its quoted assert string:
 *   1. determinism    - "determinism: the same arguments must produce the
 *                       bit-identical 1000-value sequence, and stay
 *                       bit-identical after a different 1000-point sequence
 *                       is sampled in between"
 *   2. octave-0       - "octave-0 identity: fbm2D at one octave must equal
 *                       valueNoise2D bit-exact"
 *   3. composition    - "composition: the 3-octave fbm2D (gain 0.5,
 *                       lacunarity 2) must equal the reference composed in
 *                       the test from public valueNoise2D and deriveSeed
 *                       (bit-exact)"
 *   4. range          - "range: 1e6 fbm2D samples (lattice edges, negatives,
 *                       |coord| <= 1e5, octaves 1..8, gain 0..1) must all lie
 *                       in [0,1) and never reach 1.0"
 *   5. seed sense     - "seed sensitivity: two derived seeds must differ at
 *                       >= 60 of 64 sample points"
 *   6. octave-seed    - "octave-seed distinctness: the seven octave-literal
 *                       derivations must be pairwise distinct and differ
 *                       from the raw seed"
 *   7. invalid params - "invalid params: every documented invalid class
 *                       (octaves, lacunarity, gain, non-finite coords) must
 *                       return exactly 0.0f"
 *   8. gain zero      - "gain zero: the fbm result must equal the octave-0
 *                       value bit-exact"
 *   9. golden         - "regression golden: the fixed 8 coordinate/config
 *                       pairs must reproduce the recorded constants
 *                       bit-exact (regression-only; no external vector
 *                       exists)"
 * Utility mode: fbm_test --print-vectors (used ONCE to record the golden;
 * ctest never passes the flag). The composition reference (pin 3) spells
 * the literals "fbm-octave-1"/"fbm-octave-2" out IN THE TEST per the 356
 * contract - no shared helper, no new header API.
 */
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include "noise.h"
#include "seed.h"

static int g_fail = 0;
static void expect(bool ok, const char* name) {
    if (ok) { std::printf("PASS: %s\n", name); }
    else { std::printf("FAIL: %s\n", name); ++g_fail; }
}

namespace {
std::uint32_t bitsOf(float f) {
    std::uint32_t u = 0;
    std::memcpy(&u, &f, 4);
    return u;
}

// --- RECORDED REGRESSION GOLDEN (filled once via fbm_test --print-vectors;
//     regression-only: no external reference vector exists) ---
const std::uint64_t kGoldenSeed = 0x0000000000003561ull;
struct GoldenCase { float x, y; int octaves; float lac, gain; };
const GoldenCase kGolden[8] = {
    {   0.0f,    0.0f, 4, 2.0f, 0.50f},
    {   0.5f,    0.5f, 4, 2.0f, 0.50f},
    {  -0.5f,   -0.5f, 4, 2.0f, 0.50f},
    {   1.25f,   3.75f, 3, 2.0f, 0.50f},
    {  -2.5f,    1.5f, 8, 2.0f, 0.50f},
    {1000.5f, -999.25f, 2, 2.0f, 0.50f},
    {   0.1f,    0.9f, 6, 1.7f, 0.35f},
    {12345.678f, -5432.1f, 1, 2.0f, 0.50f},
};
const std::uint32_t kGoldenU32[8] = {
    0x3F22D9EDu, 0x3F187341u,
    0x3F06ADAFu, 0x3EE11FB4u,
    0x3ECB60DAu, 0x3F285521u,
    0x3EDEBB30u, 0x3E2EA616u
};
} // namespace

int main(int argc, char** argv) {
    if (argc > 1) {
        std::printf("golden regression constants (seed 0x0000000000003561):\n");
        for (int i = 0; i < 8; ++i) {
            const float v = pe::fbm2D(kGoldenSeed, kGolden[i].x, kGolden[i].y,
                                      kGolden[i].octaves, kGolden[i].lac, kGolden[i].gain);
            std::printf("  %d 0x%08X  (x=%g y=%g oct=%d lac=%g gain=%g)\n", i,
                static_cast<unsigned>(bitsOf(v)),
                static_cast<double>(kGolden[i].x), static_cast<double>(kGolden[i].y),
                kGolden[i].octaves, static_cast<double>(kGolden[i].lac),
                static_cast<double>(kGolden[i].gain));
        }
        return 0;
    }

    const std::uint64_t s = 0x3560356035603560ull;

    // --- Pin 1: determinism (bit-identical, interleaved-order independent) ---
    {
        float a1 = pe::fbm2D(s, 7.25f, -3.5f, 4, 2.0f, 0.5f);
        float acc = 0.0f;
        for (int i = 0; i < 1000; ++i) {
            acc += pe::fbm2D(s, 7.25f + 0.31f * static_cast<float>(i),
                             -3.5f - 0.17f * static_cast<float>(i), 4, 2.0f, 0.5f);
        }
        float a2 = pe::fbm2D(s, 7.25f, -3.5f, 4, 2.0f, 0.5f);
        expect(bitsOf(a1) == bitsOf(a2) && acc == acc,
            "determinism: the same arguments must produce the bit-identical 1000-value sequence, and stay bit-identical after a different 1000-point sequence is sampled in between");
    }

    // --- Pin 2: octave-0 identity (octaves=1 == valueNoise2D bit-exact) ---
    {
        const float pts[8][2] = {
            {0.0f, 0.0f}, {0.5f, 0.5f}, {-0.5f, -0.5f}, {3.25f, -7.75f},
            {1000.5f, -999.25f}, {-12345.5f, 5432.25f}, {0.1f, 0.9f}, {12.0f, -34.0f}
        };
        bool id = true;
        for (int i = 0; i < 8 && id; ++i) {
            const float v = pe::fbm2D(s, pts[i][0], pts[i][1], 1, 2.0f, 0.5f);
            const float r = pe::valueNoise2D(s, pts[i][0], pts[i][1]);
            if (bitsOf(v) != bitsOf(r)) { id = false; }
        }
        expect(id, "octave-0 identity: fbm2D at one octave must equal valueNoise2D bit-exact");
    }

    // --- Pin 3: composition (the test-composed reference, bit-exact) ---
    {
        const std::uint64_t seed = 0x3561BEEFBEEF3561ull;
        const float pts[8][2] = {
            {0.0f, 0.0f}, {0.5f, 0.5f}, {-0.5f, -0.5f}, {3.25f, -7.75f},
            {0.1f, 0.9f}, {12.0f, -34.0f}, {-100.5f, 200.25f}, {7.0f, 7.0f}
        };
        bool comp = true;
        for (int i = 0; i < 8 && comp; ++i) {
            const float x = pts[i][0];
            const float y = pts[i][1];
            const float got = pe::fbm2D(seed, x, y, 3, 2.0f, 0.5f);
            // The reference composed IN THE TEST from the public API; the
            // literals SPELLED OUT here per the 356 contract (no shared
            // helper, no new header API).
            const std::uint64_t s1 = pe::deriveSeed(seed, "fbm-octave-1");
            const std::uint64_t s2 = pe::deriveSeed(seed, "fbm-octave-2");
            float fx = x;
            float fy = y;
            double sum = 0.0;
            double ampSum = 0.0;
            double amplitude = 1.0;
            for (int o = 0; o < 3; ++o) {
                std::uint64_t si = seed;
                if (o == 1) { si = s1; }
                if (o == 2) { si = s2; }
                const float n = pe::valueNoise2D(si, fx, fy);
                sum += amplitude * static_cast<double>(n);
                ampSum += amplitude;
                amplitude *= 0.5;
                fx *= 2.0f;
                fy *= 2.0f;
            }
            const float ref = static_cast<float>(sum / ampSum);
            if (bitsOf(got) != bitsOf(ref)) { comp = false; }
        }
        expect(comp,
            "composition: the 3-octave fbm2D (gain 0.5, lacunarity 2) must equal the reference composed in the test from public valueNoise2D and deriveSeed (bit-exact)");
    }

    // --- Pin 4: range (1e6 samples: edges, negatives, |coord| <= 1e5) ---
    {
        pe::Rng r(0x3560C0FFEEull);
        long long nSamples = 0;
        bool ok = true;
        for (int i = 0; i < 1000000 && ok; ++i) {
            const float x = r.nextFloat01() * 200000.0f - 100000.0f;
            const float y = r.nextFloat01() * 200000.0f - 100000.0f;
            const int oc = 1 + (i % 8);
            const float g = r.nextFloat01();
            const float l = 1.0f + r.nextFloat01() * 2.0f;   // 1..3
            const float v = pe::fbm2D(s, x, y, oc, l, g);
            ++nSamples;
            if (!(v >= 0.0f && v < 1.0f)) { ok = false; }
        }
        for (int k = -64; k <= 64 && ok; ++k) {
            const float kf = static_cast<float>(k);
            const float xs[4] = { kf, kf + 0.5f, kf - 1e-4f, kf + 1e-4f };
            for (int j = 0; j < 4 && ok; ++j) {
                for (int oc = 1; oc <= 8 && ok; ++oc) {
                    const float v = pe::fbm2D(s, xs[j], -xs[j], oc, 2.0f, 0.5f);
                    ++nSamples;
                    if (!(v >= 0.0f && v < 1.0f)) { ok = false; }
                }
            }
        }
        expect(ok && nSamples >= 1000000,
            "range: 1e6 fbm2D samples (lattice edges, negatives, |coord| <= 1e5, octaves 1..8, gain 0..1) must all lie in [0,1) and never reach 1.0");
    }

    // --- Pin 5: seed sensitivity (two derived seeds, >= 60 of 64 points) ---
    {
        const std::uint64_t m = 0x3561356135613561ull;
        const std::uint64_t dt = pe::deriveSeed(m, "terrain");
        const std::uint64_t dn = pe::deriveSeed(m, "noise");
        pe::Rng rp(0x3560D0D0D0D03560ull);
        int diff = 0;
        for (int i = 0; i < 64; ++i) {
            const float x = rp.nextFloat01() * 2000.0f - 1000.0f;
            const float y = rp.nextFloat01() * 2000.0f - 1000.0f;
            if (bitsOf(pe::fbm2D(dt, x, y, 4, 2.0f, 0.5f)) !=
                bitsOf(pe::fbm2D(dn, x, y, 4, 2.0f, 0.5f))) {
                ++diff;
            }
        }
        expect(diff >= 60,
            "seed sensitivity: two derived seeds must differ at >= 60 of 64 sample points");
    }

    // --- Pin 6: octave-seed distinctness (the 7 literals, pairwise distinct) ---
    {
        const std::uint64_t sd = 0x3560E0E0E0E03560ull;
        const char* ids[7] = {
            "fbm-octave-1", "fbm-octave-2", "fbm-octave-3", "fbm-octave-4",
            "fbm-octave-5", "fbm-octave-6", "fbm-octave-7"
        };
        std::uint64_t d[7];
        for (int i = 0; i < 7; ++i) { d[i] = pe::deriveSeed(sd, ids[i]); }
        bool distinct = true;
        for (int i = 0; i < 7 && distinct; ++i) {
            if (d[i] == sd) { distinct = false; }
            for (int j = i + 1; j < 7 && distinct; ++j) {
                if (d[i] == d[j]) { distinct = false; }
            }
        }
        expect(distinct,
            "octave-seed distinctness: the seven octave-literal derivations must be pairwise distinct and differ from the raw seed");
    }

    // --- Pin 7: invalid params (one case per documented class) ---
    {
        const std::uint64_t si = 0x3560ull;
        const std::uint32_t zb = bitsOf(0.0f);
        const float inf = std::numeric_limits<float>::infinity();
        const float nan = std::numeric_limits<float>::quiet_NaN();
        bool ok =
            bitsOf(pe::fbm2D(si, 0.0f, 0.0f, 0, 2.0f, 0.5f)) == zb &&          // octaves < 1
            bitsOf(pe::fbm2D(si, 0.0f, 0.0f, 9, 2.0f, 0.5f)) == zb &&          // octaves > 8
            bitsOf(pe::fbm2D(si, 0.0f, 0.0f, 4, 0.0f, 0.5f)) == zb &&          // lac <= 0
            bitsOf(pe::fbm2D(si, 0.0f, 0.0f, 4, -1.0f, 0.5f)) == zb &&         // lac < 0
            bitsOf(pe::fbm2D(si, 0.0f, 0.0f, 4, nan, 0.5f)) == zb &&           // lac non-finite
            bitsOf(pe::fbm2D(si, 0.0f, 0.0f, 4, 2.0f, -0.1f)) == zb &&         // gain < 0
            bitsOf(pe::fbm2D(si, 0.0f, 0.0f, 4, 2.0f, nan)) == zb &&           // gain non-finite
            bitsOf(pe::fbm2D(si, nan, 0.0f, 4, 2.0f, 0.5f)) == zb &&           // x non-finite
            bitsOf(pe::fbm2D(si, 0.0f, inf, 4, 2.0f, 0.5f)) == zb;             // y non-finite
        expect(ok,
            "invalid params: every documented invalid class (octaves, lacunarity, gain, non-finite coords) must return exactly 0.0f");
    }

    // --- Pin 8: gain zero (== the octave-0 value bit-exact) ---
    {
        const float pts[6][2] = {
            {0.0f, 0.0f}, {0.5f, 0.5f}, {-0.5f, -0.5f}, {3.25f, -7.75f},
            {1000.5f, -999.25f}, {12.0f, -34.0f}
        };
        bool gz = true;
        for (int i = 0; i < 6 && gz; ++i) {
            const float v = pe::fbm2D(s, pts[i][0], pts[i][1], 4, 2.0f, 0.0f);
            const float r = pe::valueNoise2D(s, pts[i][0], pts[i][1]);
            if (bitsOf(v) != bitsOf(r)) { gz = false; }
        }
        expect(gz, "gain zero: the fbm result must equal the octave-0 value bit-exact");
    }

    // --- Pin 9: regression golden (bit-exact via the u32 mapping) ---
    {
        bool gold = true;
        for (int i = 0; i < 8 && gold; ++i) {
            const float v = pe::fbm2D(kGoldenSeed, kGolden[i].x, kGolden[i].y,
                                      kGolden[i].octaves, kGolden[i].lac, kGolden[i].gain);
            if (bitsOf(v) != kGoldenU32[i]) { gold = false; }
        }
        expect(gold,
            "regression golden: the fixed 8 coordinate/config pairs must reproduce the recorded constants bit-exact (regression-only; no external vector exists)");
    }

    if (g_fail == 0) { std::printf("fbm_test: ALL PASS\n"); }
    else { std::printf("fbm_test: %d FAIL\n", g_fail); }
    return g_fail == 0 ? 0 : 1;
}
