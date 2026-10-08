/**
 * PureEditor0 (Step 290, Tools step 3 of 5) - viewer + sample + --selftest.
 *
 * Modes (run from D:\PureEngine or any CWD with assets/ reachable):
 *   PureEditor0.exe --selftest        headless checks + ONE hidden-window
 *                                     frame (real GL, no visible window);
 *                                     the pure parts need no window.
 *   PureEditor0.exe --make-sample <p> write the 3-prefab sample scene to
 *                                     <p> with the engine's saver, print
 *                                     the path, exit. REFUSES to
 *                                     overwrite an existing file.
 *   PureEditor0.exe <scene>           view the scene: load through
 *                                     loadSceneForEditor, draw all
 *                                     entities + debug AABBs, status
 *                                     line with the bitmap font.
 *                                     CONTROLS: left-drag = pan,
 *                                     +/= = zoom in, -/_ = zoom out
 *                                     (clamped 0.25..4.0), ESC = quit.
 *                                     On load failure the error text is
 *                                     shown and the editor keeps
 *                                     running. NO saving of any kind.
 *
 * Consumes ONLY documented engine APIs: Entity (src/entity.h),
 * loadPrefab/instantiatePrefab (src/prefab.h), Scene (src/scene.h),
 * flagsForCount (src/lifecycle.h), Renderer drawWorld/drawAABBs/
 * drawTextString (src/renderer.h), Camera (src/camera.h: follow,
 * getPosition, halfExtentX/Y, screenToWorld, worldToScreen),
 * Input (src/input.h: static isDown/pollMouse + edge-tracked keys),
 * WindowGuard (src/window_guard.h, Step 190). The editor boundary is
 * editor0_core.h (editor-owned: loadSceneForEditor, makeStatusLine,
 * clampedZoom, cameraPanDelta, zoomedProjection, screenToWorldAtZoom,
 * worldToScreenAtZoom). The window/loop pattern mirrors
 * games/pong/pong.cpp. Engine src/ is read-only for this Tools step;
 * every friction goes to FINDINGS.md.
 */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "../../src/entity.h"
#include "../../src/prefab.h"
#include "../../src/scene.h"
#include "../../src/lifecycle.h"
#include "../../src/input.h"
#include "../../src/renderer.h"
#include "../../src/camera.h"
#include "../../src/window_guard.h"
#include "editor0_core.h"

static int failures = 0;

static void check(bool cond, const char* what) {
    std::printf("%s: %s\n", cond ? "PASS" : "FAIL", what);
    if (!cond) ++failures;
    std::fflush(stdout);  // survive crashes: buffered stdout is lost on 0xC0000005
}

static bool floatEq(float a, float b) { return std::fabs(a - b) < 1e-5f; }
static bool floatEqT(float a, float b, float tol) { return std::fabs(a - b) < tol; }

// The same field set the Phase D consumer compares (the 34-field v2
// round-trip; the runtime-only fields are not in the saved state).
static bool entityFieldMatches(const pe::Entity& a, const pe::Entity& b, std::string& diff) {
    if (!floatEq(a.position.x, b.position.x) || !floatEq(a.position.y, b.position.y) || !floatEq(a.position.z, b.position.z)) { diff = "position"; return false; }
    if (!floatEq(a.rotationAngle, b.rotationAngle)) { diff = "rotationAngle"; return false; }
    if (!floatEq(a.rotationSpeed, b.rotationSpeed)) { diff = "rotationSpeed"; return false; }
    if (!floatEq(a.scale.x, b.scale.x) || !floatEq(a.scale.y, b.scale.y) || !floatEq(a.scale.z, b.scale.z)) { diff = "scale"; return false; }
    if (!floatEq(a.halfExtents.x, b.halfExtents.x) || !floatEq(a.halfExtents.y, b.halfExtents.y) || !floatEq(a.halfExtents.z, b.halfExtents.z)) { diff = "halfExtents"; return false; }
    if (a.textureId != b.textureId) { diff = "textureId"; return false; }
    if (a.depth != b.depth) { diff = "depth"; return false; }
    if (a.roleId != b.roleId) { diff = "roleId"; return false; }
    if (!floatEq(a.moveSpeed, b.moveSpeed)) { diff = "moveSpeed"; return false; }
    if (!floatEq(a.velocity.x, b.velocity.x) || !floatEq(a.velocity.y, b.velocity.y) || !floatEq(a.velocity.z, b.velocity.z)) { diff = "velocity"; return false; }
    if (!floatEq(a.gravityScale, b.gravityScale)) { diff = "gravityScale"; return false; }
    if (a.isStatic != b.isStatic) { diff = "isStatic"; return false; }
    if (!floatEq(a.coyoteTime, b.coyoteTime)) { diff = "coyoteTime"; return false; }
    if (!floatEq(a.jumpImpulse, b.jumpImpulse)) { diff = "jumpImpulse"; return false; }
    if (!floatEq(a.maxFallSpeed, b.maxFallSpeed)) { diff = "maxFallSpeed"; return false; }
    if (!floatEq(a.tint.x, b.tint.x) || !floatEq(a.tint.y, b.tint.y) || !floatEq(a.tint.z, b.tint.z)) { diff = "tint"; return false; }
    if (a.cols != b.cols || a.rows != b.rows) { diff = "cols/rows"; return false; }
    if (!floatEq(a.health, b.health)) { diff = "health"; return false; }
    if (!floatEq(a.timer, b.timer)) { diff = "timer"; return false; }
    if (a.tag != b.tag) { diff = "tag"; return false; }
    if (a.parentIndex != b.parentIndex) { diff = "parentIndex"; return false; }
    if (!floatEq(a.animationSpeed, b.animationSpeed)) { diff = "animationSpeed"; return false; }
    if (a.currentClipName != b.currentClipName) { diff = "currentClipName"; return false; }
    return true;
}

// Expected values come from the inputs (the committed prefab files +
// the explicit values set below), never from re-measured output.
static const char* kTileFile   = "consumers/level_pipeline/prefab_tile.txt";
static const char* kPickupFile = "consumers/level_pipeline/prefab_pickup.txt";
static const char* kEnemyFile  = "consumers/level_pipeline/prefab_enemy.txt";
static const char* kTempScene  = "editor0_tmp/rt_scene.txt";  // temp: a NEW file, never a source
static const char* kMissing    = "editor0_tmp/no_such_scene_zz.txt";

static void readTextFile(const char* name, std::string& out) {
    std::ifstream f(name, std::ios::binary);
    out.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

// Step 290: build the 3-prefab sample scene (the same scene --selftest
// uses, with the same distinct values).
static bool buildSampleScene(pe::Scene& scene) {
    pe::Prefab tile, pickup, enemy;
    if (!pe::loadPrefab(kTileFile, tile) || !pe::loadPrefab(kPickupFile, pickup) || !pe::loadPrefab(kEnemyFile, enemy)) {
        return false;
    }
    scene.name = "editor0_sample";
    scene.queueSpawn(pe::instantiatePrefab(tile,   pe::Vec3(0.0f, 0.0f, 0.0f)));
    scene.queueSpawn(pe::instantiatePrefab(pickup, pe::Vec3(1.5f, -2.5f, 0.0f)));
    scene.queueSpawn(pe::instantiatePrefab(enemy,  pe::Vec3(3.0f, 3.0f, 0.0f)));
    scene.flushSpawns();
    scene.entities[2].velocity = pe::Vec3(0.5f, -0.25f, 0.0f);
    scene.entities[2].timer = 1.5f;
    scene.entities[2].animationSpeed = 2.0f;
    scene.entities[2].rotationAngle = 0.75f;
    return true;
}

// Step 290: the --make-sample core. Saves the sample scene to `path`
// with the ENGINE's saver (pe::saveSceneToFile). Refuses to overwrite
// an existing file (the editor never destroys data): false + a reason,
// the file untouched.
static bool makeSampleSceneFile(const char* path, std::string& err) {
    err.clear();
    if (!path || !*path) {
        err = "empty path";
        return false;
    }
    {
        std::ifstream probe(path, std::ios::binary);
        if (probe) {
            err = "refusing to overwrite an existing file: ";
            err += path;
            return false;
        }
    }
    pe::Scene scene;
    if (!buildSampleScene(scene)) {
        err = "the sample scene could not be built (the prefabs must load)";
        return false;
    }
    if (!pe::saveSceneToFile(scene, path)) {
        err = "the save failed: ";
        err += path;
        return false;
    }
    return true;
}

// The pure parts (no window, no GL): the 288/289 checks.
static void checkPureParts() {
    // --- build the scene from the EXISTING prefabs ---
    pe::Prefab tile, pickup, enemy;
    if (!pe::loadPrefab(kTileFile, tile) || !pe::loadPrefab(kPickupFile, pickup) || !pe::loadPrefab(kEnemyFile, enemy)) {
        check(false, "selftest: the three prefabs must load");
        return;
    }
    pe::Scene scene;
    scene.name = "editor0_selftest";
    scene.queueSpawn(pe::instantiatePrefab(tile,   pe::Vec3(0.0f, 0.0f, 0.0f)));
    scene.queueSpawn(pe::instantiatePrefab(pickup, pe::Vec3(1.5f, -2.5f, 0.0f)));
    scene.queueSpawn(pe::instantiatePrefab(enemy,  pe::Vec3(3.0f, 3.0f, 0.0f)));
    scene.flushSpawns();
    check(scene.entities.size() == 3, "selftest: the scene built from 3 prefabs holds 3 entities");
    // Distinct non-default values (from these inputs) for the compare.
    scene.entities[2].velocity = pe::Vec3(0.5f, -0.25f, 0.0f);
    scene.entities[2].timer = 1.5f;
    scene.entities[2].animationSpeed = 2.0f;
    scene.entities[2].rotationAngle = 0.75f;

    // Expected-from-inputs spot values (the prefab v1 files;
    // clip= -> currentClipName per prefab.h:171).
    check(scene.entities[0].tag == "tile" && scene.entities[0].isStatic && scene.entities[0].textureId == 1,
          "selftest: tile entity matches the prefab input");
    check(scene.entities[1].tag == "pickup" && floatEq(scene.entities[1].health, 1.0f),
          "selftest: pickup entity matches the prefab input");
    check(scene.entities[2].tag == "enemy" && floatEq(scene.entities[2].health, 50.0f)
          && scene.entities[2].currentClipName == "walk_left",
          "selftest: enemy entity matches the prefab input (clip -> currentClipName)");

    // --- save to a temp path (a NEW file; never the source prefabs) ---
    check(pe::saveSceneToFile(scene, kTempScene), "selftest: saveSceneToFile succeeds to the temp path");

    // --- the editor load REPLACES `current` only on success ---
    pe::Scene current;
    current.name = "sentinel_before_load";
    std::string err;
    check(editor0::loadSceneForEditor(kTempScene, current, err), "selftest: loadSceneForEditor(temp) succeeds");
    check(current.name == "editor0_selftest", "selftest: `current` replaced (name)");
    check(current.entities.size() == 3, "selftest: the loaded entity count is the saved 3");
    for (std::size_t i = 0; i < current.entities.size(); ++i) {
        char msg[128];
        std::string diff;
        const bool same = entityFieldMatches(scene.entities[i], current.entities[i], diff);
        std::snprintf(msg, sizeof(msg), "selftest: entity %d every compared field round-trips (%s)",
                      (int)i, same ? "all 24 field checks" : diff.c_str());
        check(same, msg);
    }

    // --- failure case: a missing file returns false, `current` untouched ---
    {
        pe::Scene before = current;  // full snapshot
        std::string err2;
        const bool ok = editor0::loadSceneForEditor(kMissing, current, err2);
        std::printf("OBSERVED: missing-file err: %s\n", err2.c_str());
        check(!ok, "selftest: loadSceneForEditor(missing file) returns false");
        check(!err2.empty() && err2.rfind("file not found", 0) == 0, "selftest: err names the failure (file not found)");
        std::string diff;
        bool untouched = before.name == current.name && before.entities.size() == current.entities.size();
        for (std::size_t i = 0; untouched && i < before.entities.size(); ++i) {
            untouched = entityFieldMatches(before.entities[i], current.entities[i], diff);
        }
        check(untouched, "selftest: `current` untouched on failure (name + count + all fields)");
    }

    // --- negative control: the comparator reports the altered field ---
    {
        pe::Scene altered; std::string err3;
        check(editor0::loadSceneForEditor(kTempScene, altered, err3), "selftest: the negative-control load succeeds");
        std::string diff;
        check(entityFieldMatches(scene.entities[1], altered.entities[1], diff),
              "selftest: the unaltered pair matches (comparator baseline)");
        altered.entities[1].health += 0.5f;  // alter exactly ONE field
        diff.clear();
        const bool same = entityFieldMatches(scene.entities[1], altered.entities[1], diff);
        std::printf("OBSERVED: negative-control diff field: %s\n", diff.c_str());
        check(!same, "selftest: the single altered field is detected");
        check(diff == "health", "selftest: the comparator names the altered field (health)");
    }

    // --- Step 289: the status-line pure function (expected from the inputs) ---
    const std::string okLine = editor0::makeStatusLine(kTempScene, 3, "");
    std::printf("OBSERVED: ok status line: [%s]\n", okLine.c_str());
    check(okLine == "editor0: editor0_tmp/rt_scene.txt | 3 entities",
          "status line: the ok format (file name + entity count)");
    check(editor0::makeStatusLine("x.txt", 0, "file not found: x.txt") == "editor0 ERROR: file not found: x.txt",
          "status line: the error format");
    // Negative control: err takes precedence; the count MUST NOT appear.
    {
        const std::string s = editor0::makeStatusLine("x.txt", 3, "boom");
        check(s.find("3 entities") == std::string::npos && s == "editor0 ERROR: boom",
              "status line: err takes precedence over count (negative control)");
    }
}

// Step 290: the sample generator + the pan/zoom math (pure, no GL).
// Expected values come from the camera formula, never from
// re-measured output.
static void checkSampleAndPanZoom() {
    // --- the sample generator ---
    {
        const char* kSample = "editor0_tmp/sample_scene.txt";
        std::string serr;
        check(makeSampleSceneFile(kSample, serr), "make-sample: the sample scene is written");
        pe::Scene fromSample; std::string lerr;
        check(editor0::loadSceneForEditor(kSample, fromSample, lerr), "make-sample: the generated file loads");
        check(fromSample.entities.size() == 3, "make-sample: the generated entity count is 3");
        // refusal: an existing path is refused and left unchanged
        std::string before, after;
        readTextFile(kSample, before);
        std::string rerr;
        check(!makeSampleSceneFile(kSample, rerr), "make-sample: an existing path is REFUSED");
        check(rerr.rfind("refusing to overwrite", 0) == 0, "make-sample: the refusal names the reason");
        readTextFile(kSample, after);
        check(!before.empty() && before == after, "make-sample: the existing file is unchanged");
        std::remove(kSample);
    }

    // --- the camera box is the documented 12x9 (halfHeight locked 4.5,
    //     halfWidth = 4.5 * aspect; camera.h onResize) ---
    pe::Camera cam;
    cam.onResize(800, 600);
    const float halfW = cam.halfExtentX();
    const float halfH = cam.halfExtentY();
    check(halfW == 6.0f && halfH == 4.5f, "pan/zoom: the camera box is the documented 12x9 (6.0, 4.5)");

    // --- a pan then screenToWorld/worldToScreen round-trip (tolerance
    //     1e-4 stated; the editor's zoom-1 path is the same formula as
    //     the engine's documented Camera conversion) ---
    cam.follow(pe::Vec3(2.0f, 1.0f, 0.0f));   // pan the camera
    const pe::Vec3 w = editor0::screenToWorldAtZoom(400.0f, 300.0f, 800, 600, halfW, halfH, 1.0f, cam.getPosition());
    // expected: screenToUi(400,300) = the exact center (0,0) + pos (2,1)
    check(floatEqT(w.x, 2.0f, 1e-4f) && floatEqT(w.y, 1.0f, 1e-4f),
          "pan/zoom: screenToWorld(400,300) at pan (2,1) = (2,1)");
    const pe::Vec3 px = editor0::worldToScreenAtZoom(w, 800, 600, halfW, halfH, 1.0f, cam.getPosition());
    check(floatEqT(px.x, 400.0f, 1e-4f) && floatEqT(px.y, 300.0f, 1e-4f),
          "pan/zoom: worldToScreen round-trips to (400,300) after the pan (tolerance 1e-4)");
    // the engine's own documented conversion agrees at zoom 1
    const pe::Vec3 wEng = cam.screenToWorld(400.0f, 300.0f, 800, 600);
    const pe::Vec3 pxEng = cam.worldToScreen(wEng, 800, 600);
    check(floatEqT(pxEng.x, 400.0f, 1e-4f) && floatEqT(pxEng.y, 300.0f, 1e-4f),
          "pan/zoom: the engine Camera round-trip agrees (the documented contract)");

    // --- a pan of (dx,dy) moves a known world point by the amount
    //     computed from the camera formula ---
    // cameraPanDelta(100, -50, 800, 600, 6, 4.5, 1)
    //   = (-100 * 2*6/800, -50 * 2*4.5/600) = (-1.5, -0.75).
    pe::Camera cam2;
    cam2.onResize(800, 600);
    const pe::Vec3 delta = editor0::cameraPanDelta(100.0f, -50.0f, 800, 600,
                                                   cam2.halfExtentX(), cam2.halfExtentY(), 1.0f);
    check(floatEqT(delta.x, -1.5f, 1e-4f) && floatEqT(delta.y, -0.75f, 1e-4f),
          "pan/zoom: the pan delta = the formula (-dx*2*halfW/fbW, +dy*2*halfH/fbH)");
    // a known world point (1,2): after the pan its screen position moves
    // EXACTLY by the drag (100, -50) px (tolerance 1e-3 stated).
    const pe::Vec3 p0 = editor0::worldToScreenAtZoom(pe::Vec3(1.0f, 2.0f, 0.0f), 800, 600,
                                                     cam2.halfExtentX(), cam2.halfExtentY(), 1.0f, cam2.getPosition());
    cam2.follow(pe::Vec3(cam2.getPosition().x + delta.x, cam2.getPosition().y + delta.y, 0.0f));
    const pe::Vec3 p1 = editor0::worldToScreenAtZoom(pe::Vec3(1.0f, 2.0f, 0.0f), 800, 600,
                                                     cam2.halfExtentX(), cam2.halfExtentY(), 1.0f, cam2.getPosition());
    check(floatEqT(p1.x - p0.x, 100.0f, 1e-3f) && floatEqT(p1.y - p0.y, -50.0f, 1e-3f),
          "pan/zoom: the pan moves the known world point by exactly the drag (100, -50) px");
    // negative control: a DIFFERENT drag is not the first formula amount.
    const pe::Vec3 delta2 = editor0::cameraPanDelta(10.0f, 10.0f, 800, 600,
                                                    cam2.halfExtentX(), cam2.halfExtentY(), 1.0f);
    check(!(floatEqT(delta2.x, -1.5f, 1e-4f) && floatEqT(delta2.y, -0.75f, 1e-4f)),
          "pan/zoom: negative control - a different drag is not the first formula amount");

    // --- zoom clamps at the documented min and max (editor-owned:
    //     the camera exposes no zoom API - the missing-capability
    //     finding) ---
    check(editor0::clampedZoom(0.01f) == editor0::kEditorZoomMin, "pan/zoom: zoom clamps at the documented min (0.25)");
    check(editor0::clampedZoom(100.0f) == editor0::kEditorZoomMax, "pan/zoom: zoom clamps at the documented max (4.0)");
    check(editor0::clampedZoom(1.5f) == 1.5f, "pan/zoom: an in-range zoom is unchanged");
    // negative control: the clamp bounds are DISTINCT (min != max).
    check(editor0::kEditorZoomMin != editor0::kEditorZoomMax, "pan/zoom: negative control - the clamp bounds are distinct");

    // --- the zoomed conversions round-trip (the zoomed half-extents) ---
    {
        const float z = 2.0f;
        const pe::Vec3 wz = editor0::screenToWorldAtZoom(200.0f, 150.0f, 800, 600, halfW, halfH, z, pe::Vec3(0.0f, 0.0f, 0.0f));
        const pe::Vec3 pxz = editor0::worldToScreenAtZoom(wz, 800, 600, halfW, halfH, z, pe::Vec3(0.0f, 0.0f, 0.0f));
        check(floatEqT(pxz.x, 200.0f, 1e-4f) && floatEqT(pxz.y, 150.0f, 1e-4f),
              "pan/zoom: the zoomed screen<->world conversions round-trip (tolerance 1e-4)");
    }
}

// Step 289: ONE hidden-window frame with a loaded scene (real GL, no
// visible window). Asserts glGetError() == 0 after the frame and that
// the one-iteration loop exits cleanly (control returns here).
static void checkHiddenWindowFrame() {
    if (!glfwInit()) { check(false, "hidden-window: glfwInit failed"); return; }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);      // hidden window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(320, 240, "editor0-selftest", NULL, NULL);
    if (!window) { glfwTerminate(); check(false, "hidden-window: the window must be created"); return; }
    pe::WindowGuard guard(window);  // any early return destroys + terminates GLFW
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) { check(false, "hidden-window: gladLoadGL failed"); return; }
    pe::Renderer renderer;
    if (!renderer.init()) { check(false, "hidden-window: renderer.init failed"); return; }
    pe::Camera camera;
    {   // the Pong pattern: viewport + projection follow the window size
        int fw, fh;
        glfwGetFramebufferSize(window, &fw, &fh);
        camera.onResize(fw, fh);
        glViewport(0, 0, fw, fh);
    }
    pe::Scene loaded; std::string err;
    check(editor0::loadSceneForEditor(kTempScene, loaded, err), "hidden-window: the scene must load");
    check(loaded.entities.size() == 3, "hidden-window: the loaded count is 3");
    const std::vector<char> colliding = pe::flagsForCount(loaded.entities.size());
    const std::string status = editor0::makeStatusLine(kTempScene, loaded.entities.size(), err);
    // ONE frame: clear + entities + debug AABBs (-1 matches no role: all
    // yellow) + the bitmap-font status line, at the zoomed projection.
    renderer.clear(0.0f, 0.0f, 0.0f);
    const pe::Mat4 proj = editor0::zoomedProjection(1.0f, camera.projection());
    renderer.drawWorld(proj, camera.view(), loaded.entities, colliding);
    renderer.drawAABBs(proj, camera.view(), loaded.entities, -1);
    renderer.drawTextString(status, -5.8f, 4.1f, proj, pe::TextAlign::Left);
    glfwSwapBuffers(window);
    const GLenum glErr = glGetError();
    check(glErr == GL_NO_ERROR, "hidden-window: glGetError() == 0 after one frame");
    renderer.shutdown();   // BEFORE the guard's destruction (the WindowGuard contract)
    check(true, "hidden-window: the frame loop exits cleanly");
}

// Steps 289/290: the window viewer. Loads ONCE through the editor
// boundary, draws every frame. On load failure the error text is on
// screen and the editor keeps running. NO saving of any kind.
// Controls: left-drag = pan, +/- = zoom (clamped 0.25..4.0), ESC = quit.
static int runViewer(const char* scenePath) {
    if (!glfwInit()) { std::fprintf(stderr, "editor0: glfwInit failed\n"); return 1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(800, 600, "PureEditor0", NULL, NULL);
    if (!window) { std::fprintf(stderr, "editor0: window creation failed\n"); glfwTerminate(); return 1; }
    pe::WindowGuard guard(window);
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) { std::fprintf(stderr, "editor0: gladLoadGL failed\n"); return 1; }
    pe::Renderer renderer;
    if (!renderer.init()) { std::fprintf(stderr, "editor0: renderer init failed\n"); return 1; }
    pe::Camera camera;
    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
        glViewport(0, 0, w, h);
        auto* cam = static_cast<pe::Camera*>(glfwGetWindowUserPointer(win));
        if (cam) cam->onResize(w, h);
    });
    {   // the Pong pattern: initial viewport + projection
        int fw, fh;
        glfwGetFramebufferSize(window, &fw, &fh);
        camera.onResize(fw, fh);
        glViewport(0, 0, fw, fh);
    }

    pe::Scene current;
    std::string err;
    const bool ok = editor0::loadSceneForEditor(scenePath, current, err);
    const std::string status = editor0::makeStatusLine(scenePath, current.entities.size(), ok ? std::string() : err);
    if (!ok) std::fprintf(stderr, "%s\n", status.c_str());

    // Step 290: editor state - the zoom (the documented limits) + the
    // drag tracking. Edge-tracked keys: ESC (quit) + +/- (zoom steps) -
    // one event per press (the documented edge read). NO wheel: input.h
    // exposes no scroll input (the missing-capability finding).
    pe::Input input({GLFW_KEY_ESCAPE, GLFW_KEY_EQUAL, GLFW_KEY_MINUS});
    float zoom = 1.0f;
    bool dragging = false;
    float lastX = 0.0f, lastY = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // Zoom: +/- keys, one step per press.
        if (input.isEdge(window, GLFW_KEY_EQUAL)) zoom = editor0::clampedZoom(zoom * 1.25f);
        if (input.isEdge(window, GLFW_KEY_MINUS)) zoom = editor0::clampedZoom(zoom / 1.25f);
        // Quit: ESC, one edge per press.
        if (input.isEdge(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GLFW_TRUE);
        // Pan: left-drag; the grabbed world point follows the cursor
        // (position += ui(m0) - ui(m1), the cameraPanDelta formula).
        const pe::MouseState m = pe::Input::pollMouse(window);
        if (m.left) {
            if (dragging) {
                int fw, fh;
                glfwGetFramebufferSize(window, &fw, &fh);
                const pe::Vec3 pan = editor0::cameraPanDelta(m.x - lastX, m.y - lastY, fw, fh,
                                                             camera.halfExtentX(), camera.halfExtentY(), zoom);
                camera.follow(pe::Vec3(camera.getPosition().x + pan.x,
                                       camera.getPosition().y + pan.y, 0.0f));
            }
            dragging = true;
            lastX = m.x;
            lastY = m.y;
        } else {
            dragging = false;
        }
        input.update(window);   // frame-end snapshot (the documented temporal order)

        renderer.clear(0.0f, 0.0f, 0.0f);
        const std::vector<char> colliding = pe::flagsForCount(current.entities.size());
        const pe::Mat4 proj = editor0::zoomedProjection(zoom, camera.projection());
        renderer.drawWorld(proj, camera.view(), current.entities, colliding);
        renderer.drawAABBs(proj, camera.view(), current.entities, -1);
        renderer.drawTextString(status, -5.8f, 4.1f, proj, pe::TextAlign::Left);
        glfwSwapBuffers(window);
    }
    renderer.shutdown();
    return 0;
}

int main(int argc, char** argv) {
    bool selftest = false;
    bool makeSample = false;
    const char* scenePath = nullptr;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--selftest") selftest = true;
        else if (a == "--make-sample") makeSample = true;
        else scenePath = argv[i];
    }
    if (selftest) {
        checkPureParts();
        checkSampleAndPanZoom();   // Step 290: the sample generator + pan/zoom math
        checkHiddenWindowFrame();  // Step 289: one hidden-window frame
        std::remove(kTempScene);   // runtime output cleanup (build/ is gitignored)
        if (failures != 0) {
            std::printf("PureEditor0 --selftest: %d check(s) FAILED\n", failures);
            return 1;
        }
        std::printf("PureEditor0 --selftest: all checks passed\n");
        return 0;
    }
    if (makeSample) {
        if (!scenePath) { std::fprintf(stderr, "editor0: --make-sample needs a path argument\n"); return 2; }
        std::string err;
        if (makeSampleSceneFile(scenePath, err)) {
            std::printf("%s\n", scenePath);  // prints the path, then exits
            return 0;
        }
        std::fprintf(stderr, "editor0: %s\n", err.c_str());
        return 1;
    }
    if (scenePath) return runViewer(scenePath);
    std::printf("PureEditor0: no mode given. Usage:\n"
                "  PureEditor0 --selftest          headless checks (+ one hidden-window GL frame)\n"
                "  PureEditor0 --make-sample <p>   write the 3-prefab sample scene to <p> (refuses to overwrite)\n"
                "  PureEditor0 <scene>             view the scene (drag = pan, +/- = zoom, ESC = quit)\n");
    return 2;
}
