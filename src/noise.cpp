/**
 * PureEngine - Step 351: value-noise 2D definitions (see noise.h for the
 * contract). Everything routes through the seed.cpp SplitMix64 mixer and the
 * owned top-24-bit mapping; no std::rand, no <random>, no state of any kind.
 */
#include "noise.h"

#include <cmath>

namespace pe {

namespace {

inline float fade(float t) {
    // The quintic smoothstep 6t^5 - 15t^4 + 10t^3 (C2-continuous; exactly 0
    // at t=0 and exactly 1 at t=1 in float, which the lattice-exactness pin
    // relies on).
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

inline float lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

inline float toUnit01(std::uint64_t h) {
    // The OWNED top-24-bit mapping (the same contract as Rng::nextFloat01):
    // max value = (2^24-1)/2^24 < 1.0, exactly representable -> never 1.0.
    const std::uint32_t bits = static_cast<std::uint32_t>(h >> 40);
    return static_cast<float>(bits) * (1.0f / 16777216.0f);
}

} // namespace

std::uint64_t latticeHash2D(std::uint64_t seed, std::int64_t ix, std::int64_t iy) {
    // The existing seed.cpp mixer (SplitMix64) folded per coordinate; a pure
    // function of (seed, ix, iy) with NO static/state storage (the purity pin
    // fails any stateful mutation by name).
    std::uint64_t x = static_cast<std::uint64_t>(ix);
    std::uint64_t y = static_cast<std::uint64_t>(iy);
    std::uint64_t mx = splitMix64(x);
    std::uint64_t z = seed ^ mx;
    z = splitMix64(z);
    std::uint64_t my = splitMix64(y);
    z ^= my;
    z = splitMix64(z);
    return z;
}

float latticeValue2D(std::uint64_t seed, std::int64_t ix, std::int64_t iy) {
    return toUnit01(latticeHash2D(seed, ix, iy));
}

float valueNoise2D(std::uint64_t seed, float x, float y) {
    // floorf, NEVER int truncation: truncation maps the (-1,0) band onto the
    // wrong cell and extrapolates there (the negative-coords pin's target).
    const std::int64_t ix = static_cast<std::int64_t>(std::floorf(x));
    const std::int64_t iy = static_cast<std::int64_t>(std::floorf(y));
    const float tx = x - static_cast<float>(ix);
    const float ty = y - static_cast<float>(iy);
    const float fx = fade(tx);
    const float fy = fade(ty);
    const float v00 = latticeValue2D(seed, ix,     iy);
    const float v10 = latticeValue2D(seed, ix + 1, iy);
    const float v01 = latticeValue2D(seed, ix,     iy + 1);
    const float v11 = latticeValue2D(seed, ix + 1, iy + 1);
    // Bilinear: each lerp with t in [0,1] stays within its pair's [min,max]
    // (a rounded real <= a representable max cannot exceed it), so the result
    // is GUARANTEED in [0,1) - no clamp.
    return lerpf(lerpf(v00, v10, fx), lerpf(v01, v11, fx), fy);
}

} // namespace pe
