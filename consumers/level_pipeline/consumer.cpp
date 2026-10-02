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
#include <fstream>
#include <string>
#include <vector>

#include "../../src/entity.h"
#include "../../src/prefab.h"
#include "../../src/scene.h"

static int failures = 0;

static void check(bool cond, const char* what) {
    if (!cond) {
        std::printf("FAIL: %s\n", what);
        ++failures;
    } else {
        std::printf("PASS: %s\n", what);
    }
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

int main() {
    std::printf("Phase D consumer: level pipeline (Step 275)\n");
    checkLoadAndInstantiate();
    checkHostileCases();
    checkSceneComposition();
    if (failures > 0) {
        std::printf("level_pipeline consumer: FAILED (%d)\n", failures);
        return 1;
    }
    std::printf("level_pipeline consumer: PASS\n");
    return 0;
}
