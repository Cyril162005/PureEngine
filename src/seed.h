/**
 * PureEngine - Step 350: the seed/RNG foundation (Core Foundation, ADDITIVE).
 * File: seed.h (declarations) + seed.cpp (definitions) - the .h/.cpp split per
 * the architecture objective. The 350 scope ruling: no accepted file edited
 * except CMake registration and the test harness; Core stays ACCEPTED; the
 * freeze rule is NOT reopened.
 *
 * WHAT THIS IS (the smallest API the 350 prompt scopes):
 *  - splitMix64(state): one SplitMix64 step (Steele/Lea/Ettinger, public
 *    domain; the canonical mixer). State 0's first 8 outputs are pinned
 *    against the PUBLISHED reference vector (fetched 2026-11-01:
 *    hawkynt.github.io/Cipher docs, "Vector 1 - Seed 0: First 8 outputs
 *    (verified against reference C implementation)" citing
 *    prng.di.unimi.it/splitmix64.c; the three constants were verified
 *    byte-for-byte in the fetched reference source).
 *  - Rng: the named stream is xoshiro256** (Blackman/Vigna 2019), seeded via
 *    the canonical SplitMix64 expansion (4 draws). FIXED-seed only per the
 *    350 scope: no time/random seed modes yet (fixed + derived only).
 *  - nextU64 / nextU32 / nextFloat01: nextU32 takes the HIGH 32 bits of one
 *    64-bit draw (the low-word discipline); nextFloat01 is an OWNED
 *    integer->float mapping (top 24 bits * 2^-24): always in [0,1), never
 *    1.0, no std::*_distribution anywhere, no std::rand.
 *  - deriveSeed(master, stableId): order-INDEPENDENT derivation - a pure
 *    function of (master, stableId) with no shared state, so deriving A
 *    then B yields exactly the same seeds as B then A.
 *
 * WHAT THIS IS NOT (the recorded limits):
 *  - UNWIRED: zero production callers (no game/main/editor includes
 *    seed.h); the only consumer is tests/seed_test.cpp (the 350 pins).
 *  - particles.h still uses std::rand (the 350 prompt: do NOT touch it).
 *  - No noise/Perlin, no terrain/world-gen, no time/random seed modes.
 */
#ifndef PUREENGINE_SEED_H
#define PUREENGINE_SEED_H

#include <cstdint>

namespace pe {

/** One SplitMix64 step: state += GAMMA, then the 3-round mix. Mutates the
 *  caller's state and returns the output. The canonical seeding/mixing step. */
std::uint64_t splitMix64(std::uint64_t& state);

/** The named stream: xoshiro256** seeded via the SplitMix64 expansion. */
class Rng {
public:
    explicit Rng(std::uint64_t seed);
    std::uint64_t nextU64();
    std::uint32_t nextU32();
    float nextFloat01();
private:
    std::uint64_t s_[4];
};

/** Order-independent stream derivation from a master seed + a stable id
 *  (the id must be a stable compile-time literal; FNV-1a-64 over its bytes). */
std::uint64_t deriveSeed(std::uint64_t master, const char* stableId);

} // namespace pe

#endif // PUREENGINE_SEED_H
