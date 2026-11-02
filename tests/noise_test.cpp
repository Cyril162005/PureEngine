/**
 * PureEngine - Step 351: the value-noise 2D test (World/content). The six
 * pins, each with its quoted assert string:
 *   1. purity       - "purity: the same (seed,x,y) must produce identical
 *                      bits, and stay identical after a different 1000-point
 *                      sequence is sampled in between"
 *   2. range        - "range: 1e6 samples (lattice edges, negatives, |coords|
 *                      up to 1e5) must all lie in [0,1) and never reach 1.0"
 *   3. negative     - "negative coords: the field must be continuous across
 *                      x=0 and y=0 (|n(-eps)-n(+eps)| small)"; "negative
 *                      coords: n(-0.5,-0.5) must differ from n(0.5,0.5) for a
 *                      fixed seed"; "negative coords: each sampled value must
 *                      lie within the min/max of its four surrounding lattice
 *                      values (the interpolation contract)"
 *   4. exactness    - "lattice exactness: at integer coordinates the noise
 *                      value must equal the raw lattice value bit-exact"
 *   5. independence - "independence: two derived seeds must differ at >= 60
 *                      of 64 sample points"
 *   6. golden       - "golden: the fixed 8 coordinates must reproduce the
 *                      recorded regression constants bit-exact (regression-
 *                      only; no external vector exists)"
 * Utility mode: noise_test --print-vectors prints the golden candidates
 * (used ONCE to record them; ctest never passes the flag).
 */
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>
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

// --- RECORDED REGRESSION GOLDEN (filled once via noise_test --print-vectors;
//     regression-only: no external reference vector exists) ---
const std::uint64_t kGoldenSeed = 0x0000000000003501ull;
const std::uint32_t kGoldenNoise[8] = {
    0x3D298580u, 0x3EFE6107u,
    0x3F221D64u, 0x3DFB5798u,
    0x3ED507A0u, 0x3F2B333Du,
    0x3EBCC6EDu, 0x3EA8A1ABu
};
const float kGoldenX[8] = { 0.0f, 0.5f, -0.5f, 1.25f, -2.5f, 1000.5f, 0.1f, 12345.678f };
const float kGoldenY[8] = { 0.0f, 0.5f, -0.5f, 3.75f, 1.5f, -999.25f, 0.9f, -5432.1f };
} // namespace

int main(int argc, char** argv) {
    if (argc > 1) {
        std::printf("golden regression constants (seed 0x0000000000003501):\n");
        for (int i = 0; i < 8; ++i) {
            const float v = pe::valueNoise2D(kGoldenSeed, kGoldenX[i], kGoldenY[i]);
            std::printf("  %d 0x%08X  (x=%g y=%g)\n", i,
                static_cast<unsigned>(bitsOf(v)),
                static_cast<double>(kGoldenX[i]), static_cast<double>(kGoldenY[i]));
        }
        return 0;
    }

    const std::uint64_t s = 0x3501A5A5A5A53501ull;

    // --- Pin 1: purity (identical bits, order-independent) ---
    {
        const float px = 12.34f;
        const float py = -56.78f;
        const float v1 = pe::valueNoise2D(s, px, py);
        float acc = 0.0f;
        for (int i = 0; i < 1000; ++i) {
            acc += pe::valueNoise2D(s, px + 0.618034f * static_cast<float>(i),
                                    py - 0.375f * static_cast<float>(i));
        }
        const float v2 = pe::valueNoise2D(s, px, py);
        expect(bitsOf(v1) == bitsOf(v2) && acc == acc,
            "purity: the same (seed,x,y) must produce identical bits, and stay identical after a different 1000-point sequence is sampled in between");
    }

    // --- Pin 2: range (1e6 samples: edges, negatives, |coords| up to 1e5) ---
    {
        pe::Rng r(0x3501C0FFEEull);
        long long nSamples = 0;
        bool ok = true;
        for (int i = 0; i < 990000 && ok; ++i) {
            const float x = r.nextFloat01() * 200000.0f - 100000.0f;
            const float y = r.nextFloat01() * 200000.0f - 100000.0f;
            const float v = pe::valueNoise2D(s, x, y);
            ++nSamples;
            if (!(v >= 0.0f && v < 1.0f)) { ok = false; }
        }
        for (int k = -1000; k <= 1000 && ok; ++k) {
            const float kf = static_cast<float>(k);
            const float xs[6] = { kf, kf + 0.5f, kf - 0.5f, kf - 1e-4f, kf + 1e-4f, kf + 0.25f };
            const float ys[6] = { -kf, kf - 0.5f, kf + 0.5f, kf + 1e-4f, kf - 1e-4f, kf - 0.75f };
            for (int j = 0; j < 6 && ok; ++j) {
                const float v = pe::valueNoise2D(s, xs[j], ys[j]);
                ++nSamples;
                if (!(v >= 0.0f && v < 1.0f)) { ok = false; }
            }
        }
        for (int k = -8; k <= 8 && ok; ++k) {
            const float kf = static_cast<float>(k);
            const float offs[4] = { -1e-6f, 1e-6f, -1e-5f, 1e-5f };
            for (int j = 0; j < 4 && ok; ++j) {
                const float v = pe::valueNoise2D(s, kf + offs[j], kf - offs[j]);
                ++nSamples;
                if (!(v >= 0.0f && v < 1.0f)) { ok = false; }
            }
        }
        for (int i = 0; i <= 100 && ok; ++i) {
            for (int j = 0; j <= 100 && ok; ++j) {
                const float x = -1.0f + 0.01f * static_cast<float>(i);
                const float y = -1.0f + 0.01f * static_cast<float>(j);
                const float v = pe::valueNoise2D(s, x, y);
                ++nSamples;
                if (!(v >= 0.0f && v < 1.0f)) { ok = false; }
            }
        }
        expect(ok && nSamples >= 1000000,
            "range: 1e6 samples (lattice edges, negatives, |coords| up to 1e5) must all lie in [0,1) and never reach 1.0");
    }

    // --- Pin 3: negative coords (continuity at 0; the != check; the
    //     interpolation contract on the negative side) ---
    {
        bool cont = true;
        const float offs[5] = { 1e-6f, 1e-5f, 1e-4f, 1e-3f, 1e-2f };
        const float line[5] = { 0.0f, 0.3f, -0.7f, 2.5f, -3.1f };
        for (int a = 0; a < 5 && cont; ++a) {
            for (int e = 0; e < 5 && cont; ++e) {
                const float d = offs[e];
                const float nx = pe::valueNoise2D(s, -d, line[a]);
                const float px = pe::valueNoise2D(s, d, line[a]);
                if (!(std::fabs(nx - px) <= 4.0f * d + 1e-7f)) { cont = false; }
                const float ny = pe::valueNoise2D(s, line[a], -d);
                const float py = pe::valueNoise2D(s, line[a], d);
                if (!(std::fabs(ny - py) <= 4.0f * d + 1e-7f)) { cont = false; }
            }
        }
        expect(cont,
            "negative coords: the field must be continuous across x=0 and y=0 (|n(-eps)-n(+eps)| small)");
        const float nNeg = pe::valueNoise2D(s, -0.5f, -0.5f);
        const float nPos = pe::valueNoise2D(s, 0.5f, 0.5f);
        expect(bitsOf(nNeg) != bitsOf(nPos),
            "negative coords: n(-0.5,-0.5) must differ from n(0.5,0.5) for a fixed seed");
        // The interpolation contract: inside a cell the value must lie within
        // the min/max of the cell's four TRUE (floor-based) lattice corners.
        // The correct implementation is bilinear over those corners (always
        // inside); the truncation bug extrapolates the WRONG cell and leaves
        // the range. 1e-6f tolerance covers float rounding at the bounds.
        bool conv = true;
        const float pts[6] = { -0.5f, -0.25f, -0.75f, -0.1f, -0.9f, -0.45f };
        for (int a = 0; a < 6 && conv; ++a) {
            for (int b = 0; b < 6 && conv; ++b) {
                const float x = pts[a];
                const float y = pts[b];
                const std::int64_t ix = static_cast<std::int64_t>(std::floorf(x));
                const std::int64_t iy = static_cast<std::int64_t>(std::floorf(y));
                const float v = pe::valueNoise2D(s, x, y);
                float mn = pe::latticeValue2D(s, ix, iy);
                float mx = mn;
                const float cs[4] = {
                    pe::latticeValue2D(s, ix, iy), pe::latticeValue2D(s, ix + 1, iy),
                    pe::latticeValue2D(s, ix, iy + 1), pe::latticeValue2D(s, ix + 1, iy + 1)
                };
                for (int c = 0; c < 4; ++c) {
                    if (cs[c] < mn) { mn = cs[c]; }
                    if (cs[c] > mx) { mx = cs[c]; }
                }
                if (!(v >= mn - 1e-6f && v <= mx + 1e-6f)) { conv = false; }
            }
        }
        expect(conv,
            "negative coords: each sampled value must lie within the min/max of its four surrounding lattice values (the interpolation contract)");
    }

    // --- Pin 4: lattice exactness (integer coords == the raw lattice value) ---
    {
        const std::int64_t cs[16] = { 0, 0, 3, 7, -5, 2, 12, -9, 100000, -99999, -1, -1, 7, 0, 0, -3 };
        bool exact = true;
        for (int i = 0; i < 8 && exact; ++i) {
            const float v = pe::valueNoise2D(s, static_cast<float>(cs[2 * i]),
                                                static_cast<float>(cs[2 * i + 1]));
            const float l = pe::latticeValue2D(s, cs[2 * i], cs[2 * i + 1]);
            if (bitsOf(v) != bitsOf(l)) { exact = false; }
        }
        expect(exact,
            "lattice exactness: at integer coordinates the noise value must equal the raw lattice value bit-exact");
    }

    // --- Pin 5: independence (two derived seeds, >= 60 of 64 points) ---
    {
        const std::uint64_t m = 0x3501350135013501ull;
        const std::uint64_t dt = pe::deriveSeed(m, "terrain");
        const std::uint64_t dn = pe::deriveSeed(m, "noise");
        pe::Rng rp(0x3501D0D0D0D03501ull);
        int diff = 0;
        for (int i = 0; i < 64; ++i) {
            const float x = rp.nextFloat01() * 2000.0f - 1000.0f;
            const float y = rp.nextFloat01() * 2000.0f - 1000.0f;
            if (bitsOf(pe::valueNoise2D(dt, x, y)) != bitsOf(pe::valueNoise2D(dn, x, y))) {
                ++diff;
            }
        }
        expect(diff >= 60,
            "independence: two derived seeds must differ at >= 60 of 64 sample points");
    }

    // --- Pin 6: golden (regression-only, bit-exact via the u32 mapping) ---
    {
        bool gold = true;
        for (int i = 0; i < 8 && gold; ++i) {
            const float v = pe::valueNoise2D(kGoldenSeed, kGoldenX[i], kGoldenY[i]);
            if (bitsOf(v) != kGoldenNoise[i]) { gold = false; }
        }
        expect(gold,
            "golden: the fixed 8 coordinates must reproduce the recorded regression constants bit-exact (regression-only; no external vector exists)");
    }

    if (g_fail == 0) { std::printf("noise_test: ALL PASS\n"); }
    else { std::printf("noise_test: %d FAIL\n", g_fail); }
    return g_fail == 0 ? 0 : 1;
}
