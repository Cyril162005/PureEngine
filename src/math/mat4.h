/**
 * PureEngine Math Layer — Step 5
 * File: mat4.h
 *
 * A 4x4 matrix: the single tool that encodes translation, rotation, and
 * scale — everything needed to place geometry in the world. The vertex
 * shader multiplies every vertex by one of these.
 *
 * WHY 4x4 and not 3x3? Rotation and scale fit in 3x3, but translation does
 * NOT (adding an offset is not a linear operation). Homogeneous coordinates
 * fix this: we write a 3D point as (x, y, z, 1) and hide the translation in
 * the 4th column. Then translation becomes a matrix multiplication too, and
 * any combination of translate/rotate/scale collapses into ONE matrix.
 *
 * STORAGE CONVENTION — the one decision that bites people:
 * m[col][row] — the FIRST index is the COLUMN. This is "column-major"
 * order, chosen deliberately:
 *   1. OpenGL expects column-major data. glUniformMatrix4fv can upload our
 *      16 floats with transpose = GL_FALSE — zero conversion.
 *   2. GLSL's "mat4 * vec4" treats the vector as a column vector and
 *      multiplies it by the matrix's COLUMNS — our layout matches 1:1.
 * Row-major engines exist; they pay a transpose on every upload. We don't.
 *
 * Header-only for the same reason as vec3.h: no .cpp, no CMake change.
 *
 * constexpr: every pure-arithmetic member (identity constructor, builders
 * that don't call trig, multiply, transformPoint) is constexpr so main()
 * can prove them with static_assert at compile time. rotationZ is the one
 * exception: std::sin/std::cos are not constexpr on MSVC, so it runs at
 * runtime — which is exactly when we need it anyway (once per frame).
 */
#ifndef PUREENGINE_MATH_MAT4_H
#define PUREENGINE_MATH_MAT4_H

#include <cmath>   // std::sin, std::cos for the rotation builder
#include "vec3.h"  // Mat4 builders take Vec3 parameters

namespace pe {

struct Mat4 {
    // The 16 elements. m[col][row], column-major (see header comment).
    // Memory layout of m[0]..m[3] is literally COLUMN 0 of the matrix:
    //
    //  math notation              memory
    //  [ m00 m01 m02 m03 ]        m[0][0] m[1][0] m[2][0] m[3][0]   <- row 0
    //  [ m10 m11 m12 m13 ]        m[0][1] m[1][1] m[2][1] m[3][1]   <- row 1
    //  [ m20 m21 m22 m23 ]        m[0][2] m[1][2] m[2][2] m[3][2]   <- row 2
    //  [ m30 m31 m32 m33 ]        m[0][3] m[1][3] m[2][3] m[3][3]   <- row 3
    //                              ^col0   ^col1   ^col2   ^col3
    float m[4][4];

private:
    // Helper constructor: builds a Mat4 from 16 explicit floats, written
    // ROW BY ROW in reading order. It exists because constexpr constructors
    // must initialize members in the initializer list — a loop in the body
    // is not allowed. Every element is visible; nothing is hidden.
    constexpr Mat4(float m00, float m01, float m02, float m03,
                   float m10, float m11, float m12, float m13,
                   float m20, float m21, float m22, float m23,
                   float m30, float m31, float m32, float m33)
        // Note the transposed-looking mapping: parameters arrive row-major
        // (for readability) but are STORED column-major, m[col][row].
        : m{ { m00, m10, m20, m30 },
             { m01, m11, m21, m31 },
             { m02, m12, m22, m32 },
             { m03, m13, m23, m33 } } {}

public:
    // Default constructor builds the IDENTITY matrix. It is the "1" of
    // matrices: M * I == M for any M. Building every Mat4 as identity by
    // default means an unconfigured matrix transforms nothing instead of
    // corrupting geometry with garbage floats.
    // The 16 literals below ARE the identity: 1s on the diagonal
    // (positions 00, 11, 22, 33), 0s everywhere else.
    constexpr Mat4()
        : Mat4(1.0f, 0.0f, 0.0f, 0.0f,
               0.0f, 1.0f, 0.0f, 0.0f,
               0.0f, 0.0f, 1.0f, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f) {}

    // ------------------------------------------------------------------
    // Builder: TRANSLATION. Shifts every point by (tx, ty, tz).
    // The offsets live in column 3, rows 0..2 — the only column that
    // affects position additively. Verify mentally: multiplying
    // (x, y, z, 1) by this matrix yields (x+tx, y+ty, z+tz, 1).
    // ------------------------------------------------------------------
    static constexpr Mat4 translation(const Vec3& t) {
        Mat4 result;                        // starts as identity
        result.m[3][0] = t.x;               // column 3 = translation column
        result.m[3][1] = t.y;
        result.m[3][2] = t.z;
        return result;
    }

    // ------------------------------------------------------------------
    // Builder: SCALE. Multiplies each axis by its factor. The factors sit
    // on the diagonal — the only entries a pure scale ever touches.
    // (1,1,1) = identity: no change. Negative factors mirror the shape.
    // ------------------------------------------------------------------
    static constexpr Mat4 scale(const Vec3& s) {
        Mat4 result;                        // starts as identity
        result.m[0][0] = s.x;               // x-axis multiplier
        result.m[1][1] = s.y;               // y-axis multiplier
        result.m[2][2] = s.z;               // z-axis multiplier
        return result;
    }

    // ------------------------------------------------------------------
    // Builder: ROTATION about the Z axis by 'angleRadians' (RADIANS —
    // trig functions and shaders both speak radians, so we never mix
    // degrees into the math; convert at the edges if needed).
    //
    // Z rotation spins the XY plane and leaves Z alone, which is exactly
    // what a 2D view of the scene can show. The 2x2 block in the upper
    // left is the textbook 2D rotation matrix:
    //      [ cos -sin ]
    //      [ sin  cos ]
    // Placed column-major: column 0 = ( cos, sin, 0, 0),
    //                      column 1 = (-sin, cos, 0, 0).
    // Positive angles rotate counter-clockwise in OpenGL's default
    // orientation (+X right, +Y up).
    // NOT constexpr: std::sin/std::cos cannot run at compile time on MSVC.
    // ------------------------------------------------------------------
    static Mat4 rotationZ(float angleRadians) {
        float c = std::cos(angleRadians);   // cosine = horizontal component
        float s = std::sin(angleRadians);   // sine   = vertical component
        Mat4 result;                        // starts as identity (Z row/col
                                            // untouched => z passes through)
        result.m[0][0] =  c;                // column 0, row 0
        result.m[0][1] =  s;                // column 0, row 1
        result.m[1][0] = -s;                // column 1, row 0 (note the sign)
        result.m[1][1] =  c;                // column 1, row 1
        return result;
    }

    // ------------------------------------------------------------------
    // Matrix * Matrix. THE composition operation: applying (this * other)
    // to a vertex is identical to applying 'other' FIRST and 'this' SECOND.
    // Multiplication is NOT commutative: T*R != R*T (translate-then-rotate
    // orbits; rotate-then-translate slides). Order is meaning.
    //
    // Each result element = dot product of a ROW of 'this' with a COLUMN
    // of 'other'. Careful with the indices: the row of 'this' runs across
    // its columns, hence a.m[k][row] while the column of 'other' is
    // b.m[col][k]. 64 multiplies + 48 adds — trivial per frame.
    // ------------------------------------------------------------------
    constexpr Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += m[k][row] * other.m[col][k];
                }
                result.m[col][row] = sum;
            }
        }
        return result;
    }

    // ------------------------------------------------------------------
    // Matrix * Point. Applies the transform to one 3D point. The w = 1
    // makes translation take effect (see header: homogeneous coordinates).
    // The perspective divide (dividing by w') is skipped because none of
    // our builders produce a w different from 1 — add it the day a
    // projection matrix enters the engine.
    // ------------------------------------------------------------------
    constexpr Vec3 transformPoint(const Vec3& p) const {
        float x = m[0][0] * p.x + m[1][0] * p.y + m[2][0] * p.z + m[3][0];
        float y = m[0][1] * p.x + m[1][1] * p.y + m[2][1] * p.z + m[3][1];
        float z = m[0][2] * p.x + m[1][2] * p.y + m[2][2] * p.z + m[3][2];
        return Vec3(x, y, z);
    }
};

} // namespace pe

#endif // PUREENGINE_MATH_MAT4_H
