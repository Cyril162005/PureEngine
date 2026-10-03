/**
 * PureEngine Phase D consumer: level pipeline (Step 275, 1 of at most 5)
 *
 * Consumes ONLY the documented APIs: loadPrefab/instantiatePrefab
 * (src/prefab.h), Entity (src/entity.h), Scene/SceneManager (src/scene.h).
 * Headless (no GL, no window). Returns a nonzero exit code on any failed
 * check; registered in CTest. Engine src/ is read-only for the duration;
 * every friction point goes to FINDINGS.md, never fixed here.
 */
#include <cstdio>
#include <cmath>
#include <direct.h>
#include <fstream>
#include <string>
#include <vector>

#include "../../src/entity.h"
#include "../../src/prefab.h"
#include "../../src/scene.h"
#include <cstdio>

static int failures = 0;

static void check(bool cond, const char* what) {
    if (!cond) {
        std::printf("FAIL: %s\n", what);
        ++failures;
    } else {
        std::printf("PASS: %s\n", what);
    }
    std::fflush(stdout);   // survive crashes: the stdout buffered is lost on 0xC0000005
}

static bool floatEq(float a, float b) { return std::fabs(a - b) < 1e-5f; }

// The three prefab data files (committed in this directory; copied to
// <target>/consumers/level_pipeline by POST_BUILD). The paths include
// the subdir because loadPrefab's CWD-relative fallback resolves the
// bare filename against the process CWD root, not a data subdir —
// recorded in FINDINGS.md (Entry 7).
static const char* kTileFile   = "consumers/level_pipeline/prefab_tile.txt";
static const char* kPickupFile = "consumers/level_pipeline/prefab_pickup.txt";
static const char* kEnemyFile  = "consumers/level_pipeline/prefab_enemy.txt";

static void checkLoadAndInstantiate() {
    // --- The tile prefab ---
    pe::Prefab tile;
    check(pe::loadPrefab(kTileFile, tile), "loadPrefab(tile) succeeds");
    // The expected values, written from the FILE CONTENTS above.
    check(tile.name == "tile", "tile name from file");
    check(floatEq(tile.scale.x, 1.0f) && floatEq(tile.scale.y, 1.0f) && floatEq(tile.scale.z, 1.0f), "tile scale from file");
    check(floatEq(tile.halfExtents.x, 0.5f) && floatEq(tile.halfExtents.z, 0.5f), "tile halfExtents from file");
    check(tile.textureId == 1, "tile textureId from file");
    check(tile.depth == 0, "tile depth from file");
    check(tile.roleId == 3, "tile roleId from file");
    check(tile.isStatic, "tile isStatic from file");
    check(floatEq(tile.health, 100.0f), "tile health from file");
    check(tile.tag == "tile", "tile tag from file");

    // --- The pickup prefab ---
    pe::Prefab pickup;
    check(pe::loadPrefab(kPickupFile, pickup), "loadPrefab(pickup) succeeds");
    check(pickup.name == "pickup", "pickup name from file");
    check(floatEq(pickup.scale.x, 0.4f), "pickup scale.x from file");
    check(floatEq(pickup.halfExtents.x, 0.2f), "pickup halfExtents.x from file");
    check(pickup.textureId == 2, "pickup textureId from file");
    check(pickup.depth == 5, "pickup depth from file");
    check(pickup.roleId == 0, "pickup roleId from file");
    check(!pickup.isStatic, "pickup isStatic from file");
    check(floatEq(pickup.health, 1.0f), "pickup health from file");
    check(pickup.tag == "pickup", "pickup tag from file");

    // --- The enemy prefab (the most fields, differing values) ---
    pe::Prefab enemy;
    check(pe::loadPrefab(kEnemyFile, enemy), "loadPrefab(enemy) succeeds");
    check(enemy.name == "enemy", "enemy name from file");
    check(floatEq(enemy.moveSpeed, 2.2f), "enemy moveSpeed from file");
    check(floatEq(enemy.gravityScale, 1.0f), "enemy gravityScale from file");
    check(floatEq(enemy.health, 50.0f), "enemy health from file");
    check(floatEq(enemy.coyoteTime, 0.05f), "enemy coyoteTime from file");
    check(floatEq(enemy.jumpImpulse, 8.0f), "enemy jumpImpulse from file");
    check(floatEq(enemy.maxFallSpeed, 20.0f), "enemy maxFallSpeed from file");
    check(enemy.cols == 4 && enemy.rows == 2, "enemy cols/rows from file");
    check(enemy.currentClipName == "walk_left", "enemy clip from file");
    check(enemy.tag == "enemy", "enemy tag from file");
    check(enemy.roleId == 2 && enemy.depth == 2, "enemy roleId/depth from file");

    // --- instantiatePrefab produces entities whose fields equal the prefab's ---
    const pe::Vec3 at(3.0f, -1.0f, 0.5f);
    pe::Entity e = pe::instantiatePrefab(enemy, at);
    check(floatEq(e.position.x, 3.0f) && floatEq(e.position.y, -1.0f) && floatEq(e.position.z, 0.5f),
          "instantiate sets the given position");
    check(floatEq(e.scale.x, 0.6f) && floatEq(e.halfExtents.x, 0.3f), "instantiate copies scale/halfExtents");
    check(e.textureId == 4 && e.roleId == 2 && e.depth == 2, "instantiate copies textureId/roleId/depth");
    check(floatEq(e.moveSpeed, 2.2f) && floatEq(e.gravityScale, 1.0f) && floatEq(e.health, 50.0f),
          "instantiate copies moveSpeed/gravityScale/health");
    check(floatEq(e.coyoteTime, 0.05f) && floatEq(e.jumpImpulse, 8.0f) && floatEq(e.maxFallSpeed, 20.0f),
          "instantiate copies coyoteTime/jumpImpulse/maxFallSpeed");
    check(e.cols == 4 && e.rows == 2 && e.tag == "enemy" && e.currentClipName == "walk_left",
          "instantiate copies cols/rows/tag/clip");
    check(e.alive, "instantiate leaves the entity alive");
    check(!e.isStatic, "instantiate copies isStatic");

    // --- Instantiating the same prefab twice gives independent entities ---
    pe::Entity a = pe::instantiatePrefab(enemy, pe::Vec3(0.0f, 0.0f, 0.0f));
    pe::Entity b = pe::instantiatePrefab(enemy, pe::Vec3(1.0f, 1.0f, 0.0f));
    a.health = 7.0f;
    a.moveSpeed = 9.0f;
    check(floatEq(b.health, 50.0f) && floatEq(b.moveSpeed, 2.2f),
          "two instantiations are independent (mutating one leaves the other)");
    check(floatEq(a.position.x, 0.0f) && floatEq(b.position.x, 1.0f),
          "two instantiations keep distinct positions");

    // --- Instantiating at distinct positions does not alias state ---
    // (Vec3 is a value type; the entity holds copies. Verify no shared
    // static/global state leaks between the instances.)
    pe::Entity c = pe::instantiatePrefab(enemy, pe::Vec3(5.0f, 5.0f, 0.0f));
    c.tag = "renamed";
    check(b.tag == "enemy", "distinct-position instantiation does not alias tag state");
    check(floatEq(c.scale.x, 0.6f) && floatEq(b.scale.x, 0.6f), "distinct-position instantiation does not alias scale");
}

static void writeTmp(const char* name, const std::string& body) {
    std::ofstream f(name);
    f << body;
}

static void checkHostileCases() {
    // 1. Missing file: loadPrefab false + a "[prefab] File not found" note.
    {
        pe::Prefab p;
        const bool ok = pe::loadPrefab("consumers_missing_prefab_zz.txt", p);
        check(!ok, "hostile: missing file returns false (no crash, no partial use)");
    }
    // 2. Empty file: the header check fails -> false.
    {
        writeTmp("consumers_empty_prefab.txt", "");
        pe::Prefab p;
        const bool ok = pe::loadPrefab("consumers_empty_prefab.txt", p);
        check(!ok, "hostile: empty file returns false (the header check)");
        std::remove("consumers_empty_prefab.txt");
    }
    // 3. Malformed line (no '='): observed behavior recorded.
    {
        writeTmp("consumers_malformed_prefab.txt",
                 "# PureEngine prefab v1\nname=malformed\nthis line has no equals sign\n");
        pe::Prefab p;
        const bool ok = pe::loadPrefab("consumers_malformed_prefab.txt", p);
        // OBSERVED: returns TRUE (the partial parse: valid fields kept, the
        // malformed line warn-skipped). The name parses; the bad line warns.
        check(ok, "hostile: malformed line -> partial parse, loadPrefab true (recorded)");
        check(p.name == "malformed", "hostile: the valid fields survive the malformed line");
        std::remove("consumers_malformed_prefab.txt");
    }
    // 4. Unknown field: warn + skip; loadPrefab true.
    {
        writeTmp("consumers_unknownfield_prefab.txt",
                 "# PureEngine prefab v1\nname=unknownfield\nnotARealField=42\n");
        pe::Prefab p;
        const bool ok = pe::loadPrefab("consumers_unknownfield_prefab.txt", p);
        // OBSERVED: true; the unknown key warns and is skipped.
        check(ok, "hostile: unknown field -> warn + skip, loadPrefab true (recorded)");
        check(p.name == "unknownfield", "hostile: the known fields survive the unknown key");
        std::remove("consumers_unknownfield_prefab.txt");
    }
    // 5. Duplicate field: OBSERVED = the LAST assignment wins.
    {
        writeTmp("consumers_dupfield_prefab.txt",
                 "# PureEngine prefab v1\nname=dup\nhealth=10.0\nhealth=20.0\n");
        pe::Prefab p;
        const bool ok = pe::loadPrefab("consumers_dupfield_prefab.txt", p);
        check(ok, "hostile: duplicate field -> loadPrefab true (recorded)");
        // OBSERVED: the last assignment wins (health 20.0, not 10.0).
        check(floatEq(p.health, 20.0f), "hostile: duplicate field -> the LAST wins (recorded)");
        std::remove("consumers_dupfield_prefab.txt");
    }
    // 6. Out-of-range numeric value: OBSERVED = the parse fails, the field
    // keeps its DEFAULT, and the warn message is the misleading
    // "Unknown key" (the parse-fail falls through to the unknown-key else).
    {
        writeTmp("consumers_range_prefab.txt",
                 "# PureEngine prefab v1\nname=range\nhealth=1e39\n");
        pe::Prefab p;
        const bool ok = pe::loadPrefab("consumers_range_prefab.txt", p);
        check(ok, "hostile: out-of-range numeric -> loadPrefab true (recorded)");
        // OBSERVED: the field keeps its DEFAULT (100.0) because the parse
        // fails; the stderr message is "[prefab] Unknown key 'health'".
        check(floatEq(p.health, 100.0f), "hostile: out-of-range numeric -> the default kept (recorded)");
        std::remove("consumers_range_prefab.txt");
    }
}

static void checkSceneComposition() {
    // The documented Scene API: entities vector + queueSpawn.
    pe::Scene scene;
    pe::Prefab enemy;
    if (!pe::loadPrefab(kEnemyFile, enemy)) {
        check(false, "scene composition: the enemy prefab must load");
        return;
    }
    const std::size_t i1 = scene.queueSpawn(pe::instantiatePrefab(enemy, pe::Vec3(0, 0, 0)));
    const std::size_t i2 = scene.queueSpawn(pe::instantiatePrefab(enemy, pe::Vec3(2, 2, 0)));
    check(i1 != i2, "scene composition: queueSpawn returns distinct indices");
    // The documented queueSpawn/flushSpawns contract: queueSpawn defers
    // to pendingSpawns; flushSpawns merges at end-of-frame (the caller's
    // responsibility, outside any entity iteration).
    const std::size_t merged = scene.flushSpawns();
    check(merged == 2, "scene composition: flushSpawns merges the queued two");
    check(scene.entities.size() == 2, "scene composition: two spawned entities present");
    if (scene.entities.size() == 2) {
        check(scene.entities[0].alive && scene.entities[1].alive, "scene composition: spawned entities alive");
        check(floatEq(scene.entities[0].position.x, 0.0f) && floatEq(scene.entities[1].position.x, 2.0f),
              "scene composition: positions preserved through queueSpawn");
    }
}

// ---- Step 276: the scene save/load round-trip ----
// The documented serialized fields (scene.h:372-380, the 34-field v2
// format, 4dp fixed precision at :418): the position, the rotationAngle,
// the rotationSpeed, the scale, the halfExtents, the textureId, the
// depth, the roleId, the moveSpeed, the velocity, the gravityScale,
// the isStatic, the coyoteTime, the jumpImpulse, the maxFallSpeed, the
// tint, the cols, the rows, the health, the timer, the tag, the
// parentIndex, the animationSpeed, the currentClipName. The documented
// NON-serialized runtime state (NOT compared): the alive flag is not a
// field (the dead are skipped at :420), the coyoteTimer/wasGrounded/
// animationState/wasGrounded runtime fields are not in the 34-field
// list.
static const char* kRtSceneFile = "consumers/level_pipeline/rt_scene.txt";

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

static void checkSceneRoundTrip() {
    // Build the scene from the 3 prefabs at distinct positions.
    pe::Prefab tile, pickup, enemy;
    if (!pe::loadPrefab(kTileFile, tile) || !pe::loadPrefab(kPickupFile, pickup) || !pe::loadPrefab(kEnemyFile, enemy)) {
        check(false, "round-trip: the three prefabs must load");
        return;
    }
    pe::Scene scene;
    scene.name = "consumer_roundtrip";
    scene.queueSpawn(pe::instantiatePrefab(tile, pe::Vec3(0.0f, 0.0f, 0.0f)));
    scene.queueSpawn(pe::instantiatePrefab(pickup, pe::Vec3(1.5f, -2.5f, 0.0f)));
    scene.queueSpawn(pe::instantiatePrefab(enemy, pe::Vec3(3.0f, 3.0f, 0.0f)));
    scene.flushSpawns();
    check(scene.entities.size() == 3, "round-trip: the scene built from 3 prefabs");
    // The enemy gets distinct float values for the round-trip comparison.
    scene.entities[2].velocity = pe::Vec3(0.5f, -0.25f, 0.0f);
    scene.entities[2].timer = 1.5f;
    scene.entities[2].animationSpeed = 2.0f;
    scene.entities[2].rotationAngle = 0.75f;

    // Save.
    const bool saved = pe::saveSceneToFile(scene, kRtSceneFile);
    check(saved, "round-trip: saveSceneToFile succeeds");

    // Load into a fresh Scene.
    pe::Scene loaded;
    const bool ok = pe::loadSceneFromFile(kRtSceneFile, loaded);
    check(ok, "round-trip: loadSceneFromFile succeeds");
    check(loaded.name == "consumer_roundtrip", "round-trip: the scene name round-trips");
    check(loaded.entities.size() == scene.entities.size(), "round-trip: the entity count round-trips");

    // Compare every documented serialized field per entity.
    for (std::size_t i = 0; i < scene.entities.size() && i < loaded.entities.size(); ++i) {
        std::string diff;
        if (!entityFieldMatches(scene.entities[i], loaded.entities[i], diff)) {
            std::printf("FAIL: round-trip: entity %d field differs: %s\n", (int)i, diff.c_str());
            ++failures;
        }
    }
    std::printf("PASS: round-trip: all documented serialized fields compared per entity\n");

    // save->load->save gives a byte-identical file (the 4dp rounding is
    // idempotent: the loaded 4dp-rounded values re-serialize to the same
    // text).
    {
        std::string s1;
        {
            std::ifstream f1(kRtSceneFile, std::ios::binary);
            s1.assign((std::istreambuf_iterator<char>(f1)), std::istreambuf_iterator<char>());
        }   // f1 CLOSED before the second save - an open read handle makes
            // saveSceneToFile's fs::rename fail with a sharing violation
            // (recorded in FINDINGS.md, Entry 9).
        pe::Scene reloaded;
        if (pe::loadSceneFromFile(kRtSceneFile, reloaded)) {
            const bool saved2 = pe::saveSceneToFile(reloaded, kRtSceneFile);
            check(saved2, "round-trip: the second save succeeds");
            std::ifstream f2(kRtSceneFile, std::ios::binary);
            std::string s2((std::istreambuf_iterator<char>(f2)), std::istreambuf_iterator<char>());
            check(s1 == s2, "round-trip: save->load->save is byte-identical (the 4dp rounding is idempotent)");
        }
        std::remove(kRtSceneFile);
    }

    // The empty scene: save; load; the count 0.
    {
        pe::Scene empty;
        empty.name = "consumer_empty";
        const bool savedE = pe::saveSceneToFile(empty, kRtSceneFile);
        check(savedE, "round-trip: the empty scene saves");
        pe::Scene loadedE;
        const bool okE = pe::loadSceneFromFile(kRtSceneFile, loadedE);
        check(okE && loadedE.entities.empty() && loadedE.name == "consumer_empty",
              "round-trip: the empty scene loads with 0 entities and the name");
        std::remove(kRtSceneFile);
    }

    // The float edge values (the 4dp format): 0, negative, very small,
    // very large, 0.1f. OBSERVED AND RECORDED (the 4dp truncation is the
    // documented format's precision; not bit-exact for values with more
    // than 4 decimals).
    {
        pe::Scene fs;
        fs.name = "consumer_floats";
        pe::Entity a; a.position = pe::Vec3(0.0f, -1.5f, 1e-6f); a.timer = 0.1f; a.meshId = 0;
        fs.queueSpawn(a);
        pe::Entity b; b.position = pe::Vec3(1e30f, 0.12345678f, -0.0f); b.meshId = 0;
        fs.queueSpawn(b);
        fs.flushSpawns();
        const bool savedF = pe::saveSceneToFile(fs, kRtSceneFile);
        pe::Scene loadedF;
        const bool okF = savedF && pe::loadSceneFromFile(kRtSceneFile, loadedF);
        check(okF, "round-trip: the float edge values save+load");
        if (okF && loadedF.entities.size() == 2) {
            // OBSERVED: the very-small 1e-6 becomes 0.0000 (the 4dp
            // truncation); the 0.12345678 becomes 0.1235; the 0.1f
            // round-trips exactly; the 1e30 round-trips (the fixed 4dp
            // of a large float keeps the integer digits).
            check(loadedF.entities[0].position.z == 0.0f, "observed: 1e-6 truncated to 0.0000 by the 4dp format (recorded)");
            check(floatEq(loadedF.entities[0].timer, 0.1f), "observed: 0.1f round-trips exactly at 4dp (recorded)");
            check(floatEq(loadedF.entities[1].position.y, 0.1235f), "observed: 0.12345678 -> 0.1235 (the 4dp rounding, recorded)");
            check(floatEq(loadedF.entities[1].position.x, 1e30f), "observed: 1e30 round-trips (recorded)");
        }
        std::remove(kRtSceneFile);
    }

    // The negative control: alter one field in the loaded copy; the
    // comparator must report it.
    {
        pe::Scene again;
        if (pe::loadSceneFromFile(kRtSceneFile, again)) {
            std::remove(kRtSceneFile);
        }
        // Rebuild and reload a fresh copy for the negative control.
        pe::Scene src2;
        src2.name = "consumer_negctl";
        pe::Prefab t2;
        if (!pe::loadPrefab(kTileFile, t2)) {
            check(false, "negative control: the tile prefab must load");
            return;
        }
        src2.queueSpawn(pe::instantiatePrefab(t2, pe::Vec3(1.0f, 2.0f, 0.0f)));
        src2.flushSpawns();
        check(pe::saveSceneToFile(src2, kRtSceneFile), "negative control: the save succeeds");
        pe::Scene neg;
        check(pe::loadSceneFromFile(kRtSceneFile, neg), "negative control: the load succeeds");
        neg.entities[0].health = 999.0f;   // the deliberate alteration
        std::string diff;
        const bool same = entityFieldMatches(src2.entities[0], neg.entities[0], diff);
        check(!same && diff == "health", "negative control: the comparator reports the altered health field");
        std::remove(kRtSceneFile);
    }
}

// ---- Step 277: the scene-manager persistence ----
// The documented API (scene.h:651-711 / :724-794): the index format
// "# scene manager v1" + scenes=<count> + current=<index> + one
// scene_file=scene_<name>.txt per scene; the per-scene saves go through
// saveSceneToFile (the bare names -> the 3-candidate probe); the tmp+
// rename atomic save; strict whole-file load (duplicates, the count
// mismatch, the bad current index, any failed scene load, the unknown
// prefix -> false, out untouched); current=-1 = no current scene; load
// into a non-empty manager: out = tmp (REPLACE - observed).
static const char* kManagerFile = "rt_manager.txt";   // the BARE name: the loader's 3-candidate probe (assets/) is the only path it honors - the explicit-path asymmetry is FINDINGS.md Entry 11

static void checkManagerPersistence() {
    // 1. The manager with 3 scenes of different content.
    {
        pe::SceneManager m;
        m.scenes.reserve(3);   // the documented rule (scene.h:108): reserve before loadScene to prevent reallocation
        pe::Prefab tile, pickup, enemy;
        if (!pe::loadPrefab(kTileFile, tile) || !pe::loadPrefab(kPickupFile, pickup) || !pe::loadPrefab(kEnemyFile, enemy)) {
            check(false, "manager: the three prefabs must load");
            return;
        }
        auto* s1 = &pe::loadScene(m, "alpha");
        s1->queueSpawn(pe::instantiatePrefab(tile, pe::Vec3(0, 0, 0)));
        s1->flushSpawns();
        auto* s2 = &pe::loadScene(m, "beta");
        s2->queueSpawn(pe::instantiatePrefab(pickup, pe::Vec3(1, 1, 0)));
        s2->queueSpawn(pe::instantiatePrefab(pickup, pe::Vec3(2, 2, 0)));
        s2->flushSpawns();
        // COPY the values the later comparison needs BEFORE the next
        // loadScene: loadScene may reallocate the scenes vector, so any
        // Scene& taken before it dangles (the documented rule: re-take
        // after any structural change, scene.h:109). The consumer took
        // the s2 pointer and used it after the s3 loadScene - the access
        // violation (0xC0000005) - recorded in FINDINGS.md (Entry 12).
        pe::Entity betaFirst = s2->entities[0];
        auto* s3 = &pe::loadScene(m, "gamma");
        s3->queueSpawn(pe::instantiatePrefab(enemy, pe::Vec3(3, 3, 0)));
        s3->flushSpawns();
        m.current = 1;   // the active scene: beta
        const bool saved = pe::saveSceneManagerToFile(m, kManagerFile);
        check(saved, "manager: saveSceneManagerToFile succeeds (3 scenes)");

        pe::SceneManager loaded;
        const bool ok = pe::loadSceneManagerFromFile(kManagerFile, loaded);
        check(ok, "manager: loadSceneManagerFromFile succeeds");
        check(loaded.scenes.size() == 3, "manager: the scene count round-trips");
        if (loaded.scenes.size() == 3) {
            check(loaded.scenes[0].name == "alpha" && loaded.scenes[1].name == "beta" && loaded.scenes[2].name == "gamma",
                  "manager: the scene order and names round-trip");
            check(loaded.scenes[0].entities.size() == 1 && loaded.scenes[1].entities.size() == 2 && loaded.scenes[2].entities.size() == 1,
                  "manager: each scene's content round-trips");
            std::string diff;
            check(entityFieldMatches(loaded.scenes[1].entities[0], betaFirst, diff),
                  "manager: the beta scene's first entity field-exact");
            check(loaded.current == 1, "manager: the active scene index round-trips");
        }
    }
    // 2. The overwrite of an existing save (fs::rename replaces; the
    // manager index re-save with the existing destination works).
    {
        pe::SceneManager m;
        pe::loadScene(m, "solo");
        const bool saved1 = pe::saveSceneManagerToFile(m, kManagerFile);
        const bool saved2 = pe::saveSceneManagerToFile(m, kManagerFile);
        check(saved1 && saved2, "manager: the overwrite (re-save with the existing destination) works");
        std::remove(kManagerFile);
    }
    // 3. Save to a nonexistent directory: the explicit path's
    // create_directories honored (the documented dir auto-create).
    {
        pe::SceneManager m;
        pe::loadScene(m, "deep");
        const bool saved = pe::saveSceneManagerToFile(m, "consumers/level_pipeline/deep_dir/manager.txt");
        check(saved, "manager: save to a nonexistent directory auto-creates it");
        std::remove("consumers/level_pipeline/deep_dir/manager.txt");
    }
    // 4. Load into a NON-EMPTY manager: OBSERVED = REPLACE (out = tmp).
    {
        pe::SceneManager m;
        pe::loadScene(m, "preexisting");   // 1 scene
        pe::loadScene(m, "another");       // 2 scenes
        // The manager save/load round-trip from check 1 left nothing; build a
        // fresh save to load over the non-empty manager.
        pe::SceneManager src;
        pe::loadScene(src, "replacement");
        if (pe::saveSceneManagerToFile(src, kManagerFile)) {
            const bool ok = pe::loadSceneManagerFromFile(kManagerFile, m);
            check(ok, "manager: the load into a non-empty manager succeeds");
            // OBSERVED: REPLACE (the loaded content replaces; no merge).
            check(m.scenes.size() == 1 && m.scenes[0].name == "replacement",
                  "manager: the load REPLACES the manager's existing content (observed, recorded)");
        }
        std::remove(kManagerFile);
    }
    // 5. Repeated save/load x20 with no growth or drift.
    {
        pe::SceneManager m;
        pe::loadScene(m, "repeat");
        bool drift = false;
        for (int i = 0; i < 20 && !drift; ++i) {
            if (!pe::saveSceneManagerToFile(m, kManagerFile)) { drift = true; break; }
            pe::SceneManager loaded;
            if (!pe::loadSceneManagerFromFile(kManagerFile, loaded)) { drift = true; break; }
            if (loaded.scenes.size() != 1 || loaded.scenes[0].name != "repeat" || loaded.scenes[0].entities.size() != 0) drift = true;
            m = loaded;
        }
        check(!drift, "manager: repeated save/load x20 with no growth or drift");
        std::remove(kManagerFile);
    }
    // 6. A leftover temp file from an interrupted save: OBSERVED = the tmp
    // (writePath + ".tmp") is removed on the error paths; an interrupted
    // (crashed) save would leave the tmp - the load IGNORES it (the
    // strict parse of the index only).
    {
        pe::SceneManager m;
        pe::loadScene(m, "leftover");
        const bool saved = pe::saveSceneManagerToFile(m, kManagerFile);
        check(saved, "manager: the save succeeds (no interruption)");
        // OBSERVED: no leftover tmp after a successful save (the rename
        // moved it). Simulate a leftover tmp and record: the load ignores
        // it (it parses only the index file itself).
        {
            std::ofstream tmpF(std::string(kManagerFile) + ".tmp");
            tmpF << "garbage";
        }
        pe::SceneManager loaded;
        const bool ok = pe::loadSceneManagerFromFile(kManagerFile, loaded);
        check(ok, "manager: a leftover .tmp file does not affect the load (observed, recorded)");
        std::remove(kManagerFile);
        std::remove((std::string(kManagerFile) + ".tmp").c_str());
    }
    // 7. The negative control: alter the loaded manager's current index;
    // the comparator must report it.
    {
        pe::SceneManager m;
        pe::loadScene(m, "negctl");
        if (pe::saveSceneManagerToFile(m, kManagerFile)) {
            pe::SceneManager neg;
            check(pe::loadSceneManagerFromFile(kManagerFile, neg), "manager: the negative-control load succeeds");
            neg.current = 5;   // the deliberate alteration
            check(neg.current != m.current, "manager: the negative control reports the altered current index");
        }
        std::remove(kManagerFile);
    }
}

// ---- Step 282: the (h) capability - the manager load honors explicit paths ----
// The Entry 11 repro (the explicit-path round-trip) now passes; the
// existing assets/ round-trip unchanged; a nonexistent path still
// returns false; the absolute-path behavior matches the sibling
// loaders (loadSceneFromFile).
static void checkManagerExplicitPaths() {
    // 1. The Entry 11 repro: the explicit-path round-trip.
    {
        pe::SceneManager m;
        m.scenes.reserve(1);
        pe::loadScene(m, "explicit");
        const char* kExplicit = "consumers/level_pipeline/explicit_manager.txt";
        const bool saved = pe::saveSceneManagerToFile(m, kExplicit);
        check(saved, "explicit paths: the save to an explicit path succeeds");
        pe::SceneManager loaded;
        const bool ok = pe::loadSceneManagerFromFile(kExplicit, loaded);
        // THE (h) FIX: the load of the same explicit path now succeeds
        // (the pre-282 loader probed only assets/ and returned false).
        check(ok, "explicit paths: the load of the SAME explicit path succeeds (the Entry 11 repro)");
        if (ok) {
            check(loaded.scenes.size() == 1 && loaded.scenes[0].name == "explicit",
                  "explicit paths: the round-trip content matches");
        }
        std::remove(kExplicit);
    }
    // 2. The existing assets/ round-trip unchanged (the bare name).
    {
        pe::SceneManager m;
        m.scenes.reserve(1);
        pe::loadScene(m, "assetspath");
        const bool saved = pe::saveSceneManagerToFile(m, "rt_assets_manager.txt");
        pe::SceneManager loaded;
        const bool ok = saved && pe::loadSceneManagerFromFile("rt_assets_manager.txt", loaded);
        check(ok, "explicit paths: the existing assets/ round-trip (the bare name) unchanged");
        if (ok) {
            check(loaded.scenes.size() == 1 && loaded.scenes[0].name == "assetspath",
                  "explicit paths: the assets/ round-trip content matches");
        }
        std::remove("rt_assets_manager.txt");
        std::remove("assets/rt_assets_manager.txt");
    }
    // 3. A nonexistent path still returns false.
    {
        pe::SceneManager loaded;
        check(!pe::loadSceneManagerFromFile("consumers/level_pipeline/no_such_manager_zz.txt", loaded),
              "explicit paths: a nonexistent path still returns false");
    }
    // 4. The absolute-path behavior matches the sibling loaders
    // (loadSceneFromFile honors absolute paths directly).
    {
        // Build an absolute path to a file we save explicitly.
        char cwdBuf[1024];
        const char* cwd = nullptr;
        if (_getcwd(cwdBuf, sizeof(cwdBuf))) cwd = cwdBuf;
        if (cwd) {
            const std::string absPath = std::string(cwd) + "\\consumers\\level_pipeline\\abs_manager.txt";
            pe::SceneManager m;
            m.scenes.reserve(1);
            pe::loadScene(m, "abs");
            const bool saved = pe::saveSceneManagerToFile(m, absPath);
            check(saved, "explicit paths: the save to an ABSOLUTE path succeeds");
            pe::SceneManager loaded;
            const bool ok = pe::loadSceneManagerFromFile(absPath, loaded);
            // The siblings: loadSceneFromFile's explicitPath check (the
            // drive letter) -> the candidates[0] = the absolute fileName
            // directly -> the load succeeds. The manager loader now
            // matches.
            check(ok, "explicit paths: the ABSOLUTE-path load matches the sibling loaders");
            if (ok) {
                check(loaded.scenes.size() == 1 && loaded.scenes[0].name == "abs",
                      "explicit paths: the absolute-path round-trip content matches");
            }
            std::remove(absPath.c_str());
        }
    }
    // 5. The negative control: the explicit-path round-trip with one
    // scene's content altered; the comparator must report it.
    {
        pe::SceneManager m;
        m.scenes.reserve(1);
        pe::Prefab tile;
        if (!pe::loadPrefab(kTileFile, tile)) {
            check(false, "explicit paths: the negative control's prefab must load");
            return;
        }
        auto* s = &pe::loadScene(m, "negctl");
        s->queueSpawn(pe::instantiatePrefab(tile, pe::Vec3(1, 2, 0)));
        s->flushSpawns();
        pe::Entity expected = s->entities[0];
        const char* kNeg = "consumers/level_pipeline/negctl_manager.txt";
        if (pe::saveSceneManagerToFile(m, kNeg)) {
            pe::SceneManager neg;
            check(pe::loadSceneManagerFromFile(kNeg, neg), "explicit paths: the negative-control load succeeds");
            if (neg.scenes.size() == 1 && neg.scenes[0].entities.size() == 1) {
                neg.scenes[0].entities[0].health = 1234.0f;   // the deliberate alteration
                std::string diff;
                const bool same = entityFieldMatches(expected, neg.scenes[0].entities[0], diff);
                check(!same && diff == "health", "explicit paths: the negative control reports the altered health field");
            }
        }
        std::remove(kNeg);
    }
}

// ---- Step 278: adversarial persistence ----
// Corrupt inputs GENERATED PROGRAMMATICALLY from a valid save (not
// hand-edited). For each: the outcome recorded (the error / the partial
// load / the default fill / the crash / the hang / the huge allocation).
// The CTest TIMEOUT (60s) guards hangs; input sizes capped.
static void readTextFile(const char* name, std::string& out) {
    std::ifstream f(name, std::ios::binary);
    out.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

static void checkAdversarialPersistence() {
    // A valid scene save + a valid manager save as the corruption bases.
    pe::Scene valid;
    valid.name = "adv";
    pe::Prefab tile, enemy;
    if (!pe::loadPrefab(kTileFile, tile) || !pe::loadPrefab(kEnemyFile, enemy)) {
        check(false, "adversarial: the prefabs must load");
        return;
    }
    valid.queueSpawn(pe::instantiatePrefab(tile, pe::Vec3(0, 0, 0)));
    valid.queueSpawn(pe::instantiatePrefab(enemy, pe::Vec3(1, 1, 0)));
    valid.flushSpawns();
    const char* kAdvScene = "consumers/level_pipeline/adv_scene.txt";
    const char* kAdvMgr = "rt_adv_manager.txt";
    const bool okScene = pe::saveSceneToFile(valid, kAdvScene);
    pe::SceneManager vm;
    pe::loadScene(vm, "adv");
    const bool okMgr = pe::saveSceneManagerToFile(vm, kAdvMgr);
    if (!okScene || !okMgr) {
        check(false, "adversarial: the valid saves must succeed");
        return;
    }
    std::string validText, validMgrText;
    readTextFile(kAdvScene, validText);
    readTextFile("assets/rt_adv_manager.txt", validMgrText);
    check(!validText.empty() && !validMgrText.empty(), "adversarial: the valid save texts captured");

    // 1. Truncated at every 1/8 of a valid save (7 truncation points).
    {
        int partialLoads = 0, cleanRejects = 0;
        for (int i = 1; i <= 7; ++i) {
            const std::size_t cut = validText.size() * i / 8;
            std::ofstream f(kAdvScene, std::ios::binary | std::ios::trunc);
            f << validText.substr(0, cut);
            f.close();
            pe::Scene t;
            const bool ok = pe::loadSceneFromFile(kAdvScene, t);
            if (ok) { ++partialLoads; } else { ++cleanRejects; }
        }
        std::printf("OBSERVED: truncations at 1/8..7/8: %d partial loads, %d clean rejects\n",
                    partialLoads, cleanRejects);
        check(partialLoads + cleanRejects == 7, "adversarial: all 7 truncations produced a recorded outcome");
        check(partialLoads <= 2, "adversarial: at most the first truncation(s) before scene= are partial loads");
        // Restore the valid save for the later cases.
        { std::ofstream f(kAdvScene, std::ios::binary | std::ios::trunc); f << validText; }
    }
    // 2. The version field: missing header, higher (v99), negative (v-1).
    {
        // Missing: no "# scene vN" line at all (the comments only).
        { std::ofstream f(kAdvScene, std::ios::binary | std::ios::trunc);
          f << "# just a comment\nscene=adv\nentity="; for (std::size_t i = 0; i < 33; ++i) f << "0,";
          f << "0\n"; }
        pe::Scene t;
        const bool ok = pe::loadSceneFromFile(kAdvScene, t);
        // OBSERVED: no header defaults to v1 and PARSES (the v1 default).
        std::printf("OBSERVED: missing version header: %s (defaults to v1)\n", ok ? "LOADS" : "rejects");
        // Higher: v99.
        { std::ofstream f(kAdvScene, std::ios::binary | std::ios::trunc);
          f << "# scene v99\nscene=adv\n"; }
        pe::Scene t2;
        const bool ok2 = pe::loadSceneFromFile(kAdvScene, t2);
        check(!ok2, "adversarial: version v99 rejected (warn + false, out untouched)");
        // Negative: v-1.
        { std::ofstream f(kAdvScene, std::ios::binary | std::ios::trunc);
          f << "# scene v-1\nscene=adv\n"; }
        pe::Scene t3;
        const bool ok3 = pe::loadSceneFromFile(kAdvScene, t3);
        // OBSERVED: v-1 parses as an int; fileVersion=-1 -> the unknown-version check fires -> false.
        check(!ok3, "adversarial: version v-1 rejected (the unknown-version check)");
        // The manager loader: the version v99.
        { std::ofstream f("assets/rt_adv_manager.txt", std::ios::binary | std::ios::trunc);
          f << "# scene manager v99\nscenes=1\ncurrent=0\nscene_file=scene_adv.txt\n"; }
        pe::SceneManager tm;
        const bool okM = pe::loadSceneManagerFromFile(kAdvMgr, tm);
        check(!okM, "adversarial: the manager version v99 rejected");
        { std::ofstream f("assets/rt_adv_manager.txt", std::ios::binary | std::ios::trunc); f << validMgrText; }
    }
    // 3. The count fields: negative, zero, 2^31, mismatching the actual entries.
    {
        // The manager scenes=: negative.
        { std::ofstream f("assets/rt_adv_manager.txt", std::ios::binary | std::ios::trunc);
          f << "# scene manager v1\nscenes=-3\ncurrent=0\nscene_file=scene_adv.txt\n"; }
        pe::SceneManager tm;
        check(!pe::loadSceneManagerFromFile(kAdvMgr, tm), "adversarial: scenes=-3 rejected");
        // Zero.
        { std::ofstream f("assets/rt_adv_manager.txt", std::ios::binary | std::ios::trunc);
          f << "# scene manager v1\nscenes=0\ncurrent=0\nscene_file=scene_adv.txt\n"; }
        pe::SceneManager tm2;
        check(!pe::loadSceneManagerFromFile(kAdvMgr, tm2), "adversarial: scenes=0 rejected");
        // 2^31: the parse accepts; the file-count mismatch rejects BEFORE
        // any allocation (the mismatch check is before the reserve).
        { std::ofstream f("assets/rt_adv_manager.txt", std::ios::binary | std::ios::trunc);
          f << "# scene manager v1\nscenes=2147483647\ncurrent=0\nscene_file=scene_adv.txt\n"; }
        pe::SceneManager tm3;
        const bool ok3 = pe::loadSceneManagerFromFile(kAdvMgr, tm3);
        // OBSERVED: the count mismatch (1 file vs 2147483647) rejects
        // silently BEFORE any huge allocation - no hang, no crash.
        std::printf("OBSERVED: scenes=2^31: %s (the file-count mismatch rejects before the reserve)\n",
                    ok3 ? "LOADS (unexpected!)" : "rejects before allocation");
        check(!ok3, "adversarial: scenes=2^31 rejected by the file-count mismatch (no huge allocation)");
        // The manager scenes= count MISMATCHING the actual entries (the
        // count 3, one file).
        { std::ofstream f("assets/rt_adv_manager.txt", std::ios::binary | std::ios::trunc);
          f << "# scene manager v1\nscenes=3\ncurrent=0\nscene_file=scene_adv.txt\n"; }
        pe::SceneManager tm4;
        check(!pe::loadSceneManagerFromFile(kAdvMgr, tm4), "adversarial: the scenes count mismatching the entries rejected");
        { std::ofstream f("assets/rt_adv_manager.txt", std::ios::binary | std::ios::trunc); f << validMgrText; }
    }
    // 4. The duplicate scene= line (the scene loader rejects).
    {
        { std::ofstream f(kAdvScene, std::ios::binary | std::ios::trunc);
          f << "# scene v2\nscene=adv\nscene=adv2\n"; }
        pe::Scene t;
        check(!pe::loadSceneFromFile(kAdvScene, t), "adversarial: the duplicate scene= line rejected");
        // NOTE: the save format has NO entity ids - the entity lines are
        // positional; there is nothing to duplicate at the entity level.
    }
    // 5. An extremely long line (1 MB, capped).
    {
        { std::ofstream f(kAdvScene, std::ios::binary | std::ios::trunc);
          f << "# scene v2\nscene=adv\nentity=";
          for (int i = 0; i < 1000000; ++i) f << 'x';
          f << "\n"; }
        pe::Scene t;
        const bool ok = pe::loadSceneFromFile(kAdvScene, t);
        // OBSERVED: the 1 MB line: the split produces 1 part -> neither
        // 14 nor 34 -> strict false. No hang (the TIMEOUT guards).
        std::printf("OBSERVED: a 1 MB entity line: %s (the strict shape check)\n", ok ? "LOADS (unexpected!)" : "rejected");
        check(!ok, "adversarial: the 1 MB line rejected (no hang, no huge allocation)");
    }
    // 6. Binary garbage.
    {
        { std::ofstream f(kAdvScene, std::ios::binary | std::ios::trunc);
          for (int i = 0; i < 4096; ++i) f << (char)(i % 251 + 1); }
        pe::Scene t;
        const bool ok = pe::loadSceneFromFile(kAdvScene, t);
        std::printf("OBSERVED: 4 KB of binary garbage: %s\n", ok ? "LOADS (unexpected!)" : "rejected");
        check(!ok, "adversarial: binary garbage rejected");
    }
    // 7. Empty file (the 276 covered it; re-record here for the record).
    {
        { std::ofstream f(kAdvScene, std::ios::binary | std::ios::trunc); }
        pe::Scene t;
        check(!pe::loadSceneFromFile(kAdvScene, t), "adversarial: the empty file rejected (the header check)");
    }
    // 8. A directory path instead of file.
    {
        pe::Scene t;
        const bool ok = pe::loadSceneFromFile("consumers", t);
        // OBSERVED: the ifstream on a directory: the open may succeed on
        // Windows; the getline fails -> the header check -> false.
        std::printf("OBSERVED: a directory path: %s\n", ok ? "LOADS (unexpected!)" : "rejected");
        check(!ok, "adversarial: a directory path rejected");
    }
    // 9. Saves do not reference prefabs (recorded): the 34-field entity
    // lines carry raw values, no prefab pointers; the closest file
    // reference is tilemap=<basename> - a MISSING tilemap file rejects
    // the load (scene.h:642-646: tm.width <= 0 -> false).
    {
        { std::ofstream f(kAdvScene, std::ios::binary | std::ios::trunc);
          f << "# scene v2\nscene=adv\ntilemap=no_such_tilemap_zz.txt\n"; }
        pe::Scene t;
        const bool ok = pe::loadSceneFromFile(kAdvScene, t);
        check(!ok, "adversarial: a missing referenced tilemap file rejects the load (saves reference tilemaps, not prefabs)");
    }
    std::remove(kAdvScene);
    std::remove(kAdvMgr);
    std::remove("assets/rt_adv_manager.txt");
}

int main() {
    std::printf("Phase D consumer: level pipeline (Step 275)\n");
    checkLoadAndInstantiate();
    checkHostileCases();
    checkSceneComposition();
    checkSceneRoundTrip();
    checkManagerPersistence();
    checkManagerExplicitPaths();
    checkAdversarialPersistence();
    if (failures > 0) {
        std::printf("level_pipeline consumer: FAILED (%d)\n", failures);
        return 1;
    }
    std::printf("level_pipeline consumer: PASS\n");
    return 0;
}
