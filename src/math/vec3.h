/**
 * PureEngine Math Layer — Step 5
 * File: vec3.h
 *
 * A minimal 3D vector: the basic building block of all engine math.
 * Positions, directions, velocities, and scales are all Vec3s.
 *
 * Design decisions, stated plainly:
 *  - Header-only: every function is defined inline in this header, so no
 *    vec3.cpp exists and CMakeLists.txt needs no changes. The compiler
 *    pastes this file into every translation unit that includes it.
 *  - Plain struct, public members: math types are pure data. Hiding x/y/z
 *    behind getters would be ceremony, not safety.
 *  - Free-standing operators: `a + b` reads like math. Member functions are
 *    kept for operations with no natural operator symbol (dot, cross).
 *  - float precision: OpenGL shaders speak float; matching avoids silent
 *    double->float truncation when we upload math to the GPU.
 *  - constexpr wherever possible: pure-arithmetic functions are marked
 *    constexpr so the compiler can evaluate them at COMPILE time — which is
 *    what lets main() prove the math with static_assert. Only functions that
 *    call non-constexpr library code (none here) must stay runtime-only.
 */
#ifndef PUREENGINE_MATH_VEC3_H
#define PUREENGINE_MATH_VEC3_H
// Include guard: if this header is ever included twice in one translation
// unit (directly and transitively), the preprocessor skips the second copy
// instead of redefining the struct and breaking the compile.

// <cmath> provides std::sqrt, used by length() below.
#include <cmath>

namespace pe {
// pe = PureEngine. A namespace keeps our names (Vec3, Mat4) from colliding
// with any library we might add later that defines its own Vec3.

struct Vec3 {
    float x, y, z;  // The three components. Public by design.

    // Default constructor: the zero vector. Value-initializing every member
    // in the initializer list means an accidental "Vec3 v;" can never hold
    // uninitialized garbage — a class of bug that is invisible until it isn't.
    // constexpr: usable in compile-time evaluation (static_assert).
    constexpr Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

    // Component constructor. Explicit on purpose: it forbids the compiler
    // from silently inventing a Vec3 from a lone float, which is almost
    // always a bug when it happens.
    constexpr explicit Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Dot product: x1*x2 + y1*y2 + z1*z3. Geometrically it measures how
    // aligned two vectors are (positive = same-ish direction, zero =
    // perpendicular, negative = opposed). Future lighting and collision
    // code will lean on it constantly.
    constexpr float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product: returns a vector perpendicular to both inputs,
    // following the right-hand rule. This is THE tool for building normals
    // and camera axes later. The three lines are the cofactor expansion of
    // the formal determinant — pure formula, nothing clever.
    constexpr Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Squared length. Cheaper than length() (no square root) — whenever you
    // only need to COMPARE distances, compare squared distances instead.
    // constexpr: pure arithmetic, no sqrt involved (unlike length(), whose
    // std::sqrt call is not constexpr on MSVC).
    constexpr float lengthSquared() const {
        return x * x + y * y + z * z;
    }

    // Euclidean length: sqrt(x^2 + y^2 + z^2), straight from Pythagoras
    // extended to three dimensions.
    float length() const {
        return std::sqrt(lengthSquared());
    }

    // Returns a vector pointing the same way but with length exactly 1.
    // Unit vectors ("directions") are what shaders and physics expect.
    // Dividing by a near-zero length would produce garbage, so the zero
    // vector maps to itself instead of NaN.
    Vec3 normalized() const {
        float len = length();
        if (len <= 0.0f) {
            return Vec3();
        }
        return Vec3(x / len, y / len, z / len);
    }

    // Component-wise addition/subtraction: the usual vector arithmetic.
    constexpr Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    constexpr Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    // Scalar multiplication: stretch or shrink every component equally.
    // (velocity * deltaTime lives right here, in this operator.)
    constexpr Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    // Equality — needed so the static_assert in main() can verify the math
    // at compile time.
    constexpr bool operator==(const Vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    constexpr bool operator!=(const Vec3& other) const {
        return !(*this == other);
    }
};

} // namespace pe

#endif // PUREENGINE_MATH_VEC3_H
