/**
 * PureEngine - Step 351: value-noise 2D (World/content, category 2, ADDITIVE).
 * File: noise.h (declarations) + noise.cpp (definitions). Per the 351 scope:
 * noise.h includes seed.h ONLY among engine headers (<cstdint> is the system
 * exception needed for the fixed-width API). No accepted file edited except
 * the CMake registration and the test harness.
 *
 * WHAT THIS IS:
 *  - valueNoise2D(seed, x, y): PURE 2D value noise - no Rng&, no state, no
 *    globals; the same (seed,x,y) always yields the same bits, independent of
 *    the sampling history (pinned).
 *  - The lattice value = hash(seed, floor(x), floor(y)) built ONLY from the
 *    existing seed.cpp mixer (SplitMix64), mapped with the OWNED top-24-bit
 *    mapping (identical contract to Rng::nextFloat01) -> [0,1).
 *  - The fade is the quintic smoothstep (6t^5-15t^4+10t^3, C2 at the cell
 *    edges); the interpolation is bilinear; the cell is chosen with floorf,
 *    NEVER int truncation (the classic negative-coordinate bug).
 *  - The result is GUARANTEED < 1.0 by construction: every corner value is in
 *    [0,1) and bilinear interpolation with weights in [0,1] cannot leave the
 *    corner min/max (float rounding cannot exceed a representable max) - no
 *    clamp, nothing to hide behind.
 *  - latticeValue2D: the raw mapped corner primitive (exposed because the
 *    lattice-exactness pin and future terrain work must observe it directly;
 *    duplicating the hash inside the test would violate single-source).
 *
 * WHAT THIS IS NOT (the recorded limits):
 *  - UNWIRED: zero production callers; the only consumer is tests/noise_test.cpp.
 *  - No fbm/octaves, no gradient/Perlin noise, no 3D (later steps if opened).
 *  - The golden constants are REGRESSION-ONLY (recorded from this
 *    implementation; no external reference vector exists for this composition).
 */
#ifndef PUREENGINE_NOISE_H
#define PUREENGINE_NOISE_H

#include <cstdint>

#include "seed.h"

namespace pe {

/** The raw mapped lattice corner value in [0,1) for the integer cell (ix,iy). */
float latticeValue2D(std::uint64_t seed, std::int64_t ix, std::int64_t iy);

/** PURE 2D value noise in [0,1): quintic fade + bilinear over the SplitMix64
 *  lattice. floorf cell selection (never truncation). Pure: no state. */
float valueNoise2D(std::uint64_t seed, float x, float y);

} // namespace pe

#endif // PUREENGINE_NOISE_H
