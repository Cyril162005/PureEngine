/**
 * PureEngine - Step 317: the dedicated math unit test (Core Foundation slice 3).
 * Deterministic value asserts ONLY - no timing, no GL, no window, no files.
 * Covers the Vec3 ops the engine uses and every Mat4 builder (identity,
 * multiply, translation, scale, orthographic, perspective zNear/zFar,
 * rotationZ, rotationY, lookAt) + transformPoint, with hand-computed expected
 * values traced from the formulas in src/math/vec3.h and src/math/mat4.h
 * (column-major, m[col][row]).
 * The slice-3 STAY-INLINE decision (recorded in V3): Vec3/Mat4 stay header-
 * inline - small hot bodies; a .cpp split would fan out to 6-8 consumer
 * targets for no behavior or clarity gain.
 */
#include <cstdio>
#include <cmath>
#include "math/vec3.h"
#include "math/mat4.h"

static int g_fail = 0;
static void expect(bool ok, const char* name) {
    if (ok) { std::printf("PASS: %s\n", name); }
    else { std::printf("FAIL: %s\n", name); ++g_fail; }
}
static bool nearf(float a, float b, float eps) { return std::fabs(a - b) <= eps; }
static bool nearv(const pe::Vec3& a, float x, float y, float z, float eps) {
    return nearf(a.x, x, eps) && nearf(a.y, y, eps) && nearf(a.z, z, eps);
}

int main() {
    // --- Vec3: the ops the engine uses ---
    const pe::Vec3 a(1.0f, 2.0f, 3.0f);
    const pe::Vec3 b(4.0f, 5.0f, 6.0f);
    expect(nearf(a.dot(b), 32.0f, 1e-5f), "vec3: dot((1,2,3),(4,5,6)) = 32");
    expect(nearv(a.cross(b), -3.0f, 6.0f, -3.0f, 1e-5f), "vec3: cross((1,2,3),(4,5,6)) = (-3,6,-3)");
    const pe::Vec3 u(3.0f, 4.0f, 0.0f);
    expect(nearf(u.lengthSquared(), 25.0f, 1e-5f), "vec3: lengthSquared((3,4,0)) = 25");
    expect(nearf(u.length(), 5.0f, 1e-5f), "vec3: length((3,4,0)) = 5");
    expect(nearv(u.normalized(), 0.6f, 0.8f, 0.0f, 1e-5f), "vec3: normalized((3,4,0)) = (0.6,0.8,0)");
    expect(nearv(pe::Vec3().normalized(), 0.0f, 0.0f, 0.0f, 1e-5f), "vec3: normalized(zero) stays zero (the guard)");
    expect(nearv(a + b, 5.0f, 7.0f, 9.0f, 1e-5f), "vec3: operator+");
    expect(nearv(b - a, 3.0f, 3.0f, 3.0f, 1e-5f), "vec3: operator-");
    expect(nearv(a * 2.0f, 2.0f, 4.0f, 6.0f, 1e-5f), "vec3: operator* scalar");
    expect(a == pe::Vec3(1.0f, 2.0f, 3.0f) && a != b, "vec3: operator== / !=");

    // --- Mat4 identity ---
    const pe::Mat4 I;
    bool idOk = true;
    for (int c = 0; c < 4 && idOk; ++c) {
        for (int r = 0; r < 4; ++r) {
            if (!nearf(I.m[c][r], (c == r) ? 1.0f : 0.0f, 0.0f)) { idOk = false; break; }
        }
    }
    expect(idOk, "mat4: the default ctor is the exact identity");

    // --- multiply + transformPoint: T(1,2,3) * S(2,3,4), traced from mat4.h:311-345 ---
    const pe::Mat4 T = pe::Mat4::translation(1.0f, 2.0f, 3.0f);
    const pe::Mat4 S = pe::Mat4::scale(2.0f, 3.0f, 4.0f);
    const pe::Mat4 M = T * S;
    expect(nearf(M.m[0][0], 2.0f, 1e-5f) && nearf(M.m[1][1], 3.0f, 1e-5f)
        && nearf(M.m[2][2], 4.0f, 1e-5f), "mat4: T*S keeps the diagonal of S");
    expect(nearf(M.m[3][0], 1.0f, 1e-5f) && nearf(M.m[3][1], 2.0f, 1e-5f)
        && nearf(M.m[3][2], 3.0f, 1e-5f), "mat4: T*S keeps the translation column of T");
    expect(nearv(M.transformPoint(pe::Vec3(1.0f, 1.0f, 1.0f)), 3.0f, 5.0f, 7.0f, 1e-4f),
        "mat4: transformPoint(T*S,(1,1,1)) = (3,5,7)");
    const pe::Mat4 IT = I * T;
    expect(nearv(IT.transformPoint(pe::Vec3(1.0f, 2.0f, 3.0f)), 2.0f, 4.0f, 6.0f, 1e-5f),
        "mat4: I*T acts as T (the point gains the translation: (1,2,3)+(1,2,3))");
    expect(nearv(T.transformPoint(pe::Vec3(1.0f, 1.0f, 1.0f)), 2.0f, 3.0f, 4.0f, 1e-5f),
        "mat4: translation(Vec3) moves the point by (1,2,3)");
    expect(nearv(S.transformPoint(pe::Vec3(1.0f, 1.0f, 1.0f)), 2.0f, 3.0f, 4.0f, 1e-5f),
        "mat4: scale(x,y,z) scales the point");

    // --- orthographic(0,8,0,6,0.1,100) ---
    const pe::Mat4 O = pe::Mat4::orthographic(0.0f, 8.0f, 0.0f, 6.0f, 0.1f, 100.0f);
    expect(nearf(O.m[0][0], 0.25f, 1e-5f) && nearf(O.m[3][0], -1.0f, 1e-5f),
        "mat4: ortho x-scale 0.25, x-center -1");
    expect(nearf(O.m[1][1], 2.0f / 6.0f, 1e-5f) && nearf(O.m[3][1], -1.0f, 1e-5f),
        "mat4: ortho y-scale 2/6, y-center -1");
    expect(nearf(O.m[2][2], -2.0f / 99.9f, 1e-5f) && nearf(O.m[3][2], -100.1f / 99.9f, 1e-4f),
        "mat4: ortho z-scale/center");

    // --- perspective(90deg, aspect 1, zNear 1, zFar 3) - exact plane math ---
    const float halfPi = 1.5707963267948966f;
    const pe::Mat4 P = pe::Mat4::perspective(halfPi, 1.0f, 1.0f, 3.0f);
    expect(nearf(P.m[0][0], 1.0f, 1e-4f) && nearf(P.m[1][1], 1.0f, 1e-4f),
        "mat4: perspective f=1 at 90deg fovy, aspect 1");
    expect(nearf(P.m[2][2], -2.0f, 1e-5f) && nearf(P.m[3][2], -3.0f, 1e-5f),
        "mat4: perspective zNear=1/zFar=3 -> m[2][2]=-2, m[3][2]=-3");
    expect(nearf(P.m[2][3], -1.0f, 1e-5f), "mat4: perspective w-row -1");
    expect(nearf(P.transformPoint(pe::Vec3(0.0f, 0.0f, -1.0f)).z, -1.0f, 1e-4f),
        "mat4: perspective maps the near plane to pre-divide z=-1");
    expect(nearf(P.transformPoint(pe::Vec3(0.0f, 0.0f, -3.0f)).z, 3.0f, 1e-4f),
        "mat4: perspective maps the far plane to pre-divide z=+3 (w=3 -> NDC +1)");

    // --- rotationZ ---
    expect(nearv(pe::Mat4::rotationZ(halfPi).transformPoint(pe::Vec3(1.0f, 0.0f, 0.0f)),
        0.0f, 1.0f, 0.0f, 1e-4f), "mat4: rotationZ(90deg) maps +X to +Y");
    expect(nearv(pe::Mat4::rotationZ(0.0f).transformPoint(pe::Vec3(1.0f, 2.0f, 3.0f)),
        1.0f, 2.0f, 3.0f, 1e-6f), "mat4: rotationZ(0) is the identity on points");

    // --- rotationY (the current behavior, pinned) ---
    expect(nearv(pe::Mat4::rotationY(halfPi).transformPoint(pe::Vec3(0.0f, 0.0f, 1.0f)),
        1.0f, 0.0f, 0.0f, 1e-4f), "mat4: rotationY(90deg) maps +Z to +X (as implemented - pinned)");

    // --- lookAt ---
    const pe::Mat4 V = pe::Mat4::lookAt(pe::Vec3(0.0f, 0.0f, 5.0f), pe::Vec3(0.0f, 0.0f, 0.0f),
        pe::Vec3(0.0f, 1.0f, 0.0f));
    expect(nearv(V.transformPoint(pe::Vec3(0.0f, 0.0f, 5.0f)), 0.0f, 0.0f, 0.0f, 1e-4f),
        "mat4: lookAt maps the eye to the origin");
    expect(nearv(V.transformPoint(pe::Vec3(0.0f, 0.0f, 0.0f)), 0.0f, 0.0f, -5.0f, 1e-4f),
        "mat4: lookAt maps the target to z=-5 (in front)");

    if (g_fail == 0) { std::printf("math_test: ALL PASS\n"); }
    else { std::printf("math_test: %d FAIL\n", g_fail); }
    return g_fail == 0 ? 0 : 1;
}
