/**
 * PureEditor0 (Step 291, Tools step 4 of 5) - viewer + sample + --selftest.
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
 *                                     (clamped 0.25..4.0), R = reload
 *                                     the current path, ESC = quit.
 *                                     On load/reload failure the error
 *                                     text is shown and the editor
 *                                     keeps running (the scene is
 *                                     untouched). NO saving of any kind.
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
#include <sstream>
#include <algorithm>
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

// The full-unchanged check: current vs the snapshot (name + count +
// every compared field). The mutation-catcher for the failed loads.
static bool sceneUnchanged(const pe::Scene& a, const pe::Scene& b, std::string& diff) {
    if (a.name != b.name) { diff = "name"; return false; }
    if (a.entities.size() != b.entities.size()) { diff = "entity count"; return false; }
    for (std::size_t i = 0; i < a.entities.size(); ++i) {
        if (!entityFieldMatches(a.entities[i], b.entities[i], diff)) return false;
    }
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

// Hostile inputs are GENERATED PROGRAMMATICALLY (never hand-edited)
// and written to the temp dir (gitignored; nothing committed).
static void writeRawFile(const char* path, const std::string& content) {
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    f << content;
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
        check(!ok && sceneUnchanged(before, current, diff), "selftest: `current` untouched on failure (name + count + all fields)");
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

// Step 291: the reload path. After a successful load, rewrite the file
// with a DIFFERENT entity count (generated programmatically), reload,
// and the scene equals the second file exactly (count + per-entity
// fields), with no leftover entities and no pending spawns from the
// first load. Negative control included.
static void checkReload() {
    const char* kReload = "editor0_tmp/reload_scene.txt";
    std::string serr;
    if (!makeSampleSceneFile(kReload, serr)) { check(false, "reload: the sample must be written"); return; }
    pe::Scene current; std::string err;
    if (!editor0::loadSceneForEditor(kReload, current, err)) { check(false, "reload: the first load must succeed"); return; }
    check(current.entities.size() == 3, "reload: the first load has 3 entities");
    check(current.pendingSpawns.empty(), "reload: the first load has no pending spawns");

    // Rewrite the file with a DIFFERENT entity count (2: tile + pickup),
    // generated programmatically, saved to the SAME path (the engine
    // saver's rename-overwrite contract, Step 162).
    pe::Scene second;
    {
        pe::Prefab tile, pickup;
        if (!pe::loadPrefab(kTileFile, tile) || !pe::loadPrefab(kPickupFile, pickup)) {
            check(false, "reload: the two prefabs must load");
            return;
        }
        second.name = "editor0_sample2";
        second.queueSpawn(pe::instantiatePrefab(tile,   pe::Vec3(0.0f, 0.0f, 0.0f)));
        second.queueSpawn(pe::instantiatePrefab(pickup, pe::Vec3(1.0f, 1.0f, 0.0f)));
        second.flushSpawns();
    }
    check(pe::saveSceneToFile(second, kReload), "reload: the second save (overwrite, a different count)");

    check(editor0::loadSceneForEditor(kReload, current, err), "reload: the second load succeeds");
    // The scene equals the second file EXACTLY: count + per-entity
    // fields; NO leftover entities; NO pending spawns.
    check(current.entities.size() == 2, "reload: the count is the second file's 2 (no leftover from the first 3)");
    check(current.pendingSpawns.empty(), "reload: no pending spawns after the reload");
    std::string diff;
    bool same = current.name == second.name;
    for (std::size_t i = 0; same && i < 2; ++i) {
        same = entityFieldMatches(second.entities[i], current.entities[i], diff);
    }
    check(same, "reload: the scene equals the second file (name + both entities' fields)");

    // Negative control: alter one field of the reloaded scene; the
    // comparator reports it.
    current.entities[0].health += 0.5f;
    diff.clear();
    const bool same2 = entityFieldMatches(second.entities[0], current.entities[0], diff);
    std::printf("OBSERVED: reload negative-control diff field: %s\n", diff.c_str());
    check(!same2 && diff == "health", "reload: negative control - the altered field is reported (health)");

    std::remove(kReload);
}

// Step 291: hostile loads, generated PROGRAMMATICALLY into the temp
// directory (never hand-edited, sizes capped, nothing committed). For
// each: loadSceneForEditor returns false with a non-empty err, and the
// previous scene is UNCHANGED (a full field comparison against the
// saved snapshot). The CTest TIMEOUT (60s) guards hangs; a crash or
// hang would be an engine-defect finding with the exact repro, not a
// fix.
static void checkHostileLoads() {
    pe::Scene snapshot; std::string lerr;
    if (!editor0::loadSceneForEditor(kTempScene, snapshot, lerr)) {
        check(false, "hostile: the snapshot scene must load");
        return;
    }
    std::string validText;
    readTextFile(kTempScene, validText);
    check(!validText.empty(), "hostile: the valid sample text captured");
    const char* kHostile = "editor0_tmp/hostile_scene.txt";
    pe::Scene current = snapshot;  // the protected scene

    // 1. Empty file.
    {
        writeRawFile(kHostile, "");
        std::string err;
        const bool ok = editor0::loadSceneForEditor(kHostile, current, err);
        std::printf("OBSERVED: the empty file: %s\n", ok ? "LOADED (unexpected!)" : "rejected");
        std::string diff;
        check(!ok && !err.empty(), "hostile: the empty file rejected with a non-empty err");
        check(!ok && sceneUnchanged(current, snapshot, diff), "hostile: the empty file leaves the scene unchanged (all fields)");
    }

    // 2. Truncated at every 1/8 of the valid sample (7 points).
    {
        int rejects = 0, prefixLoads = 0;
        for (int i = 1; i <= 7; ++i) {
            const std::size_t cut = validText.size() * (std::size_t)i / 8;
            writeRawFile(kHostile, validText.substr(0, cut));
            std::string err;
            const bool ok = editor0::loadSceneForEditor(kHostile, current, err);
            std::string diff;
            if (ok) {
                // OBSERVED: a line-boundary truncation is a VALID SHORTER
                // scene (the scene= line carries no count, so the strict
                // whole-file contract cannot tell a prefix from a full
                // file). Recorded; the protected scene restored.
                ++prefixLoads;
                std::printf("OBSERVED: truncation %d/8 LOADS as a shorter scene (%d entities)\n", i, (int)current.entities.size());
                check(current.entities.size() < 3, "hostile: a prefix-load has fewer entities than the sample");
                std::string rerr;
                if (!editor0::loadSceneForEditor(kTempScene, current, rerr)) { check(false, "hostile: the snapshot re-load"); return; }
            } else {
                ++rejects;
                check(!err.empty(), "hostile: a truncation reject has a non-empty err");
                check(sceneUnchanged(current, snapshot, diff), "hostile: a truncation reject leaves the scene unchanged");
            }
        }
        std::printf("OBSERVED: truncations: %d rejected, %d valid-prefix loads\n", rejects, prefixLoads);
        check(rejects + prefixLoads == 7, "hostile: all 7 truncation points ran");
    }

    // 3. Wrong version ("# scene v99").
    {
        std::string bad = validText;
        const std::size_t pos = bad.find("# scene v2");
        if (pos != std::string::npos) bad.replace(pos, 10, "# scene v99");
        writeRawFile(kHostile, bad);
        std::string err;
        const bool ok = editor0::loadSceneForEditor(kHostile, current, err);
        std::printf("OBSERVED: the wrong version: %s\n", ok ? "LOADED (unexpected!)" : "rejected");
        std::string diff;
        check(!ok && !err.empty(), "hostile: the wrong version rejected with a non-empty err");
        check(!ok && sceneUnchanged(current, snapshot, diff), "hostile: the wrong version leaves the scene unchanged");
    }

    // 4. Negative version ("# scene v-1").
    {
        std::string bad = validText;
        const std::size_t pos = bad.find("# scene v2");
        if (pos != std::string::npos) bad.replace(pos, 10, "# scene v-1");
        writeRawFile(kHostile, bad);
        std::string err;
        const bool ok = editor0::loadSceneForEditor(kHostile, current, err);
        std::printf("OBSERVED: the negative version: %s\n", ok ? "LOADED (unexpected!)" : "rejected");
        std::string diff;
        check(!ok && !err.empty(), "hostile: the negative version rejected with a non-empty err");
        check(!ok && sceneUnchanged(current, snapshot, diff), "hostile: the negative version leaves the scene unchanged");
    }

    // 5. Garbage bytes (4 KB).
    {
        std::string g;
        g.reserve(4096);
        for (int i = 0; i < 4096; ++i) g.push_back((char)(i % 251 + 1));
        writeRawFile(kHostile, g);
        std::string err;
        const bool ok = editor0::loadSceneForEditor(kHostile, current, err);
        std::printf("OBSERVED: 4 KB of garbage bytes: %s\n", ok ? "LOADED (unexpected!)" : "rejected");
        std::string diff;
        check(!ok && !err.empty(), "hostile: the garbage bytes rejected with a non-empty err");
        check(!ok && sceneUnchanged(current, snapshot, diff), "hostile: the garbage bytes leave the scene unchanged");
    }

    // 6. One 1 MB line (sizes capped; the TIMEOUT guards hangs).
    {
        std::string big = "# scene v2\nscene=adv\nentity=";
        big.append(1000000, 'x');
        big.append("\n");
        writeRawFile(kHostile, big);
        std::string err;
        const bool ok = editor0::loadSceneForEditor(kHostile, current, err);
        std::printf("OBSERVED: the 1 MB line: %s\n", ok ? "LOADED (unexpected!)" : "rejected");
        std::string diff;
        check(!ok && !err.empty(), "hostile: the 1 MB line rejected with a non-empty err (no hang)");
        check(!ok && sceneUnchanged(current, snapshot, diff), "hostile: the 1 MB line leaves the scene unchanged");
    }

    // 7-10. The count fields: the SCENE (v2) format has NO count field,
    // so the injected "count=N" lines are UNKNOWN keys -> the strict
    // reject (the count field lives in the manager format; the observed
    // outcome is recorded in FINDINGS).
    {
        const char* countTexts[4] = {"0", "-1", "2147483648", "5"};
        const char* countLabels[4] = {"0", "-1", "2^31 (2147483648)", "5 (mismatching the 3 entries)"};
        for (int i = 0; i < 4; ++i) {
            std::string bad = validText + "count=" + countTexts[i] + "\n";
            writeRawFile(kHostile, bad);
            std::string err;
            const bool ok = editor0::loadSceneForEditor(kHostile, current, err);
            std::printf("OBSERVED: count=%s: %s\n", countLabels[i], ok ? "LOADED (unexpected!)" : "rejected");
            std::string diff;
            check(!ok && !err.empty(), "hostile: the count-field line rejected with a non-empty err");
            check(!ok && sceneUnchanged(current, snapshot, diff), "hostile: the count-field line leaves the scene unchanged");
        }
    }

    // 11. A path that is a directory.
    {
        std::string err;
        const bool ok = editor0::loadSceneForEditor("editor0_tmp", current, err);
        std::printf("OBSERVED: a directory path: %s (err: %s)\n", ok ? "LOADED (unexpected!)" : "rejected", err.c_str());
        std::string diff;
        check(!ok && !err.empty(), "hostile: a directory path rejected with a non-empty err");
        check(!ok && sceneUnchanged(current, snapshot, diff), "hostile: a directory path leaves the scene unchanged");
    }

    // 12. A missing path.
    {
        std::string err;
        const bool ok = editor0::loadSceneForEditor(kMissing, current, err);
        std::printf("OBSERVED: a missing path: %s (err: %s)\n", ok ? "LOADED (unexpected!)" : "rejected", err.c_str());
        std::string diff;
        check(!ok && !err.empty(), "hostile: a missing path rejected with a non-empty err");
        check(!ok && sceneUnchanged(current, snapshot, diff), "hostile: a missing path leaves the scene unchanged");
    }

    // Negative control: the unchanged-checker itself - a copy of the
    // snapshot with ONE altered field is detected and named.
    {
        pe::Scene altered = snapshot;
        altered.entities[1].health += 0.5f;
        std::string diff;
        check(!sceneUnchanged(altered, snapshot, diff) && diff == "health",
              "hostile: negative control - the unchanged-checker detects the altered field (health)");
    }
    std::remove(kHostile);
}

// Step 294: selection + pick + gesture + the print discipline (pure,
// no GL). Expected values come from the formulas, never from
// re-measured output.
static void checkSelectionAndPick() {
    // --- the pick at a NON-DEFAULT pan and zoom: a click at the screen
    //     position computed from a known entity's world position
    //     selects it ---
    pe::Camera cam;
    cam.onResize(800, 600);
    const float halfW = cam.halfExtentX();   // 6.0
    const float halfH = cam.halfExtentY();   // 4.5
    cam.follow(pe::Vec3(2.0f, 1.0f, 0.0f));  // the non-default pan
    const float z = 1.5f;                    // the non-default zoom
    // A known entity at world (3,3,0): its screen position from the
    // formulas: rel = (1,2); the zoomed half-extents (4, 3);
    // uiToScreen(1,2,800,600,4,3) = (500, 100).
    pe::Entity e(pe::Vec3(3.0f, 3.0f, 0.0f), 0.0f, pe::Vec3(0.5f, 0.5f, 1.0f), pe::Vec3(0.5f, 0.5f, 0.5f), 1);
    e.alive = true;
    e.roleId = 7;
    e.depth = 0;
    std::vector<pe::Entity> entities;
    entities.push_back(e);
    const pe::Vec3 screenPos = editor0::worldToScreenAtZoom(e.position, 800, 600, halfW, halfH, z, cam.getPosition());
    std::printf("OBSERVED: the known entity's screen position: (%.4f, %.4f)\n", screenPos.x, screenPos.y);
    check(floatEqT(screenPos.x, 500.0f, 1e-3f) && floatEqT(screenPos.y, 100.0f, 1e-3f),
          "pick: the screen position from the formulas = (500, 100) at pan (2,1) zoom 1.5");
    const int picked = editor0::pickEntityAtScreenZoomed(entities, screenPos.x, screenPos.y, 800, 600, halfW, halfH, z, cam.getPosition());
    check(picked == 0, "pick: a click at the computed screen position selects the known entity");

    // --- a click OUTSIDE selects nothing ---
    const int missed = editor0::pickEntityAtScreenZoomed(entities, 10.0f, 10.0f, 800, 600, halfW, halfH, z, cam.getPosition());
    check(missed == -1, "pick: a click outside selects nothing (-1)");

    // --- negative control: a click computed with the WRONG zoom misses ---
    {
        const float wrongZoom = 0.75f;  // ignoring the zoom -> the wrong world point
        const pe::Vec3 wrongWorld = editor0::screenToWorldAtZoom(screenPos.x, screenPos.y, 800, 600, halfW, halfH, wrongZoom, cam.getPosition());
        // expected: the unzoomed conversion of (500,100) with the 0.75
        // half-extents: rel = (2,4) -> world (4,5) - clearly outside.
        std::printf("OBSERVED: the wrong-zoom world point: (%.4f, %.4f)\n", wrongWorld.x, wrongWorld.y);
        check(floatEqT(wrongWorld.x, 4.0f, 1e-3f) && floatEqT(wrongWorld.y, 5.0f, 1e-3f),
              "pick: the wrong-zoom conversion lands at (4,5) (from the formulas)");
        const int wrongPick = editor0::pickEntityAtScreenZoomed(entities, screenPos.x, screenPos.y, 800, 600, halfW, halfH, wrongZoom, cam.getPosition());
        check(wrongPick == -1, "pick: negative control - a click computed with the wrong zoom MISSES");
    }

    // --- overlapping entities: the observed pick order (a finding, NOT
    //     asserted as a contract) ---
    {
        pe::Entity e2 = e;
        e2.depth = 2;        // a LOWER depth
        e.depth = 5;         // the higher depth
        std::vector<pe::Entity> both;
        both.push_back(e);   // index 0, depth 5
        both.push_back(e2);  // index 1, depth 2
        const int pickedBoth = editor0::pickEntityAtScreenZoomed(both, screenPos.x, screenPos.y, 800, 600, halfW, halfH, z, cam.getPosition());
        std::printf("OBSERVED: overlapping entities (depths 5 then 2): the pick returned index %d\n", pickedBoth);
        // recorded as a finding; NOT asserted as a contract (the step).
    }

    // --- a dead entity is not selectable (the engine skips it) ---
    {
        pe::Entity dead = e;
        dead.alive = false;
        std::vector<pe::Entity> withDead;
        withDead.push_back(dead);  // ONLY the dead entity
        const int pickedDead = editor0::pickEntityAtScreenZoomed(withDead, screenPos.x, screenPos.y, 800, 600, halfW, halfH, z, cam.getPosition());
        check(pickedDead == -1, "pick: a dead entity is NOT selectable (the engine skips alive=false)");
    }

    // --- the click-vs-drag classification at the threshold boundary ---
    check(editor0::classifyPointerGesture(0.0f, 0.0f, 4.0f, 0.0f, editor0::kEditorClickThresholdPx) == editor0::PointerGesture::Click,
          "gesture: dist == the threshold (4 px) is a CLICK (the inclusive boundary)");
    check(editor0::classifyPointerGesture(0.0f, 0.0f, 4.1f, 0.0f, editor0::kEditorClickThresholdPx) == editor0::PointerGesture::Drag,
          "gesture: dist just past the threshold is a DRAG");
    check(editor0::classifyPointerGesture(0.0f, 0.0f, 0.0f, 0.0f, editor0::kEditorClickThresholdPx) == editor0::PointerGesture::Click,
          "gesture: a zero-distance release is a CLICK");
    // negative control: a far release is a DRAG, not a CLICK.
    check(editor0::classifyPointerGesture(0.0f, 0.0f, 100.0f, 50.0f, editor0::kEditorClickThresholdPx) == editor0::PointerGesture::Drag,
          "gesture: negative control - a far release is a DRAG");

    // --- the selection lifecycle across reloads (the editor contract) ---
    {
        const char* kSel = "editor0_tmp/sel_scene.txt";
        std::string serr;
        if (!makeSampleSceneFile(kSel, serr)) {
            check(false, "reload/selection: the sample must be written");
        } else {
            pe::Scene cur; std::string lerr;
            check(editor0::loadSceneForEditor(kSel, cur, lerr), "reload/selection: the first load");
            int sel = 1;
            // a SUCCESSFUL reload CLEARS the selection
            check(editor0::reloadForEditor(kSel, cur, sel, lerr), "reload/selection: the successful reload");
            check(sel == -1, "reload/selection: the successful reload CLEARS the selection");
            // a FAILED reload KEEPS the selection
            sel = 2;
            const bool ok2 = editor0::reloadForEditor(kMissing, cur, sel, lerr);
            check(!ok2, "reload/selection: the failed reload returns false");
            check(sel == 2, "reload/selection: the failed reload KEEPS the selection");
        }
        std::remove(kSel);
    }

    // --- the print discipline (Step 294): ONE failed load = exactly
    //     ONE report (the test sink counts them) ---
    {
        std::ostringstream sink;
        pe::Scene cur; std::string lerr;
        const bool ok3 = editor0::loadSceneForEditor(kMissing, cur, lerr);
        if (!ok3) {
            editor0::reportMessage(sink, editor0::makeStatusLine(kMissing, cur.entities.size(), lerr));
        }
        const std::string reported = sink.str();
        const int lines = static_cast<int>(std::count(reported.begin(), reported.end(), '\n'));
        std::printf("OBSERVED: one failed load produced %d report line(s): [%s]\n", lines, reported.c_str());
        check(lines == 1, "print: one failed load reports EXACTLY once (the test sink)");
        check(reported == "editor0 ERROR: file not found: editor0_tmp/no_such_scene_zz.txt\n",
              "print: the report is the editor0 ERROR status line");
    }
}

// Step 295: the live-path wiring, driven by SYNTHETIC EditorInput
// sequences (no GL, no window). The cursor is given in WINDOW
// coordinates; the ratio conversion is exercised at 1.0, 1.25 and
// 1.5. Expected values come from the formulas, never from
// re-measured output.
static void checkLiveInputWiring() {
    // a) press and release at the same point over a known entity ->
    //    selected (ratio 1.0).
    {
        editor0::EditorState state;
        buildSampleScene(state.current);
        state.path = "editor0_tmp/synth.txt";
        state.camera.onResize(800, 600);
        // The enemy (entities[2], world (3,3)): its screen position at
        // zoom 1, window == fb (800x600): (600, 100) from the formulas.
        const pe::Vec3 sp = editor0::worldToScreenAtZoom(state.current.entities[2].position, 800, 600,
                                                         6.0f, 4.5f, 1.0f, pe::Vec3(0.0f, 0.0f, 0.0f));
        check(floatEqT(sp.x, 600.0f, 1e-3f) && floatEqT(sp.y, 100.0f, 1e-3f),
              "input: the enemy's screen position = (600, 100) (from the formulas)");
        editor0::EditorInput in;
        in.cursorX = sp.x; in.cursorY = sp.y;
        in.windowWidth = 800; in.windowHeight = 600;
        in.fbWidth = 800; in.fbHeight = 600;
        in.leftDown = true;
        editor0::stepEditorFrame(state, in);
        in.leftDown = false;
        editor0::stepEditorFrame(state, in);
        check(state.selected == 2, "input: a press+release at the same point selects the entity (ratio 1.0)");
    }

    // b) the ratio cases (display scaling): the cursor in WINDOW
    //    coordinates -> selected; the raw cursor (skipping the ratio)
    //    misses (the negative control).
    {
        struct RatioCase { int winW, winH, fbW, fbH; };
        const RatioCase cases[2] = { {1280, 720, 1600, 900}, {1920, 1080, 2880, 1620} };
        for (int i = 0; i < 2; ++i) {
            const float ratio = static_cast<float>(cases[i].fbW) / static_cast<float>(cases[i].winW);
            editor0::EditorState state;
            buildSampleScene(state.current);
            state.path = "editor0_tmp/synth.txt";
            state.camera.onResize(cases[i].fbW, cases[i].fbH);  // the real code: the FB size
            const float halfW = state.camera.halfExtentX();
            const float halfH = state.camera.halfExtentY();
            // The entity's FB position from the formulas; the WINDOW
            // position = the FB position / the ratio.
            const pe::Vec3 fbPos = editor0::worldToScreenAtZoom(state.current.entities[2].position,
                                                                cases[i].fbW, cases[i].fbH, halfW, halfH, 1.0f,
                                                                pe::Vec3(0.0f, 0.0f, 0.0f));
            const float winX = fbPos.x / ratio;
            const float winY = fbPos.y / ratio;
            editor0::EditorInput in;
            in.cursorX = winX; in.cursorY = winY;
            in.windowWidth = cases[i].winW; in.windowHeight = cases[i].winH;
            in.fbWidth = cases[i].fbW; in.fbHeight = cases[i].fbH;
            in.leftDown = true;
            editor0::stepEditorFrame(state, in);
            in.leftDown = false;
            editor0::stepEditorFrame(state, in);
            char msg[128];
            std::snprintf(msg, sizeof(msg), "input: the click at the WINDOW position selects it (ratio %.2f)", ratio);
            check(state.selected == 2, msg);
            // Negative control: the raw cursor as FB pixels (skipping the
            // ratio) misses the entity.
            const pe::Vec3 rawWorld = editor0::screenToWorldAtZoom(winX, winY, cases[i].fbW, cases[i].fbH,
                                                                   halfW, halfH, 1.0f, pe::Vec3(0.0f, 0.0f, 0.0f));
            const int rawPick = pe::pickEntity(state.current.entities, rawWorld.x, rawWorld.y);
            check(rawPick != 2, "input: negative control - the raw cursor (skipping the ratio) misses");
        }
    }

    // c) press, move beyond the threshold, release -> panned, not selected.
    {
        editor0::EditorState state;
        buildSampleScene(state.current);
        state.camera.onResize(800, 600);
        editor0::EditorInput in;
        in.cursorX = 400.0f; in.cursorY = 300.0f;
        in.windowWidth = 800; in.windowHeight = 600;
        in.fbWidth = 800; in.fbHeight = 600;
        in.leftDown = true;
        editor0::stepEditorFrame(state, in);
        const float camXBefore = state.camera.getPosition().x;
        const float camYBefore = state.camera.getPosition().y;
        in.cursorX = 500.0f; in.cursorY = 250.0f;  // a 100x50 px drag
        editor0::stepEditorFrame(state, in);
        check(state.camera.getPosition().x != camXBefore || state.camera.getPosition().y != camYBefore,
              "input: the drag pans the camera");
        in.leftDown = false;
        editor0::stepEditorFrame(state, in);
        check(state.selected == -1, "input: a drag does NOT select (panned, not selected)");
    }

    // d) a click on empty space clears.
    {
        editor0::EditorState state;
        buildSampleScene(state.current);
        state.camera.onResize(800, 600);
        state.selected = 1;  // pre-selected
        editor0::EditorInput in;
        in.cursorX = 10.0f; in.cursorY = 10.0f;
        in.windowWidth = 800; in.windowHeight = 600;
        in.fbWidth = 800; in.fbHeight = 600;
        in.leftDown = true;
        editor0::stepEditorFrame(state, in);
        in.leftDown = false;
        editor0::stepEditorFrame(state, in);
        check(state.selected == -1, "input: a click on empty space CLEARS the selection");
    }

    // e) the R reload through the real step function: replaces the
    //    scene, clears the selection, and shows the feedback; a failed
    //    reload keeps both.
    {
        const char* kSynth = "editor0_tmp/synth_reload.txt";
        std::string serr;
        if (!makeSampleSceneFile(kSynth, serr)) {
            check(false, "input: the synthetic reload sample must be written");
        } else {
            editor0::EditorState state;
            state.path = kSynth;
            std::string lerr;
            check(editor0::loadSceneForEditor(kSynth, state.current, lerr), "input: the synthetic load");
            state.camera.onResize(800, 600);
            state.selected = 1;
            editor0::EditorInput in;
            in.windowWidth = 800; in.windowHeight = 600;
            in.fbWidth = 800; in.fbHeight = 600;
            in.keyR = true;
            editor0::stepEditorFrame(state, in);
            check(state.selected == -1, "input: the R reload CLEARS the selection");
            check(state.current.entities.size() == 3, "input: the R reload keeps the scene");
            check(state.feedback == std::string("reloaded ") + kSynth + " (3 entities)",
                  "input: the R feedback is the exact reloaded line");
            // The failed reload: the file disappeared.
            state.selected = 2;
            state.path = kMissing;
            editor0::stepEditorFrame(state, in);
            check(state.feedback.rfind("reload failed: ", 0) == 0,
                  "input: the failed reload's feedback is the reload-failed line");
            check(state.selected == 2, "input: the failed reload KEEPS the selection");
            check(state.current.entities.size() == 3, "input: the failed reload KEEPS the scene");
            std::remove(kSynth);
        }
    }

    // The feedback exact strings (Step 295).
    check(editor0::makeReloadFeedback(true, "p.txt", 3, "") == "reloaded p.txt (3 entities)",
          "feedback: the reloaded format");
    check(editor0::makeReloadFeedback(false, "p.txt", 0, "file not found: p.txt") == "reload failed: file not found: p.txt",
          "feedback: the reload-failed format");
    // The status line's startup/empty state (the 5-param overload).
    {
        std::vector<pe::Entity> ents;
        check(editor0::makeStatusLine("editor0_tmp/rt_scene.txt", 3, "", -1, ents)
                  == "editor0: editor0_tmp/rt_scene.txt | 3 entities | no selection",
              "feedback: the startup status shows 'no selection'");
        pe::Entity tagged(pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f), pe::Vec3(0.5f, 0.5f, 0.5f), 1);
        tagged.tag = "pickup";
        ents.push_back(tagged);
        check(editor0::makeStatusLine("editor0_tmp/rt_scene.txt", 3, "", 0, ents)
                  == "editor0: editor0_tmp/rt_scene.txt | 3 entities | selected 0 (pickup)",
              "feedback: the selected status format");
    }
}

// Step 296: the read-only inspector panel (pure, no GL). Expected
// strings come from the inputs (the hand-built entity's fields), never
// from re-measured output.
static void checkInspectorPanel() {
    // --- the exact strings: the 36-line list for a known entity ---
    pe::Entity e(pe::Vec3(3.0f, 3.0f, 0.0f), 0.5f, pe::Vec3(0.6f, 0.6f, 1.0f), pe::Vec3(0.3f, 0.3f, 0.5f), 4);
    e.alive = true;
    e.roleId = 2;
    e.depth = 2;
    e.health = 50.0f;
    e.timer = 1.5f;
    e.velocity = pe::Vec3(0.5f, -0.25f, 0.0f);
    e.gravityScale = 1.0f;
    e.isStatic = false;
    e.coyoteTime = 0.05f;
    e.jumpImpulse = 8.0f;
    e.maxFallSpeed = 20.0f;
    e.tint = pe::Vec3(1.0f, 1.0f, 1.0f);
    e.cols = 4;
    e.rows = 2;
    e.tag = "enemy";
    e.parentIndex = -1;
    e.animationSpeed = 2.0f;
    e.currentClipName = "walk_left";
    e.rotationAngle = 0.75f;
    e.moveSpeed = 2.2f;
    const char* expected[36] = {
        "entity 2",
        "tag enemy",
        "role 2",
        "pos.x 3.0000",
        "pos.y 3.0000",
        "pos.z 0.0000",
        "scale.x 0.6000",
        "scale.y 0.6000",
        "scale.z 1.0000",
        "rotation 0.7500 rad",
        "rot.speed 0.5000 rad/s",
        "rot.axis z(2d)/y(3d)",
        "half.x 0.3000",
        "half.y 0.3000",
        "half.z 0.5000",
        "health 50.0000",
        "textureId 4",
        "tint.r 1.0000",
        "tint.g 1.0000",
        "tint.b 1.0000",
        "depth 2",
        "moveSpeed 2.2000",
        "vel.x 0.5000",
        "vel.y -0.2500",
        "vel.z 0.0000",
        "gravityScale 1.0000",
        "isStatic 0",
        "coyoteTime 0.0500",
        "jumpImpulse 8.0000",
        "maxFallSpeed 20.0000",
        "cols 4",
        "rows 2",
        "timer 1.5000",
        "parentIndex -1",
        "animSpeed 2.0000",
        "clip walk_left",
    };
    const std::vector<std::string> lines = editor0::makeInspectorLines(e, 2, 100);
    check(lines.size() == 36, "inspector: the full list is 36 lines (24 saver fields + the context lines)");
    int exactFails = 0;
    for (int i = 0; i < 36; ++i) {
        if (lines[static_cast<std::size_t>(i)] != expected[i]) {
            ++exactFails;
            std::printf("FAIL: inspector line %d: [%s] != [%s]\n", i, lines[static_cast<std::size_t>(i)].c_str(), expected[i]);
        }
    }
    check(exactFails == 0, "inspector: all 36 lines exact (the fixed order, the saver 4dp style)");

    // --- the field coverage: every field the scene saver writes appears
    //     (the 24 saver fields -> the inspector labels) ---
    {
        const char* labels[25] = {
            "entity ", "tag ", "role ", "pos.", "scale.", "rotation ", "rot.speed", "rot.axis",
            "half.", "health", "textureId", "tint.", "depth", "moveSpeed", "vel.",
            "gravityScale", "isStatic", "coyoteTime", "jumpImpulse", "maxFallSpeed",
            "cols", "rows", "timer", "parentIndex", "animSpeed",
        };
        int missing = 0;
        for (int i = 0; i < 25; ++i) {
            bool found = false;
            for (const std::string& l : lines) {
                if (l.rfind(labels[i], 0) == 0) { found = true; break; }
            }
            if (!found) {
                ++missing;
                std::printf("FAIL: the coverage: the label [%s] has no line\n", labels[i]);
            }
        }
        check(missing == 0, "inspector: the field coverage - every saver field has a line (the negative control: a dropped field FAILS this)");
    }

    // --- the +N more boundary ---
    {
        const std::vector<std::string> capped = editor0::makeInspectorLines(e, 2, 12);
        check(capped.size() == 12, "inspector: maxLines 12 -> 12 lines");
        check(capped[10] == "rot.speed 0.5000 rad/s", "inspector: the 11th line is the 11th field (the cap boundary)");
        check(capped[11] == "+25 more", "inspector: the overflow line is +25 more (36 - 11)");
        const std::vector<std::string> full = editor0::makeInspectorLines(e, 2, 36);
        check(full.size() == 36 && full[35] == "clip walk_left", "inspector: maxLines 36 -> all 36, no overflow line");
        const std::vector<std::string> over = editor0::makeInspectorLines(e, 2, 50);
        check(over.size() == 36, "inspector: maxLines beyond the total -> all 36, no overflow line");
    }

    // --- no selection -> one 'no selection' line ---
    {
        const std::vector<std::string> none = editor0::inspectorLinesForSelection(pe::Scene(), -1, 12);
        check(none.size() == 1 && none[0] == "no selection", "inspector: no selection -> exactly one 'no selection' line");
    }

    // --- the click inside the panel rect KEEPS the selection (the
    //     point would otherwise CLEAR: no entity at the world point) ---
    {
        editor0::EditorState state;
        buildSampleScene(state.current);
        state.camera.onResize(800, 600);
        state.selected = 1;  // pre-selected
        editor0::EditorInput in;
        in.cursorX = 100.0f; in.cursorY = 100.0f;  // inside the panel rect (fb 27..975 x 0..380 at 800x600)
        in.windowWidth = 800; in.windowHeight = 600;
        in.fbWidth = 800; in.fbHeight = 600;
        in.leftDown = true;
        editor0::stepEditorFrame(state, in);
        in.leftDown = false;
        editor0::stepEditorFrame(state, in);
        check(state.selected == 1, "panel: a click inside the panel rect KEEPS the selection (no pick, no clear)");
        // The control: the same-size click OUTSIDE the rect clears.
        editor0::EditorInput in2;
        in2.cursorX = 400.0f; in2.cursorY = 500.0f;  // y=500 > 380: outside the rect; no entity there
        in2.windowWidth = 800; in2.windowHeight = 600;
        in2.fbWidth = 800; in2.fbHeight = 600;
        in2.leftDown = true;
        editor0::stepEditorFrame(state, in2);
        in2.leftDown = false;
        editor0::stepEditorFrame(state, in2);
        check(state.selected == -1, "panel: the control - a click outside the rect clears (empty space)");
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
    // The panel draw (the Step 296 test): the inspector lines in UI
    // space (the unzoomed projection), one frame, glGetError() == 0.
    {
        const std::vector<std::string> panelLines = editor0::inspectorLinesForSelection(loaded, 0, 12);
        float py = 3.6f;
        for (const std::string& l : panelLines) {
            renderer.drawTextString(l, -5.8f, py, camera.projection(), pe::TextAlign::Left);
            py -= 0.42f;
        }
        glfwSwapBuffers(window);
        const GLenum glErr2 = glGetError();
        check(glErr2 == GL_NO_ERROR, "panel: glGetError() == 0 after the panel draw");
    }
    renderer.shutdown();   // BEFORE the guard's destruction (the WindowGuard contract)
    check(true, "hidden-window: the frame loop exits cleanly");
}

// Steps 289-291: the window viewer. Loads ONCE through the editor
// boundary, draws every frame. On load/reload failure the error text
// is on screen and the editor keeps running (the scene is untouched).
// NO saving of any kind.
// Controls: left-drag = pan, +/- = zoom (clamped 0.25..4.0), R =
// reload the current path, ESC = quit.
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
    // The editor state owns the camera (the Step 295 extraction); the
    // framebuffer callback keeps the pe::Camera* user pointer.
    editor0::EditorState state;
    glfwSetWindowUserPointer(window, &state.camera);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
        glViewport(0, 0, w, h);
        auto* cam = static_cast<pe::Camera*>(glfwGetWindowUserPointer(win));
        if (cam) cam->onResize(w, h);
    });
    {   // the Pong pattern: initial viewport + projection
        int fw, fh;
        glfwGetFramebufferSize(window, &fw, &fh);
        state.camera.onResize(fw, fh);
        glViewport(0, 0, fw, fh);
    }

    // The editor state (everything editor-owned; the Step 295
    // extraction): the scene, the path, the status, the selection.
    state.path = scenePath;
    std::string err;
    const bool ok = editor0::loadSceneForEditor(scenePath, state.current, err);
    state.status = editor0::makeStatusLine(scenePath, state.current.entities.size(),
                                           ok ? std::string() : err, state.selected, state.current.entities);
    if (!ok) editor0::reportMessage(std::cerr, state.status);  // ONE report per attempt

    // Edge-tracked keys: ESC (quit) + +/- (zoom steps) + R (reload) -
    // one event per press (the documented edge read). NO wheel:
    // input.h exposes no scroll input (the missing-capability finding).
    pe::Input input({GLFW_KEY_ESCAPE, GLFW_KEY_EQUAL, GLFW_KEY_MINUS, GLFW_KEY_R});

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // Fill the per-frame input from the REAL input (Step 295): the
        // cursor in WINDOW coordinates (pollMouse), the window's
        // logical size, the framebuffer's pixel size, the button level,
        // and the key EDGES (read before update()).
        const pe::MouseState m = pe::Input::pollMouse(window);
        editor0::EditorInput in;
        in.cursorX = m.x;
        in.cursorY = m.y;
        glfwGetWindowSize(window, &in.windowWidth, &in.windowHeight);
        glfwGetFramebufferSize(window, &in.fbWidth, &in.fbHeight);
        in.leftDown = m.left;
        in.keyR = input.isEdge(window, GLFW_KEY_R);
        in.keyZoomIn = input.isEdge(window, GLFW_KEY_EQUAL);
        in.keyZoomOut = input.isEdge(window, GLFW_KEY_MINUS);
        in.keyEsc = input.isEdge(window, GLFW_KEY_ESCAPE);
        editor0::stepEditorFrame(state, in);
        input.update(window);   // frame-end snapshot (the documented temporal order)
        if (state.quitRequested) glfwSetWindowShouldClose(window, GLFW_TRUE);

        renderer.clear(0.0f, 0.0f, 0.0f);
        const std::vector<char> colliding = pe::flagsForCount(state.current.entities.size());
        const pe::Mat4 proj = editor0::zoomedProjection(state.zoom, state.camera.projection());
        renderer.drawWorld(proj, state.camera.view(), state.current.entities, colliding);
        // The highlight: the selected entity's ROLE GROUP draws orange
        // (the engine's playerRoleId parameter - no engine change; the
        // sample's roles are distinct so exactly the selected entity
        // highlights). Nothing selected (-1): all yellow.
        renderer.drawAABBs(proj, state.camera.view(), state.current.entities,
                           (state.selected >= 0) ? state.current.entities[static_cast<std::size_t>(state.selected)].roleId : -1);
        // The inspector panel (the Step 296 read-only inspector): the
        // lines in UI space with the UNZOOMED base projection - immune
        // to pan and zoom (the pan lives in the view matrix, the zoom
        // in the composed projection; neither is used here). The panel
        // never edits.
        {
            const std::vector<std::string> lines = editor0::inspectorLinesForSelection(state.current, state.selected, 12);
            float y = 3.6f;
            for (const std::string& l : lines) {
                renderer.drawTextString(l, -5.8f, y, state.camera.projection(), pe::TextAlign::Left);
                y -= 0.42f;
            }
        }
        // The status line: the transient reload feedback until the next
        // event, then the base + selection.
        const std::string& line = state.feedback.empty() ? state.status : state.feedback;
        renderer.drawTextString(line, -5.8f, 4.1f, proj, pe::TextAlign::Left);
        glfwSwapBuffers(window);
    }
    renderer.shutdown();
    return 0;
}

// Step 295: the input diagnostics. ONE line per click (not per
// frame): the cursor (window coords), the window size, the
// framebuffer size, the converted world position, and the pick
// result index. Cyril runs it and pastes the lines.
static int runDiagInput(const char* scenePath) {
    if (!glfwInit()) { std::fprintf(stderr, "editor0: glfwInit failed\n"); return 1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "PureEditor0 diag-input", NULL, NULL);
    if (!window) { std::fprintf(stderr, "editor0: window creation failed\n"); glfwTerminate(); return 1; }
    pe::WindowGuard guard(window);
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) { std::fprintf(stderr, "editor0: gladLoadGL failed\n"); return 1; }
    pe::Renderer renderer;
    if (!renderer.init()) { std::fprintf(stderr, "editor0: renderer init failed\n"); return 1; }
    editor0::EditorState state;
    glfwSetWindowUserPointer(window, &state.camera);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
        glViewport(0, 0, w, h);
        auto* cam = static_cast<pe::Camera*>(glfwGetWindowUserPointer(win));
        if (cam) cam->onResize(w, h);
    });
    {
        int fw, fh;
        glfwGetFramebufferSize(window, &fw, &fh);
        state.camera.onResize(fw, fh);
        glViewport(0, 0, fw, fh);
    }
    state.path = scenePath;
    std::string err;
    const bool ok = editor0::loadSceneForEditor(scenePath, state.current, err);
    state.status = editor0::makeStatusLine(scenePath, state.current.entities.size(),
                                           ok ? std::string() : err, state.selected, state.current.entities);
    if (!ok) editor0::reportMessage(std::cerr, state.status);
    std::printf("diag-input: click entities; ONE line per click. ESC quits.\n");
    std::fflush(stdout);

    pe::Input input({GLFW_KEY_ESCAPE});
    int lastPickCount = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        const pe::MouseState m = pe::Input::pollMouse(window);
        editor0::EditorInput in;
        in.cursorX = m.x;
        in.cursorY = m.y;
        glfwGetWindowSize(window, &in.windowWidth, &in.windowHeight);
        glfwGetFramebufferSize(window, &in.fbWidth, &in.fbHeight);
        in.leftDown = m.left;
        in.keyEsc = input.isEdge(window, GLFW_KEY_ESCAPE);
        editor0::stepEditorFrame(state, in);
        input.update(window);
        if (state.quitRequested) glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (state.pickCount != lastPickCount) {
            lastPickCount = state.pickCount;
            std::printf("click: cursor=(%.1f,%.1f) window=(%d,%d) fb=(%d,%d) world=(%.4f,%.4f) pick=%d\n",
                        in.cursorX, in.cursorY, in.windowWidth, in.windowHeight, in.fbWidth, in.fbHeight,
                        state.lastClickWorldX, state.lastClickWorldY, state.lastPickIndex);
            std::fflush(stdout);
        }
        renderer.clear(0.0f, 0.0f, 0.0f);
        const std::vector<char> colliding = pe::flagsForCount(state.current.entities.size());
        const pe::Mat4 proj = editor0::zoomedProjection(state.zoom, state.camera.projection());
        renderer.drawWorld(proj, state.camera.view(), state.current.entities, colliding);
        renderer.drawAABBs(proj, state.camera.view(), state.current.entities, -1);
        const std::string& line = state.feedback.empty() ? state.status : state.feedback;
        renderer.drawTextString(line, -5.8f, 4.1f, proj, pe::TextAlign::Left);
        glfwSwapBuffers(window);
    }
    renderer.shutdown();
    return 0;
}

int main(int argc, char** argv) {
    bool selftest = false;
    bool makeSample = false;
    bool diagInput = false;
    const char* scenePath = nullptr;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--selftest") selftest = true;
        else if (a == "--make-sample") makeSample = true;
        else if (a == "--diag-input") diagInput = true;
        else scenePath = argv[i];
    }
    if (selftest) {
        checkPureParts();
        checkSampleAndPanZoom();  // Step 290: the sample generator + pan/zoom math
        checkReload();            // Step 291: the reload path
        checkHostileLoads();      // Step 291: the hostile loads
        checkSelectionAndPick();  // Step 294: selection + pick + gesture + print discipline
        checkLiveInputWiring();   // Step 295: the live-path wiring (synthetic input)
        checkInspectorPanel();    // Step 296: the read-only inspector panel
        checkHiddenWindowFrame(); // Step 289: one hidden-window frame
        std::remove(kTempScene);  // runtime output cleanup (build/ is gitignored)
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
    if (diagInput) {
        if (!scenePath) { std::fprintf(stderr, "editor0: --diag-input needs a scene path\n"); return 2; }
        return runDiagInput(scenePath);
    }
    if (scenePath) return runViewer(scenePath);
    std::printf("PureEditor0: no mode given. Usage:\n"
                "  PureEditor0 --selftest          headless checks (+ one hidden-window GL frame)\n"
                "  PureEditor0 --make-sample <p>   write the 3-prefab sample scene to <p> (refuses to overwrite)\n"
                "  PureEditor0 <scene>             view the scene (drag = pan, +/- = zoom, R = reload, ESC = quit)\n");
    return 2;
}
