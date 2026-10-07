/**
 * PureEditor0 (Step 289, Tools step 2 of 5) - viewer + headless --selftest.
 *
 * Modes:
 *   PureEditor0.exe --selftest   headless pure checks + ONE hidden-window
 *                                frame (real GL, no visible window); the
 *                                pure parts need no window.
 *   PureEditor0.exe <scene>      opens a window, loads the scene through
 *                                loadSceneForEditor, draws all entities and
 *                                their debug AABBs, shows the status line
 *                                with the bitmap font (file name, entity
 *                                count). On load failure the error text is
 *                                shown on screen and the editor keeps
 *                                running. NO saving of any kind.
 *
 * Consumes ONLY documented engine APIs: Entity (src/entity.h),
 * loadPrefab/instantiatePrefab (src/prefab.h), Scene (src/scene.h),
 * flagsForCount (src/lifecycle.h), Renderer drawWorld/drawAABBs/
 * drawTextString (src/renderer.h), Camera (src/camera.h), WindowGuard
 * (src/window_guard.h, Step 190). The editor boundary is
 * editor0_core.h (editor-owned). The window/loop pattern mirrors
 * games/pong/pong.cpp (glfwInit -> hints -> window -> context+glad ->
 * renderer.init -> camera+onResize -> loop -> shutdown). Engine src/ is
 * read-only for this Tools step; every friction goes to FINDINGS.md.
 */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>

#include "../../src/entity.h"
#include "../../src/prefab.h"
#include "../../src/scene.h"
#include "../../src/lifecycle.h"
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

// The pure parts (no window, no GL): the 288 checks + the status line.
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
    // yellow) + the bitmap-font status line.
    renderer.clear(0.0f, 0.0f, 0.0f);
    renderer.drawWorld(camera.projection(), camera.view(), loaded.entities, colliding);
    renderer.drawAABBs(camera.projection(), camera.view(), loaded.entities, -1);
    renderer.drawTextString(status, -5.8f, 4.1f, camera.projection(), pe::TextAlign::Left);
    glfwSwapBuffers(window);
    const GLenum glErr = glGetError();
    check(glErr == GL_NO_ERROR, "hidden-window: glGetError() == 0 after one frame");
    renderer.shutdown();   // BEFORE the guard's destruction (the WindowGuard contract)
    check(true, "hidden-window: the frame loop exits cleanly");
}

// Step 289: the window viewer. Loads ONCE through the editor boundary,
// draws every frame. On load failure the error text is on screen and
// the editor keeps running. NO saving of any kind.
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

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
        glfwPollEvents();
        renderer.clear(0.0f, 0.0f, 0.0f);
        const std::vector<char> colliding = pe::flagsForCount(current.entities.size());
        renderer.drawWorld(camera.projection(), camera.view(), current.entities, colliding);
        renderer.drawAABBs(camera.projection(), camera.view(), current.entities, -1);
        renderer.drawTextString(status, -5.8f, 4.1f, camera.projection(), pe::TextAlign::Left);
        glfwSwapBuffers(window);
    }
    renderer.shutdown();
    return 0;
}

int main(int argc, char** argv) {
    bool selftest = false;
    const char* scenePath = nullptr;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--selftest") selftest = true;
        else scenePath = argv[i];
    }
    if (selftest) {
        checkPureParts();
        checkHiddenWindowFrame();  // Step 289: one hidden-window frame
        std::remove(kTempScene);   // runtime output cleanup (build/ is gitignored)
        if (failures != 0) {
            std::printf("PureEditor0 --selftest: %d check(s) FAILED\n", failures);
            return 1;
        }
        std::printf("PureEditor0 --selftest: all checks passed\n");
        return 0;
    }
    if (scenePath) return runViewer(scenePath);
    std::printf("PureEditor0: no mode given. Usage: PureEditor0 --selftest | PureEditor0 <scene>\n");
    return 2;
}
