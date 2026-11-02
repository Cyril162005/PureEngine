/**
 * PureEngine - Step 350: the seed/RNG foundation test (Core Foundation).
 * Deterministic value asserts ONLY - no timing, no GL, no window, no files.
 * The five 350 pins, each with its quoted assert string:
 *   1. golden       - "golden: SplitMix64 state 0 must match the published
 *                      reference vector" / "golden: the first 8 xoshiro256**
 *                      outputs must equal the recorded constants"
 *                      (SplitMix64 state 0 was cross-checked against the
 *                      PUBLISHED reference vector BEFORE recording; fetched
 *                      2026-11-01 from the Cipher docs page "Vector 1 -
 *                      Seed 0: First 8 outputs (verified against reference C
 *                      implementation)" citing prng.di.unimi.it/splitmix64.c;
 *                      the seed-42 Rosetta Code vector pins a second state).
 *   2. determinism  - "determinism: the same seed must produce the identical
 *                      1000-value sequence"
 *   3. order        - "derivation: the derived seeds must not depend on the
 *                      derivation order"
 *   4. independence - "independence: the derived streams must differ from
 *                      the master and each other, and their first 64 outputs
 *                      must differ"
 *   5. range        - "range: 1e6 nextFloat01 values must all lie in [0,1)
 *                      and never reach 1.0"
 * Utility mode: seed_test --print-vectors prints the golden candidates (used
 * ONCE to record them; ctest never passes the flag).
 */
#include <cstdio>
#include <cstdint>
#include "seed.h"

static int g_fail = 0;
static void expect(bool ok, const char* name) {
    if (ok) { std::printf("PASS: %s\n", name); }
    else { std::printf("FAIL: %s\n", name); ++g_fail; }
}

namespace {

// --- PUBLISHED REFERENCE VECTOR (do not edit; the golden cross-check) ---
// SplitMix64, state 0, first 8 outputs. Fetched 2026-11-01:
// hawkynt.github.io/Cipher/docs/algorithms/random-number-generators/
// splitmix64.html "Vector 1 - Seed 0: First 8 outputs (verified against
// reference C implementation)" citing prng.di.unimi.it/splitmix64.c.
const std::uint64_t kPublishedState0[8] = {
    0xE220A8397B1DCDAFull, 0x6E789E6AA1B965F4ull,
    0x06C45D188009454Full, 0xF88BB8A8724C81ECull,
    0x1B39896A51A8749Bull, 0x53CB9F0C747EA2EAull,
    0x2C829ABE1F4532E1ull, 0xC584133AC916AB3Cull
};
// SplitMix64, seed 42, first 5 outputs (the Rosetta Code vector, same page).
const std::uint64_t kPublishedSeed42[5] = {
    0xBDD732262FEB6E95ull, 0x28EFE333B266F103ull,
    0x47526757130F9F52ull, 0x581CE1FF0E4AE394ull,
    0x09BC585A244823F2ull
};
// --- RECORDED GOLDEN CONSTANTS (filled once via seed_test --print-vectors) ---
const std::uint64_t kGoldenRng0[8] = {
    0x99EC5F36CB75F2B4ull, 0xBF6E1F784956452Aull,
    0x1A5F849D4933E6E0ull, 0x6AA594F1262D2D2Cull,
    0xBBA5AD4A1F842E59ull, 0xFFEF8375D9EBCACAull,
    0x6C160DEED2F54C98ull, 0x8920AD648FC30A3Full
};

} // namespace

int main(int argc, char** argv) {
    if (argc > 1) {
        // Utility mode: print the golden candidates, then exit.
        std::uint64_t st = 0ull;
        std::printf("splitmix64 state0 (the published cross-check):\n");
        for (int i = 0; i < 8; ++i) {
            std::printf("  %d 0x%016llX\n", i,
                static_cast<unsigned long long>(pe::splitMix64(st)));
        }
        pe::Rng r(0ull);
        std::printf("Rng(0) first 8 nextU64 (record as kGoldenRng0):\n");
        for (int i = 0; i < 8; ++i) {
            std::printf("  %d 0x%016llX\n", i,
                static_cast<unsigned long long>(r.nextU64()));
        }
        return 0;
    }

    // --- Pin 1: golden (the published cross-check + the recorded constants) ---
    {
        std::uint64_t st = 0ull;
        bool pub0 = true;
        for (int i = 0; i < 8; ++i) {
            if (pe::splitMix64(st) != kPublishedState0[i]) { pub0 = false; break; }
        }
        expect(pub0, "golden: SplitMix64 state 0 must match the published reference vector");
        std::uint64_t st42 = 42ull;
        bool pub42 = true;
        for (int i = 0; i < 5; ++i) {
            if (pe::splitMix64(st42) != kPublishedSeed42[i]) { pub42 = false; break; }
        }
        expect(pub42, "golden: SplitMix64 seed 42 must match the published Rosetta Code vector");
        pe::Rng r(0ull);
        bool gold = true;
        for (int i = 0; i < 8; ++i) {
            if (r.nextU64() != kGoldenRng0[i]) { gold = false; break; }
        }
        expect(gold, "golden: the first 8 xoshiro256** outputs must equal the recorded constants");
    }

    // --- Pin 2: determinism (1000 values across two instances) ---
    {
        const std::uint64_t seed = 0x0123456789ABCDEFull;
        pe::Rng a(seed);
        pe::Rng b(seed);
        bool det = true;
        for (int i = 0; i < 1000; ++i) {
            if (a.nextU64() != b.nextU64()) { det = false; break; }
        }
        expect(det, "determinism: the same seed must produce the identical 1000-value sequence");
    }

    // --- Pin 3: derivation order-independence ---
    {
        const std::uint64_t m = 0xDEADBEEFCAFEF00Dull;
        const std::uint64_t a1 = pe::deriveSeed(m, "terrain");
        const std::uint64_t b1 = pe::deriveSeed(m, "noise");
        const std::uint64_t b2 = pe::deriveSeed(m, "noise");
        const std::uint64_t a2 = pe::deriveSeed(m, "terrain");
        expect(a1 == a2 && b1 == b2,
            "derivation: the derived seeds must not depend on the derivation order");
    }

    // --- Pin 4: independence (seeds differ; the first 64 outputs differ) ---
    {
        const std::uint64_t m = 0xDEADBEEFCAFEF00Dull;
        const std::uint64_t dt = pe::deriveSeed(m, "terrain");
        const std::uint64_t dn = pe::deriveSeed(m, "noise");
        bool ind = (dt != dn) && (dt != m) && (dn != m);
        pe::Rng rt(dt);
        pe::Rng rn(dn);
        for (int i = 0; i < 64 && ind; ++i) {
            if (rt.nextU64() == rn.nextU64()) { ind = false; }
        }
        expect(ind,
            "independence: the derived streams must differ from the master and each other, and their first 64 outputs must differ");
    }

    // --- Pin 5: range (1e6 values, all in [0,1), never 1.0) ---
    {
        pe::Rng r(0xC0FFEEull);
        bool ok = true;
        for (int i = 0; i < 1000000; ++i) {
            const float v = r.nextFloat01();
            if (!(v >= 0.0f && v < 1.0f)) { ok = false; break; }
        }
        expect(ok, "range: 1e6 nextFloat01 values must all lie in [0,1) and never reach 1.0");
    }

    if (g_fail == 0) { std::printf("seed_test: ALL PASS\n"); }
    else { std::printf("seed_test: %d FAIL\n", g_fail); }
    return g_fail == 0 ? 0 : 1;
}
