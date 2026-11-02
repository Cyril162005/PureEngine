/**
 * PureEngine - Step 350: seed/RNG definitions (see seed.h for the contract).
 * No std::rand, no <random> distributions, no time sources: the mapping from
 * raw bits to floats is owned here (top-24-bits * 2^-24, provably in [0,1)
 * because the largest produced numerator is 2^24-1 over exactly 2^24).
 */
#include "seed.h"

namespace pe {

namespace {
inline std::uint64_t rotl(const std::uint64_t x, const int k) {
    return (x << k) | (x >> (64 - k));
}
} // namespace

std::uint64_t splitMix64(std::uint64_t& state) {
    std::uint64_t z = (state += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

Rng::Rng(std::uint64_t seed) {
    std::uint64_t sm = seed;
    for (int i = 0; i < 4; ++i) {
        s_[i] = splitMix64(sm);   // the canonical seeding recipe
    }
    if ((s_[0] | s_[1] | s_[2] | s_[3]) == 0ull) {
        // The all-zero state is invalid for the xoshiro family; unreachable
        // through the SplitMix64 expansion but guarded regardless.
        s_[0] = 0x9E3779B97F4A7C15ull;
    }
}

std::uint64_t Rng::nextU64() {
    // xoshiro256** (Blackman/Vigna): result = rotl(s1 * 5, 7) * 9, then the
    // standard xor/rotl state update.
    const std::uint64_t result = rotl(s_[1] * 5, 7) * 9;
    const std::uint64_t t = s_[1] << 17;
    s_[2] ^= s_[0];
    s_[3] ^= s_[1];
    s_[1] ^= s_[2];
    s_[0] ^= s_[3];
    s_[2] ^= t;
    s_[3] = rotl(s_[3], 45);
    return result;
}

std::uint32_t Rng::nextU32() {
    // The HIGH 32 bits of one 64-bit draw (the documented low-word discipline).
    return static_cast<std::uint32_t>(nextU64() >> 32);
}

float Rng::nextFloat01() {
    // Owned integer->float mapping: the top 24 bits, scaled by 2^-24.
    // Max value = (2^24-1) / 2^24 < 1.0 exactly representable -> NEVER 1.0.
    const std::uint32_t bits = static_cast<std::uint32_t>(nextU64() >> 40);
    return static_cast<float>(bits) * (1.0f / 16777216.0f);
}

std::uint64_t deriveSeed(std::uint64_t master, const char* stableId) {
    std::uint64_t h = 1469598103934665603ull;   // FNV-1a 64 offset basis
    if (stableId) {
        for (const char* p = stableId; *p; ++p) {
            h ^= static_cast<unsigned char>(*p);
            h *= 1099511628211ull;              // FNV-1a 64 prime
        }
    }
    // Order-INDEPENDENT by construction: a pure function of (master, stableId)
    // with no shared/static state, so deriving A then B yields exactly the
    // same seeds as B then A. Two SplitMix64 mix rounds give the avalanche.
    std::uint64_t z = master ^ h;
    z = splitMix64(z);
    z ^= h;   // re-fold the id before the second mix
    z = splitMix64(z);
    return z;
}

} // namespace pe
