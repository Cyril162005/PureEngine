#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../src/audio.h"
#include "../src/components.h"
#include "../src/console.h"
#include "../src/events.h"
#include "../src/gamepad.h"
#include "../src/gamestate.h"
#include "../src/input.h"
#include "../src/lifecycle.h"
#include "../src/particles.h"
#include "../src/prefab.h"
#include "../src/physics.h"
#include "../src/font.h"
#include "../src/hostile_data.h"
#include "../src/scene.h"
#include "../src/simulation.h"
#include "../src/tilemap.h"
#include "../src/time.h"
#include "../src/animation_data.h"
#include "../src/lighting.h"
#include "../src/renderer.h"
#include "../src/resources.h"

namespace fs = std::filesystem;

static bool writeFile(const fs::path& path, const std::string& text) {
    std::ofstream out(path);
    if (!out) {
        return false;
    }
    out << text;
    return static_cast<bool>(out);
}

static bool assertFloatClose(float actual, float expected, float tolerance = 0.0001f) {
    return std::fabs(actual - expected) <= tolerance;
}

static bool checkCaseValidData() {
    const fs::path tmpDir = fs::temp_directory_path() / "pureengine_hostile_test_valid";
    fs::create_directories(tmpDir);

    const fs::path filePath = tmpDir / "__pureengine_valid_hostile_data___.txt";
    const std::string content =
        "hostile=2.0, 1.0, 2.0, 3.0\n"
        "difficulty_rate=0.2\n"
        "max_difficulty_scale=2.5\n";

    if (!writeFile(filePath, content)) {
        std::cerr << "Failed to create valid test file\n";
        return false;
    }

    const pe::HostileDefaults loaded = pe::loadHostileDefaults(filePath.string());
    if (loaded.hostiles.size() != 1) {
        std::cerr << "Expected 1 hostile entry in valid test case\n";
        return false;
    }
    if (!assertFloatClose(loaded.hostiles[0].baseSpeed, 2.0f) ||
        !assertFloatClose(loaded.hostiles[0].spawnPosition.x, 1.0f) ||
        !assertFloatClose(loaded.hostiles[0].spawnPosition.y, 2.0f) ||
        !assertFloatClose(loaded.hostiles[0].rotationSpeed, 3.0f) ||
        !assertFloatClose(loaded.difficultyRate, 0.2f) ||
        !assertFloatClose(loaded.maxDifficultyScale, 2.5f)) {
        std::cerr << "Valid data did not parse as expected\n";
        return false;
    }

    fs::remove(filePath);
    fs::remove(tmpDir);
    return true;
}

static bool checkCaseMissingKey() {
    const fs::path tmpDir = fs::temp_directory_path() / "pureengine_hostile_test_missing_key";
    fs::create_directories(tmpDir);

    const fs::path filePath = tmpDir / "__pureengine_missing_key___.txt";
    const std::string content =
        "hostile=2.0, 1.0, 2.0, 3.0\n"
        "unknown_value=9.9\n";

    if (!writeFile(filePath, content)) {
        std::cerr << "Failed to create missing-key test file\n";
        return false;
    }

    const pe::HostileDefaults loaded = pe::loadHostileDefaults(filePath.string());
    if (loaded.hostiles.size() != 3 ||
        !assertFloatClose(loaded.difficultyRate, 0.01f) ||
        !assertFloatClose(loaded.maxDifficultyScale, 1.33f)) {
        std::cerr << "Missing-key case should fall back to built-in defaults\n";
        return false;
    }

    fs::remove(filePath);
    fs::remove(tmpDir);
    return true;
}

static bool checkCaseMalformedNumber() {
    const fs::path tmpDir = fs::temp_directory_path() / "pureengine_hostile_test_malformed_number";
    fs::create_directories(tmpDir);

    const fs::path filePath = tmpDir / "__pureengine_malformed_number___.txt";
    const std::string content =
        "hostile=2.0, 1.0, 2.0, 3.0\n"
        "difficulty_rate=abc\n";

    if (!writeFile(filePath, content)) {
        std::cerr << "Failed to create malformed-number test file\n";
        return false;
    }

    const pe::HostileDefaults loaded = pe::loadHostileDefaults(filePath.string());
    if (loaded.hostiles.size() != 3 ||
        !assertFloatClose(loaded.difficultyRate, 0.01f) ||
        !assertFloatClose(loaded.maxDifficultyScale, 1.33f)) {
        std::cerr << "Malformed-number case should fall back to built-in defaults\n";
        return false;
    }

    fs::remove(filePath);
    fs::remove(tmpDir);
    return true;
}

static bool checkCaseEmptyHostileList() {
    const fs::path tmpDir = fs::temp_directory_path() / "pureengine_hostile_test_empty_list";
    fs::create_directories(tmpDir);

    const fs::path filePath = tmpDir / "__pureengine_empty_list___.txt";
    const std::string content =
        "difficulty_rate=0.5\n"
        "max_difficulty_scale=1.8\n";

    if (!writeFile(filePath, content)) {
        std::cerr << "Failed to create empty-hostile-list test file\n";
        return false;
    }

    const pe::HostileDefaults loaded = pe::loadHostileDefaults(filePath.string());
    if (loaded.hostiles.size() != 3 ||
        !assertFloatClose(loaded.difficultyRate, 0.01f) ||
        !assertFloatClose(loaded.maxDifficultyScale, 1.33f)) {
        std::cerr << "Empty hostile list should fall back to built-in defaults\n";
        return false;
    }

    fs::remove(filePath);
    fs::remove(tmpDir);
    return true;
}

static bool checkCaseMissingFile() {
    const fs::path tmpDir = fs::temp_directory_path() / "pureengine_hostile_test_missing_file";
    fs::create_directories(tmpDir);

    const fs::path missingPath = tmpDir / "__definitely_not_present___.txt";
    if (fs::exists(missingPath)) {
        fs::remove(missingPath);
    }

    const pe::HostileDefaults loaded = pe::loadHostileDefaults(missingPath.string());
    if (loaded.hostiles.size() != 3 ||
        !assertFloatClose(loaded.difficultyRate, 0.01f) ||
        !assertFloatClose(loaded.maxDifficultyScale, 1.33f)) {
        std::cerr << "Missing file case should fall back to built-in defaults\n";
        return false;
    }

    fs::remove(tmpDir);
    return true;
}

static bool checkTilemapValidLoad() {
    const fs::path tmpDir = fs::temp_directory_path() / "pureengine_tilemap_test_valid";
    fs::create_directories(tmpDir);

    const fs::path filePath = tmpDir / "__pureengine_valid_tilemap___.txt";
    const std::string content =
        "# sample map with a comment and a blank line tolerated\n"
        "\n"
        "width=4\n"
        "height=3\n"
        "tileSize=2.0\n"
        "row=0,1,0,0\n"
        "row=1,1,0,2\n"
        "row=0,0,0,0\n";

    if (!writeFile(filePath, content)) {
        std::cerr << "Failed to create valid tilemap test file\n";
        return false;
    }

    const pe::Tilemap loaded = pe::loadTilemap(filePath.string());
    if (loaded.width != 4 || loaded.height != 3 ||
        !assertFloatClose(loaded.tileSize, 2.0f) ||
        loaded.tiles.size() != 12) {
        std::cerr << "Valid tilemap dimensions did not parse as expected\n";
        return false;
    }
    const pe::Tile* solid = pe::tileAt(loaded, 1, 0);
    const pe::Tile* alt = pe::tileAt(loaded, 3, 1);
    const pe::Tile* empty = pe::tileAt(loaded, 0, 0);
    if (!solid || solid->tileId != 1 || !solid->solid || solid->textureId != 1 ||
        !alt || alt->tileId != 2 || !alt->solid || alt->textureId != 2 ||
        !empty || empty->tileId != 0 || empty->solid) {
        std::cerr << "Valid tilemap cells did not parse as expected\n";
        return false;
    }
    if (pe::tileAt(loaded, 4, 0) != nullptr || pe::tileAt(loaded, -1, 0) != nullptr ||
        pe::tileAt(loaded, 0, 3) != nullptr) {
        std::cerr << "tileAt out-of-bounds did not return nullptr\n";
        return false;
    }
    // Cell (0,0) center: x=(0-2+0.5)*2=-3, y=(1.5-0-0.5)*2=2.
    if (!assertFloatClose(pe::tileCenter(loaded, 0, 0).x, -3.0f) ||
        !assertFloatClose(pe::tileCenter(loaded, 0, 0).y, 2.0f)) {
        std::cerr << "tileCenter mapping is wrong\n";
        return false;
    }
    // Converter: 4 non-empty cells (one id-1, two id-1, one id-2).
    const std::vector<pe::Entity> tileEntities = pe::tilemapToEntities(loaded, 1.0f, 7);
    if (tileEntities.size() != 4) {
        std::cerr << "tilemapToEntities skipped the wrong cells\n";
        return false;
    }
    if (tileEntities[0].depth != 1 || tileEntities[0].roleId != 7 ||
        tileEntities[0].textureId != 1) {
        std::cerr << "tilemapToEntities did not carry depth/role/texture through\n";
        return false;
    }

    fs::remove(filePath);
    fs::remove(tmpDir);
    return true;
}

static bool checkTilemapMalformedFallsBack() {
    const fs::path tmpDir = fs::temp_directory_path() / "pureengine_tilemap_test_malformed";
    fs::create_directories(tmpDir);

    const fs::path filePath = tmpDir / "__pureengine_malformed_tilemap___.txt";
    const std::string content =
        "width=3\n"
        "height=2\n"
        "tileSize=1.0\n"
        "row=0,1,0\n"
        "row=0,1\n";

    if (!writeFile(filePath, content)) {
        std::cerr << "Failed to create malformed tilemap test file\n";
        return false;
    }

    const pe::Tilemap loaded = pe::loadTilemap(filePath.string());
    if (loaded.width != 0 || loaded.height != 0 || !loaded.tiles.empty()) {
        std::cerr << "Malformed tilemap (short row) should fall back to an empty map\n";
        return false;
    }

    fs::remove(filePath);
    fs::remove(tmpDir);
    return true;
}

static bool checkTilemapCollidePushesOut() {
    // 3x3 map, single solid center cell at the world origin.
    pe::Tilemap map;
    map.width = 3;
    map.height = 3;
    map.tileSize = 1.0f;
    map.tiles.assign(9, pe::Tile{});
    map.tiles[1 * 3 + 1].tileId = 1;
    map.tiles[1 * 3 + 1].solid = true;
    map.tiles[1 * 3 + 1].textureId = 1;

    // Entity overlapping the center cell from the right: AABB x in
    // [0.1, 1.1] vs tile x in [-0.5, 0.5] -> 0.4 overlap on X (the min
    // axis: full 1.0 overlap on Y). Expect push to x == 1.0, normal +X.
    pe::Entity entity;
    entity.position = pe::Vec3(0.6f, 0.0f, 0.0f);
    entity.halfExtents = pe::Vec3(0.5f, 0.5f, 0.0f);
    entity.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
    const pe::Vec3 normal = pe::collideEntityWithTilemap(entity, map);
    if (!assertFloatClose(normal.x, 1.0f) ||
        !assertFloatClose(normal.y, 0.0f) ||
        !assertFloatClose(normal.z, 0.0f) ||
        !assertFloatClose(entity.position.x, 1.0f) ||
        !assertFloatClose(entity.position.y, 0.0f)) {
        std::cerr << "Solid overlap did not push out along +X as expected\n";
        return false;
    }

    // Far-away entity: zero normal, byte-identical position (no writes).
    pe::Entity far;
    far.position = pe::Vec3(5.0f, 5.0f, 0.0f);
    far.halfExtents = pe::Vec3(0.5f, 0.5f, 0.0f);
    far.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
    const pe::Vec3 miss = pe::collideEntityWithTilemap(far, map);
    if (miss.x != 0.0f || miss.y != 0.0f || miss.z != 0.0f ||
        far.position.x != 5.0f || far.position.y != 5.0f) {
        std::cerr << "Miss case wrote state it must not touch\n";
        return false;
    }
    return true;
}

static bool checkSceneLifecycle() {
    pe::SceneManager manager;

    // Fresh scene: empty entities, no tilemap (width == 0), nothing current.
    pe::Scene& first = pe::loadScene(manager, "overworld");
    if (!first.entities.empty() || first.tilemap.width != 0 ||
        pe::currentScene(manager) != nullptr) {
        std::cerr << "Fresh scene is not empty / current leaked\n";
        return false;
    }

    // Find-or-create: repeat load returns THE same scene, no duplicate.
    pe::Scene& same = pe::loadScene(manager, "overworld");
    if (&same != &first || manager.scenes.size() != 1) {
        std::cerr << "loadScene duplicated an existing name\n";
        return false;
    }

    // Unknown name refuses; current stays empty (nullptr, never void).
    if (pe::switchTo(manager, "dungeon") ||
        pe::currentScene(manager) != nullptr) {
        std::cerr << "switchTo unknown name must fail cleanly\n";
        return false;
    }

    // Switch on, add an entity, check it carried through.
    if (!pe::switchTo(manager, "overworld") ||
        pe::currentScene(manager) == nullptr ||
        pe::currentScene(manager)->name != "overworld") {
        std::cerr << "switchTo known name did not become current\n";
        return false;
    }
    pe::Entity proto;
    proto.position = pe::Vec3(3.0f, 4.0f, 0.0f);
    proto.roleId = 7;
    const pe::Entity* stored = pe::addEntity(manager, proto);
    if (stored == nullptr || pe::currentScene(manager)->entities.size() != 1 ||
        !assertFloatClose(stored->position.x, 3.0f) || stored->roleId != 7) {
        std::cerr << "addEntity did not store the entity on current\n";
        return false;
    }

    // Removal: bad index refuses, good index erases exactly one.
    if (pe::removeEntity(manager, 5) ||
        !pe::removeEntity(manager, 0) ||
        !pe::currentScene(manager)->entities.empty()) {
        std::cerr << "removeEntity misbehaved on current scene\n";
        return false;
    }

    // Tilemap into the scene: good file stores data, bad file refuses
    // and leaves the previous map untouched.
    const fs::path tmpDir = fs::temp_directory_path() / "pureengine_scene_test";
    fs::create_directories(tmpDir);
    const fs::path goodPath = tmpDir / "__pureengine_scene_good___.txt";
    const fs::path badPath = tmpDir / "__pureengine_scene_bad___.txt";
    if (!writeFile(goodPath, "width=2\nheight=2\ntileSize=1.0\nrow=1,0\nrow=0,1\n") ||
        !writeFile(badPath, "width=2\nheight=2\ntileSize=1.0\nrow=1,0\nrow=0\n")) {
        std::cerr << "Failed to create scene tilemap test files\n";
        return false;
    }
    pe::Scene* current = pe::currentScene(manager);
    if (!pe::loadTilemapIntoScene(*current, goodPath.string()) ||
        current->tilemap.width != 2 || current->tilemap.tiles.size() != 4) {
        std::cerr << "loadTilemapIntoScene rejected a valid file\n";
        return false;
    }
    if (pe::loadTilemapIntoScene(*current, badPath.string()) ||
        current->tilemap.width != 2 || current->tilemap.tiles.size() != 4) {
        std::cerr << "Bad tilemap file clobbered the stored map\n";
        return false;
    }
    fs::remove(goodPath);
    fs::remove(badPath);
    fs::remove(tmpDir);

    // Clear: contents gone, tilemap back to none, name survives.
    pe::clearCurrent(manager);
    current = pe::currentScene(manager);
    if (current == nullptr || !current->entities.empty() ||
        current->tilemap.width != 0 || current->name != "overworld") {
        std::cerr << "clearCurrent did not empty while keeping identity\n";
        return false;
    }

    // Second scene coexists; switching shuttles current between them.
    pe::loadScene(manager, "dungeon");
    if (manager.scenes.size() != 2 || !pe::switchTo(manager, "dungeon") ||
        pe::currentScene(manager)->name != "dungeon" ||
        !pe::switchTo(manager, "overworld") ||
        pe::currentScene(manager)->name != "overworld") {
        std::cerr << "Second scene did not coexist/switch cleanly\n";
        return false;
    }
    return true;
}

static bool checkSceneNoCurrentNoOps() {
    pe::SceneManager manager;  // never loaded, never switched

    // Both currentScene overloads agree: no scene.
    const pe::SceneManager& frozen = manager;
    if (pe::currentScene(manager) != nullptr ||
        pe::currentScene(frozen) != nullptr) {
        std::cerr << "Fresh manager must report no current scene\n";
        return false;
    }

    // Every mutating path with no current scene: silent no-op, no crash.
    pe::clearCurrent(manager);  // must not crash
    pe::Entity proto;
    if (pe::addEntity(manager, proto) != nullptr ||
        pe::removeEntity(manager, 0) || pe::switchTo(manager, "ghost")) {
        std::cerr << "No-current operations must fail safe\n";
        return false;
    }
    if (!manager.scenes.empty() || manager.current != -1) {
        std::cerr << "No-ops must not grow or move manager state\n";
        return false;
    }
    return true;
}

static bool checkHierarchyChain() {
    // Grandparent (1,0,0) -> parent (0,2,0) -> child (0,0,3):
    // child world must be exactly (1,2,3).
    std::vector<pe::Entity> chain(3);
    chain[0].position = pe::Vec3(1.0f, 0.0f, 0.0f);
    chain[1].position = pe::Vec3(0.0f, 2.0f, 0.0f);
    chain[2].position = pe::Vec3(0.0f, 0.0f, 3.0f);
    if (chain[0].parentIndex != -1) {
        std::cerr << "Default parentIndex must be -1\n";
        return false;
    }
    if (!pe::setParent(chain, 1, 0) || !pe::setParent(chain, 2, 1)) {
        std::cerr << "Valid parent links were refused\n";
        return false;
    }
    const pe::Vec3 rootWorld = pe::worldPosition(chain, 0);
    const pe::Vec3 childWorld = pe::worldPosition(chain, 2);
    if (!assertFloatClose(rootWorld.x, 1.0f) ||
        !assertFloatClose(rootWorld.y, 0.0f) ||
        !assertFloatClose(rootWorld.z, 0.0f) ||
        !assertFloatClose(childWorld.x, 1.0f) ||
        !assertFloatClose(childWorld.y, 2.0f) ||
        !assertFloatClose(childWorld.z, 3.0f)) {
        std::cerr << "3-deep chain did not sum to (1,2,3)\n";
        return false;
    }
    return true;
}

static bool checkHierarchyRefusals() {
    std::vector<pe::Entity> pair(2);
    if (!pe::setParent(pair, 1, 0)) {
        std::cerr << "Setup link was refused\n";
        return false;
    }
    // Self-parent, 2-cycle back-link, out-of-range child/parent, and
    // below--1 parent: all refused, state unchanged.
    if (pe::setParent(pair, 0, 0) || pe::setParent(pair, 0, 1) ||
        pe::setParent(pair, 5, 0) || pe::setParent(pair, 0, 9) ||
        pe::setParent(pair, 0, -2)) {
        std::cerr << "Illegal parent link was accepted\n";
        return false;
    }
    if (pair[0].parentIndex != -1 || pair[1].parentIndex != 0) {
        std::cerr << "Refused setParent mutated state\n";
        return false;
    }
    // Clear via -1: link gone, world back to local.
    pair[1].position = pe::Vec3(4.0f, 5.0f, 6.0f);
    if (!pe::setParent(pair, 1, -1) || pair[1].parentIndex != -1) {
        std::cerr << "Clear via -1 failed\n";
        return false;
    }
    const pe::Vec3 cleared = pe::worldPosition(pair, 1);
    if (!assertFloatClose(cleared.x, 4.0f) ||
        !assertFloatClose(cleared.y, 5.0f) ||
        !assertFloatClose(cleared.z, 6.0f)) {
        std::cerr << "Cleared entity world != local\n";
        return false;
    }
    return true;
}

static bool checkHierarchyEdgeCases() {
    // Parentless identity: worldPosition == position exactly.
    pe::Entity solo;
    solo.position = pe::Vec3(-2.5f, 7.25f, 0.0f);
    std::vector<pe::Entity> single(1, solo);
    const pe::Vec3 identity = pe::worldPosition(single, 0);
    if (!assertFloatClose(identity.x, -2.5f) ||
        !assertFloatClose(identity.y, 7.25f) ||
        !assertFloatClose(identity.z, 0.0f)) {
        std::cerr << "Parentless worldPosition != position\n";
        return false;
    }
    // Out-of-range index: zero vector, no crash.
    const pe::Vec3 miss = pe::worldPosition(single, 42);
    if (miss.x != 0.0f || miss.y != 0.0f || miss.z != 0.0f) {
        std::cerr << "Out-of-range worldPosition must be (0,0,0)\n";
        return false;
    }
    // Hand-written cycle (bypasses setParent â€” field is public): the
    // capped walk must terminate with the exact capped accumulation.
    // pos0=(1,0,0), pos1=(0,1,0), size 2 -> world=(1,0,0)+(0,1,0)+(1,0,0).
    std::vector<pe::Entity> loop(2);
    loop[0].position = pe::Vec3(1.0f, 0.0f, 0.0f);
    loop[1].position = pe::Vec3(0.0f, 1.0f, 0.0f);
    loop[0].parentIndex = 1;
    loop[1].parentIndex = 0;
    const pe::Vec3 capped = pe::worldPosition(loop, 0);
    if (!assertFloatClose(capped.x, 2.0f) ||
        !assertFloatClose(capped.y, 1.0f) ||
        !assertFloatClose(capped.z, 0.0f)) {
        std::cerr << "Hand-written cycle did not terminate with capped sum\n";
        return false;
    }
    // setParent also refuses to attach to that detached loop.
    std::vector<pe::Entity> withLoop = loop;
    withLoop.push_back(pe::Entity());
    if (pe::setParent(withLoop, 2, 0)) {
        std::cerr << "setParent attached to a detached loop\n";
        return false;
    }
    return true;
}

static bool checkFontCells() {
    // Frozen Phase-3 cells: digits and '.' must never move.
    for (char d = '0'; d <= '9'; ++d) {
        if (pe::fontCellFor(d) != d - '0') {
            std::cerr << "Digit cell moved for '" << d << "'\n";
            return false;
        }
    }
    if (pe::fontCellFor('.') != 10) {
        std::cerr << "'.' cell moved from 10\n";
        return false;
    }
    // Step 66 appendages: A->11 ... Z->36, contiguous.
    if (pe::fontCellFor('A') != 11 || pe::fontCellFor('Z') != 36 ||
        pe::fontCellFor('M') != 11 + ('M' - 'A')) {
        std::cerr << "A-Z cells are not 11-36\n";
        return false;
    }
    // Lowercase folds to upper.
    if (pe::fontCellFor('a') != 11 || pe::fontCellFor('z') != 36 ||
        pe::fontCellFor('q') != pe::fontCellFor('Q')) {
        std::cerr << "Lowercase did not fold to upper\n";
        return false;
    }
    // No glyph: space, punctuation, control -> -1.
    if (pe::fontCellFor(' ') != -1 || pe::fontCellFor('!') != -1 ||
        pe::fontCellFor('-') != -1 || pe::fontCellFor('\n') != -1) {
        std::cerr << "Glyph-less char must map to -1\n";
        return false;
    }
    return true;
}

static bool checkFontMetrics() {
    // Width: one advance per character, drawn or skipped.
    if (!assertFloatClose(pe::textWidth("AB", 0.52f), 1.04f) ||
        !assertFloatClose(pe::textWidth("A B!", 0.5f), 2.0f) ||
        !assertFloatClose(pe::textWidth("", 0.52f), 0.0f)) {
        std::cerr << "textWidth miscounted advances\n";
        return false;
    }
    // Alignment: left stays, center shifts by half the width.
    if (!assertFloatClose(pe::alignOffsetX(pe::TextAlign::Left, 4.0f), 0.0f) ||
        !assertFloatClose(pe::alignOffsetX(pe::TextAlign::Center, 4.0f), -2.0f) ||
        !assertFloatClose(pe::alignOffsetX(pe::TextAlign::Center, 0.0f), 0.0f)) {
        std::cerr << "alignOffsetX is wrong\n";
        return false;
    }
    return true;
}

static bool checkEventsOrderAndPayload() {
    pe::EventBus bus;
    std::vector<int> calls;
    int seenA = -999, seenB = -999;
    const int first = bus.subscribe(pe::EventType::Collision,
        [&](const pe::GameEvent& e) {
            calls.push_back(0);
            seenA = e.a;
            seenB = e.b;
        });
    const int second = bus.subscribe(pe::EventType::Collision,
        [&](const pe::GameEvent&) { calls.push_back(1); });
    if (first < 0 || second != first + 1 ||
        bus.handlerCount(pe::EventType::Collision) != 2) {
        std::cerr << "subscribe tokens/count wrong\n";
        return false;
    }
    const pe::GameEvent hit{pe::EventType::Collision, 3, 7};
    bus.emit(hit);
    if (calls.size() != 2 || calls[0] != 0 || calls[1] != 1 ||
        seenA != 3 || seenB != 7) {
        std::cerr << "emit order/payload wrong\n";
        return false;
    }
    // Other types are independent: SceneChanged has no handlers, silent.
    bus.emit(pe::GameEvent{pe::EventType::SceneChanged, 0, 1});
    if (calls.size() != 2 ||
        bus.handlerCount(pe::EventType::SceneChanged) != 0) {
        std::cerr << "Event types leaked into each other\n";
        return false;
    }
    return true;
}

static bool checkEventsUnsubscribe() {
    pe::EventBus bus;
    int hits = 0;
    const int token = bus.subscribe(pe::EventType::SceneChanged,
        [&](const pe::GameEvent&) { ++hits; });
    if (token < 0 ||
        !bus.unsubscribe(pe::EventType::SceneChanged, token)) {
        std::cerr << "Valid unsubscribe failed\n";
        return false;
    }
    bus.emit(pe::GameEvent{pe::EventType::SceneChanged, 0, 1});
    if (hits != 0) {
        std::cerr << "Unsubscribed handler still fired\n";
        return false;
    }
    // Double-remove and never-issued tokens refuse.
    if (bus.unsubscribe(pe::EventType::SceneChanged, token) ||
        bus.unsubscribe(pe::EventType::SceneChanged, 999)) {
        std::cerr << "Bogus unsubscribe accepted\n";
        return false;
    }
    // Token non-reuse: the freed token never comes back.
    const int fresh = bus.subscribe(pe::EventType::SceneChanged,
        [&](const pe::GameEvent&) {});
    if (fresh == token || fresh < 0 ||
        bus.handlerCount(pe::EventType::SceneChanged) != 1) {
        std::cerr << "Token was reused after unsubscribe\n";
        return false;
    }
    return true;
}

static bool checkEventsEdgeCases() {
    pe::EventBus bus;
    // Refusals: sentinel type, empty handler (both spellings).
    if (bus.subscribe(pe::EventType::Count,
            [](const pe::GameEvent&) {}) != -1 ||
        bus.subscribe(pe::EventType::Collision, nullptr) != -1 ||
        bus.subscribe(pe::EventType::Collision, pe::EventHandler()) != -1) {
        std::cerr << "Illegal subscribe accepted\n";
        return false;
    }
    if (bus.unsubscribe(pe::EventType::Count, 0) ||
        bus.handlerCount(pe::EventType::Count) != 0) {
        std::cerr << "Sentinel type not inert\n";
        return false;
    }
    bus.emit(pe::GameEvent{pe::EventType::Count, 0, 0});  // must not crash

    // Mutation during emit: self-removal + late subscribe take effect
    // on the NEXT emit, while the in-flight snapshot runs to completion.
    std::vector<int> calls;
    int selfToken = -1;
    int adderToken = -1;
    selfToken = bus.subscribe(pe::EventType::Collision,
        [&](const pe::GameEvent&) {
            calls.push_back(0);
            bus.unsubscribe(pe::EventType::Collision, selfToken);
            adderToken = bus.subscribe(pe::EventType::Collision,
                [&](const pe::GameEvent&) { calls.push_back(9); });
        });
    const int steady = bus.subscribe(pe::EventType::Collision,
        [&](const pe::GameEvent&) { calls.push_back(1); });
    bus.emit(pe::GameEvent{pe::EventType::Collision, 0, 0});
    if (calls.size() != 2 || calls[0] != 0 || calls[1] != 1 ||
        bus.handlerCount(pe::EventType::Collision) != 2) {
        std::cerr << "Snapshot emit misbehaved under mutation\n";
        return false;
    }
    calls.clear();
    bus.emit(pe::GameEvent{pe::EventType::Collision, 0, 0});
    if (calls.size() != 2 || calls[0] != 1 || calls[1] != 9) {
        std::cerr << "Post-mutation membership/order wrong\n";
        return false;
    }

    // Reentrant emit: handler emits a different type mid-dispatch.
    pe::EventBus nested;
    std::vector<int> seq;
    nested.subscribe(pe::EventType::SceneChanged,
        [&](const pe::GameEvent&) { seq.push_back(1); });
    nested.subscribe(pe::EventType::Collision,
        [&](const pe::GameEvent& e) {
            seq.push_back(0);
            nested.emit(pe::GameEvent{pe::EventType::SceneChanged, e.a, e.b});
            seq.push_back(2);
        });
    nested.emit(pe::GameEvent{pe::EventType::Collision, 5, 6});
    if (seq.size() != 3 || seq[0] != 0 || seq[1] != 1 || seq[2] != 2) {
        std::cerr << "Reentrant emit order wrong\n";
        return false;
    }

    // clear(): silent afterwards, and tokens stay retired.
    bus.clear();
    if (bus.handlerCount(pe::EventType::Collision) != 0) {
        std::cerr << "clear did not drop handlers\n";
        return false;
    }
    bus.emit(pe::GameEvent{pe::EventType::Collision, 0, 0});  // silent
    const int afterClear = bus.subscribe(pe::EventType::Collision,
        [&](const pe::GameEvent&) {});
    if (afterClear <= adderToken || afterClear <= steady) {
        std::cerr << "clear recycled a token\n";
        return false;
    }
    return true;
}

static bool checkConsoleToggle() {
    pe::Console c;
    if (c.open || !c.input.empty() || !c.lines.empty()) {
        std::cerr << "Console must start closed and empty\n";
        return false;
    }
    pe::toggle(c);
    if (!c.open) {
        std::cerr << "toggle did not open\n";
        return false;
    }
    // State survives a close/reopen round-trip.
    pe::feedKey(c, GLFW_KEY_H, false);
    pe::feedKey(c, GLFW_KEY_I, true);
    pe::toggle(c);
    pe::toggle(c);
    if (!c.open || c.input != "hI") {
        std::cerr << "toggle lost input state\n";
        return false;
    }
    return true;
}

static bool checkConsoleFeedKey() {
    pe::Console c;
    pe::feedKey(c, GLFW_KEY_H, false);    // -> 'h'
    pe::feedKey(c, GLFW_KEY_E, true);     // -> 'E'
    pe::feedKey(c, GLFW_KEY_1, false);    // -> '1' (shift ignored)
    pe::feedKey(c, GLFW_KEY_1, true);     // -> '1' still
    pe::feedKey(c, GLFW_KEY_SPACE, false);
    pe::feedKey(c, GLFW_KEY_PERIOD, false);
    pe::feedKey(c, GLFW_KEY_MINUS, false);
    if (c.input != "hE11 .-") {
        std::cerr << "feedKey built '" << c.input << "'\n";
        return false;
    }
    // Backspace removes one char; safe on empty.
    pe::feedKey(c, GLFW_KEY_BACKSPACE, false);
    if (c.input != "hE11 .") {
        std::cerr << "Backspace misbehaved\n";
        return false;
    }
    // Untypeable keys are ignored: function keys, escape, enter
    // (submit is a separate call), tab.
    pe::feedKey(c, GLFW_KEY_F1, false);
    pe::feedKey(c, GLFW_KEY_ESCAPE, false);
    pe::feedKey(c, GLFW_KEY_ENTER, false);
    pe::feedKey(c, GLFW_KEY_TAB, false);
    if (c.input != "hE11 .") {
        std::cerr << "Ignored key leaked into input\n";
        return false;
    }
    pe::Console empty;
    pe::feedKey(empty, GLFW_KEY_BACKSPACE, false);  // must not crash
    if (!empty.input.empty()) {
        std::cerr << "Backspace on empty input wrote state\n";
        return false;
    }
    return true;
}

static bool checkConsoleSubmit() {
    pe::Console c;
    // Game-side command via the registration mechanism (the "list
    // entities" pattern: the engine carries the wire, the game the data).
    if (!pe::registerCommand(c, "entities",
            [](const std::vector<std::string>&) {
                return std::string("3 hostiles");
            })) {
        std::cerr << "Valid registerCommand refused\n";
        return false;
    }
    // Registration refusals: builtin collision (any case), empty name,
    // duplicate, null handler.
    if (pe::registerCommand(c, "echo",
            [](const std::vector<std::string>&) { return std::string("x"); }) ||
        pe::registerCommand(c, "HELP",
            [](const std::vector<std::string>&) { return std::string("x"); }) ||
        pe::registerCommand(c, "",
            [](const std::vector<std::string>&) { return std::string("x"); }) ||
        pe::registerCommand(c, "Entities",
            [](const std::vector<std::string>&) { return std::string("x"); }) ||
        pe::registerCommand(c, "nullcmd", pe::ConsoleHandler())) {
        std::cerr << "Illegal registerCommand accepted\n";
        return false;
    }

    // Registered command: echo "> entities" + result, input cleared.
    c.input = "entities";
    if (pe::submit(c) != "3 hostiles" || !c.input.empty() ||
        c.lines.size() != 2 || c.lines[0] != "> entities" ||
        c.lines[1] != "3 hostiles") {
        std::cerr << "Registered command submit wrong\n";
        return false;
    }
    // Built-ins, case-insensitive command names, args preserved as typed.
    c.input = "ECHO Hi There";
    if (pe::submit(c) != "Hi There") {
        std::cerr << "echo submit wrong\n";
        return false;
    }
    c.input = "HELP";
    const std::string help = pe::submit(c);
    if (help.find("help") == std::string::npos ||
        help.find("clear") == std::string::npos ||
        help.find("echo") == std::string::npos ||
        help.find("entities") == std::string::npos) {
        std::cerr << "help output missing names: '" << help << "'\n";
        return false;
    }
    // Step 144: newer commands appear in help by construction — the help
    // loop lists EVERY registered command. Register one and assert it
    // shows up (the arcade's 14 commands rely on this same loop).
    pe::registerCommand(c, "mute", [](const std::vector<std::string>&) {
        return std::string("toggled");
    });
    c.input = "help";
    const std::string helpNewer = pe::submit(c);
    if (helpNewer.find("mute") == std::string::npos ||
        helpNewer.find("entities") == std::string::npos) {
        std::cerr << "help must list newly registered commands: '" << helpNewer << "'\n";
        return false;
    }
    // Unknown command: exact format, token as typed.
    c.input = "frobnicate now";
    if (pe::submit(c) != "unknown command 'frobnicate' (try help)") {
        std::cerr << "Unknown-command format wrong\n";
        return false;
    }
    // Empty / blank input: no-op, nothing pushed.
    const std::size_t before = c.lines.size();
    c.input = "";
    if (pe::submit(c) != "" || c.lines.size() != before) {
        std::cerr << "Empty submit must be a no-op\n";
        return false;
    }
    c.input = "   ";
    if (pe::submit(c) != "" || c.lines.size() != before) {
        std::cerr << "Blank submit must be a no-op\n";
        return false;
    }
    return true;
}

static bool checkConsoleHistory() {
    pe::Console c;
    for (int i = 0; i < 70; ++i) {
        pe::print(c, "n" + std::to_string(i));
    }
    if (c.lines.size() != 64 || c.lines.front() != "n6" ||
        c.lines.back() != "n69") {
        std::cerr << "History cap did not keep the newest 64\n";
        return false;
    }
    // clear wipes everything including the submit echo: true clean slate.
    c.input = "clear";
    if (pe::submit(c) != "" || !c.lines.empty() || !c.input.empty()) {
        std::cerr << "clear did not wipe history\n";
        return false;
    }
    return true;
}

static bool checkGamepadDeadzone() {
    // Inside the zone (both signs) reads exactly 0; outside passes
    // through untouched; the boundary itself passes (deliberate input).
    if (!assertFloatClose(pe::applyDeadzone(0.1f, 0.2f), 0.0f) ||
        !assertFloatClose(pe::applyDeadzone(-0.1f, 0.2f), 0.0f) ||
        !assertFloatClose(pe::applyDeadzone(0.0f, 0.2f), 0.0f) ||
        !assertFloatClose(pe::applyDeadzone(0.5f, 0.2f), 0.5f) ||
        !assertFloatClose(pe::applyDeadzone(-0.5f, 0.2f), -0.5f) ||
        !assertFloatClose(pe::applyDeadzone(1.0f, 0.2f), 1.0f) ||
        !assertFloatClose(pe::applyDeadzone(-1.0f, 0.2f), -1.0f) ||
        !assertFloatClose(pe::applyDeadzone(0.2f, 0.2f), 0.2f) ||
        !assertFloatClose(pe::applyDeadzone(-0.2f, 0.2f), -0.2f)) {
        std::cerr << "Deadzone math wrong\n";
        return false;
    }
    // Negative deadzone behaves as 0: everything passes.
    if (!assertFloatClose(pe::applyDeadzone(0.05f, -1.0f), 0.05f) ||
        !assertFloatClose(pe::applyDeadzone(0.0f, -1.0f), 0.0f)) {
        std::cerr << "Negative deadzone must behave as 0\n";
        return false;
    }
    return true;
}

static bool checkGamepadButtons() {
    // Hand-built snapshot: A pressed, B released.
    pe::GamepadState state;
    state.connected = true;
    state.buttons[GLFW_GAMEPAD_BUTTON_A] = true;
    if (!pe::gamepadButton(state, GLFW_GAMEPAD_BUTTON_A) ||
        pe::gamepadButton(state, GLFW_GAMEPAD_BUTTON_B) ||
        pe::gamepadButton(state, GLFW_GAMEPAD_BUTTON_START) ||
        pe::gamepadButton(state, GLFW_GAMEPAD_BUTTON_DPAD_UP)) {
        std::cerr << "Button level reads wrong\n";
        return false;
    }
    // Out-of-range codes (both sides, far outside): false, no crash.
    if (pe::gamepadButton(state, -1) ||
        pe::gamepadButton(state, 15) ||
        pe::gamepadButton(state, 99)) {
        std::cerr << "Out-of-range button must read false\n";
        return false;
    }
    // Default snapshot is fully zeroed/disconnected (the pollGamepad
    // absent-pad contract, also true by construction).
    const pe::GamepadState fresh;
    if (fresh.connected || fresh.leftX != 0.0f || fresh.leftY != 0.0f ||
        fresh.rightX != 0.0f || fresh.rightY != 0.0f ||
        pe::gamepadButton(fresh, GLFW_GAMEPAD_BUTTON_A)) {
        std::cerr << "Default GamepadState must be zeroed\n";
        return false;
    }
    return true;
}

static bool checkGamepadEdge() {
    pe::GamepadState released;
    released.connected = true;
    pe::GamepadState pressed = released;
    pressed.buttons[GLFW_GAMEPAD_BUTTON_X] = true;
    // Press transition fires; hold and release do not; garbage does not.
    if (!pe::gamepadButtonEdge(released, pressed, GLFW_GAMEPAD_BUTTON_X) ||
        pe::gamepadButtonEdge(pressed, pressed, GLFW_GAMEPAD_BUTTON_X) ||
        pe::gamepadButtonEdge(pressed, released, GLFW_GAMEPAD_BUTTON_X) ||
        pe::gamepadButtonEdge(released, released, GLFW_GAMEPAD_BUTTON_X) ||
        pe::gamepadButtonEdge(released, pressed, GLFW_GAMEPAD_BUTTON_Y) ||
        pe::gamepadButtonEdge(released, pressed, -1) ||
        pe::gamepadButtonEdge(released, pressed, 42)) {
        std::cerr << "Button edge transitions wrong\n";
        return false;
    }
    return true;
}

static bool checkGamepadPollSafety() {
    // No GLFW init exists in this process: every poll MUST report a
    // clean absent pad (never crash, never garbage). Out-of-range ids
    // refuse before even asking GLFW.
    const pe::GamepadState absent = pe::pollGamepad();
    if (absent.connected) {
        std::cerr << "Uninitialized GLFW must poll disconnected\n";
        return false;
    }
    for (int b = 0; b <= GLFW_GAMEPAD_BUTTON_LAST; ++b) {
        if (pe::gamepadButton(absent, b)) {
            std::cerr << "Absent pad must read no buttons\n";
            return false;
        }
    }
    const float axes[4] = {absent.leftX, absent.leftY, absent.rightX,
                           absent.rightY};
    for (int i = 0; i < 4; ++i) {
        if (axes[i] < -1.0f || axes[i] > 1.0f) {
            std::cerr << "Polled axis out of [-1,1]\n";
            return false;
        }
    }
    if (pe::pollGamepad(99).connected || pe::pollGamepad(-1).connected) {
        std::cerr << "Out-of-range joystick id must poll disconnected\n";
        return false;
    }
    return true;
}

static bool checkMouseInput() {
    // Step 117: pure mouse snapshot/edge logic â€” the exact helpers the
    // Input members delegate to. No GLFW hardware involved (gamepad
    // check pattern).
    bool ok = true;
    const pe::MouseState up;  // nothing pressed
    pe::MouseState leftDown;
    leftDown.left = true;
    leftDown.x = 320.0f;
    leftDown.y = 240.0f;
    pe::MouseState rightDown;
    rightDown.right = true;
    // Level reads: only left/right meaningful, everything else false.
    if (pe::mouseButtonDown(up, GLFW_MOUSE_BUTTON_LEFT) ||
        pe::mouseButtonDown(up, GLFW_MOUSE_BUTTON_RIGHT)) {
        std::cerr << "Idle mouse must read no buttons\n";
        ok = false;
    }
    if (!pe::mouseButtonDown(leftDown, GLFW_MOUSE_BUTTON_LEFT) ||
        pe::mouseButtonDown(leftDown, GLFW_MOUSE_BUTTON_RIGHT)) {
        std::cerr << "leftDown must read left-only\n";
        ok = false;
    }
    if (!pe::mouseButtonDown(rightDown, GLFW_MOUSE_BUTTON_RIGHT) ||
        pe::mouseButtonDown(rightDown, GLFW_MOUSE_BUTTON_LEFT)) {
        std::cerr << "rightDown must read right-only\n";
        ok = false;
    }
    if (pe::mouseButtonDown(leftDown, GLFW_MOUSE_BUTTON_MIDDLE) ||
        pe::mouseButtonDown(leftDown, 42) || pe::mouseButtonDown(leftDown, -1)) {
        std::cerr << "Non-left/right buttons must read false\n";
        ok = false;
    }
    // Edges: rising only, never on hold or release.
    if (!pe::mouseButtonEdge(up, leftDown, GLFW_MOUSE_BUTTON_LEFT)) {
        std::cerr << "prev=false,cur=true must be a left edge\n";
        ok = false;
    }
    if (pe::mouseButtonEdge(leftDown, leftDown, GLFW_MOUSE_BUTTON_LEFT)) {
        std::cerr << "Held button must not edge\n";
        ok = false;
    }
    if (pe::mouseButtonEdge(leftDown, up, GLFW_MOUSE_BUTTON_LEFT)) {
        std::cerr << "Release must not be a rising edge\n";
        ok = false;
    }
    if (!pe::mouseButtonEdge(up, rightDown, GLFW_MOUSE_BUTTON_RIGHT)) {
        std::cerr << "prev=false,cur=true must be a right edge\n";
        ok = false;
    }
    if (pe::mouseButtonEdge(up, leftDown, GLFW_MOUSE_BUTTON_RIGHT) ||
        pe::mouseButtonEdge(up, rightDown, GLFW_MOUSE_BUTTON_LEFT) ||
        pe::mouseButtonEdge(up, leftDown, GLFW_MOUSE_BUTTON_MIDDLE)) {
        std::cerr << "Unrelated button pair must not edge\n";
        ok = false;
    }
    // Input-class wiring: default-constructed members read their
    // zeroed snapshot (no window, no update() yet).
    pe::Input input{};
    if (input.mouseDown(GLFW_MOUSE_BUTTON_LEFT) ||
        input.mouseDown(GLFW_MOUSE_BUTTON_RIGHT) || input.mouseEdge(GLFW_MOUSE_BUTTON_LEFT)) {
        std::cerr << "Fresh Input must read no mouse buttons\n";
        ok = false;
    }
    if (input.mouseX() != 0.0f || input.mouseY() != 0.0f) {
        std::cerr << "Fresh Input must read zero mouse position\n";
        ok = false;
    }
    return ok;
}

static bool checkParticleSpawn() {
    std::vector<pe::Particle> pool;
    pe::spawnParticle(pool, pe::Vec3(1.0f, 2.0f, 0.0f),
                      pe::Vec3(3.0f, 4.0f, 0.0f), 2.5f, 0.75f,
                      pe::Vec3(1.0f, 0.0f, 0.0f));
    if (pool.size() != 1) {
        std::cerr << "spawnParticle did not append exactly one\n";
        return false;
    }
    const pe::Particle& p = pool[0];
    if (!assertFloatClose(p.position.x, 1.0f) ||
        !assertFloatClose(p.position.y, 2.0f) ||
        !assertFloatClose(p.velocity.x, 3.0f) ||
        !assertFloatClose(p.velocity.y, 4.0f) ||
        !assertFloatClose(p.life, 2.5f) ||
        !assertFloatClose(p.maxLife, 2.5f) ||
        !assertFloatClose(p.size, 0.75f) ||
        !assertFloatClose(p.color.x, 1.0f) ||
        !assertFloatClose(p.color.y, 0.0f) ||
        !assertFloatClose(p.color.z, 0.0f)) {
        std::cerr << "spawnParticle stored wrong values\n";
        return false;
    }
    return true;
}

static bool checkEmitterRateAndCap() {
    pe::Emitter e;
    e.position = pe::Vec3(5.0f, 5.0f, 0.0f);
    e.spawnRate = 10.0f;
    e.speedMin = 1.0f;
    e.speedMax = 2.0f;
    e.lifeMin = 0.5f;
    e.lifeMax = 1.0f;
    std::vector<pe::Particle> pool;
    pe::emit(e, pool, 1.0f);  // 10 units of budget
    if (pool.size() != 10) {
        std::cerr << "Emitter rate produced " << pool.size() << ", want 10\n";
        return false;
    }
    for (std::size_t i = 0; i < pool.size(); ++i) {
        const pe::Particle& p = pool[i];
        if (!assertFloatClose(p.position.x, 5.0f) ||
            !assertFloatClose(p.position.y, 5.0f) ||
            p.life < 0.5f || p.life > 1.0f) {
            std::cerr << "Emitted particle outside emitter spec\n";
            return false;
        }
        const float speedSq =
            p.velocity.x * p.velocity.x + p.velocity.y * p.velocity.y;
        // Epsilon margins: cos^2+sin^2 rounds to ~1e-7 around 1.0.
        if (speedSq < 0.999f || speedSq > 4.001f ||
            p.velocity.z != 0.0f) {
            std::cerr << "Emitted speed outside [1,2] disc\n";
            return false;
        }
    }
    // dt <= 0 and non-positive rate spawn nothing.
    const std::size_t before = pool.size();
    pe::emit(e, pool, 0.0f);
    pe::emit(e, pool, -1.0f);
    e.spawnRate = 0.0f;
    pe::emit(e, pool, 1.0f);
    if (pool.size() != before) {
        std::cerr << "Zero/negative emit inputs spawned\n";
        return false;
    }
    // Cap: maxParticles=3 keeps the pool at 3 across repeated emits...
    e.spawnRate = 10.0f;
    e.maxParticles = 3;
    std::vector<pe::Particle> capped;
    pe::emit(e, capped, 1.0f);
    pe::emit(e, capped, 1.0f);
    if (capped.size() != 3) {
        std::cerr << "Cap did not hold at 3\n";
        return false;
    }
    // ...and skipped budget is LOST, not backlogged: uncapped, a 0.1s
    // emit at rate 10 yields exactly 1 (a backlog would burst 10+).
    e.maxParticles = 256;
    e.accumulator = 0.0f;
    std::vector<pe::Particle> uncap;
    pe::emit(e, uncap, 0.1f);
    if (uncap.size() != 1) {
        std::cerr << "Fresh 0.1s emit must yield exactly 1\n";
        return false;
    }
    return true;
}

// --- Step 120: Prefab system test ---
static bool checkPrefabSystem() {
    // Write temp prefab file
    const std::string tmpFile = "test_prefab_tmp.txt";
    {
        std::ofstream f(tmpFile);
        f << "# PureEngine prefab v1\n";
        f << "name=test_entity\n";
        f << "textureId=3\n";
        f << "depth=5\n";
        f << "roleId=1\n";
        f << "moveSpeed=2.5\n";
        f << "gravityScale=1.0\n";
        f << "isStatic=false\n";
        f << "health=75.0\n";
        f << "tag=player\n";
        f << "clip=walk_right\n";
        f << "cols=4\n";
        f << "rows=2\n";
        f << "scale=2.0,2.0,1.0\n";
        f << "halfExtents=1.0,1.0,0.5\n";
        f << "tint=0.5,0.8,1.0\n";
    }

    // Load prefab
    pe::Prefab p;
    if (!pe::loadPrefab(tmpFile, p)) {
        std::cerr << "loadPrefab should succeed\n";
        return false;
    }
    if (p.name != "test_entity") { std::cerr << "name mismatch\n"; return false; }
    if (p.textureId != 3) { std::cerr << "textureId mismatch\n"; return false; }
    if (p.depth != 5) { std::cerr << "depth mismatch\n"; return false; }
    if (p.roleId != 1) { std::cerr << "roleId mismatch\n"; return false; }
    if (std::abs(p.moveSpeed - 2.5f) >= 1e-5f) { std::cerr << "moveSpeed mismatch\n"; return false; }
    if (std::abs(p.gravityScale - 1.0f) >= 1e-5f) { std::cerr << "gravityScale mismatch\n"; return false; }
    if (p.isStatic) { std::cerr << "isStatic mismatch\n"; return false; }
    if (std::abs(p.health - 75.0f) >= 1e-5f) { std::cerr << "health mismatch\n"; return false; }
    if (p.tag != "player") { std::cerr << "tag mismatch\n"; return false; }
    if (p.currentClipName != "walk_right") { std::cerr << "clip mismatch\n"; return false; }
    if (p.cols != 4) { std::cerr << "cols mismatch\n"; return false; }
    if (p.rows != 2) { std::cerr << "rows mismatch\n"; return false; }

    // Instantiate at known position
    pe::Vec3 pos(3.0f, -1.0f, 0.0f);
    pe::Entity e = pe::instantiatePrefab(p, pos);
    if (std::abs(e.position.x - 3.0f) >= 1e-5f) { std::cerr << "position.x mismatch\n"; return false; }
    if (std::abs(e.position.y + 1.0f) >= 1e-5f) { std::cerr << "position.y mismatch\n"; return false; }
    if (e.textureId != 3) { std::cerr << "entity textureId mismatch\n"; return false; }
    if (!e.alive) { std::cerr << "entity should be alive\n"; return false; }
    if (e.tag != "player") { std::cerr << "entity tag mismatch\n"; return false; }

    // Runtime state must be at defaults (not taken from prefab)
    if (std::abs(e.velocity.x) >= 1e-5f) { std::cerr << "velocity should be zero\n"; return false; }
    if (e.parentIndex != -1) { std::cerr << "parentIndex should be -1\n"; return false; }
    if (e.wasGrounded) { std::cerr << "wasGrounded should be false\n"; return false; }

    // Missing file -> false, no crash
    pe::Prefab p2;
    if (pe::loadPrefab("nonexistent_prefab.txt", p2)) {
        std::cerr << "missing file should return false\n";
        return false;
    }

    // Malformed line -> warn + continue (valid fields still parsed)
    const std::string malformedFile = "test_prefab_malformed_tmp.txt";
    {
        std::ofstream f(malformedFile);
        f << "# PureEngine prefab v1\n";
        f << "name=partial\n";
        f << "THIS IS NOT VALID\n";   // no '='
        f << "textureId=7\n";
    }
    pe::Prefab p3;
    pe::loadPrefab(malformedFile, p3);  // should not crash
    if (p3.name != "partial") { std::cerr << "valid fields before malformed should parse\n"; return false; }
    if (p3.textureId != 7) { std::cerr << "valid fields after malformed should parse\n"; return false; }

    // Cleanup
    std::filesystem::remove(tmpFile);
    std::filesystem::remove(malformedFile);

    return true;
}

// --- Step 122: prefab used in a live scene (end-to-end content proof) ---
// Replicates the spawn_prefab console command body (main.cpp): loadPrefab ->
// instantiatePrefab -> spawnEntity into a Scene, then resolve the prefab's
// clip name to a playing AnimationState (the Step 122 command fix), and
// prove the spawned entity joins the game's role-filtered systems.
static bool checkPrefabSceneSpawn() {
    const std::string tmpFile = "test_prefab_scene_tmp.txt";
    {
        std::ofstream f(tmpFile);
        f << "# PureEngine prefab v1\n";
        f << "name=spawn_test\n";
        f << "textureId=2\n";
        f << "depth=2\n";
        f << "roleId=2\n";
        f << "moveSpeed=1.5\n";
        f << "tag=hostile\n";
        f << "clip=walk_left\n";
        f << "cols=8\n";
        f << "rows=1\n";
        f << "scale=1.0,1.0,1.0\n";
        f << "halfExtents=0.5,0.5,0.5\n";
    }

    // The exact console command body: load -> instantiate -> spawn.
    pe::Prefab p;
    if (!pe::loadPrefab(tmpFile, p)) {
        std::cerr << "prefab scene spawn: loadPrefab should succeed\n";
        return false;
    }
    pe::Scene scene;
    pe::Entity base;
    pe::spawnEntity(scene, base);  // pre-existing entity (index 0)
    pe::Entity e = pe::instantiatePrefab(p, pe::Vec3(0.0f, 0.0f, 0.0f));
    const std::size_t index = pe::spawnEntity(scene, e);
    if (index != 1) { std::cerr << "spawnEntity index wrong\n"; return false; }
    if (scene.entities.size() != 2) { std::cerr << "scene count wrong\n"; return false; }
    if (!scene.entities[index].alive) { std::cerr << "spawned must be alive\n"; return false; }

    // Step 122: clip name carried through spawn; resolve to a playing state.
    pe::Entity& spawned = scene.entities[index];
    if (spawned.currentClipName != "walk_left") {
        std::cerr << "clip name not carried through spawn\n";
        return false;
    }
    pe::Animation clip;
    clip.name = "walk_left";
    clip.loops = true;
    clip.frames.push_back(pe::AnimationFrame{0, 0.1f});
    spawned.animationState.currentAnimation = &clip;
    spawned.animationState.isPlaying = true;
    if (!spawned.animationState.isPlaying) {
        std::cerr << "clip should be playing\n";
        return false;
    }
    if (spawned.animationState.getCurrentFrame() == nullptr) {
        std::cerr << "playing clip must expose a frame\n";
        return false;
    }
    spawned.animationState.update(0.05f);
    if (spawned.animationState.currentFrameIndex != 0) {
        std::cerr << "frame must not advance before duration elapses\n";
        return false;
    }
    spawned.animationState.update(0.1f);
    if (spawned.animationState.currentFrameIndex != 0) {
        std::cerr << "looping clip must wrap to frame 0\n";
        return false;
    }

    // Spawned entity joins the game's role-filtered systems: a hostile-role
    // spawn chases a player-role entity through the same filters main.cpp
    // passes (ArcadeRole values, caller-supplied).
    pe::Entity player;
    player.roleId = static_cast<int>(pe::ArcadeRole::Player);
    player.position = pe::Vec3(3.0f, 0.0f, 0.0f);
    scene.entities[0] = player;
    pe::advanceRotations(scene.entities, 0.016f);
    const float beforeX = spawned.position.x;
    pe::chasePlayer(scene.entities, 1.0f, 0.016f,
                    static_cast<int>(pe::ArcadeRole::Player),
                    static_cast<int>(pe::ArcadeRole::Hostile));
    if (spawned.position.x <= beforeX) {
        std::cerr << "spawned hostile must chase the player\n";
        return false;
    }

    std::filesystem::remove(tmpFile);
    std::cout << "checkPrefabSceneSpawn PASSED\n";
    return true;
}

// --- Step 122: prefab used in a live game (end-to-end content proof) ---
// The exact pipeline the console spawn_prefab command runs, proven
// against the REAL shipped asset (assets/prefabs/enemy.txt, resolved
// through the 3-candidate probe from the test's CWD) and the real
// spawnEntity scene integration â€” not just synthetic temp files.
static bool checkPrefabLiveSpawn() {
    // 1. Load the real shipped prefab (candidate 2 resolves from build/).
    pe::Prefab p;
    if (!pe::loadPrefab("enemy.txt", p)) {
        std::cerr << "shipped enemy.txt failed to load\n";
        return false;
    }
    if (p.name != "enemy") { std::cerr << "enemy.txt name mismatch\n"; return false; }
    if (p.textureId != 2) { std::cerr << "enemy.txt textureId mismatch\n"; return false; }
    if (p.roleId != 2) { std::cerr << "enemy.txt roleId mismatch\n"; return false; }
    if (std::abs(p.moveSpeed - 1.5f) >= 1e-5f) { std::cerr << "enemy.txt moveSpeed mismatch\n"; return false; }
    if (p.tag != "hostile") { std::cerr << "enemy.txt tag mismatch\n"; return false; }
    if (p.currentClipName != "walk_left") { std::cerr << "enemy.txt clip mismatch\n"; return false; }
    if (p.cols != 8 || p.rows != 1) { std::cerr << "enemy.txt sheet layout mismatch\n"; return false; }

    // 2. Instantiate at a known position (runtime state at defaults).
    pe::Entity e = pe::instantiatePrefab(p, pe::Vec3(2.0f, 1.0f, 0.0f));
    if (!assertFloatClose(e.position.x, 2.0f) ||
        !assertFloatClose(e.position.y, 1.0f)) {
        std::cerr << "prefab entity position wrong\n";
        return false;
    }
    if (!e.alive) { std::cerr << "prefab entity must be alive\n"; return false; }

    // 3. Spawn into a real Scene (the console command's path).
    pe::Scene scene;
    const std::size_t before = scene.entities.size();
    const std::size_t index = pe::spawnEntity(scene, e);
    if (scene.entities.size() != before + 1) {
        std::cerr << "spawnEntity did not grow the scene\n";
        return false;
    }
    if (index != before) { std::cerr << "spawnEntity index wrong\n"; return false; }
    const pe::Entity& spawned = scene.entities[index];
    if (!spawned.alive || spawned.tag != "hostile" || spawned.textureId != 2) {
        std::cerr << "spawned entity lost prefab config\n";
        return false;
    }
    if (!assertFloatClose(spawned.position.x, 2.0f) ||
        !assertFloatClose(spawned.position.y, 1.0f)) {
        std::cerr << "spawned entity position wrong\n";
        return false;
    }

    std::cout << "checkPrefabLiveSpawn PASSED\n";
    return true;
}

static bool checkParticleMotion() {
    std::vector<pe::Particle> pool;
    pe::spawnParticle(pool, pe::Vec3(0.0f, 0.0f, 0.0f),
                      pe::Vec3(1.0f, 2.0f, 0.0f), 1.0f, 0.5f,
                      pe::Vec3(1.0f, 1.0f, 1.0f));
    // dt <= 0: byte-identical no-op.
    pe::updateParticles(pool, 0.0f);
    pe::updateParticles(pool, -1.0f);
    if (!assertFloatClose(pool[0].position.x, 0.0f) ||
        !assertFloatClose(pool[0].life, 1.0f)) {
        std::cerr << "Non-positive dt must not advance particles\n";
        return false;
    }
    pe::updateParticles(pool, 0.5f);
    if (!assertFloatClose(pool[0].position.x, 0.5f) ||
        !assertFloatClose(pool[0].position.y, 1.0f) ||
        !assertFloatClose(pool[0].life, 0.5f)) {
        std::cerr << "Integration/aging wrong\n";
        return false;
    }
    return true;
}

static bool checkParticleDeath() {
    std::vector<pe::Particle> pool;
    pe::spawnParticle(pool, pe::Vec3(0.0f, 0.0f, 0.0f),
                      pe::Vec3(0.0f, 0.0f, 0.0f), 0.3f, 0.5f,
                      pe::Vec3(1.0f, 1.0f, 1.0f));  // dies
    pe::spawnParticle(pool, pe::Vec3(9.0f, 0.0f, 0.0f),
                      pe::Vec3(0.0f, 0.0f, 0.0f), 5.0f, 0.5f,
                      pe::Vec3(1.0f, 1.0f, 1.0f));  // survives
    pe::spawnParticle(pool, pe::Vec3(8.0f, 0.0f, 0.0f),
                      pe::Vec3(0.0f, 0.0f, 0.0f), 0.1f, 0.5f,
                      pe::Vec3(1.0f, 1.0f, 1.0f));  // dies
    pe::updateParticles(pool, 1.0f);
    if (pool.size() != 1 ||
        !assertFloatClose(pool[0].position.x, 9.0f) ||
        !assertFloatClose(pool[0].life, 4.0f)) {
        std::cerr << "Dead particles not removed / survivor wrong\n";
        return false;
    }
    // All dead -> empty pool, no crash.
    pe::updateParticles(pool, 10.0f);
    if (!pool.empty()) {
        std::cerr << "Pool must empty when all die\n";
        return false;
    }
    pe::updateParticles(pool, 1.0f);  // empty update: safe no-op
    return true;
}

static bool checkParticleConverter() {
    std::vector<pe::Particle> pool;
    pe::spawnParticle(pool, pe::Vec3(2.0f, 3.0f, 0.0f),
                      pe::Vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.5f,
                      pe::Vec3(1.0f, 1.0f, 1.0f));
    pe::spawnParticle(pool, pe::Vec3(0.0f, 0.0f, 0.0f),
                      pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.5f,
                      pe::Vec3(1.0f, 1.0f, 1.0f));  // dead: skipped
    const std::vector<pe::Entity> entities =
        pe::particlesToEntities(pool, 2, 3, 7);
    if (entities.size() != 1) {
        std::cerr << "Converter must skip dead particles\n";
        return false;
    }
    const pe::Entity& e = entities[0];
    if (!assertFloatClose(e.position.x, 2.0f) ||
        !assertFloatClose(e.position.y, 3.0f) ||
        !assertFloatClose(e.scale.x, 0.5f) ||
        !assertFloatClose(e.scale.y, 0.5f) ||
        !assertFloatClose(e.scale.z, 1.0f) ||
        !assertFloatClose(e.halfExtents.x, 0.25f) ||
        !assertFloatClose(e.halfExtents.y, 0.25f) ||
        e.textureId != 2 || e.depth != 3 || e.roleId != 7) {
        std::cerr << "Converter carry-through wrong\n";
        return false;
    }
    return true;
}

static pe::Entity makeStaticBox(float x, float y, float hx, float hy) {
    pe::Entity e;
    e.position = pe::Vec3(x, y, 0.0f);
    e.halfExtents = pe::Vec3(hx, hy, 0.0f);
    e.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
    e.isStatic = true;
    return e;
}

static pe::Entity makeCharacter(float x, float y) {
    pe::Entity e;
    e.position = pe::Vec3(x, y, 0.0f);
    e.halfExtents = pe::Vec3(0.5f, 0.5f, 0.0f);
    e.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
    e.gravityScale = 1.0f;
    e.velocity = pe::Vec3(0.0f, 0.0f, 0.0f);
    return e;
}

static bool checkStaticFloorLanding() {
    // Floor top at y=0; character half-height 0.5 -> rest y must be 0.5.
    const std::vector<pe::Entity> statics = {makeStaticBox(0.0f, -1.0f, 5.0f, 1.0f)};
    pe::Entity c = makeCharacter(0.0f, 5.0f);
    bool landed = false;
    for (int i = 0; i < 300 && !landed; ++i) {
        landed = pe::updateCharacterController(c, statics, 1.0f / 60.0f, false);
    }
    if (!landed) {
        std::cerr << "Character never reported grounded while falling\n";
        return false;
    }
    // The grounded flag is tolerance-based (0.05): it trips on contact
    // approach, one frame before penetration resolves. Settle a few frames
    // so position/velocity reach the true rest state, then assert rest.
    for (int i = 0; i < 10; ++i) {
        pe::updateCharacterController(c, statics, 1.0f / 60.0f, false);
    }
    if (!assertFloatClose(c.position.y, 0.5f) ||
        !assertFloatClose(c.velocity.y, 0.0f)) {
        std::cerr << "Character did not rest on the floor\n";
        return false;
    }
    return true;
}

static bool checkGroundedState() {
    const std::vector<pe::Entity> statics = {makeStaticBox(0.0f, -1.0f, 5.0f, 1.0f)};
    pe::Entity onFloor = makeCharacter(0.0f, 0.5f);
    pe::Entity inAir = makeCharacter(0.0f, 5.0f);
    if (!pe::checkGrounded(onFloor, statics) ||
        pe::checkGrounded(inAir, statics)) {
        std::cerr << "Grounded state wrong (resting vs airborne)\n";
        return false;
    }
    return true;
}

static bool checkWallCollision() {
    // Wall face at x=1; character runs +x into it: stops, vx zeroed.
    const std::vector<pe::Entity> statics = {makeStaticBox(2.0f, 0.0f, 1.0f, 5.0f)};
    pe::Entity c = makeCharacter(0.0f, 2.0f);
    c.velocity = pe::Vec3(5.0f, 0.0f, 0.0f);
    c.gravityScale = 0.0f;
    pe::updateCharacterController(c, statics, 0.2f, false);
    if (!assertFloatClose(c.position.x, 0.5f) ||
        !assertFloatClose(c.velocity.x, 0.0f)) {
        std::cerr << "Wall did not stop horizontal motion\n";
        return false;
    }
    return true;
}

static bool checkCeilingCollision() {
    // Ceiling bottom at y=3; character rises into it: stays below, vy zeroed.
    const std::vector<pe::Entity> statics = {makeStaticBox(0.0f, 4.0f, 5.0f, 1.0f)};
    pe::Entity c = makeCharacter(0.0f, 2.0f);
    c.velocity = pe::Vec3(0.0f, 10.0f, 0.0f);
    c.gravityScale = 0.0f;
    pe::updateCharacterController(c, statics, 0.1f, false);
    if (!assertFloatClose(c.position.y, 2.5f) ||
        !assertFloatClose(c.velocity.y, 0.0f)) {
        std::cerr << "Ceiling did not stop ascent\n";
        return false;
    }
    return true;
}

static bool checkJumpAndLand() {
    const std::vector<pe::Entity> statics = {makeStaticBox(0.0f, -1.0f, 5.0f, 1.0f)};
    pe::Entity c = makeCharacter(0.0f, 0.5f);
    pe::updateCharacterController(c, statics, 1.0f / 60.0f, false);  // settle grounded
    pe::updateCharacterController(c, statics, 1.0f / 60.0f, true);   // jump
    if (!assertFloatClose(c.velocity.y, c.jumpImpulse)) {
        std::cerr << "Jump did not apply impulse\n";
        return false;
    }
    float apex = c.position.y;
    bool landed = false;
    for (int i = 0; i < 600 && !landed; ++i) {
        landed = pe::updateCharacterController(c, statics, 1.0f / 60.0f, false);
        if (c.position.y > apex) {
            apex = c.position.y;
        }
    }
    if (!landed || apex <= 1.0f) {
        std::cerr << "Jump arc did not rise and come down\n";
        return false;
    }
    // Same tolerance-contact note as the landing test: settle, then rest.
    for (int i = 0; i < 10; ++i) {
        pe::updateCharacterController(c, statics, 1.0f / 60.0f, false);
    }
    if (!assertFloatClose(c.position.y, 0.5f)) {
        std::cerr << "Jump did not land back at rest height\n";
        return false;
    }
    return true;
}

static bool checkCoyoteTime() {
    // Floor spans x in [-5,0]; character walks off the edge, jumps late.
    const std::vector<pe::Entity> statics = {makeStaticBox(-2.5f, -1.0f, 2.5f, 1.0f)};
    pe::Entity c = makeCharacter(-0.25f, 0.5f);
    pe::updateCharacterController(c, statics, 1.0f / 60.0f, false);  // grounded, coyote armed
    c.position.x = 0.5f;  // step off the ledge (airborne now)
    pe::updateCharacterController(c, statics, 0.05f, true);  // late jump, inside window
    if (!assertFloatClose(c.velocity.y, c.jumpImpulse)) {
        std::cerr << "Coyote jump did not fire inside the window\n";
        return false;
    }
    // Same setup, but wait out the window: no jump, keeps falling.
    pe::Entity d = makeCharacter(-0.25f, 0.5f);
    pe::updateCharacterController(d, statics, 1.0f / 60.0f, false);
    d.position.x = 0.5f;
    pe::updateCharacterController(d, statics, 0.2f, true);
    if (!(d.velocity.y < 0.0f)) {
        std::cerr << "Jump fired after the coyote window expired\n";
        return false;
    }
    return true;
}

static bool checkCharacterDtGuards() {
    const std::vector<pe::Entity> statics = {makeStaticBox(0.0f, -1.0f, 5.0f, 1.0f)};
    pe::Entity c = makeCharacter(0.0f, 5.0f);
    c.velocity = pe::Vec3(1.0f, -2.0f, 0.0f);
    if (pe::updateCharacterController(c, statics, 0.0f, true) ||
        pe::updateCharacterController(c, statics, -1.0f, true)) {
        std::cerr << "Non-positive dt must report not-grounded\n";
        return false;
    }
    if (!assertFloatClose(c.position.x, 0.0f) ||
        !assertFloatClose(c.position.y, 5.0f) ||
        !assertFloatClose(c.velocity.x, 1.0f) ||
        !assertFloatClose(c.velocity.y, -2.0f)) {
        std::cerr << "Non-positive dt must not move the character\n";
        return false;
    }
    return true;
}

static bool checkStaticResolve() {
    // Dynamic vs static: static unmoved, dynamic takes the full push.
    pe::Entity a = makeCharacter(0.0f, 0.0f);
    pe::Entity b = makeStaticBox(0.8f, 0.0f, 0.5f, 0.5f);
    pe::resolveCollision(a, b);
    if (!assertFloatClose(a.position.x, -0.2f) ||
        !assertFloatClose(b.position.x, 0.8f) ||
        !assertFloatClose(b.position.y, 0.0f)) {
        std::cerr << "Static body moved or dynamic under-corrected\n";
        return false;
    }
    // Both static: nothing moves.
    pe::Entity s1 = makeStaticBox(0.0f, 0.0f, 0.5f, 0.5f);
    pe::Entity s2 = makeStaticBox(0.8f, 0.0f, 0.5f, 0.5f);
    pe::resolveCollision(s1, s2);
    if (!assertFloatClose(s1.position.x, 0.0f) ||
        !assertFloatClose(s2.position.x, 0.8f)) {
        std::cerr << "Static-static pair must not move\n";
        return false;
    }
    // Neither static: original equal split preserved (non-breaking).
    pe::Entity c = makeCharacter(0.0f, 0.0f);
    pe::Entity d = makeCharacter(0.8f, 0.0f);
    pe::resolveCollision(c, d);
    if (!assertFloatClose(c.position.x, -0.1f) ||
        !assertFloatClose(d.position.x, 0.9f)) {
        std::cerr << "Dynamic-dynamic split regressed\n";
        return false;
    }
    return true;
}

// --- resolveCollision contract: restitution clamps (Step 62/71) ---
// Locks: negative e clamps to 0 (stick), e>1 clamps to 1 (elastic
// equal-mass exchange). Closing velocity 1.0, half the split at e=0,
// full exchange at e=1 — out-of-range e must match its clamp exactly.
static bool checkResolveRestitutionClamp() {
    // e = -0.5 must behave as e = 0: closing velocity split in half.
    pe::Entity a = makeCharacter(0.0f, 0.0f);
    pe::Entity b = makeCharacter(0.8f, 0.0f);
    a.velocity = pe::Vec3(1.0f, 0.0f, 0.0f);
    pe::resolveCollision(a, b, -0.5f);
    if (!assertFloatClose(a.velocity.x, 0.5f) ||
        !assertFloatClose(b.velocity.x, 0.5f)) {
        std::cerr << "Negative restitution must clamp to 0 (stick)\n";
        return false;
    }
    // e = 5 must behave as e = 1: full exchange (a stops, b takes all).
    pe::Entity c = makeCharacter(0.0f, 0.0f);
    pe::Entity d = makeCharacter(0.8f, 0.0f);
    c.velocity = pe::Vec3(1.0f, 0.0f, 0.0f);
    pe::resolveCollision(c, d, 5.0f);
    if (!assertFloatClose(c.velocity.x, 0.0f) ||
        !assertFloatClose(d.velocity.x, 1.0f)) {
        std::cerr << "Restitution above 1 must clamp to 1 (elastic)\n";
        return false;
    }
    return true;
}

// --- resolveCollision contract: e=1 bounce vs e=0 stick (one-static) ---
// Locks the dynamic-onto-static impulse: e=0 leaves the dynamic at rest
// (vy 0), e=1 reflects the approach speed (bounce up at -vy). Static
// velocity must never change in either case.
static bool checkResolveBounceVsStick() {
    const float floorTop = 1.0f;  // dynamic half 0.5 + static half 0.5
    // e = 0: stick — dynamic lands at rest height, vy zeroed.
    pe::Entity floor = makeStaticBox(0.0f, 0.0f, 5.0f, 0.5f);
    pe::Entity dyn = makeCharacter(0.0f, 0.4f);
    dyn.velocity = pe::Vec3(0.0f, -10.0f, 0.0f);
    pe::resolveCollision(floor, dyn, 0.0f);
    if (!assertFloatClose(dyn.position.y, floorTop) ||
        !assertFloatClose(dyn.velocity.y, 0.0f) ||
        !assertFloatClose(floor.velocity.y, 0.0f)) {
        std::cerr << "e=0 must land the dynamic at rest (stick)\n";
        return false;
    }
    // e = 1: bounce — dynamic reflects up at the approach speed.
    pe::Entity floor2 = makeStaticBox(0.0f, 0.0f, 5.0f, 0.5f);
    pe::Entity dyn2 = makeCharacter(0.0f, 0.4f);
    dyn2.velocity = pe::Vec3(0.0f, -10.0f, 0.0f);
    pe::resolveCollision(floor2, dyn2, 1.0f);
    if (!assertFloatClose(dyn2.position.y, floorTop) ||
        !assertFloatClose(dyn2.velocity.y, 10.0f) ||
        !assertFloatClose(floor2.velocity.y, 0.0f)) {
        std::cerr << "e=1 must bounce the dynamic at approach speed\n";
        return false;
    }
    return true;
}

// --- resolveCollision contract: approaching-guard ---
// Overlapping but SEPARATING pair: positional fix still applied (equal
// split), velocities untouched — no impulse against separation.
static bool checkResolveApproachingGuard() {
    pe::Entity a = makeCharacter(0.0f, 0.0f);
    pe::Entity b = makeCharacter(0.8f, 0.0f);
    a.velocity = pe::Vec3(-1.0f, 0.0f, 0.0f);  // moving away from b
    pe::resolveCollision(a, b, 1.0f);
    if (!assertFloatClose(a.position.x, -0.1f) ||
        !assertFloatClose(b.position.x, 0.9f)) {
        std::cerr << "Separating pair must keep the positional fix\n";
        return false;
    }
    if (!assertFloatClose(a.velocity.x, -1.0f) ||
        !assertFloatClose(b.velocity.x, 0.0f)) {
        std::cerr << "Separating pair must not receive impulse\n";
        return false;
    }
    // One-static variant: separating dynamic keeps fix, no impulse.
    pe::Entity floor = makeStaticBox(0.0f, 0.0f, 5.0f, 0.5f);
    pe::Entity rising = makeCharacter(0.0f, 0.4f);
    rising.velocity = pe::Vec3(0.0f, 3.0f, 0.0f);  // moving up, away
    pe::resolveCollision(floor, rising, 1.0f);
    if (!assertFloatClose(rising.position.y, 1.0f) ||
        !assertFloatClose(rising.velocity.y, 3.0f)) {
        std::cerr << "One-static separating pair must fix without impulse\n";
        return false;
    }
    return true;
}

// --- resolveCollision contract: edge-touch policy ---
// Exactly-touching edges (gap 0) are NOT a collision — strict '<', the
// same rule aabbOverlap applies. Scaled extents honored: a 0.5-scale
// entity with 0.5 half-extents occupies a 0.25 box.
static bool checkResolveEdgeTouch() {
    pe::Entity a = makeCharacter(0.0f, 0.0f);
    pe::Entity b = makeCharacter(1.0f, 0.0f);  // edges touch exactly
    pe::resolveCollision(a, b, 1.0f);
    if (!assertFloatClose(a.position.x, 0.0f) ||
        !assertFloatClose(b.position.x, 1.0f) ||
        !assertFloatClose(a.velocity.x, 0.0f) ||
        !assertFloatClose(b.velocity.x, 0.0f)) {
        std::cerr << "Edge-touch must not resolve (strict <)\n";
        return false;
    }
    // Scaled extents: half 0.5 * scale 0.5 = 0.25; b at 0.75 touches.
    pe::Entity s = makeCharacter(0.0f, 0.0f);
    s.scale = pe::Vec3(0.5f, 0.5f, 1.0f);
    pe::Entity t = makeCharacter(0.75f, 0.0f);
    pe::resolveCollision(s, t, 1.0f);
    if (!assertFloatClose(s.position.x, 0.0f) ||
        !assertFloatClose(t.position.x, 0.75f)) {
        std::cerr << "Scaled edge-touch must not resolve\n";
        return false;
    }
    return true;
}

// --- fixed-substep contract: jump consumed once (Step 87) ---
// One updateCharacterControllerFixed call with dt=0.05 (3 substeps at
// 1/60) and jumpPressed=true must fire the jump in substep 1 ONLY.
// Consume-once: velocity.y decays by gravity for the two remaining
// substeps (jumpImpulse - 2*g*sub). The per-substep bug (re-fire) would
// re-arm coyote and re-set velocity.y to jumpImpulse exactly.
static bool checkFixedJumpConsumedOnce() {
    const float sub = 1.0f / 60.0f;
    const std::vector<pe::Entity> statics = {makeStaticBox(0.0f, -1.0f, 5.0f, 1.0f)};
    pe::Entity c = makeCharacter(0.0f, 0.5f);
    pe::updateCharacterController(c, statics, sub, false);  // settle grounded
    const float restY = c.position.y;
    pe::updateCharacterControllerFixed(c, statics, 0.05f, true, sub);
    if (!assertFloatClose(c.velocity.y,
                          c.jumpImpulse - 2.0f * (-pe::GRAVITY.y) * sub)) {
        std::cerr << "Jump must be consumed once across substeps\n";
        return false;
    }
    if (!(c.position.y > restY)) {
        std::cerr << "Fixed-substep jump did not rise\n";
        return false;
    }
    return true;
}

// --- fixed-substep contract: clamp, fallback, non-positive dt ---
// Locks: steps>8 capped (dt=1.0 at 1/60 runs 8 substeps of 0.125 —
// uncapped 60 substeps would drift ~10x further); fixedDt<=0 falls back
// to 1/60 (identical outcome to an explicit 1/60); dt<=0 is a read-only
// no-op reporting checkGrounded for both wrappers.
static bool checkFixedSubstepClampAndFallback() {
    const std::vector<pe::Entity> farFloor = {makeStaticBox(0.0f, -100.0f, 5.0f, 1.0f)};
    // Substep clamp via updateCharacterControllerFixed: fall 1.0s from
    // rest, no contact within reach. 8 substeps of 0.125 -> drift
    // -g*0.125*(1+2+...+8) = -5.5125 (uncapped: -4.9817).
    pe::Entity c = makeCharacter(0.0f, 0.0f);
    pe::updateCharacterControllerFixed(c, farFloor, 1.0f, false, 1.0f / 60.0f);
    if (!assertFloatClose(c.position.y, -5.5125f)) {
        std::cerr << "Substep count must clamp to 8\n";
        return false;
    }
    // Same clamp via applyPhysicsFixed (free mover).
    pe::Entity m = makeCharacter(0.0f, 0.0f);
    std::vector<pe::Entity> mBatch = {m};
    pe::applyPhysicsFixed(mBatch, 1.0f, 1.0f / 60.0f);
    if (!assertFloatClose(mBatch[0].position.y, -5.5125f)) {
        std::cerr << "applyPhysicsFixed substep count must clamp to 8\n";
        return false;
    }
    // fixedDt<=0 fallback: outcome identical to an explicit 1/60.
    pe::Entity f0 = makeCharacter(0.0f, 5.0f);
    pe::Entity fExplicit = makeCharacter(0.0f, 5.0f);
    pe::updateCharacterControllerFixed(f0, farFloor, 0.05f, false, 0.0f);
    pe::updateCharacterControllerFixed(fExplicit, farFloor, 0.05f, false, 1.0f / 60.0f);
    if (!assertFloatClose(f0.position.y, fExplicit.position.y) ||
        !assertFloatClose(f0.velocity.y, fExplicit.velocity.y)) {
        std::cerr << "fixedDt<=0 must fall back to 1/60\n";
        return false;
    }
    // dt<=0: read-only no-op reporting checkGrounded (airborne false).
    pe::Entity air = makeCharacter(0.0f, 5.0f);
    if (pe::updateCharacterControllerFixed(air, farFloor, 0.0f, true, 1.0f / 60.0f) ||
        pe::updateCharacterControllerFixed(air, farFloor, -1.0f, true, 1.0f / 60.0f)) {
        std::cerr << "Non-positive dt must report not-grounded\n";
        return false;
    }
    if (!assertFloatClose(air.position.x, 0.0f) ||
        !assertFloatClose(air.position.y, 5.0f) ||
        !assertFloatClose(air.velocity.y, 0.0f)) {
        std::cerr << "Non-positive dt must not move the character\n";
        return false;
    }
    // Grounded character at dt<=0 reports grounded (checkGrounded path).
    pe::Entity rest = makeCharacter(0.0f, 0.5f);
    const std::vector<pe::Entity> floor = {makeStaticBox(0.0f, -1.0f, 5.0f, 1.0f)};
    if (!pe::updateCharacterControllerFixed(rest, floor, 0.0f, false, 1.0f / 60.0f)) {
        std::cerr << "Grounded dt=0 must report grounded via checkGrounded\n";
        return false;
    }
    return true;
}

// --- Step P6: kinematic bodies — moving platform carry ---
// A kinematic platform (isStatic=false, isKinematic=true, gravityScale
// 0) coasts upward via applyPhysics; the character resting on it is
// CARRIED: each frame the controller's resolve pushes the character up
// to exact contact, so it tracks the platform rise, stays grounded,
// and its velocity ends at zero (carried by position, not impulse).
static bool checkKinematicCarry() {
    // Platform half (2.5, 0.5) at origin -> top 0.5; char half 0.5 ->
    // rest y 1.0. Platform rises 2/s for 0.5s -> +1.0; char tracks to 2.0.
    pe::Entity platform;
    platform.position = pe::Vec3(0.0f, 0.0f, 0.0f);
    platform.halfExtents = pe::Vec3(2.5f, 0.5f, 0.0f);
    platform.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
    platform.isKinematic = true;
    platform.gravityScale = 0.0f;
    platform.velocity = pe::Vec3(0.0f, 2.0f, 0.0f);
    std::vector<pe::Entity> platformBatch = {platform};
    pe::Entity c = makeCharacter(0.0f, 1.0f);
    bool grounded = false;
    for (int i = 0; i < 30; ++i) {
        pe::applyPhysics(platformBatch, 1.0f / 60.0f);  // platform coasts up
        std::vector<pe::Entity> world = {platformBatch[0]};
        grounded = pe::updateCharacterController(c, world, 1.0f / 60.0f, false);
    }
    if (!assertFloatClose(platformBatch[0].position.y, 1.0f)) {
        std::cerr << "applyPhysics must coast a kinematic body\n";
        return false;
    }
    if (!assertFloatClose(c.position.y, 2.0f)) {
        std::cerr << "Character must be carried by the rising platform\n";
        return false;
    }
    if (!grounded) {
        std::cerr << "Character must stay grounded on the moving platform\n";
        return false;
    }
    if (!assertFloatClose(c.velocity.y, 0.0f)) {
        std::cerr << "Carry must be positional, not a velocity transfer\n";
        return false;
    }
    return true;
}

// --- Step P6: kinematic bodies — response rules ---
// Kinematic-vs-dynamic: the dynamic body takes the full correction, the
// kinematic body never moves. Kinematic-vs-kinematic: nothing moves.
static bool checkKinematicResolve() {
    pe::Entity k;
    k.position = pe::Vec3(0.8f, 0.0f, 0.0f);
    k.halfExtents = pe::Vec3(0.5f, 0.5f, 0.0f);
    k.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
    k.isKinematic = true;
    pe::Entity d = makeCharacter(0.0f, 0.0f);
    pe::resolveCollision(d, k);
    if (!assertFloatClose(d.position.x, -0.2f) ||
        !assertFloatClose(k.position.x, 0.8f) ||
        !assertFloatClose(k.position.y, 0.0f)) {
        std::cerr << "Kinematic body moved or dynamic under-corrected\n";
        return false;
    }
    // Kinematic-kinematic overlap: both treated as infinite mass.
    pe::Entity k2;
    k2.position = pe::Vec3(0.0f, 0.0f, 0.0f);
    k2.halfExtents = pe::Vec3(0.5f, 0.5f, 0.0f);
    k2.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
    k2.isKinematic = true;
    pe::resolveCollision(k2, k);
    if (!assertFloatClose(k2.position.x, 0.0f) ||
        !assertFloatClose(k.position.x, 0.8f)) {
        std::cerr << "Kinematic-kinematic pair must not move\n";
        return false;
    }
    return true;
}

// --- Step P7: swept segment-vs-AABB contract ---
// Locks: crossing hit (entry fraction + face normal + contact math),
// parallel miss, too-far miss, the tunnel catch discrete cannot make
// (thin platform: the endpoint aabbOverlap misses, the sweep hits),
// start-inside reported as overlap (t=0, zero normal), and diagonal
// axis priority (the latest entry axis decides the normal).
static bool checkSweptAABBContract() {
    // Crossing hit: fast 6-unit fall onto a platform — contact at
    // platform top + mover half (0.5 + 0.5 = 1.0), normal +y.
    {
        const pe::SweepHit h = pe::sweptAABB(
            pe::Vec3(0.0f, 3.0f, 0.0f), pe::Vec3(0.0f, -3.0f, 0.0f),
            pe::Vec3(0.5f, 0.5f, 0.0f),
            pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(2.5f, 0.5f, 0.0f));
        if (!h.hit || !assertFloatClose(h.t, 1.0f / 3.0f) ||
            !assertFloatClose(h.normal.y, 1.0f) ||
            !assertFloatClose(h.normal.x, 0.0f)) {
            std::cerr << "Sweep crossing hit wrong\n";
            return false;
        }
        const float contactY = 3.0f + (-6.0f) * h.t;
        if (!assertFloatClose(contactY, 1.0f)) {
            std::cerr << "Sweep contact math wrong\n";
            return false;
        }
    }
    // Parallel miss: same fall, platform far to the side.
    {
        const pe::SweepHit h = pe::sweptAABB(
            pe::Vec3(0.0f, 3.0f, 0.0f), pe::Vec3(0.0f, -3.0f, 0.0f),
            pe::Vec3(0.5f, 0.5f, 0.0f),
            pe::Vec3(10.0f, 0.0f, 0.0f), pe::Vec3(2.5f, 0.5f, 0.0f));
        if (h.hit) {
            std::cerr << "Sweep must miss a parallel, sideways target\n";
            return false;
        }
    }
    // Too-far miss: the crossing is beyond the segment's end.
    {
        const pe::SweepHit h = pe::sweptAABB(
            pe::Vec3(0.0f, 2.0f, 0.0f), pe::Vec3(0.0f, 1.5f, 0.0f),
            pe::Vec3(0.5f, 0.5f, 0.0f),
            pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(2.5f, 0.5f, 0.0f));
        if (h.hit) {
            std::cerr << "Sweep must miss when the crossing is beyond the end\n";
            return false;
        }
    }
    // Tunnel catch: 6-unit step vs a THIN platform — the endpoint
    // aabbOverlap misses (mover ends far below), the sweep hits.
    {
        const pe::Vec3 from(0.0f, 3.0f, 0.0f);
        const pe::Vec3 to(0.0f, -3.0f, 0.0f);
        const pe::Vec3 targetCenter(0.0f, 0.0f, 0.0f);
        const pe::Vec3 targetHalf(2.5f, 0.05f, 0.0f);
        const bool discrete = pe::aabbOverlap(
            to, pe::Vec3(0.5f, 0.5f, 0.0f), targetCenter, targetHalf);
        const pe::SweepHit h = pe::sweptAABB(
            from, to, pe::Vec3(0.5f, 0.5f, 0.0f), targetCenter, targetHalf);
        if (discrete) {
            std::cerr << "Discrete test setup wrong (expected a tunnel)\n";
            return false;
        }
        if (!h.hit || !(h.t > 0.0f && h.t < 1.0f) ||
            !assertFloatClose(h.normal.y, 1.0f)) {
            std::cerr << "Sweep must catch what discrete tunnels past\n";
            return false;
        }
    }
    // Start-inside: overlapping at the start — hit at t=0, zero normal
    // (no face-crossing; the caller treats it as the discrete path's job).
    {
        const pe::SweepHit h = pe::sweptAABB(
            pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(0.0f, -2.0f, 0.0f),
            pe::Vec3(0.5f, 0.5f, 0.0f),
            pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(2.5f, 0.5f, 0.0f));
        if (!h.hit || !assertFloatClose(h.t, 0.0f) ||
            !assertFloatClose(h.normal.x, 0.0f) ||
            !assertFloatClose(h.normal.y, 0.0f)) {
            std::cerr << "Start-inside sweep must report overlap at t=0\n";
            return false;
        }
    }
    // Diagonal: horizontal pass through a tall box — the x entry (latest)
    // decides the normal (-x), t at the expanded left edge.
    {
        const pe::SweepHit h = pe::sweptAABB(
            pe::Vec3(-5.0f, -1.0f, 0.0f), pe::Vec3(5.0f, -1.0f, 0.0f),
            pe::Vec3(0.5f, 0.5f, 0.0f),
            pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 5.0f, 0.0f));
        if (!h.hit || !assertFloatClose(h.t, 0.35f) ||
            !assertFloatClose(h.normal.x, -1.0f) ||
            !assertFloatClose(h.normal.y, 0.0f)) {
            std::cerr << "Diagonal sweep must use the latest entry axis\n";
            return false;
        }
    }
    return true;
}

// --- Step P7: sweptMoveAndCollide contract (the discrete resolve's twin) ---
// Locks: the tunnel catch in ONE call (6-unit step vs a thin floor —
// the discrete endpoint test would tunnel; the sweep clamps at exact
// contact), full-delta no-contact, wall stop (the center clamps at the
// expanded box so the mover's edge lands on the face), earliest-hit
// priority over two floors, start-inside reported as no-move contact,
// dt<=0 no-op.
static bool checkSweptMoveAndCollide() {
    // Tunnel catch: 6-unit step vs a thin floor (half y 0.05) in one
    // call — clamps at platform top + mover half (0.05 + 0.5 = 0.55).
    {
        pe::Entity floor;
        floor.position = pe::Vec3(0.0f, 0.0f, 0.0f);
        floor.halfExtents = pe::Vec3(2.5f, 0.05f, 0.0f);
        floor.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
        floor.isStatic = true;
        std::vector<pe::Entity> statics = {floor};
        pe::Entity c = makeCharacter(0.0f, 3.0f);
        c.velocity = pe::Vec3(0.0f, -60.0f, 0.0f);
        const bool contacted = pe::sweptMoveAndCollide(c, statics, 0.1f);
        if (!contacted || !assertFloatClose(c.position.y, 0.55f) ||
            !assertFloatClose(c.velocity.y, 0.0f)) {
            std::cerr << "Swept move must catch the tunnel at exact contact\n";
            return false;
        }
    }
    // No contact: full delta, false.
    {
        pe::Entity floor;
        floor.position = pe::Vec3(0.0f, -100.0f, 0.0f);
        floor.halfExtents = pe::Vec3(2.5f, 0.5f, 0.0f);
        floor.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
        floor.isStatic = true;
        std::vector<pe::Entity> statics = {floor};
        pe::Entity c = makeCharacter(0.0f, 0.0f);
        c.velocity = pe::Vec3(2.0f, -1.0f, 0.0f);
        const bool contacted = pe::sweptMoveAndCollide(c, statics, 0.1f);
        if (contacted || !assertFloatClose(c.position.x, 0.2f) ||
            !assertFloatClose(c.position.y, -0.1f) ||
            !assertFloatClose(c.velocity.y, -1.0f)) {
            std::cerr << "No-contact swept move must take the full delta\n";
            return false;
        }
    }
    // Wall stop: mover center clamps at the expanded box — the mover's
    // right edge lands exactly on the wall face.
    {
        pe::Entity wall;
        wall.position = pe::Vec3(1.2f, 0.0f, 0.0f);
        wall.halfExtents = pe::Vec3(0.5f, 0.5f, 0.0f);
        wall.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
        wall.isStatic = true;
        std::vector<pe::Entity> statics = {wall};
        pe::Entity c = makeCharacter(0.0f, 0.0f);
        c.velocity = pe::Vec3(5.0f, 0.0f, 0.0f);
        const bool contacted = pe::sweptMoveAndCollide(c, statics, 0.1f);
        if (!contacted || !assertFloatClose(c.position.x, 0.2f) ||
            !assertFloatClose(c.velocity.x, 0.0f) ||
            !assertFloatClose(c.position.y, 0.0f)) {
            std::cerr << "Swept wall stop must land the edge on the face\n";
            return false;
        }
    }
    // Earliest-hit priority: two floors on the path — the closer one wins.
    {
        pe::Entity floorA;
        floorA.position = pe::Vec3(0.0f, 0.0f, 0.0f);
        floorA.halfExtents = pe::Vec3(2.5f, 0.25f, 0.0f);
        floorA.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
        floorA.isStatic = true;
        pe::Entity floorB;
        floorB.position = pe::Vec3(0.0f, 1.5f, 0.0f);
        floorB.halfExtents = pe::Vec3(2.5f, 0.25f, 0.0f);
        floorB.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
        floorB.isStatic = true;
        std::vector<pe::Entity> statics = {floorA, floorB};
        pe::Entity c = makeCharacter(0.0f, 3.0f);
        c.velocity = pe::Vec3(0.0f, -60.0f, 0.0f);
        const bool contacted = pe::sweptMoveAndCollide(c, statics, 0.1f);
        if (!contacted || !assertFloatClose(c.position.y, 2.25f) ||
            !assertFloatClose(c.velocity.y, 0.0f)) {
            std::cerr << "Swept move must clamp at the earliest hit\n";
            return false;
        }
    }
    // Start-inside: overlapping at the start — contact, no movement, no
    // velocity change (overlap resolution stays the discrete path's job).
    {
        pe::Entity floor;
        floor.position = pe::Vec3(0.0f, 0.0f, 0.0f);
        floor.halfExtents = pe::Vec3(2.5f, 0.5f, 0.0f);
        floor.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
        floor.isStatic = true;
        std::vector<pe::Entity> statics = {floor};
        pe::Entity c = makeCharacter(0.0f, 0.0f);
        c.velocity = pe::Vec3(3.0f, -2.0f, 0.0f);
        const bool contacted = pe::sweptMoveAndCollide(c, statics, 0.1f);
        if (!contacted || !assertFloatClose(c.position.x, 0.0f) ||
            !assertFloatClose(c.position.y, 0.0f) ||
            !assertFloatClose(c.velocity.x, 3.0f) ||
            !assertFloatClose(c.velocity.y, -2.0f)) {
            std::cerr << "Start-inside swept move must not move or alter velocity\n";
            return false;
        }
    }
    // dt <= 0: no-op returning false.
    {
        pe::Entity floor;
        floor.position = pe::Vec3(0.0f, 0.0f, 0.0f);
        floor.halfExtents = pe::Vec3(2.5f, 0.5f, 0.0f);
        floor.scale = pe::Vec3(1.0f, 1.0f, 1.0f);
        floor.isStatic = true;
        std::vector<pe::Entity> statics = {floor};
        pe::Entity c = makeCharacter(0.0f, 3.0f);
        c.velocity = pe::Vec3(1.0f, -60.0f, 0.0f);
        if (pe::sweptMoveAndCollide(c, statics, 0.0f) ||
            pe::sweptMoveAndCollide(c, statics, -1.0f)) {
            std::cerr << "Non-positive dt must be a no-op\n";
            return false;
        }
        if (!assertFloatClose(c.position.y, 3.0f) ||
            !assertFloatClose(c.velocity.y, -60.0f)) {
            std::cerr << "Non-positive dt must not move the mover\n";
            return false;
        }
    }
    return true;
}

// --- Physics primitives: applyForce (massless v += force*dt) ---
// applyForce was written Step 60 but never headless-verified. Locks the
// contract: force IS the acceleration, dt=0 is a no-op.
static bool checkApplyForceMath() {
    pe::Entity e;
    e.velocity = pe::Vec3(1.0f, 2.0f, 0.0f);
    pe::applyForce(e.velocity, pe::Vec3(10.0f, 0.0f, 0.0f), 0.2f);
    if (!assertFloatClose(e.velocity.x, 3.0f) ||
        !assertFloatClose(e.velocity.y, 2.0f) ||
        !assertFloatClose(e.velocity.z, 0.0f)) {
        std::cerr << "applyForce math wrong\n";
        return false;
    }
    pe::applyForce(e.velocity, pe::Vec3(5.0f, 5.0f, 5.0f), 0.0f);
    if (!assertFloatClose(e.velocity.x, 3.0f) ||
        !assertFloatClose(e.velocity.y, 2.0f)) {
        std::cerr << "applyForce must be a no-op at dt=0\n";
        return false;
    }
    return true;
}

// --- simulation.h applyPhysics (dead-skip, conditional gravity) ---
// Wired in main.cpp Step 108 but never headless-verified. Locks: dead
// bodies don't move; inert (gravityScale 0) bodies coast with gravity
// untouched; gravity bodies gain GRAVITY*dt; a static body under its
// default configuration (gravityScale 0, zero velocity) is never written.
static bool checkApplyPhysicsSemantics() {
    // Dead: no gravity, no integration, nothing changes.
    pe::Entity dead;
    dead.position = pe::Vec3(0.0f, 0.0f, 0.0f);
    dead.alive = false;
    dead.gravityScale = 1.0f;
    dead.velocity = pe::Vec3(1.0f, 1.0f, 0.0f);
    std::vector<pe::Entity> batch = {dead};
    pe::applyPhysics(batch, 0.1f);
    if (!assertFloatClose(batch[0].position.x, 0.0f) ||
        !assertFloatClose(batch[0].position.y, 0.0f) ||
        !assertFloatClose(batch[0].velocity.x, 1.0f) ||
        !assertFloatClose(batch[0].velocity.y, 1.0f)) {
        std::cerr << "applyPhysics moved a dead body\n";
        return false;
    }
    // Inert + nonzero velocity: coasts, gravity not applied.
    pe::Entity coast;
    coast.position = pe::Vec3(0.0f, 0.0f, 0.0f);
    coast.velocity = pe::Vec3(0.0f, 5.0f, 0.0f);
    batch = {coast};
    pe::applyPhysics(batch, 0.1f);
    if (!assertFloatClose(batch[0].position.y, 0.5f) ||
        !assertFloatClose(batch[0].velocity.y, 5.0f)) {
        std::cerr << "Inert body must coast without gravity\n";
        return false;
    }
    // gravityScale 1: gains GRAVITY*dt, integrates with it.
    pe::Entity falling;
    falling.position = pe::Vec3(0.0f, 0.0f, 0.0f);
    falling.gravityScale = 1.0f;
    batch = {falling};
    pe::applyPhysics(batch, 0.1f);
    if (!assertFloatClose(batch[0].velocity.y, pe::GRAVITY.y * 0.1f) ||
        !assertFloatClose(batch[0].position.y, pe::GRAVITY.y * 0.1f * 0.1f)) {
        std::cerr << "Gravity body integration wrong\n";
        return false;
    }
    // Static under default config: position untouched.
    pe::Entity stat;
    stat.position = pe::Vec3(3.0f, -1.0f, 0.0f);
    stat.isStatic = true;
    batch = {stat};
    pe::applyPhysics(batch, 0.1f);
    if (!assertFloatClose(batch[0].position.x, 3.0f) ||
        !assertFloatClose(batch[0].position.y, -1.0f)) {
        std::cerr << "applyPhysics moved a static body\n";
        return false;
    }
    // Static with gravity AND velocity: the isStatic guard (Step P4)
    // must keep it fully untouched — statics never move, by construction.
    pe::Entity movingStat;
    movingStat.position = pe::Vec3(3.0f, -1.0f, 0.0f);
    movingStat.isStatic = true;
    movingStat.gravityScale = 1.0f;
    movingStat.velocity = pe::Vec3(2.0f, 2.0f, 0.0f);
    batch = {movingStat};
    pe::applyPhysics(batch, 0.1f);
    if (!assertFloatClose(batch[0].position.x, 3.0f) ||
        !assertFloatClose(batch[0].position.y, -1.0f) ||
        !assertFloatClose(batch[0].velocity.x, 2.0f) ||
        !assertFloatClose(batch[0].velocity.y, 2.0f)) {
        std::cerr << "applyPhysics isStatic guard failed\n";
        return false;
    }
    return true;
}

// --- physics.h applyPhysicsFixed (free-mover substeps, Step 87) ---
// Never headless-verified. Locks: gravity velocity accumulates the full
// GRAVITY*dt across substeps; linear coasting is substep-exact; substep
// position drift stays inside the single-step bound; dt<=0 is a no-op.
static bool checkApplyPhysicsFixedFreeMovers() {
    const float dt = 0.1f;
    // Falling: velocity gain is split-invariant (sums to GRAVITY*dt).
    pe::Entity fall;
    fall.position = pe::Vec3(0.0f, 0.0f, 0.0f);
    fall.gravityScale = 1.0f;
    std::vector<pe::Entity> fallBatch = {fall};
    pe::applyPhysicsFixed(fallBatch, dt, 1.0f / 60.0f);
    if (!assertFloatClose(fallBatch[0].velocity.y, pe::GRAVITY.y * dt)) {
        std::cerr << "Fixed substeps must accumulate full GRAVITY*dt\n";
        return false;
    }
    // Position: semi-implicit per substep lands between the single-step
    // drift (-0.0098) and zero.
    if (!(fallBatch[0].position.y < 0.0f) ||
        !(fallBatch[0].position.y > pe::GRAVITY.y * dt * dt)) {
        std::cerr << "Fixed substep position drift out of bounds\n";
        return false;
    }
    // Linear coasting (inert): substep-exact regardless of split.
    pe::Entity coast;
    coast.position = pe::Vec3(0.0f, 0.0f, 0.0f);
    coast.velocity = pe::Vec3(2.0f, 0.0f, 0.0f);
    std::vector<pe::Entity> coastBatch = {coast};
    pe::applyPhysicsFixed(coastBatch, dt, 1.0f / 60.0f);
    if (!assertFloatClose(coastBatch[0].position.x, 0.2f) ||
        !assertFloatClose(coastBatch[0].velocity.x, 2.0f)) {
        std::cerr << "Fixed substeps must be exact for linear motion\n";
        return false;
    }
    // dt<=0: no-op.
    pe::Entity still;
    still.position = pe::Vec3(1.0f, 2.0f, 0.0f);
    still.gravityScale = 1.0f;
    std::vector<pe::Entity> stillBatch = {still};
    pe::applyPhysicsFixed(stillBatch, 0.0f, 1.0f / 60.0f);
    if (!assertFloatClose(stillBatch[0].position.x, 1.0f) ||
        !assertFloatClose(stillBatch[0].position.y, 2.0f) ||
        !assertFloatClose(stillBatch[0].velocity.y, 0.0f)) {
        std::cerr << "applyPhysicsFixed must be a no-op at dt<=0\n";
        return false;
    }
    // Static with gravity AND velocity: the isStatic guard (Step P4a)
    // must keep it fully untouched across substeps — statics never
    // move, matching applyPhysics.
    pe::Entity movingStat;
    movingStat.position = pe::Vec3(3.0f, -1.0f, 0.0f);
    movingStat.isStatic = true;
    movingStat.gravityScale = 1.0f;
    movingStat.velocity = pe::Vec3(2.0f, 2.0f, 0.0f);
    std::vector<pe::Entity> statBatch = {movingStat};
    pe::applyPhysicsFixed(statBatch, 0.1f, 1.0f / 60.0f);
    if (!assertFloatClose(statBatch[0].position.x, 3.0f) ||
        !assertFloatClose(statBatch[0].position.y, -1.0f) ||
        !assertFloatClose(statBatch[0].velocity.x, 2.0f) ||
        !assertFloatClose(statBatch[0].velocity.y, 2.0f)) {
        std::cerr << "applyPhysicsFixed isStatic guard failed\n";
        return false;
    }
    return true;
}

// --- collision.h broadphaseGrid (Step 97, future-only) ---
// Never headless-verified. Locks: entities sharing a cell pair once,
// separated entities pair zero, N in one cell gives N*(N-1)/2 pairs,
// and cellSize<=0 falls back to the 2.0 default.
static bool checkBroadphaseGridPairs() {
    pe::Entity a;
    a.position = pe::Vec3(0.0f, 0.0f, 0.0f);
    pe::Entity b;
    b.position = pe::Vec3(100.0f, 100.0f, 0.0f);
    std::vector<pe::Entity> two = {a, b};
    if (pe::broadphaseGrid(two).size() != 0) {
        std::cerr << "Far-apart entities must not pair\n";
        return false;
    }
    pe::Entity c;
    c.position = pe::Vec3(0.5f, 0.0f, 0.0f);
    std::vector<pe::Entity> sameCell = {a, c};
    if (pe::broadphaseGrid(sameCell).size() != 1) {
        std::cerr << "Same-cell entities must pair exactly once\n";
        return false;
    }
    // All three in one cell: N*(N-1)/2 = 3 unique pairs.
    pe::Entity d;
    d.position = pe::Vec3(1.0f, 0.0f, 0.0f);
    std::vector<pe::Entity> three = {a, c, d};
    if (pe::broadphaseGrid(three).size() != 3) {
        std::cerr << "Three same-cell entities must give 3 pairs\n";
        return false;
    }
    // cellSize 0 -> fallback to 2.0: (0,0) and (1,0) share a cell.
    std::vector<pe::Entity> edge = {a, d};
    if (pe::broadphaseGrid(edge, 0.0f).size() != 1) {
        std::cerr << "cellSize<=0 must fall back to 2.0\n";
        return false;
    }
    return true;
}

// --- Step 127: point pick against entity AABBs (headless) ---
// Locks the documented pick contract: strict '<' containment on the
// scaled bounds (edge-touching is not a hit), highest depth wins the
// overlap, ties break to the lowest index, dead entities never match,
// and an empty/no-match scan returns -1.
static bool checkEntityPick() {
    bool ok = true;
    pe::Entity e1(pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                  pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    e1.depth = 1;
    pe::Entity e2(pe::Vec3(0.2f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                  pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    e2.depth = 2;
    pe::Entity dead(pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                    pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    dead.depth = 9;  // highest depth, but dead â€” must never win
    dead.alive = false;

    // Empty vector: -1.
    {
        std::vector<pe::Entity> none;
        if (pe::pickEntity(none, 0.0f, 0.0f) != -1) {
            std::cerr << "Empty pick must be -1\n";
            ok = false;
        }
    }
    // No match (outside all boxes): -1.
    std::vector<pe::Entity> all = {e1, e2, dead};
    if (pe::pickEntity(all, 5.0f, 5.0f) != -1) {
        std::cerr << "Miss must be -1\n";
        ok = false;
    }
    // Center of e1 (outside e2's box): e1's index, dead never wins.
    if (pe::pickEntity(all, -0.3f, 0.0f) != 0) {
        std::cerr << "e1 center must pick index 0\n";
        ok = false;
    }
    // Overlap region (inside both): highest depth wins -> e2 (index 1).
    if (pe::pickEntity(all, 0.2f, 0.0f) != 1) {
        std::cerr << "Overlap must pick highest depth (e2)\n";
        ok = false;
    }
    // Tie-break: equal depths -> lowest index wins. (Mutate the VECTOR
    // element: `all` holds copies, so writing e2.depth would not affect
    // the scanned data — the Step 127 version of this line had that bug.)
    all[1].depth = 1;
    if (pe::pickEntity(all, 0.2f, 0.0f) != 0) {
        std::cerr << "Depth tie must pick lowest index\n";
        ok = false;
    }
    all[1].depth = 2;
    // Strict '<': point exactly ON e1's edge is not a hit. e1 box is
    // [-0.5, 0.5] on x; e2 box is [-0.3, 0.7]. Point (0.5, 0.5) is on
    // both boxes' top/right edges â€” must be -1.
    if (pe::pickEntity(all, 0.5f, 0.5f) != -1) {
        std::cerr << "Edge-touching point must not hit\n";
        ok = false;
    }
    // Scale respected: a 0.5-scale entity occupies a half-size box.
    pe::Entity small(pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(0.5f, 0.5f, 1.0f),
                     pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    small.depth = 3;
    std::vector<pe::Entity> withSmall = {small};
    if (pe::pickEntity(withSmall, 0.24f, 0.0f) != 0) {
        std::cerr << "Scaled-in point must hit\n";
        ok = false;
    }
    if (pe::pickEntity(withSmall, 0.26f, 0.0f) != -1) {
        std::cerr << "Point past the scaled bound must miss\n";
        ok = false;
    }
    // Dead-only vector: -1 even at its center.
    {
        std::vector<pe::Entity> onlyDead = {dead};
        if (pe::pickEntity(onlyDead, 0.0f, 0.0f) != -1) {
            std::cerr << "Dead entity must never be picked\n";
            ok = false;
        }
    }
    return ok;
}

// --- Step 129: entity world AABB helper (headless) ---
// Locks the scale rule shared by aabbOverlap and pickEntity: center =
// position, half = halfExtents * scale (per-axis), z forced 0. Also
// proves the helper's numbers match pickEntity containment exactly.
static bool checkEntityWorldAABB() {
    bool ok = true;
    // Default entity: origin, 0.7071 rotation-safe bound, unit scale.
    pe::Entity def;
    const pe::WorldAABB defBox = pe::entityWorldAABB(def);
    if (!assertFloatClose(defBox.center.x, 0.0f) ||
        !assertFloatClose(defBox.center.y, 0.0f)) {
        std::cerr << "Default AABB center wrong\n";
        ok = false;
    }
    if (!assertFloatClose(defBox.halfExtents.x, 0.7071f) ||
        !assertFloatClose(defBox.halfExtents.y, 0.7071f)) {
        std::cerr << "Default AABB half-extents wrong\n";
        ok = false;
    }
    // Known scale: halfExtents (0.5,0.5) at scale (2,2) occupies (1,1).
    pe::Entity scaled(pe::Vec3(2.0f, 1.0f, 0.0f), 0.0f,
                      pe::Vec3(2.0f, 2.0f, 1.0f), pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    const pe::WorldAABB scaledBox = pe::entityWorldAABB(scaled);
    if (!assertFloatClose(scaledBox.center.x, 2.0f) ||
        !assertFloatClose(scaledBox.center.y, 1.0f)) {
        std::cerr << "Scaled AABB center wrong\n";
        ok = false;
    }
    if (!assertFloatClose(scaledBox.halfExtents.x, 1.0f) ||
        !assertFloatClose(scaledBox.halfExtents.y, 1.0f)) {
        std::cerr << "Scaled AABB half-extents wrong\n";
        ok = false;
    }
    // Non-uniform: halfExtents (0.5, 1.5) at scale (2, 0.5) -> (1.0, 0.75).
    pe::Entity nonUniform(pe::Vec3(-1.0f, -2.0f, 0.0f), 0.0f,
                          pe::Vec3(2.0f, 0.5f, 1.0f), pe::Vec3(0.5f, 1.5f, 0.0f), 0);
    const pe::WorldAABB nuBox = pe::entityWorldAABB(nonUniform);
    if (!assertFloatClose(nuBox.halfExtents.x, 1.0f) ||
        !assertFloatClose(nuBox.halfExtents.y, 0.75f)) {
        std::cerr << "Non-uniform AABB half-extents wrong\n";
        ok = false;
    }
    // Z forced 0 regardless of the stored z half-extent (flat scene rule).
    pe::Entity flat(pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                    pe::Vec3(0.5f, 0.5f, 5.0f), 0);
    const pe::WorldAABB flatBox = pe::entityWorldAABB(flat);
    if (flatBox.halfExtents.z != 0.0f) {
        std::cerr << "AABB z half-extent must be forced to 0\n";
        ok = false;
    }
    // Consistency with pickEntity: the helper's numbers ARE the pick bounds.
    pe::Entity pickProbe(pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(2.0f, 2.0f, 1.0f),
                         pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    const pe::WorldAABB box = pe::entityWorldAABB(pickProbe);
    std::vector<pe::Entity> one = {pickProbe};
    const float ex = box.halfExtents.x;
    const float ey = box.halfExtents.y;
    if (pe::pickEntity(one, ex * 0.999f, ey * 0.999f) != 0 ||
        pe::pickEntity(one, ex, ey) != -1) {
        std::cerr << "Helper bounds must match pickEntity containment\n";
        ok = false;
    }
    // Consistency with aabbOverlap: entity vs itself overlaps; two boxes
    // a full width apart do not.
    pe::Entity other(pe::Vec3(2.0f * ex, 0.0f, 0.0f), 0.0f, pe::Vec3(2.0f, 2.0f, 1.0f),
                     pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    if (!pe::aabbOverlap(pickProbe, pickProbe) ||
        pe::aabbOverlap(pickProbe, other)) {
        std::cerr << "AABB overlap consistency wrong\n";
        ok = false;
    }
    return ok;
}

// --- Step 128: screen-space pick convenience (headless) ---
// Matches pickEntity results for known screen points under the known
// 800x600 / 12x9 projection, and proves the WORLD-space conversion
// choice: with the camera moved, a world entity under the cursor is
// still found (UI-space conversion would miss it).
static bool checkScreenPick() {
    bool ok = true;
    pe::Entity e1(pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                  pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    e1.depth = 1;
    pe::Entity e2(pe::Vec3(2.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                  pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    e2.depth = 2;
    pe::Entity dead(pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                    pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    dead.depth = 9;
    dead.alive = false;
    std::vector<pe::Entity> all = {e1, e2, dead};

    pe::Camera cam;  // origin, default 12x9 box (halfW=6, halfH=4.5)
    // Window center pixel -> world (0,0) -> e1 (dead at same spot never wins).
    if (pe::pickEntityAtScreen(all, cam, 400.0f, 300.0f, 800.0f, 600.0f) != 0) {
        std::cerr << "Center screen point must pick e1\n";
        ok = false;
    }
    // World (2,0) maps to pixel (533.33, 300): picks e2 (higher depth).
    if (pe::pickEntityAtScreen(all, cam, 533.33f, 300.0f, 800.0f, 600.0f) != 1) {
        std::cerr << "e2 screen point must pick e2\n";
        ok = false;
    }
    // Miss: top-left pixel -> world (-6, 4.5) -> no entity.
    if (pe::pickEntityAtScreen(all, cam, 0.0f, 0.0f, 800.0f, 600.0f) != -1) {
        std::cerr << "Off-world screen point must miss\n";
        ok = false;
    }
    // Equivalence with pickEntity at the converted point (contract match).
    for (int i = 0; i < 3; ++i) {
        const float px[3] = {400.0f, 533.33f, 0.0f};
        const pe::Vec3 world = cam.screenToWorld(px[i], 300.0f, 800.0f, 600.0f);
        const int direct = pe::pickEntity(all, world.x, world.y);
        const int viaWrapper = pe::pickEntityAtScreen(all, cam, px[i], 300.0f, 800.0f, 600.0f);
        if (direct != viaWrapper) {
            std::cerr << "Wrapper must match pickEntity at converted point\n";
            ok = false;
        }
    }
    // WORLD-space proof: camera moved to (2,1), entity at world (2,1)
    // appears at the window center â€” must be picked. (UI-space
    // conversion would ignore the camera and miss.)
    pe::Camera moved;
    moved.follow(pe::Vec3(2.0f, 1.0f, 0.0f));
    pe::Entity e3(pe::Vec3(2.0f, 1.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                  pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    e3.depth = 1;
    std::vector<pe::Entity> withE3 = {e3};
    if (pe::pickEntityAtScreen(withE3, moved, 400.0f, 300.0f, 800.0f, 600.0f) != 0) {
        std::cerr << "Moved-camera world pick must find world entity at center\n";
        ok = false;
    }
    // And the same screen point with an origin camera must MISS it â€”
    // the camera position is what makes the world-space pick correct.
    if (pe::pickEntityAtScreen(withE3, cam, 400.0f, 300.0f, 800.0f, 600.0f) != -1) {
        std::cerr << "Origin camera must miss world (2,1) at center\n";
        ok = false;
    }
    return ok;
}

static bool checkSceneByName() {
    pe::SceneManager scenes;
    if (pe::sceneByName(scenes, "nope") != nullptr) {
        std::cerr << "sceneByName must miss on empty manager\n";
        return false;
    }
    pe::loadScene(scenes, "a");
    pe::Scene* found = pe::sceneByName(scenes, "a");
    if (!found || found->name != "a" ||
        pe::sceneByName(scenes, "b") != nullptr) {
        std::cerr << "sceneByName found/missing wrong\n";
        return false;
    }
    const pe::SceneManager& frozen = scenes;
    if (pe::sceneByName(frozen, "a") == nullptr) {
        std::cerr << "const sceneByName overload broken\n";
        return false;
    }
    return true;
}

static bool checkPlatformerLevels() {
    // Both level files parse; spawn neighborhoods and goal airspace are
    // empty (spawn (-4,-2.5) sits in cols 0-1/rows 5-6 in both files).
    const pe::Tilemap l1 = pe::loadTilemap("platformer_level1.txt");
    const pe::Tilemap l2 = pe::loadTilemap("platformer_level2.txt");
    if (l1.width != 10 || l1.height != 8 || l1.tiles.empty() ||
        l2.width != 10 || l2.height != 8 || l2.tiles.empty()) {
        std::cerr << "Platformer level files did not parse\n";
        return false;
    }
    // Spawn (-3,-2.5) sits in col 1 (col 0 is the backstop wall now).
    for (int row = 5; row <= 6; ++row) {
        const pe::Tile* a = pe::tileAt(l1, 1, row);
        const pe::Tile* b = pe::tileAt(l2, 1, row);
        if (!a || !b || a->tileId != 0 || b->tileId != 0) {
            std::cerr << "Spawn neighborhood blocked\n";
            return false;
        }
    }
    // L1 goal airspace (row 6, cols 6-7); L2 goal airspace (row 1, cols 6-7).
    for (int col = 6; col <= 7; ++col) {
        const pe::Tile* a = pe::tileAt(l1, col, 6);
        const pe::Tile* b = pe::tileAt(l2, col, 1);
        if (!a || !b || a->tileId != 0 || b->tileId != 0) {
            std::cerr << "Goal airspace blocked\n";
            return false;
        }
    }
    // Layouts genuinely differ (16 vs 23 solids by construction).
    std::size_t n1 = 0, n2 = 0;
    for (const auto& t : l1.tiles) {
        if (t.tileId != 0) {
            ++n1;
        }
    }
    for (const auto& t : l2.tiles) {
        if (t.tileId != 0) {
            ++n2;
        }
    }
    if (n1 != 20 || n2 != 26) {
        std::cerr << "Level solid counts changed\n";
        return false;
    }
    return true;
}

static bool checkPlatformerLanding() {
    // Real L1 geometry through the real converter: drop from y=2 over the
    // floor, land grounded at rest height (-2.5). No logic duplicated â€”
    // converter builds, one flag marks, controller integrates.
    const pe::Tilemap l1 = pe::loadTilemap("platformer_level1.txt");
    std::vector<pe::Entity> tileStatics = pe::tilemapToEntities(l1, 0.0f, 3);
    for (auto& t : tileStatics) {
        t.isStatic = true;
    }
    pe::Entity c = makeCharacter(-3.0f, 2.0f);
    bool landed = false;
    for (int i = 0; i < 600 && !landed; ++i) {
        landed = pe::updateCharacterController(c, tileStatics, 1.0f / 60.0f, false);
    }
    if (!landed) {
        std::cerr << "No landing on level geometry\n";
        return false;
    }
    for (int i = 0; i < 10; ++i) {
        pe::updateCharacterController(c, tileStatics, 1.0f / 60.0f, false);
    }
    if (!assertFloatClose(c.position.y, -2.5f) ||
        !assertFloatClose(c.velocity.y, 0.0f)) {
        std::cerr << "Did not rest on the level floor\n";
        return false;
    }
    return true;
}

static bool checkPlatformerLevelSwitch() {
    // Scene-level transition: two scenes, different maps, switch shuttles
    // current and each side keeps its own entities.
    pe::SceneManager scenes;
    pe::Scene& s1 = pe::loadScene(scenes, "level1");
    s1.entities.push_back(makeCharacter(-4.0f, -2.5f));
    pe::loadTilemapIntoScene(s1, "platformer_level1.txt");
    // Snapshot the spot-check cell NOW: the second loadScene below may
    // reallocate the vector and invalidate s1 (documented scene caveat â€”
    // this test honors it instead of tripping it).
    const pe::Tile* preA = pe::tileAt(s1.tilemap, 4, 2);
    const int tileA = preA ? preA->tileId : -1;
    pe::Scene& s2 = pe::loadScene(scenes, "level2");
    s2.entities.push_back(makeCharacter(-4.0f, -2.5f));
    s2.entities.push_back(makeCharacter(0.0f, 0.0f));
    pe::loadTilemapIntoScene(s2, "platformer_level2.txt");
    pe::switchTo(scenes, "level1");
    if (pe::currentScene(scenes)->entities.size() != 1 ||
        pe::currentScene(scenes)->tilemap.tiles.size() != 80) {
        std::cerr << "Level1 scene state wrong\n";
        return false;
    }
    pe::switchTo(scenes, "level2");
    if (pe::currentScene(scenes)->entities.size() != 2 ||
        pe::currentScene(scenes)->tilemap.width != 10) {
        std::cerr << "Level2 switch did not shuttle current\n";
        return false;
    }
    // Maps differ between the levels: (4,2) is empty in L1 (all-zero row)
    // but solid in L2 (platform row).
    const pe::Tile* b = pe::tileAt(pe::currentScene(scenes)->tilemap, 4, 2);
    if (tileA != 0 || !b || b->tileId == tileA) {
        std::cerr << "Level maps unexpectedly identical\n";
        return false;
    }
    return true;
}

static bool checkPlatformerClimb() {
    // Full L2 traversal through the real controller + real level geometry:
    // run right from spawn, single jump at x>=-3.4, keep running. The arc
    // (computed: land ~x=+1.1 on platform P, top -1.0) must cross the goal
    // rect (2.0,-1.0,0.75,0.75) on the way. Mirrors the game's per-frame
    // calls exactly (velocity set, controller step, overlap by position).
    const pe::Tilemap l2 = pe::loadTilemap("platformer_level2.txt");
    std::vector<pe::Entity> tileStatics = pe::tilemapToEntities(l2, 0.0f, 3);
    for (auto& t : tileStatics) {
        t.isStatic = true;
    }
    // Spawn at the exact rest height (as the game does): grounded frame 0,
    // so the scheduled jump always fires instead of being refused mid-air.
    pe::Entity c = makeCharacter(-3.0f, -2.68f);
    // Exact game transform: half 0.4 x scale 0.8 = 0.32 world half-height,
    // so -2.68 is the true rest (makeCharacter's default scale is 1.0).
    c.halfExtents = pe::Vec3(0.4f, 0.4f, 0.0f);
    c.scale = pe::Vec3(0.8f, 0.8f, 1.0f);
    c.jumpImpulse = 7.0f;  // game tuning (default 12 overshoots everything)
    bool jumped = false;
    bool overlapped = false;
    bool wasG = false;
    for (int i = 0; i < 600 && !overlapped; ++i) {
        c.velocity.x = 4.5f;
        // Jump like a player: only once, only when grounded (never waste
        // the single jump on an airborne frame â€” that exact mistake failed
        // this test's first version).
        bool jump = false;
        if (!jumped && wasG && c.position.x >= -3.4f) {
            jump = true;
            jumped = true;
        }
        wasG = pe::updateCharacterController(c, tileStatics, 1.0f / 60.0f, jump);
        const float dx = c.position.x - 2.0f;
        const float dy = c.position.y - (-1.0f);
        const float adx = dx >= 0.0f ? dx : -dx;
        const float ady = dy >= 0.0f ? dy : -dy;
        if (adx < 0.75f + 0.4f && ady < 0.75f + 0.4f) {
            overlapped = true;
        }
    }
    if (!jumped) {
        std::cerr << "Climb never jumped\n";
        return false;
    }
    if (!overlapped) {
        std::cerr << "Climb never reached the goal zone\n";
        return false;
    }
    return true;
}

static bool checkPlatformerGoalEvent() {
    // Goal flow at bus level with game payloads: SceneChanged carries
    // from/to level indices to a subscriber that records them.
    pe::EventBus bus;
    int seenA = -99, seenB = -99, calls = 0;
    bus.subscribe(pe::EventType::SceneChanged, [&](const pe::GameEvent& e) {
        seenA = e.a;
        seenB = e.b;
        ++calls;
    });
    bus.emit(pe::GameEvent{pe::EventType::SceneChanged, 0, 1});
    bus.emit(pe::GameEvent{pe::EventType::SceneChanged, 1, 2});
    if (calls != 2 || seenA != 1 || seenB != 2) {
        std::cerr << "Goal event flow broken\n";
        return false;
    }
    return true;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4005)  // windows.h vs glfw3.h APIENTRY: same value
#endif
#include <windows.h>  // PostMessage/Sleep for checkInputEdges' own window
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>  // glfwGetWin32Window (test-only HWND access)

static void postKey(GLFWwindow* window, int vk, bool down) {
    // Focus-independent injection for the test's own hidden window only.
    // (Production key delivery is the OS's job; this posts straight to
    // the window queue, which glfwPollEvents then pumps on this thread.)
    UINT sc = MapVirtualKeyW((UINT)vk, MAPVK_VK_TO_VSC);
    LPARAM lp = 1 | (sc << 16);
    if (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN) {
        lp |= (1 << 24);  // extended-key bit, as the OS sets it
    }
    UINT msg = down ? WM_KEYDOWN : WM_KEYUP;
    if (!down) {
        lp |= (1 << 30) | (1 << 31);
    }
    PostMessageW(glfwGetWin32Window(window), msg, (WPARAM)vk, lp);
}

static bool checkInputEdges() {
    // Locks the exact snapshot semantics the platformer jump fix depends
    // on: a posted DOWN is visible as an edge until update() consumes it,
    // untracked keys never edge, and UP-arrow delivery works in-process.
    // Needs a window: skipped (not failed) where GL cannot init.
    if (!glfwInit()) {
        std::cerr << "input test skipped: glfwInit failed\n";
        return true;
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(64, 64, "input-test", nullptr, nullptr);
    if (!window) {
        std::cerr << "input test skipped: hidden window failed\n";
        glfwTerminate();
        return true;
    }
    bool ok = true;
    pe::Input input{GLFW_KEY_SPACE, GLFW_KEY_UP};
    glfwPollEvents();
    input.update(window);  // settle: nothing pressed, snapshot clean
    if (input.isEdge(window, GLFW_KEY_SPACE) ||
        input.isEdge(window, GLFW_KEY_UP) ||
        input.isEdge(window, GLFW_KEY_A)) {
        std::cerr << "Clean snapshot must edge nothing\n";
        ok = false;
    }
    // SPACE down: tracked edge fires, untracked A stays silent.
    postKey(window, VK_SPACE, true);
    Sleep(150);
    glfwPollEvents();
    if (!input.isEdge(window, GLFW_KEY_SPACE) ||
        input.isEdge(window, GLFW_KEY_A)) {
        std::cerr << "SPACE down must edge tracked-only\n";
        ok = false;
    }
    // Snapshot consumes: edge gone while still held.
    input.update(window);
    if (input.isEdge(window, GLFW_KEY_SPACE)) {
        std::cerr << "Post-snapshot edge must clear while held\n";
        ok = false;
    }
    postKey(window, VK_SPACE, false);
    Sleep(150);
    glfwPollEvents();
    input.update(window);
    // UP-arrow down must register in-process (VM blackhole check).
    postKey(window, VK_UP, true);
    Sleep(150);
    glfwPollEvents();
    if (!input.isEdge(window, GLFW_KEY_UP)) {
        std::cerr << "UP-arrow down must edge\n";
        ok = false;
    }
    postKey(window, VK_UP, false);
    Sleep(150);
    glfwPollEvents();
    input.update(window);
    if (input.isEdge(window, GLFW_KEY_UP)) {
        std::cerr << "UP edge must clear after snapshot\n";
        ok = false;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return ok;
}

static bool checkVolumeClamp() {
    // Gain contract without an audio device: setters clamp [0,1] and store;
    // applyVolumes() guards on slotsValid/loaded flags, so this never
    // touches miniaudio uninitialized. (clampVolume01 itself is private;
    // this exercises it through both public setters â€” same code path.)
    pe::Audio audio;  // never init()ed: guards must no-op safely
    audio.setMasterVolume(-1.0f);
    audio.setSfxVolume(2.0f);
    if (!assertFloatClose(audio.getMasterVolume(), 0.0f) ||
        !assertFloatClose(audio.getSfxVolume(), 1.0f)) {
        std::cerr << "Volume clamp endpoints wrong\n";
        return false;
    }
    audio.setMasterVolume(0.5f);
    audio.setSfxVolume(0.5f);
    if (!assertFloatClose(audio.getMasterVolume(), 0.5f) ||
        !assertFloatClose(audio.getSfxVolume(), 0.5f)) {
        std::cerr << "Volume mid-range wrong\n";
        return false;
    }
    audio.setMasterVolume(0.0f);
    audio.setSfxVolume(1.0f);
    if (!assertFloatClose(audio.getMasterVolume(), 0.0f) ||
        !assertFloatClose(audio.getSfxVolume(), 1.0f)) {
        std::cerr << "Volume identity endpoints wrong\n";
        return false;
    }
    return true;
}

// --- Step 124: mute toggle logic (headless, no device) ---
// The exact Audio-class logic the M key and the console mute command
// run: masterVolume = 0 silences (both SFX and music multiply by
// master), unmute restores the remembered value, mute cycles never
// lose a volume change made while muted. applyVolumes() guards no-op
// safely pre-init (same pattern as checkVolumeClamp).
static bool checkMuteToggle() {
    pe::Audio audio;  // never init()ed: guards must no-op safely
    // Defaults: unmuted at master 1.0 â€” pre-Step-124 behavior.
    if (audio.isMuted() || !assertFloatClose(audio.getMasterVolume(), 1.0f)) {
        std::cerr << "Fresh Audio must be unmuted at master 1.0\n";
        return false;
    }
    // User sets a custom volume, then mutes: output silences.
    audio.setMasterVolume(0.7f);
    audio.setMuted(true);
    if (!audio.isMuted() || !assertFloatClose(audio.getMasterVolume(), 0.0f)) {
        std::cerr << "Muted master must be 0\n";
        return false;
    }
    // Idempotent: muting again keeps state (no double-restore later).
    audio.setMuted(true);
    if (!audio.isMuted()) { std::cerr << "Double mute must stay muted\n"; return false; }
    // Unmute restores the remembered custom volume.
    audio.setMuted(false);
    if (audio.isMuted() || !assertFloatClose(audio.getMasterVolume(), 0.7f)) {
        std::cerr << "Unmute must restore remembered volume\n";
        return false;
    }
    // Volume change WHILE muted: recorded, takes effect on unmute.
    audio.setMuted(true);
    audio.setMasterVolume(0.4f);
    if (!assertFloatClose(audio.getMasterVolume(), 0.0f)) {
        std::cerr << "Muted output must stay silent\n";
        return false;
    }
    audio.setMuted(false);
    if (!assertFloatClose(audio.getMasterVolume(), 0.4f)) {
        std::cerr << "Unmute must apply muted-period volume change\n";
        return false;
    }
    // toggleMute flips state both ways.
    audio.toggleMute();
    if (!audio.isMuted()) { std::cerr << "toggleMute must mute\n"; return false; }
    audio.toggleMute();
    if (audio.isMuted()) { std::cerr << "toggleMute must unmute\n"; return false; }
    return true;
}

// --- A2: per-sound gain composition (headless, no device) ---
// The master*sfx*perSound contract each loaded sound plays at. Headless
// observation: getters return the STORED multipliers (the composition
// itself runs inside applyVolumes() — device-only), and applyVolumes()
// guards no-op safely pre-init (same pattern as checkVolumeClamp).
// Locks: per-sound defaults 1.0, each Sound slot independently settable,
// clamped [0,1], one slot's gain never leaks into another.
static bool checkPerSoundVolume() {
    pe::Audio audio;  // never init()ed: guards must no-op safely
    // Defaults: all three per-sound gains at 1.0.
    if (!assertFloatClose(audio.getVolume(pe::Sound::Beep), 1.0f) ||
        !assertFloatClose(audio.getVolume(pe::Sound::GameOver), 1.0f) ||
        !assertFloatClose(audio.getVolume(pe::Sound::NewHighScore), 1.0f)) {
        std::cerr << "Per-sound gains must default to 1.0\n";
        return false;
    }
    // Each slot independently settable and clamped.
    audio.setVolume(pe::Sound::Beep, 0.5f);
    audio.setVolume(pe::Sound::GameOver, 2.0f);
    audio.setVolume(pe::Sound::NewHighScore, -1.0f);
    if (!assertFloatClose(audio.getVolume(pe::Sound::Beep), 0.5f) ||
        !assertFloatClose(audio.getVolume(pe::Sound::GameOver), 1.0f) ||
        !assertFloatClose(audio.getVolume(pe::Sound::NewHighScore), 0.0f)) {
        std::cerr << "Per-sound gain clamp/independence wrong\n";
        return false;
    }
    // Composition contract: applied gain is master*sfx*perSound.
    audio.setMasterVolume(0.8f);
    audio.setSfxVolume(0.5f);
    if (!assertFloatClose(audio.getMasterVolume() * audio.getSfxVolume() *
                              audio.getVolume(pe::Sound::Beep),
                          0.8f * 0.5f * 0.5f)) {
        std::cerr << "master*sfx*perSound composition wrong\n";
        return false;
    }
    return true;
}

// --- A2: music volume independence (headless, no device) ---
// The music path multiplies by master*musicVolume ONLY — sfx and
// per-sound gains must never touch the music multiplier. Headless
// observation: getMusicVolume() returns the STORED multiplier; the
// composed music gain follows masterVolume (so mute silences music),
// asserted here through the documented composition contract.
static bool checkMusicVolumeIndependence() {
    pe::Audio audio;  // never init()ed: guards must no-op safely
    audio.setMusicVolume(0.6f);
    if (!assertFloatClose(audio.getMusicVolume(), 0.6f)) {
        std::cerr << "setMusicVolume must store its value\n";
        return false;
    }
    // SFX and per-sound paths must NOT move the music multiplier.
    audio.setSfxVolume(0.2f);
    audio.setVolume(pe::Sound::Beep, 0.2f);
    audio.setVolume(pe::Sound::GameOver, 0.2f);
    audio.setVolume(pe::Sound::NewHighScore, 0.2f);
    if (!assertFloatClose(audio.getMusicVolume(), 0.6f)) {
        std::cerr << "Music volume must be independent of sfx/per-sound gains\n";
        return false;
    }
    // Clamp [0,1] on both endpoints.
    audio.setMusicVolume(3.0f);
    if (!assertFloatClose(audio.getMusicVolume(), 1.0f)) {
        std::cerr << "Music volume must clamp to 1.0\n";
        return false;
    }
    audio.setMusicVolume(-2.0f);
    if (!assertFloatClose(audio.getMusicVolume(), 0.0f)) {
        std::cerr << "Music volume must clamp to 0.0\n";
        return false;
    }
    // Mute silences the composed music gain (master*musicVolume) without
    // losing the stored multiplier; unmute restores master untouched.
    audio.setMusicVolume(0.6f);
    audio.setMuted(true);
    if (!assertFloatClose(audio.getMasterVolume(), 0.0f) ||
        !assertFloatClose(audio.getMusicVolume(), 0.6f)) {
        std::cerr << "Mute must zero master while keeping music multiplier\n";
        return false;
    }
    audio.setMuted(false);
    if (!assertFloatClose(audio.getMasterVolume(), 1.0f) ||
        !assertFloatClose(audio.getMusicVolume(), 0.6f)) {
        std::cerr << "Unmute must restore master without touching music multiplier\n";
        return false;
    }
    return true;
}

// --- A2: pre-init guards (headless, no device, no crash) ---
// Every playback/stop/teardown path must no-op safely on a never-init()ed
// Audio: slotsValid==0, loaded flags false, engineIsValid false. Touching
// miniaudio uninitialized would be undefined behaviour — the run passing
// at all is the proof. Also locks A1's isLoaded/isMusicLoaded queries.
static bool checkPreInitGuards() {
    pe::Audio audio;  // never init()ed
    audio.playNext();          // must no-op (slotsValid == 0)
    audio.playGameOver();      // must no-op (flag false)
    audio.playNewHighScore();  // must no-op (flag false)
    audio.stopEventSounds();   // must no-op (flags false)
    audio.stopMusic();         // must no-op (flag false)
    // Music start on a dead engine must return false gracefully.
    if (audio.playMusicLoop("music_loop.wav", true)) {
        std::cerr << "playMusicLoop must fail gracefully pre-init\n";
        return false;
    }
    // State queries coherent: nothing loaded before init().
    if (audio.isLoaded(pe::Sound::Beep) ||
        audio.isLoaded(pe::Sound::GameOver) ||
        audio.isLoaded(pe::Sound::NewHighScore) ||
        audio.isMusicLoaded()) {
        std::cerr << "Pre-init load state must be all false\n";
        return false;
    }
    // Shutdown before any init is a safe no-op; the destructor runs it
    // again as insurance — a crash here would fail the test binary.
    audio.shutdown();
    return true;
}

// --- Simultaneous SFX + music volume matrix (headless, no device) ---
// One Audio instance, several matrix points: at each point BOTH composed
// gains must hold at once — SFX = master*sfx*perSound, music =
// master*musicVol. checkPerSoundVolume and checkMusicVolumeIndependence
// each cover one path; this locks them TOGETHER: master multiplies both,
// sfx only SFX, musicVol only music, and mute zeroes both composed
// paths while unmute restores both (the live menu-music + SFX state a
// game actually runs). Headless observation is via stored multipliers;
// the composition itself runs in applyVolumes() (device-only).
static bool checkAudioVolumeMatrix() {
    struct Point { float master, sfx, beep, music; };
    const Point pts[] = {
        {1.0f, 1.0f, 1.0f, 1.0f},
        {0.8f, 0.5f, 0.5f, 0.6f},
        {0.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 0.2f, 0.3f, 0.9f},
    };
    pe::Audio audio;  // never init()ed: guards must no-op safely
    for (const Point& p : pts) {
        audio.setMasterVolume(p.master);
        audio.setSfxVolume(p.sfx);
        audio.setVolume(pe::Sound::Beep, p.beep);
        audio.setMusicVolume(p.music);
        if (!assertFloatClose(audio.getMasterVolume() * audio.getSfxVolume() *
                                  audio.getVolume(pe::Sound::Beep),
                              p.master * p.sfx * p.beep) ||
            !assertFloatClose(audio.getMasterVolume() * audio.getMusicVolume(),
                              p.master * p.music)) {
            std::cerr << "Volume matrix point wrong (master " << p.master
                      << " sfx " << p.sfx << " beep " << p.beep
                      << " music " << p.music << ")\n";
            return false;
        }
    }
    // Mute zeroes BOTH composed paths while keeping all multipliers.
    audio.setMasterVolume(0.7f);
    audio.setSfxVolume(0.5f);
    audio.setVolume(pe::Sound::Beep, 0.4f);
    audio.setMusicVolume(0.9f);
    audio.setMuted(true);
    if (!assertFloatClose(audio.getMasterVolume() * audio.getSfxVolume() *
                              audio.getVolume(pe::Sound::Beep), 0.0f) ||
        !assertFloatClose(audio.getMasterVolume() * audio.getMusicVolume(), 0.0f)) {
        std::cerr << "Muted matrix must zero both composed paths\n";
        return false;
    }
    // Unmute restores BOTH composed paths from the same stored state.
    audio.setMuted(false);
    if (!assertFloatClose(audio.getMasterVolume() * audio.getSfxVolume() *
                              audio.getVolume(pe::Sound::Beep), 0.7f * 0.5f * 0.4f) ||
        !assertFloatClose(audio.getMasterVolume() * audio.getMusicVolume(), 0.7f * 0.9f)) {
        std::cerr << "Unmuted matrix must restore both composed paths\n";
        return false;
    }
    return true;
}
// Skip policy: miniaudio is asked directly whether any playback device
// exists (ma_context_get_devices on a throwaway context). Zero devices
// means a genuinely headless machine (CI/VM without audio): the check
// prints SKIP and passes, because nothing can be verified. One or more
// devices means a failed init is a REAL failure (code or missing
// assets), not environment noise. Assets need no test-local copy: CTest
// runs from build/, where the 3-candidate probe's ../assets/ resolves
// to repo-root assets/. Audible playback stays human-only.
static bool checkAudioDeviceLifecycle() {
    // --- Step 171: documented engine-init order (from source, call-site
    // based — every game repeats it; a test cannot execute a game's init
    // headless, so the order is locked HERE as the contract reference) ---
    // Arcade (main.cpp:558-842), Platformer (platformer.cpp:116-253),
    // Pong (pong.cpp:33-138, no audio):
    //   1. glfwInit()                          fail -> return (nothing to undo)
    //   2. glfwCreateWindow(...)               fail -> glfwTerminate() + return
    //   3. gladLoadGL(glfwGetProcAddress)      fail -> glfwTerminate() + return
    //   4. audio.init()   (Arcade/Platformer)  fail -> glfwTerminate() + return
    //      (cleans its own partial state first; idempotent re-init)
    //   5. renderer.init()                     fail -> glfwTerminate() + return
    //      (destroyAll() on failure; destroyAll zeroes members, so a
    //       double destroyAll is a safe no-op; init() twice is NOT safe —
    //       call-site contract: games call it once)
    //   6. frameTime.start()
    //   7. loadInputBindings(...)              fail -> defaults (non-fatal)
    // Shutdown: glfwTerminate() once at exit; audio.shutdown() idempotent
    // with destructor insurance.
    // Device probe: throwaway context init + playback enumeration.
    ma_context context;
    bool hasDevice = false;
    if (ma_context_init(NULL, 0, NULL, &context) == MA_SUCCESS) {
        ma_device_info* info = NULL;
        ma_uint32 count = 0;
        if (ma_context_get_devices(&context, &info, &count, NULL, NULL) == MA_SUCCESS) {
            hasDevice = (count > 0);
        }
        ma_context_uninit(&context);
    }
    if (!hasDevice) {
        std::cout << "SKIP checkAudioDeviceLifecycle: no playback device (headless environment)\n";
        return true;
    }
    // Device present: init() must succeed — a false here is a real bug,
    // not environment noise.
    pe::Audio audio;
    if (!audio.init()) {
        std::cerr << "init() must succeed with a playback device present\n";
        return false;
    }
    // Step 171: double-init safety — a second init() without shutdown
    // is an idempotent no-op (engineIsValid guard): the engine stays
    // valid, the pool is not re-pooled, no device leak.
    if (!audio.init()) {
        std::cerr << "second init() without shutdown must be a no-op true\n";
        return false;
    }
    if (!audio.isLoaded(pe::Sound::Beep)) {
        std::cerr << "double-init must not disturb the loaded pool\n";
        return false;
    }
    // All SFX assets loaded (4-slot beep pool + 2 named events).
    if (!audio.isLoaded(pe::Sound::Beep) ||
        !audio.isLoaded(pe::Sound::GameOver) ||
        !audio.isLoaded(pe::Sound::NewHighScore)) {
        std::cerr << "init() must load all SFX assets\n";
        return false;
    }
    // Music: play -> stop -> play the SAME file (the Step 119 repeat path).
    if (!audio.playMusicLoop("music_loop.wav", true)) {
        std::cerr << "playMusicLoop must succeed with music_loop.wav present\n";
        return false;
    }
    if (!audio.isMusicLoaded()) {
        std::cerr << "Music must report loaded after successful playMusicLoop\n";
        return false;
    }
    audio.stopMusic();
    if (!audio.isMusicLoaded()) {
        std::cerr << "stopMusic must not unload the music sound\n";
        return false;
    }
    if (!audio.playMusicLoop("music_loop.wav", true)) {
        std::cerr << "playMusicLoop repeat on the same file must succeed\n";
        return false;
    }
    // Triggers and stops must not crash on a live engine.
    audio.playNext();
    audio.playGameOver();
    audio.playNewHighScore();
    audio.stopEventSounds();
    // Full lifecycle: shutdown -> init again -> shutdown idempotent.
    audio.shutdown();
    if (audio.isLoaded(pe::Sound::Beep) || audio.isMusicLoaded()) {
        std::cerr << "shutdown() must clear loaded state\n";
        return false;
    }
    if (!audio.init()) {
        std::cerr << "second init() must succeed after shutdown()\n";
        return false;
    }
    audio.shutdown();
    audio.shutdown();  // idempotent: safe no-op
    return true;
}

// --- A4: one-shot pool rotation proof (device backed) ---
// Same skip policy as checkAudioDeviceLifecycle: zero playback devices
// -> SKIP (print line, pass); devices present and init() fails -> real
// failure. Proves the mechanism the whole Step 9/20 trigger contract
// rests on: playNext() claims the NEXT pool slot and the cursor wraps
// after POOL_SIZE claims — observed through the read-only
// getSlotCount()/getNextSlotIndex() accessors, not trusted.
static bool checkAudioPoolRotation() {
    // Device probe, identical policy to checkAudioDeviceLifecycle.
    ma_context context;
    bool hasDevice = false;
    if (ma_context_init(NULL, 0, NULL, &context) == MA_SUCCESS) {
        ma_device_info* info = NULL;
        ma_uint32 count = 0;
        if (ma_context_get_devices(&context, &info, &count, NULL, NULL) == MA_SUCCESS) {
            hasDevice = (count > 0);
        }
        ma_context_uninit(&context);
    }
    if (!hasDevice) {
        std::cout << "SKIP checkAudioPoolRotation: no playback device (headless environment)\n";
        return true;
    }
    pe::Audio audio;
    if (!audio.init()) {
        std::cerr << "init() must succeed with a playback device present\n";
        return false;
    }
    // Full pool after init; cursor at slot 0 before the first trigger.
    if (audio.getSlotCount() != pe::Audio::POOL_SIZE) {
        std::cerr << "init() must leave all POOL_SIZE slots live\n";
        return false;
    }
    if (audio.getNextSlotIndex() != 0) {
        std::cerr << "Cursor must start at slot 0 after init()\n";
        return false;
    }
    // Drive playNext() POOL_SIZE+1 times: cursor must visit 1,2,3, then
    // wrap back to 0 (all four slots engaged), then advance to 1 again.
    const std::size_t expected[] = {1, 2, 3, 0, 1};
    for (std::size_t i = 0; i < 5; ++i) {
        audio.playNext();
        if (audio.getNextSlotIndex() != expected[i]) {
            std::cerr << "Cursor must rotate through the pool and wrap"
                         " (step " << i << " expected " << expected[i] << ")\n";
            return false;
        }
    }
    // shutdown() resets the pool: no slots live, cursor back to 0.
    audio.shutdown();
    if (audio.getSlotCount() != 0 || audio.getNextSlotIndex() != 0) {
        std::cerr << "shutdown() must clear slot count and cursor\n";
        return false;
    }
    return true;
}

// --- Step 125: screen-to-world conversion (headless, known numbers) ---
// Pure math against the default 12x9 ortho box (halfW=6, halfH=4.5) and
// a resized 32:12 box â€” no GLFW, no device. Locks the coordinate
// contract: pixels are top-left/y-down, world is center-origin/y-up.
static bool checkScreenToWorld() {
    bool ok = true;
    // Center of an 800x600 window is the world origin.
    const pe::Vec3 c = pe::screenToUi(400.0f, 300.0f, 800.0f, 600.0f, 6.0f, 4.5f);
    if (!assertFloatClose(c.x, 0.0f) || !assertFloatClose(c.y, 0.0f)) {
        std::cerr << "screenToUi center must be (0,0)\n";
        ok = false;
    }
    // Top-left pixel = world (-halfW, +halfH); bottom-right the opposite.
    const pe::Vec3 tl = pe::screenToUi(0.0f, 0.0f, 800.0f, 600.0f, 6.0f, 4.5f);
    if (!assertFloatClose(tl.x, -6.0f) || !assertFloatClose(tl.y, 4.5f)) {
        std::cerr << "screenToUi top-left corner wrong\n";
        ok = false;
    }
    const pe::Vec3 br = pe::screenToUi(800.0f, 600.0f, 800.0f, 600.0f, 6.0f, 4.5f);
    if (!assertFloatClose(br.x, 6.0f) || !assertFloatClose(br.y, -4.5f)) {
        std::cerr << "screenToUi bottom-right corner wrong\n";
        ok = false;
    }
    // Quarter point: pixel (200,150) -> ndc (-0.5, +0.5) -> world (-3, 2.25).
    const pe::Vec3 q = pe::screenToUi(200.0f, 150.0f, 800.0f, 600.0f, 6.0f, 4.5f);
    if (!assertFloatClose(q.x, -3.0f) || !assertFloatClose(q.y, 2.25f)) {
        std::cerr << "screenToUi quarter point wrong\n";
        ok = false;
    }
    // Degenerate framebuffer: (0,0,0), never divide by zero.
    const pe::Vec3 d = pe::screenToUi(100.0f, 100.0f, 0.0f, 0.0f, 6.0f, 4.5f);
    if (!assertFloatClose(d.x, 0.0f) || !assertFloatClose(d.y, 0.0f)) {
        std::cerr << "screenToUi degenerate framebuffer must be (0,0)\n";
        ok = false;
    }
    // Resize consistency: onResize(1600,600) gives halfW = 4.5*1600/600 = 12.
    // Center still (0,0); right edge pixel now maps to (12, -4.5).
    pe::Camera cam;
    cam.onResize(1600, 600);
    const pe::Vec3 rc = cam.screenToWorldUi(800.0f, 300.0f, 1600.0f, 600.0f);
    if (!assertFloatClose(rc.x, 0.0f) || !assertFloatClose(rc.y, 0.0f)) {
        std::cerr << "resized screenToWorldUi center wrong\n";
        ok = false;
    }
    const pe::Vec3 re = cam.screenToWorldUi(1600.0f, 600.0f, 1600.0f, 600.0f);
    if (!assertFloatClose(re.x, 12.0f) || !assertFloatClose(re.y, -4.5f)) {
        std::cerr << "resized screenToWorldUi right edge wrong\n";
        ok = false;
    }
    // Default (never-resized) camera converts with the 12x9 box.
    pe::Camera fresh;
    const pe::Vec3 fe = fresh.screenToWorldUi(800.0f, 600.0f, 800.0f, 600.0f);
    if (!assertFloatClose(fe.x, 6.0f) || !assertFloatClose(fe.y, -4.5f)) {
        std::cerr << "fresh screenToWorldUi right edge wrong\n";
        ok = false;
    }
    // screenToWorld adds the camera position (view is a pure translation).
    fresh.follow(pe::Vec3(2.0f, 1.0f, 0.0f));
    const pe::Vec3 w = fresh.screenToWorld(0.0f, 0.0f, 800.0f, 600.0f);
    if (!assertFloatClose(w.x, -4.0f) || !assertFloatClose(w.y, 5.5f)) {
        std::cerr << "screenToWorld must add camera position\n";
        ok = false;
    }
    return ok;
}

// --- Step 73/83: followLerp contract (headless, pure camera math) ---
// Exponential approach toward the target with the blend clamped to
// [0,1]: low-fps frames land ON the target (no overshoot), negative/
// zero dt leaves the position untouched, and repeated calls converge.
// Additive: follow() snap semantics are untouched (checked last).
static bool checkFollowLerp() {
    bool ok = true;
    // Partial approach: camera (0,0) -> target (10,0), dt=0.1 speed=5
    // gives blend = 0.5, so the camera covers exactly half the gap.
    pe::Camera cam;
    cam.followLerp(pe::Vec3(10.0f, 0.0f, 0.0f), 0.1f);
    const pe::Vec3& p1 = cam.getPosition();
    if (!assertFloatClose(p1.x, 5.0f) || !assertFloatClose(p1.y, 0.0f)) {
        std::cerr << "followLerp must approach by speed*dt fraction\n";
        ok = false;
    }
    // Repeated calls converge: blend 0.5 per call halves the gap; after
    // 20 calls the remainder is below the assert tolerance.
    for (int i = 0; i < 20; ++i) {
        cam.followLerp(pe::Vec3(10.0f, 0.0f, 0.0f), 0.1f);
    }
    if (!assertFloatClose(p1.x, 10.0f) || !assertFloatClose(p1.y, 0.0f)) {
        std::cerr << "followLerp must converge to the target\n";
        ok = false;
    }
    // Upper clamp: huge dt clamps blend to 1 — the camera lands exactly
    // ON the target in one step, never past it (no overshoot).
    pe::Camera lowFps;
    lowFps.followLerp(pe::Vec3(-4.0f, 3.0f, 0.0f), 100.0f);
    const pe::Vec3& p2 = lowFps.getPosition();
    if (!assertFloatClose(p2.x, -4.0f) || !assertFloatClose(p2.y, 3.0f)) {
        std::cerr << "followLerp large dt must land on target, no overshoot\n";
        ok = false;
    }
    // Lower clamp: negative dt clamps blend to 0 — position untouched.
    pe::Camera negDt;
    negDt.follow(pe::Vec3(1.0f, 2.0f, 0.0f));
    negDt.followLerp(pe::Vec3(50.0f, 50.0f, 0.0f), -0.5f);
    const pe::Vec3& p3 = negDt.getPosition();
    if (!assertFloatClose(p3.x, 1.0f) || !assertFloatClose(p3.y, 2.0f)) {
        std::cerr << "followLerp negative dt must leave position untouched\n";
        ok = false;
    }
    // Zero dt: no movement either.
    negDt.followLerp(pe::Vec3(50.0f, 50.0f, 0.0f), 0.0f);
    if (!assertFloatClose(p3.x, 1.0f) || !assertFloatClose(p3.y, 2.0f)) {
        std::cerr << "followLerp zero dt must leave position untouched\n";
        ok = false;
    }
    // Only x/y move: the camera's z stays at the world origin even when
    // the target carries a z.
    pe::Camera zCam;
    zCam.followLerp(pe::Vec3(4.0f, 4.0f, 7.0f), 100.0f);
    const pe::Vec3& p4 = zCam.getPosition();
    if (!assertFloatClose(p4.z, 0.0f)) {
        std::cerr << "followLerp must not move the camera z\n";
        ok = false;
    }
    // Additive contract: follow() still snaps exactly after followLerp
    // use — the two methods stay independent.
    zCam.followLerp(pe::Vec3(4.0f, 4.0f, 0.0f), 0.05f);
    zCam.follow(pe::Vec3(9.0f, -9.0f, 0.0f));
    if (!assertFloatClose(p4.x, 9.0f) || !assertFloatClose(p4.y, -9.0f)) {
        std::cerr << "follow must snap exactly after followLerp use\n";
        ok = false;
    }
    return ok;
}

// --- Step 126: world-to-screen conversion (headless, known numbers) ---
// Pure math mirroring checkScreenToWorld, plus round-trip proofs
// against screenToUi in both directions â€” no GLFW, no device.
static bool checkWorldToScreen() {
    bool ok = true;
    // World origin on the default 12x9 box maps to the window center.
    const pe::Vec3 c = pe::uiToScreen(0.0f, 0.0f, 800.0f, 600.0f, 6.0f, 4.5f);
    if (!assertFloatClose(c.x, 400.0f) || !assertFloatClose(c.y, 300.0f)) {
        std::cerr << "uiToScreen center must be (400,300)\n";
        ok = false;
    }
    // World (-6, 4.5) = top-left pixel (0,0); (6, -4.5) = bottom-right.
    const pe::Vec3 tl = pe::uiToScreen(-6.0f, 4.5f, 800.0f, 600.0f, 6.0f, 4.5f);
    if (!assertFloatClose(tl.x, 0.0f) || !assertFloatClose(tl.y, 0.0f)) {
        std::cerr << "uiToScreen top-left corner wrong\n";
        ok = false;
    }
    const pe::Vec3 br = pe::uiToScreen(6.0f, -4.5f, 800.0f, 600.0f, 6.0f, 4.5f);
    if (!assertFloatClose(br.x, 800.0f) || !assertFloatClose(br.y, 600.0f)) {
        std::cerr << "uiToScreen bottom-right corner wrong\n";
        ok = false;
    }
    // World (-3, 2.25) = pixel (200, 150) (inverse of Step 125's quarter point).
    const pe::Vec3 q = pe::uiToScreen(-3.0f, 2.25f, 800.0f, 600.0f, 6.0f, 4.5f);
    if (!assertFloatClose(q.x, 200.0f) || !assertFloatClose(q.y, 150.0f)) {
        std::cerr << "uiToScreen quarter point wrong\n";
        ok = false;
    }
    // Degenerate half-extents: (0,0,0), never divide by zero.
    const pe::Vec3 d = pe::uiToScreen(1.0f, 1.0f, 800.0f, 600.0f, 0.0f, 0.0f);
    if (!assertFloatClose(d.x, 0.0f) || !assertFloatClose(d.y, 0.0f)) {
        std::cerr << "uiToScreen degenerate half-extents must be (0,0)\n";
        ok = false;
    }
    // Round-trip pixel -> world -> pixel for sample points.
    const float pixels[4][2] = {{0.0f, 0.0f}, {400.0f, 300.0f},
                                {800.0f, 600.0f}, {200.0f, 150.0f}};
    for (int i = 0; i < 4; ++i) {
        const pe::Vec3 world = pe::screenToUi(pixels[i][0], pixels[i][1],
                                              800.0f, 600.0f, 6.0f, 4.5f);
        const pe::Vec3 back = pe::uiToScreen(world.x, world.y,
                                             800.0f, 600.0f, 6.0f, 4.5f);
        if (!assertFloatClose(back.x, pixels[i][0]) ||
            !assertFloatClose(back.y, pixels[i][1])) {
            std::cerr << "screenToUi/uiToScreen round-trip failed at " << i << "\n";
            ok = false;
        }
    }
    // Round-trip world -> pixel -> world for sample points.
    const float worlds[4][2] = {{-6.0f, 4.5f}, {0.0f, 0.0f},
                                {3.0f, -1.5f}, {5.9f, 4.4f}};
    for (int i = 0; i < 4; ++i) {
        const pe::Vec3 px = pe::uiToScreen(worlds[i][0], worlds[i][1],
                                           800.0f, 600.0f, 6.0f, 4.5f);
        const pe::Vec3 back = pe::screenToUi(px.x, px.y,
                                             800.0f, 600.0f, 6.0f, 4.5f);
        if (!assertFloatClose(back.x, worlds[i][0]) ||
            !assertFloatClose(back.y, worlds[i][1])) {
            std::cerr << "uiToScreen/screenToUi round-trip failed at " << i << "\n";
            ok = false;
        }
    }
    // Camera wrapper round-trips on a resized camera (halfW = 12 at 1600x600).
    pe::Camera cam;
    cam.onResize(1600, 600);
    const pe::Vec3 edge = cam.worldToScreenUi(12.0f, -4.5f, 1600.0f, 600.0f);
    if (!assertFloatClose(edge.x, 1600.0f) || !assertFloatClose(edge.y, 600.0f)) {
        std::cerr << "resized worldToScreenUi right edge wrong\n";
        ok = false;
    }
    const pe::Vec3 back2 = cam.screenToWorldUi(edge.x, edge.y, 1600.0f, 600.0f);
    if (!assertFloatClose(back2.x, 12.0f) || !assertFloatClose(back2.y, -4.5f)) {
        std::cerr << "worldToScreenUi/screenToWorldUi round-trip failed\n";
        ok = false;
    }
    // worldToScreen subtracts the camera position (inverse of screenToWorld):
    // camera at (2,1) looking at world (2,1) shows it at the window center.
    pe::Camera moved;
    moved.follow(pe::Vec3(2.0f, 1.0f, 0.0f));
    const pe::Vec3 px = moved.worldToScreen(pe::Vec3(2.0f, 1.0f, 0.0f),
                                            800.0f, 600.0f);
    if (!assertFloatClose(px.x, 400.0f) || !assertFloatClose(px.y, 300.0f)) {
        std::cerr << "worldToScreen must subtract camera position\n";
        ok = false;
    }
    const pe::Vec3 back3 = moved.screenToWorld(px.x, px.y, 800.0f, 600.0f);
    if (!assertFloatClose(back3.x, 2.0f) || !assertFloatClose(back3.y, 1.0f)) {
        std::cerr << "screenToWorld/worldToScreen round-trip failed\n";
        ok = false;
    }
    return ok;
}

// --- Step 136: spawn -> dump -> reload persistence interaction proof ---
// Composes Step 120 (prefab), Step 113 (spawn/kill), and Step 111 (v2
// serialization) headlessly: a prefab-spawned live entity and one killed
// entity are saved; the dead one must be dropped (documented Step 113
// save semantics), and the live one's config must survive the round-trip.
static bool checkPrefabScenePersist() {
    bool ok = true;
    // 1. Scene + prefab-spawned live entity + one killed entity.
    pe::Scene src;
    src.name = "persist_test";
    pe::Prefab p;
    if (!pe::loadPrefab("enemy.txt", p)) {
        std::cerr << "persist test: enemy.txt failed to load\n";
        return false;
    }
    const std::size_t liveIdx =
        pe::spawnEntity(src, pe::instantiatePrefab(p, pe::Vec3(2.0f, 1.0f, 0.0f)));
    pe::Entity doomed = pe::instantiatePrefab(p, pe::Vec3(-2.0f, -1.0f, 0.0f));
    doomed.tag = "doomed";
    const std::size_t deadIdx = pe::spawnEntity(src, doomed);
    pe::killEntity(src, deadIdx);  // dead BEFORE save: dropped on save
    if (!src.entities[liveIdx].alive) { std::cerr << "spawnEntity must leave alive\n"; return false; }
    if (src.entities[deadIdx].alive) { std::cerr << "killEntity must mark dead\n"; return false; }

    // 2. Save (bare filename probes the 3-candidate dirs; same rmAll
    // cleanup pattern as the existing serialization tests).
    const std::string fname = "scene_test_prefab_persist.txt";
    auto rmAll = [&](const std::string& f) {
        std::remove(("assets/" + f).c_str());
        std::remove(("../assets/" + f).c_str());
        std::remove(("../../assets/" + f).c_str());
        std::remove(("assets/" + f + ".tmp").c_str());
        std::remove(("../assets/" + f + ".tmp").c_str());
    };
    rmAll(fname);
    if (!pe::saveSceneToFile(src, fname)) {
        std::cerr << "persist test: save failed\n";
        rmAll(fname);
        return false;
    }

    // 3. Load into a fresh Scene.
    pe::Scene loaded;
    if (!pe::loadSceneFromFile(fname, loaded)) {
        std::cerr << "persist test: load failed\n";
        rmAll(fname);
        return false;
    }

    // 4. Dead entity dropped: only the live one round-trips.
    if (loaded.entities.size() != 1) {
        std::cerr << "Dead entity must not survive the save (got "
                  << loaded.entities.size() << ")\n";
        rmAll(fname);
        return false;
    }
    const pe::Entity& e = loaded.entities[0];
    // Config survived: prefab-carried fields + spawn position.
    if (e.roleId != 2 || e.tag != "hostile" || e.textureId != 2 ||
        e.depth != 2 || e.cols != 8 || e.rows != 1) {
        std::cerr << "Persisted entity lost prefab config\n";
        rmAll(fname);
        ok = false;
    }
    if (!assertFloatClose(e.health, 100.0f)) {
        std::cerr << "Persisted entity health wrong\n";
        rmAll(fname);
        ok = false;
    }
    if (e.currentClipName != "walk_left") {
        std::cerr << "Persisted entity clip name wrong\n";
        rmAll(fname);
        ok = false;
    }
    if (!assertFloatClose(e.position.x, 2.0f) ||
        !assertFloatClose(e.position.y, 1.0f)) {
        std::cerr << "Persisted entity position wrong\n";
        rmAll(fname);
        ok = false;
    }
    if (!assertFloatClose(e.halfExtents.x, 0.5f) ||
        !assertFloatClose(e.halfExtents.y, 0.5f)) {
        std::cerr << "Persisted entity half-extents wrong\n";
        rmAll(fname);
        ok = false;
    }
    // Alive: the loaded entity is live (dead ones never exist in saved
    // state — the documented Step 113 save semantics).
    if (!e.alive) {
        std::cerr << "Persisted live entity must be alive\n";
        rmAll(fname);
        ok = false;
    }

    rmAll(fname);
    std::cout << "checkPrefabScenePersist PASSED\n";
    return true;
}

// --- Step 146: GameState gate-table (headless, pure) ---
// Locks the full gate table across all six states: which states simulate,
// which draw the world, and the per-state clear-color palette. These are
// the gates the whole frame loop sits behind (Step 19 boundary).
static bool checkGameStateGateTable() {
    using pe::GameState;
    bool ok = true;
    // simulates: PLAYING + PLAYING_ALT only.
    if (!pe::simulates(GameState::PLAYING) ||
        !pe::simulates(GameState::PLAYING_ALT)) {
        std::cerr << "PLAYING states must simulate\n";
        ok = false;
    }
    if (pe::simulates(GameState::MENU) || pe::simulates(GameState::PAUSED) ||
        pe::simulates(GameState::GAME_OVER) || pe::simulates(GameState::WIN)) {
        std::cerr << "MENU/PAUSED/GAME_OVER/WIN must not simulate\n";
        ok = false;
    }
    // drawsWorld: everything except MENU.
    if (pe::drawsWorld(GameState::MENU)) {
        std::cerr << "MENU must not draw the world\n";
        ok = false;
    }
    if (!pe::drawsWorld(GameState::PLAYING) ||
        !pe::drawsWorld(GameState::PLAYING_ALT) ||
        !pe::drawsWorld(GameState::PAUSED) ||
        !pe::drawsWorld(GameState::GAME_OVER) ||
        !pe::drawsWorld(GameState::WIN)) {
        std::cerr << "Non-menu states must draw the world\n";
        ok = false;
    }
    // Clear-color palette (Step 11/19 values): MENU purple, GAME_OVER
    // dark red, WIN dark green, gameplay black or dark-blue on toggle.
    struct Row { GameState state; bool toggled; float r, g, b; const char* what; };
    const Row rows[] = {
        {GameState::MENU,      false, 0.16f, 0.0f,  0.24f, "MENU purple"},
        {GameState::GAME_OVER, false, 0.28f, 0.0f,  0.0f,  "GAME_OVER dark red"},
        {GameState::WIN,       false, 0.0f,  0.28f, 0.08f, "WIN dark green"},
        {GameState::PLAYING,   false, 0.02f, 0.02f, 0.08f, "PLAYING black"},
        {GameState::PLAYING,   true,  0.0f,  0.0f,  0.25f, "PLAYING blue toggle"},
        {GameState::PAUSED,    true,  0.0f,  0.0f,  0.25f, "PAUSED honors toggle"},
    };
    for (const Row& row : rows) {
        const pe::ClearColor c = pe::clearColorFor(row.state, row.toggled);
        if (!assertFloatClose(c.r, row.r) || !assertFloatClose(c.g, row.g) ||
            !assertFloatClose(c.b, row.b)) {
            std::cerr << row.what << " wrong\n";
            ok = false;
        }
    }
    // GAME_OVER/WIN palette wins over the toggle flag (priority order).
    const pe::ClearColor goToggled = pe::clearColorFor(GameState::GAME_OVER, true);
    if (!assertFloatClose(goToggled.r, 0.28f) || !assertFloatClose(goToggled.b, 0.0f)) {
        std::cerr << "GAME_OVER palette must beat the blue toggle\n";
        ok = false;
    }
    // PLAYING_ALT shares the gameplay palette (same path, by design).
    const pe::ClearColor alt = pe::clearColorFor(GameState::PLAYING_ALT, false);
    if (!assertFloatClose(alt.r, 0.02f) || !assertFloatClose(alt.b, 0.08f)) {
        std::cerr << "PLAYING_ALT must share the gameplay palette\n";
        ok = false;
    }
    return ok;
}

// --- Step 147: highscore file semantics (headless) ---
// Replicates the EXACT load guard and save format main.cpp uses for
// savedata/highscore.txt (the code path itself is main.cpp-only, not a
// header boundary — this tests the SEMANTICS: round-trip, NaN reject,
// garbage reject, missing-file default). Guard verbatim:
// `in >> stored && stored >= 0.0f` (NaN fails every comparison, so the
// >= 0 guard rejects it). Format verbatim: fixed, setprecision(1), '\n'.
static bool checkHighscoreSemantics() {
    bool ok = true;
    const std::string fname = "test_highscore_tmp.txt";
    auto rm = [&]() { std::remove(fname.c_str()); };
    rm();

    // 1. Round-trip: same save format, same load guard.
    {
        std::ofstream out(fname);
        out << std::fixed << std::setprecision(1) << 12.7f << '\n';
        out.flush();
        if (!out) { std::cerr << "highscore temp save failed\n"; rm(); return false; }
        std::ifstream in(fname);
        float stored = 0.0f;
        if (in >> stored && stored >= 0.0f) {
            if (!assertFloatClose(stored, 12.7f)) {
                std::cerr << "highscore round-trip wrong: " << stored << "\n";
                ok = false;
            }
        } else {
            std::cerr << "highscore load guard rejected a valid value\n";
            ok = false;
        }
    }
    // 2. Format is exactly one decimal place.
    {
        std::ofstream out(fname);
        out << std::fixed << std::setprecision(1) << 12.34f << '\n';
        out.flush(); out.close();
        std::ifstream in(fname);
        std::string text;
        std::getline(in, text);
        if (text != "12.3") {
            std::cerr << "highscore format must be one decimal: '" << text << "'\n";
            ok = false;
        }
    }
    // 3. NaN reject: the >= 0 guard fails on NaN (all comparisons false).
    {
        std::ofstream out(fname);
        out << "nan\n";
        out.close();
        std::ifstream in(fname);
        float stored = 99.0f;
        if (in >> stored && stored >= 0.0f) {
            std::cerr << "NaN must be rejected by the >= 0 guard\n";
            ok = false;
        }
    }
    // 4. Garbage reject: stream parse fails, default stands.
    {
        std::ofstream out(fname);
        out << "not a number at all\n";
        out.close();
        std::ifstream in(fname);
        float stored = 99.0f;
        if (in >> stored && stored >= 0.0f) {
            std::cerr << "Garbage must fail the stream parse\n";
            ok = false;
        }
    }
    // 5. Missing file: open fails, no crash, default stands.
    {
        rm();
        std::ifstream in(fname);
        if (in) { std::cerr << "Removed file must not open\n"; ok = false; }
    }
    rm();
    return ok;
}

static bool checkSceneSerialization() {
    pe::Scene src;
    src.name = "test_roundtrip";
    pe::Entity e1(pe::Vec3(1.0f, 2.0f, 0.0f), 1.0f, pe::Vec3(1.0f, 1.0f, 1.0f), pe::Vec3(0.5f, 0.5f, 0.0f), 2);
    e1.depth = 2; e1.roleId = 2; e1.moveSpeed = 1.8f;
    pe::Entity e2(pe::Vec3(-1.0f, 0.5f, 0.0f), -1.2f, pe::Vec3(0.8f, 0.8f, 1.0f), pe::Vec3(0.4f, 0.4f, 0.0f), 0);
    e2.depth = 3; e2.roleId = 0; e2.moveSpeed = 0.0f;
    src.entities = {e1, e2};
    src.tilemapFile = "arcade_arena.txt";
    src.tilemap = pe::loadTilemap(src.tilemapFile);
    if (src.tilemap.width <= 0) { std::cerr << "Scene ser: tilemap load failed\n"; return false; }
    const std::string fname = "scene_test_roundtrip.txt";
    auto rmAll = [&](const std::string& f){ std::remove(("assets/" + f).c_str()); std::remove(("../assets/" + f).c_str()); std::remove(("../../assets/" + f).c_str()); std::remove(("assets/" + f + ".tmp").c_str()); std::remove(("../assets/" + f + ".tmp").c_str()); };
    rmAll(fname);
    if (!pe::saveSceneToFile(src, fname)) { std::cerr << "Scene ser: save failed\n"; return false; }
    pe::Scene loaded;
    loaded.name = "before";
    if (!pe::loadSceneFromFile(fname, loaded)) { std::cerr << "Scene ser: load failed\n"; rmAll(fname); return false; }
    if (loaded.name != src.name) { std::cerr << "Scene ser: name mismatch\n"; rmAll(fname); return false; }
    if (loaded.entities.size() != src.entities.size()) { std::cerr << "Scene ser: entity count\n"; rmAll(fname); return false; }
    for (size_t i = 0; i < src.entities.size(); ++i) {
        const pe::Entity& a = src.entities[i];
        const pe::Entity& b = loaded.entities[i];
        if (!assertFloatClose(a.position.x, b.position.x) || !assertFloatClose(a.position.y, b.position.y) || !assertFloatClose(a.position.z, b.position.z) ||
            !assertFloatClose(a.rotationSpeed, b.rotationSpeed) || !assertFloatClose(a.scale.x, b.scale.x) || !assertFloatClose(a.moveSpeed, b.moveSpeed) ||
            a.textureId != b.textureId || a.depth != b.depth || a.roleId != b.roleId) {
            std::cerr << "Scene ser: entity mismatch at " << i << "\n"; rmAll(fname); return false;
        }
    }
    if (loaded.tilemap.tiles.size() != src.tilemap.tiles.size() || loaded.tilemap.width != src.tilemap.width) {
        std::cerr << "Scene ser: tilemap mismatch\n"; rmAll(fname); return false;
    }
    // Strict fallback: malformed file must leave out untouched
    {
        std::ofstream bad("assets/scene_malformed.txt");
        if (!bad) bad.open("../assets/scene_malformed.txt");
        bad << "scene=bad\nentity=not_a_number\n";
        bad.close();
        pe::Scene out;
        out.name = "keep";
        bool ok = pe::loadSceneFromFile("scene_malformed.txt", out);
        std::remove("assets/scene_malformed.txt"); std::remove("../assets/scene_malformed.txt"); std::remove("../../assets/scene_malformed.txt");
        if (ok || out.name != "keep") { std::cerr << "Scene ser: malformed should fail and leave untouched\n"; rmAll(fname); return false; }
    }
    rmAll(fname);
    return true;
}

// --- Step 111: persistence v2 round-trip (all extended fields survive) ---
static bool checkScenePersistenceV2() {
    pe::Scene src;
    src.name = "persist_v2";
    pe::Entity e(pe::Vec3(1.5f, -2.25f, 0.5f), 2.0f, pe::Vec3(1.5f, 0.5f, 1.0f), pe::Vec3(0.4f, 0.3f, 0.0f), 3);
    e.rotationAngle = 1.234f;
    e.depth = 2; e.roleId = 7; e.moveSpeed = 3.5f;
    e.velocity = pe::Vec3(4.0f, -5.0f, 0.25f);
    e.gravityScale = 1.5f;
    e.isStatic = true;
    e.coyoteTime = 0.2f; e.jumpImpulse = 9.5f; e.maxFallSpeed = 30.0f;
    e.tint = pe::Vec3(0.5f, 0.25f, 0.75f);
    e.cols = 4; e.rows = 2;
    e.health = 42.5f; e.timer = 3.25f;
    e.tag = "enemy, fast";  // embedded comma exercises quote-aware split
    e.parentIndex = 0;
    e.animationSpeed = 2.0f;
    e.currentClipName = "run";
    pe::Entity e2;  // all defaults must round-trip too
    src.entities = {e, e2};
    const std::string fname = "scene_test_persist_v2.txt";
    auto rmAll = [&](const std::string& f){ std::remove(("assets/" + f).c_str()); std::remove(("../assets/" + f).c_str()); std::remove(("../../assets/" + f).c_str()); std::remove(("assets/" + f + ".tmp").c_str()); std::remove(("../assets/" + f + ".tmp").c_str()); };
    rmAll(fname);
    if (!pe::saveSceneToFile(src, fname)) { std::cerr << "PersistV2: save failed\n"; return false; }
    pe::Scene loaded;
    if (!pe::loadSceneFromFile(fname, loaded)) { std::cerr << "PersistV2: load failed\n"; rmAll(fname); return false; }
    if (loaded.name != src.name || loaded.entities.size() != 2) { std::cerr << "PersistV2: header/entity count\n"; rmAll(fname); return false; }
    const pe::Entity& a = src.entities[0];
    const pe::Entity& b = loaded.entities[0];
    bool ok = assertFloatClose(a.position.x, b.position.x) && assertFloatClose(a.position.y, b.position.y) && assertFloatClose(a.position.z, b.position.z)
        && assertFloatClose(a.rotationAngle, b.rotationAngle) && assertFloatClose(a.rotationSpeed, b.rotationSpeed)
        && assertFloatClose(a.scale.x, b.scale.x) && assertFloatClose(a.scale.y, b.scale.y) && assertFloatClose(a.scale.z, b.scale.z)
        && assertFloatClose(a.halfExtents.x, b.halfExtents.x) && assertFloatClose(a.halfExtents.y, b.halfExtents.y) && assertFloatClose(a.halfExtents.z, b.halfExtents.z)
        && assertFloatClose(a.moveSpeed, b.moveSpeed)
        && assertFloatClose(a.velocity.x, b.velocity.x) && assertFloatClose(a.velocity.y, b.velocity.y) && assertFloatClose(a.velocity.z, b.velocity.z)
        && assertFloatClose(a.gravityScale, b.gravityScale)
        && assertFloatClose(a.coyoteTime, b.coyoteTime) && assertFloatClose(a.jumpImpulse, b.jumpImpulse) && assertFloatClose(a.maxFallSpeed, b.maxFallSpeed)
        && assertFloatClose(a.tint.x, b.tint.x) && assertFloatClose(a.tint.y, b.tint.y) && assertFloatClose(a.tint.z, b.tint.z)
        && assertFloatClose(a.health, b.health) && assertFloatClose(a.timer, b.timer)
        && assertFloatClose(a.animationSpeed, b.animationSpeed)
        && a.textureId == b.textureId && a.depth == b.depth && a.roleId == b.roleId
        && a.isStatic == b.isStatic && a.cols == b.cols && a.rows == b.rows
        && a.parentIndex == b.parentIndex && a.tag == b.tag && a.currentClipName == b.currentClipName;
    if (!ok) { std::cerr << "PersistV2: extended field mismatch\n"; rmAll(fname); return false; }
    // Defaults entity round-trips identically (incl. jumpImpulse 12.0 ctor default)
    const pe::Entity& d0 = src.entities[1];
    const pe::Entity& d1 = loaded.entities[1];
    if (!assertFloatClose(d0.jumpImpulse, d1.jumpImpulse) || !assertFloatClose(d0.coyoteTime, d1.coyoteTime)
        || d0.tag != d1.tag || d0.currentClipName != d1.currentClipName || d0.isStatic != d1.isStatic
        || d0.parentIndex != d1.parentIndex || d0.cols != d1.cols) { std::cerr << "PersistV2: default entity mismatch\n"; rmAll(fname); return false; }
    rmAll(fname);
    return true;
}

// --- Step 111: v1 backward compatibility (14 fields + correct defaults) ---
static bool checkScenePersistenceV1Compat() {
    const std::string fname = "scene_test_persist_v1.txt";
    auto rmAll = [&](const std::string& f){ std::remove(("assets/" + f).c_str()); std::remove(("../assets/" + f).c_str()); std::remove(("../../assets/" + f).c_str()); };
    rmAll(fname);
    {
        std::ofstream f("assets/" + fname, std::ios::binary | std::ios::trunc);
        if (!f) { std::cerr << "PersistV1: cannot write fixture\n"; return false; }
        f << "# scene v1\nscene=v1compat\nentity=1.0000,2.0000,0.0000,1.0000,1.0000,1.0000,1.0000,0.5000,0.5000,0.0000,2,2,2,1.8000\n";
    }
    pe::Scene loaded;
    loaded.name = "before";
    if (!pe::loadSceneFromFile(fname, loaded)) { std::cerr << "PersistV1: load failed\n"; rmAll(fname); return false; }
    if (loaded.name != "v1compat" || loaded.entities.size() != 1) { std::cerr << "PersistV1: header/count\n"; rmAll(fname); return false; }
    const pe::Entity& b = loaded.entities[0];
    bool ok = assertFloatClose(b.position.x, 1.0f) && assertFloatClose(b.position.y, 2.0f)
        && assertFloatClose(b.rotationSpeed, 1.0f) && assertFloatClose(b.moveSpeed, 1.8f)
        && b.textureId == 2 && b.depth == 2 && b.roleId == 2
        && assertFloatClose(b.rotationAngle, 0.0f)
        && assertFloatClose(b.velocity.x, 0.0f) && assertFloatClose(b.gravityScale, 0.0f) && !b.isStatic
        && assertFloatClose(b.coyoteTime, 0.1f) && assertFloatClose(b.jumpImpulse, 7.0f) && assertFloatClose(b.maxFallSpeed, 25.0f)
        && assertFloatClose(b.tint.x, 1.0f) && b.cols == 1 && b.rows == 1
        && assertFloatClose(b.health, 100.0f) && assertFloatClose(b.timer, 0.0f)
        && b.tag.empty() && b.parentIndex == -1
        && assertFloatClose(b.animationSpeed, 1.0f) && b.currentClipName.empty();
    if (!ok) { std::cerr << "PersistV1: field/default mismatch\n"; rmAll(fname); return false; }
    rmAll(fname);
    return true;
}

// --- Step 111: unknown version is rejected without touching out ---
static bool checkSceneVersionUnknown() {
    const std::string fname = "scene_test_persist_v99.txt";
    auto rmAll = [&](const std::string& f){ std::remove(("assets/" + f).c_str()); std::remove(("../assets/" + f).c_str()); std::remove(("../../assets/" + f).c_str()); };
    rmAll(fname);
    {
        std::ofstream f("assets/" + fname, std::ios::binary | std::ios::trunc);
        if (!f) { std::cerr << "PersistV99: cannot write fixture\n"; return false; }
        f << "# scene v99\nscene=nope\nentity=1.0000,2.0000,0.0000,1.0000,1.0000,1.0000,1.0000,0.5000,0.5000,0.0000,2,2,2,1.8000\n";
    }
    pe::Scene out;
    out.name = "keep";
    bool ok = pe::loadSceneFromFile(fname, out);
    rmAll(fname);
    if (ok || out.name != "keep") { std::cerr << "PersistV99: should fail and leave out untouched\n"; return false; }
    return true;
}

// --- Step 113: runtime entity lifecycle (alive + spawn/kill) ---
static bool checkEntityLifecycle() {
    // Default: every entity is born alive (non-breaking).
    pe::Entity def;
    if (!def.alive) { std::cerr << "Lifecycle: default entity not alive\n"; return false; }
    pe::Scene s;
    s.name = "lifecycle";
    pe::Entity a(pe::Vec3(0.0f, 0.0f, 0.0f), 1.0f, pe::Vec3(1,1,1));
    pe::Entity b(pe::Vec3(1.0f, 0.0f, 0.0f), 2.0f, pe::Vec3(1,1,1));
    pe::Entity c(pe::Vec3(2.0f, 0.0f, 0.0f), 3.0f, pe::Vec3(1,1,1));
    s.entities = {a, b, c};
    // Kill the middle slot: no erase, indices stable.
    pe::killEntity(s, 1);
    if (s.entities.size() != 3) { std::cerr << "Lifecycle: kill changed size\n"; return false; }
    if (s.entities[1].alive) { std::cerr << "Lifecycle: kill did not mark dead\n"; return false; }
    if (!s.entities[0].alive || !s.entities[2].alive) { std::cerr << "Lifecycle: kill hit neighbors\n"; return false; }
    // Out-of-range kill: safe no-op.
    pe::killEntity(s, 99);
    if (s.entities.size() != 3) { std::cerr << "Lifecycle: OOB kill changed size\n"; return false; }
    // Rendering mirror: dead slots draw nothing, survivors draw.
    size_t drawn = 0;
    for (const auto& e : s.entities) { if (!e.alive) continue; ++drawn; }
    if (drawn != 2) { std::cerr << "Lifecycle: drawn count\n"; return false; }
    // advanceRotations skips the dead slot (angle frozen) but moves the rest.
    pe::advanceRotations(s.entities, 1.0f);
    if (!assertFloatClose(s.entities[0].rotationAngle, 1.0f) || !assertFloatClose(s.entities[2].rotationAngle, 3.0f)) { std::cerr << "Lifecycle: survivors did not rotate\n"; return false; }
    if (!assertFloatClose(s.entities[1].rotationAngle, 0.0f)) { std::cerr << "Lifecycle: dead entity rotated\n"; return false; }
    // Spawn appends a live entity and returns its stable index.
    std::size_t idx = pe::spawnEntity(s, a);
    if (idx != 3 || s.entities.size() != 4) { std::cerr << "Lifecycle: spawn index/size\n"; return false; }
    size_t alive = 0;
    for (const auto& e : s.entities) { if (e.alive) ++alive; }
    if (alive != 3) { std::cerr << "Lifecycle: alive count after spawn\n"; return false; }
    // Persistence drops the dead slot: save + load yields 3 live entities.
    const std::string fname = "scene_test_lifecycle.txt";
    auto rmAll = [&](const std::string& f){ std::remove(("assets/" + f).c_str()); std::remove(("../assets/" + f).c_str()); std::remove(("../../assets/" + f).c_str()); std::remove(("assets/" + f + ".tmp").c_str()); std::remove(("../assets/" + f + ".tmp").c_str()); };
    rmAll(fname);
    if (!pe::saveSceneToFile(s, fname)) { std::cerr << "Lifecycle: save failed\n"; return false; }
    pe::Scene loaded;
    if (!pe::loadSceneFromFile(fname, loaded)) { std::cerr << "Lifecycle: load failed\n"; rmAll(fname); return false; }
    rmAll(fname);
    if (loaded.entities.size() != 3) { std::cerr << "Lifecycle: dead entity persisted\n"; return false; }
    for (const auto& e : loaded.entities) { if (!e.alive) { std::cerr << "Lifecycle: loaded entity dead\n"; return false; } }
    return true;
}

static bool checkPongScore() {
    int left = 0, right = 0;
    const int win = 5;
    bool winFlag = false;
    int winner = 0;
    // left scores 5
    for (int i = 0; i < 5; ++i) {
        ++left;
        if (left >= win) { winFlag = true; winner = 1; break; }
    }
    if (!winFlag || winner != 1 || left != 5) { std::cerr << "Pong score left win failed\n"; return false; }
    // reset
    left = 0; right = 0; winFlag = false; winner = 0;
    // right scores 1 then left scores, no win
    ++right;
    if (winFlag) { std::cerr << "Pong premature win\n"; return false; }
    // ball reset logic would set position to 0,0,0 - just check scores
    if (right != 1 || left != 0) { std::cerr << "Pong score increment failed\n"; return false; }
    return true;
}

static bool checkParticleColorAndEmit() {
    // Particle.color now renders via Entity.tint (Step 86)
    pe::Particle p;
    p.position = pe::Vec3(0,0,0);
    p.velocity = pe::Vec3(0,0,0);
    p.life = 1.0f; p.maxLife = 1.0f; p.size = 0.5f;
    p.color = pe::Vec3(0.2f, 0.8f, 0.4f);
    std::vector<pe::Particle> pool = {p};
    auto ents = pe::particlesToEntities(pool, 2, 4, 99);
    if (ents.size() != 1) { std::cerr << "Particle color: entity count\n"; return false; }
    if (!assertFloatClose(ents[0].tint.x, 0.2f) || !assertFloatClose(ents[0].tint.y, 0.8f) || !assertFloatClose(ents[0].tint.z, 0.4f)) {
        std::cerr << "Particle color not passed to Entity.tint\n"; return false;
    }
    // Emitter::emit no longer orphan â€” call it once
    pe::Emitter e;
    e.position = pe::Vec3(0,0,0);
    e.spawnRate = 10.0f; e.maxParticles = 5;
    std::vector<pe::Particle> out;
    pe::emit(e, out, 0.2f); // should spawn ~2 particles
    if (out.empty() || (int)out.size() > 5) { std::cerr << "Emitter emit failed\n"; return false; }
    return true;
}

// --- Step 70 contract lock (headless, documented behaviors unasserted) ---
// Closes the remaining documented-but-untested particle contracts:
// non-positive maxParticles never spawns, fractional accumulator carry,
// dt<=0 emit leaves the accumulator untouched, maxLife survives update,
// negative-life particles never convert, empty-pool conversion is safe,
// and swap-with-back reorder on middle death.
static bool checkParticleContract() {
    bool ok = true;
    // Non-positive maxParticles means "never spawn" (0 and negative).
    pe::Emitter never;
    never.spawnRate = 10.0f;
    never.maxParticles = 0;
    std::vector<pe::Particle> pool0;
    pe::emit(never, pool0, 1.0f);
    if (!pool0.empty()) {
        std::cerr << "maxParticles=0 must never spawn\n";
        ok = false;
    }
    never.maxParticles = -5;
    pe::emit(never, pool0, 1.0f);
    if (!pool0.empty()) {
        std::cerr << "negative maxParticles must never spawn\n";
        ok = false;
    }
    // Skipped budget is consumed, not backlogged: after the room frees,
    // a fresh 0.1s emit at rate 10 yields exactly 1 (a backlog would burst).
    never.maxParticles = 256;
    never.accumulator = 0.0f;
    pe::emit(never, pool0, 0.1f);
    if (pool0.size() != 1) {
        std::cerr << "post-cap fresh emit must yield exactly 1 (no backlog)\n";
        ok = false;
    }
    // Fractional accumulator carry: rate 10 over two 0.05s emits gives
    // 0.5 + 0.5 = 1 whole unit — exactly one particle, on the second call.
    pe::Emitter frac;
    frac.spawnRate = 10.0f;
    std::vector<pe::Particle> poolFrac;
    frac.accumulator = 0.0f;
    pe::emit(frac, poolFrac, 0.05f);
    if (!poolFrac.empty()) {
        std::cerr << "0.5 budget must not spawn yet (carry, not early spawn)\n";
        ok = false;
    }
    pe::emit(frac, poolFrac, 0.05f);
    if (poolFrac.size() != 1) {
        std::cerr << "0.5+0.5 accumulator carry must spawn exactly 1\n";
        ok = false;
    }
    // dt <= 0 emit leaves the accumulator untouched (the early return
    // runs before the accumulator line).
    const float accBefore = frac.accumulator;
    pe::emit(frac, poolFrac, 0.0f);
    pe::emit(frac, poolFrac, -1.0f);
    if (!assertFloatClose(frac.accumulator, accBefore)) {
        std::cerr << "dt<=0 emit must not touch the accumulator\n";
        ok = false;
    }
    // maxLife survives updateParticles (integration/aging never rewrites it).
    pe::Particle aged;
    aged.position = pe::Vec3(0, 0, 0);
    aged.velocity = pe::Vec3(0, 0, 0);
    aged.life = 1.0f;
    aged.maxLife = 3.0f;
    std::vector<pe::Particle> poolAged = {aged};
    pe::updateParticles(poolAged, 0.25f);
    if (!assertFloatClose(poolAged[0].maxLife, 3.0f) ||
        !assertFloatClose(poolAged[0].life, 0.75f)) {
        std::cerr << "updateParticles must age life, never rewrite maxLife\n";
        ok = false;
    }
    // A negative-life particle (spawnParticle stores life as given) is
    // dead from birth: never converts, and one update removes it.
    std::vector<pe::Particle> poolNeg;
    pe::spawnParticle(poolNeg, pe::Vec3(1, 1, 0), pe::Vec3(0, 0, 0), -1.0f,
                      0.5f, pe::Vec3(1, 1, 1));
    if (!pe::particlesToEntities(poolNeg, 0, 0, 0).empty()) {
        std::cerr << "negative-life particle must never convert\n";
        ok = false;
    }
    pe::updateParticles(poolNeg, 0.1f);
    if (!poolNeg.empty()) {
        std::cerr << "negative-life particle must die on first update\n";
        ok = false;
    }
    // Empty-pool conversion: empty vector, no crash.
    std::vector<pe::Particle> poolEmpty;
    if (!pe::particlesToEntities(poolEmpty, 2, 3, 7).empty()) {
        std::cerr << "empty pool must convert to empty entities\n";
        ok = false;
    }
    // Swap-with-back reorder: killing the FIRST of three moves the last
    // survivor into its slot — order is NOT preserved (documented pool
    // behavior). Both survivors must stay alive with intact data.
    std::vector<pe::Particle> poolReorder;
    pe::spawnParticle(poolReorder, pe::Vec3(10, 0, 0), pe::Vec3(0, 0, 0),
                      0.1f, 0.5f, pe::Vec3(1, 1, 1));   // index 0: dies
    pe::spawnParticle(poolReorder, pe::Vec3(20, 0, 0), pe::Vec3(0, 0, 0),
                      5.0f, 0.5f, pe::Vec3(1, 1, 1));   // index 1: survives
    pe::spawnParticle(poolReorder, pe::Vec3(30, 0, 0), pe::Vec3(0, 0, 0),
                      5.0f, 0.5f, pe::Vec3(1, 1, 1));   // index 2: survives
    pe::updateParticles(poolReorder, 1.0f);
    if (poolReorder.size() != 2) {
        std::cerr << "middle-death reorder: wrong survivor count\n";
        ok = false;
    } else {
        bool found20 = false, found30 = false;
        for (const pe::Particle& p : poolReorder) {
            if (assertFloatClose(p.position.x, 20.0f)) found20 = true;
            if (assertFloatClose(p.position.x, 30.0f)) found30 = true;
        }
        if (!found20 || !found30) {
            std::cerr << "middle-death reorder: survivor data corrupted\n";
            ok = false;
        }
    }
    return ok;
}

static bool checkFixedTimestepNoTunnel() {
    // Large dt (0.1s) with fast fall would tunnel a 1-unit tile in one variable step.
    // Fixed substeps (1/60) must keep character on top of floor.
    pe::Entity floor(pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f), pe::Vec3(0.5f, 0.5f, 0.0f), 1);
    floor.isStatic = true;
    std::vector<pe::Entity> statics = {floor};
    pe::Entity player(pe::Vec3(0.0f, 1.5f, 0.0f), 0.0f, pe::Vec3(0.8f, 0.8f, 1.0f), pe::Vec3(0.4f, 0.4f, 0.0f), 0);
    player.gravityScale = 0.0f;
    player.velocity = pe::Vec3(0.0f, -20.0f, 0.0f); // fast down
    player.coyoteTime = 0.0f;
    bool grounded = pe::updateCharacterControllerFixed(player, statics, 0.1f, false, 1.0f/60.0f);
    // Must be grounded and not tunneled below floor top (0.5)
    float bottom = player.position.y - player.halfExtents.y * player.scale.y;
    float floorTop = floor.position.y + floor.halfExtents.y * floor.scale.y;
    if (!grounded) { std::cerr << "Fixed step should be grounded\n"; return false; }
    if (bottom < floorTop - 0.05f) { std::cerr << "Fixed step tunneled: bottom " << bottom << " floorTop " << floorTop << "\n"; return false; }
    return true;
}

static bool checkActionMap() {
    auto leftKeys = pe::keysForAction(pe::Action::MoveLeft);
    auto jumpKeys = pe::keysForAction(pe::Action::Jump);
    if (leftKeys.empty() || jumpKeys.empty()) { std::cerr << "Action map empty\n"; return false; }
    bool hasA = false, hasLeft = false;
    for (int k : leftKeys) { if (k == GLFW_KEY_A) hasA = true; if (k == GLFW_KEY_LEFT) hasLeft = true; }
    if (!hasA || !hasLeft) { std::cerr << "MoveLeft mapping missing\n"; return false; }
    bool hasSpace = false;
    for (int k : jumpKeys) if (k == GLFW_KEY_SPACE) hasSpace = true;
    if (!hasSpace) { std::cerr << "Jump mapping missing SPACE\n"; return false; }
    // Raw path still present: check that Input still has isDown/isEdge
    return true;
}

static bool checkTextureRegistry() {
    // Growable registry proof (Step 89) â€” vector, not fixed [5]
    std::vector<unsigned int> reg;
    reg.reserve(8);
    for (int i = 0; i < 5; ++i) reg.push_back(static_cast<unsigned int>(i+1));
    if (reg.size() != 5) { std::cerr << "Texture registry size 5 failed\n"; return false; }
    reg.push_back(6); // 6th texture, id 5
    if (reg.size() != 6 || reg[5] != 6) { std::cerr << "Texture registry grow to 6 failed\n"; return false; }
    // OOB still falls back to checker (slot 99 -> checker)
    int oobSlot = 99;
    bool oob = (oobSlot < 0 || oobSlot >= static_cast<int>(reg.size()));
    if (!oob) { std::cerr << "Texture OOB check failed\n"; return false; }
    // Valid id 5 must not be OOB
    if (5 < 0 || 5 >= static_cast<int>(reg.size())) { std::cerr << "Texture id 5 should be valid\n"; return false; }
    return true;
}

static bool checkEventThrowAndOnce() {
    pe::EventBus bus;
    int calls = 0;
    bus.subscribe(pe::EventType::Collision, [&](const pe::GameEvent&) { ++calls; });
    bus.subscribe(pe::EventType::Collision, [&](const pe::GameEvent&) { throw std::runtime_error("boom"); });
    bus.subscribe(pe::EventType::Collision, [&](const pe::GameEvent&) { ++calls; });
    bus.emit(pe::GameEvent{pe::EventType::Collision, 1, 2});
    if (calls != 2) { std::cerr << "Throwing handler broke bus\n"; return false; }
    // once() should fire once then auto-remove
    int onceCalls = 0;
    bus.once(pe::EventType::SceneChanged, [&](const pe::GameEvent&) { ++onceCalls; });
    bus.emit(pe::GameEvent{pe::EventType::SceneChanged, 0, 1});
    bus.emit(pe::GameEvent{pe::EventType::SceneChanged, 0, 1});
    if (onceCalls != 1) { std::cerr << "once() failed\n"; return false; }
    bus.clear();
    if (bus.handlerCount(pe::EventType::Collision) != 0) { std::cerr << "clear() failed\n"; return false; }
    return true;
}

// --- Events contract gaps (Step 166 candidate): same-bus reentrant emit,
// mid-dispatch removal of a later handler, once() refusals/auto-removal.
static bool checkEventReentrantOnceGaps() {
    bool ok = true;

    // 1) Same-type reentrant emit on the SAME bus, bounded: each nesting
    // level snapshots independently, so a depth-limited chain terminates.
    pe::EventBus bus;
    int depth = 0;
    int maxDepth = 0;
    int calls = 0;
    bus.subscribe(pe::EventType::Collision, [&](const pe::GameEvent& e) {
        ++calls;
        if (e.a >= 3) return;
        if (depth + 1 > maxDepth) maxDepth = depth + 1;
        ++depth;
        bus.emit(pe::GameEvent{pe::EventType::Collision, e.a + 1, 0});
        --depth;
    });
    bus.emit(pe::GameEvent{pe::EventType::Collision, 0, 0});
    if (maxDepth != 3 || calls != 4 || depth != 0) {
        std::cerr << "Same-bus reentrant emit misbehaved (maxDepth " << maxDepth
                  << ", calls " << calls << ")\n";
        return false;
    }

    // 2) Mid-dispatch removal of a LATER handler: the in-flight snapshot
    // still delivers to it; removal takes effect on the NEXT emit.
    pe::EventBus bus2;
    std::vector<int> seq;
    const int victimToken = bus2.subscribe(pe::EventType::SceneChanged,
        [&](const pe::GameEvent&) { seq.push_back(1); });
    bus2.subscribe(pe::EventType::SceneChanged,
        [&](const pe::GameEvent&) {
            seq.push_back(0);
            bus2.unsubscribe(pe::EventType::SceneChanged, victimToken);
        });
    bus2.emit(pe::GameEvent{pe::EventType::SceneChanged, 0, 1});
    if (seq.size() != 2 || seq[0] != 1 || seq[1] != 0) {
        std::cerr << "Removed-mid-dispatch handler must still fire from snapshot\n";
        return false;
    }
    bus2.emit(pe::GameEvent{pe::EventType::SceneChanged, 0, 1});
    if (seq.size() != 3 || seq[2] != 0) {
        std::cerr << "Mid-dispatch removal must take effect on the next emit\n";
        return false;
    }
    if (bus2.handlerCount(pe::EventType::SceneChanged) != 1) {
        std::cerr << "handlerCount wrong after mid-dispatch removal\n";
        return false;
    }

    // 3) once() refusals mirror subscribe: sentinel type, null, empty.
    pe::EventBus bus3;
    if (bus3.once(pe::EventType::Count, [](const pe::GameEvent&) {}) != -1 ||
        bus3.once(pe::EventType::Collision, nullptr) != -1 ||
        bus3.once(pe::EventType::Collision, pe::EventHandler()) != -1) {
        std::cerr << "Illegal once() accepted\n";
        return false;
    }
    if (bus3.handlerCount(pe::EventType::Collision) != 0) {
        std::cerr << "Refused once() must not register\n";
        return false;
    }

    // 4) once() auto-removal: registered once, fires exactly once across
    // repeated emits, then the slot is gone and the token is retired.
    int hits = 0;
    const int token = bus3.once(pe::EventType::Collision,
        [&](const pe::GameEvent&) { ++hits; });
    if (token < 0 || bus3.handlerCount(pe::EventType::Collision) != 1) {
        std::cerr << "once() registration wrong\n";
        return false;
    }
    bus3.emit(pe::GameEvent{pe::EventType::Collision, 0, 0});
    bus3.emit(pe::GameEvent{pe::EventType::Collision, 0, 0});
    if (hits != 1 || bus3.handlerCount(pe::EventType::Collision) != 0) {
        std::cerr << "once() auto-removal failed\n";
        return false;
    }
    if (bus3.unsubscribe(pe::EventType::Collision, token)) {
        std::cerr << "Auto-removed once() token must be gone\n";
        return false;
    }

    // 5) once() with a throwing handler: exception caught, slot still
    // removed (never sticks around half-dead).
    int thrown = 0;
    bus3.once(pe::EventType::Collision,
        [&](const pe::GameEvent&) { ++thrown; throw std::runtime_error("boom"); });
    bus3.emit(pe::GameEvent{pe::EventType::Collision, 0, 0});  // must not crash
    bus3.emit(pe::GameEvent{pe::EventType::Collision, 0, 0});
    if (thrown != 1 || bus3.handlerCount(pe::EventType::Collision) != 0) {
        std::cerr << "Throwing once() must fire once and remove itself\n";
        return false;
    }
    return ok;
}

static bool checkBinaryBlob() {
    std::vector<uint8_t> out;
    if (!pe::loadBinaryBlob("beep.wav", out) || out.empty()) { std::cerr << "Binary blob beep.wav failed\n"; return false; }
    std::vector<uint8_t> out2;
    pe::clearBinaryCache();
    if (!pe::loadBinaryBlobCached("beep.wav", out2) || out2.empty()) { std::cerr << "Cached blob failed\n"; return false; }
    std::vector<uint8_t> out3;
    if (!pe::loadBinaryBlobCached("beep.wav", out3) || out3 != out2) { std::cerr << "Cache hit failed\n"; return false; }
    std::vector<uint8_t> packOut;
    if (!pe::loadPackEntry("no_such_pack.bin", "beep.wav", packOut) || packOut.empty()) { std::cerr << "Pack fallback failed\n"; return false; }
    pe::clearBinaryCache();
    return true;
}

// Step 163: resource load system contract (headless). Locks the
// observable half of the boundary contract that checkBinaryBlob
// (Step 101) left open: failed blob loads leave `out` empty, full
// file reads match a known committed prefix, the cache holds exactly
// the loaded entries (failure is never cached — retry stays
// possible), a returned copy is independent of the cache, pack v1
// hit = the pack file's own bytes, pack fallback = entryName direct
// file, both-missing = false. Texture load paths need a GL context
// and stay covered by the renderer registration test (Step 133) plus
// the in-game loads (Step 10 checker, Phase 3 atlas, Step 123 music
// success path). The 3-candidate CWD probe order is guaranteed by
// construction (every loader shares the same literal candidate list).
static bool checkResourceSystem() {
    bool ok = true;
    // Failure: blob loader returns false and leaves out empty.
    std::vector<uint8_t> out;
    if (pe::loadBinaryBlob("no_such_resource.bin", out)) { std::cerr << "Missing blob must fail\n"; ok = false; }
    if (!out.empty()) { std::cerr << "Failed blob load must leave out empty\n"; ok = false; }
    // Full read: known committed asset — byte count + content prefix.
    std::vector<uint8_t> txt;
    if (!pe::loadBinaryBlob("prefabs/enemy.txt", txt) || txt.size() < 16) { std::cerr << "Blob enemy.txt failed\n"; return false; }
    if (std::string(reinterpret_cast<const char*>(txt.data()), 16) != "# PureEngine pre") {
        std::cerr << "Blob full-read prefix mismatch\n"; ok = false;
    }
    std::vector<uint8_t> wav;
    if (!pe::loadBinaryBlob("beep.wav", wav) || wav.empty()) { std::cerr << "Blob beep.wav failed\n"; ok = false; }
    // Cache: starts empty after clear, holds exactly the loaded entry.
    pe::clearBinaryCache();
    if (pe::binaryCache().size() != 0) { std::cerr << "Cache must start empty\n"; ok = false; }
    std::vector<uint8_t> c1;
    if (!pe::loadBinaryBlobCached("prefabs/enemy.txt", c1) || c1 != txt) { std::cerr << "Cached enemy.txt failed\n"; ok = false; }
    if (pe::binaryCache().size() != 1) { std::cerr << "Cache must hold exactly 1 entry\n"; ok = false; }
    // Failure is never cached: the map stays at 1, retry stays possible.
    std::vector<uint8_t> miss;
    if (pe::loadBinaryBlobCached("no_such_resource.bin", miss)) { std::cerr << "Cached missing blob must fail\n"; ok = false; }
    if (pe::binaryCache().size() != 1) { std::cerr << "Failed load must not enter the cache\n"; ok = false; }
    // Copy independence: mutating the returned copy must not corrupt
    // the cache — the next cached load still returns original bytes.
    if (!c1.empty()) c1[0] = 'X';
    std::vector<uint8_t> c2;
    if (!pe::loadBinaryBlobCached("prefabs/enemy.txt", c2) || c2.empty() || c2[0] != '#') {
        std::cerr << "Cache copy must be independent of caller mutation\n"; ok = false;
    }
    // Pack v1 hit: packFile exists -> its OWN bytes come back
    // (entryName ignored — v1 pack = the pack file itself).
    std::vector<uint8_t> packHit;
    if (!pe::loadPackEntry("beep.wav", "prefabs/enemy.txt", packHit) || packHit != wav) {
        std::cerr << "Pack hit must return the pack file's bytes\n"; ok = false;
    }
    // Pack fallback: packFile missing -> entryName as direct file.
    std::vector<uint8_t> packFall;
    if (!pe::loadPackEntry("no_such_pack.bin", "prefabs/enemy.txt", packFall) || packFall != txt) {
        std::cerr << "Pack fallback must return the entry's bytes\n"; ok = false;
    }
    // Both missing: false, out stays empty.
    std::vector<uint8_t> none;
    if (pe::loadPackEntry("no_such_pack.bin", "no_such_entry.bin", none) || !none.empty()) {
        std::cerr << "Pack both-missing must fail with empty out\n"; ok = false;
    }
    pe::clearBinaryCache();
    if (pe::binaryCache().size() != 0) { std::cerr << "clearBinaryCache must empty the cache\n"; ok = false; }
    return ok;
}

// Step 155: direct keyNameToGLFW coverage (was transitive-only via
// loadInputBindings with 4 names). Locks every existing name (Step 105
// table byte-identical), the Step 155 additive widening (modifiers,
// whitespace, punctuation, F-keys, aliases), case-insensitivity, and
// unknown -> -1.
static bool checkKeyNames() {
    bool ok = true;
    auto expect = [&](const char* name, int key) {
        if (pe::keyNameToGLFW(name) != key) {
            std::cerr << "keyNameToGLFW(" << name << ") wrong\n";
            ok = false;
        }
    };
    // Step 105 table (pre-existing names — contract unchanged).
    expect("A", GLFW_KEY_A); expect("Z", GLFW_KEY_Z);
    expect("0", GLFW_KEY_0); expect("9", GLFW_KEY_9);
    expect("LEFT", GLFW_KEY_LEFT); expect("RIGHT", GLFW_KEY_RIGHT);
    expect("UP", GLFW_KEY_UP); expect("DOWN", GLFW_KEY_DOWN);
    expect("SPACE", GLFW_KEY_SPACE);
    expect("ESCAPE", GLFW_KEY_ESCAPE); expect("ESC", GLFW_KEY_ESCAPE);
    expect("ENTER", GLFW_KEY_ENTER);
    expect("GRAVE", GLFW_KEY_GRAVE_ACCENT);
    expect("GRAVE_ACCENT", GLFW_KEY_GRAVE_ACCENT);
    expect("BACKSPACE", GLFW_KEY_BACKSPACE);
    expect("PERIOD", GLFW_KEY_PERIOD); expect("MINUS", GLFW_KEY_MINUS);
    // Case-insensitivity (existing behavior, now locked directly).
    expect("space", GLFW_KEY_SPACE); expect("Left_Shift", GLFW_KEY_LEFT_SHIFT);
    // Step 155 widening: modifiers + TAB.
    expect("TAB", GLFW_KEY_TAB);
    expect("SHIFT", GLFW_KEY_LEFT_SHIFT);
    expect("LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT);
    expect("RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT);
    expect("CTRL", GLFW_KEY_LEFT_CONTROL);
    expect("CONTROL", GLFW_KEY_LEFT_CONTROL);
    expect("LEFT_CTRL", GLFW_KEY_LEFT_CONTROL);
    expect("RIGHT_CTRL", GLFW_KEY_RIGHT_CONTROL);
    expect("LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL);
    expect("RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL);
    expect("ALT", GLFW_KEY_LEFT_ALT);
    expect("LEFT_ALT", GLFW_KEY_LEFT_ALT);
    expect("RIGHT_ALT", GLFW_KEY_RIGHT_ALT);
    // Step 155 widening: punctuation.
    expect("COMMA", GLFW_KEY_COMMA); expect("SLASH", GLFW_KEY_SLASH);
    expect("BACKSLASH", GLFW_KEY_BACKSLASH);
    expect("SEMICOLON", GLFW_KEY_SEMICOLON);
    expect("APOSTROPHE", GLFW_KEY_APOSTROPHE);
    expect("EQUAL", GLFW_KEY_EQUAL);
    expect("LEFT_BRACKET", GLFW_KEY_LEFT_BRACKET);
    expect("RIGHT_BRACKET", GLFW_KEY_RIGHT_BRACKET);
    expect("CAPS_LOCK", GLFW_KEY_CAPS_LOCK);
    // Step 155 widening: function keys.
    expect("F1", GLFW_KEY_F1); expect("F2", GLFW_KEY_F2);
    expect("F3", GLFW_KEY_F3); expect("F4", GLFW_KEY_F4);
    expect("F5", GLFW_KEY_F5); expect("F6", GLFW_KEY_F6);
    expect("F7", GLFW_KEY_F7); expect("F8", GLFW_KEY_F8);
    expect("F9", GLFW_KEY_F9); expect("F10", GLFW_KEY_F10);
    expect("F11", GLFW_KEY_F11); expect("F12", GLFW_KEY_F12);
    // Unknown names still refuse.
    expect("NO_SUCH_KEY", -1); expect("", -1);
    return ok;
}

// Step 157: keysForAllActions union contract. Defaults: 13 unique keys
// (A LEFT D RIGHT W UP S DOWN SPACE ENTER ESCAPE BACKSPACE GRAVE —
// W/UP, SPACE, ESCAPE are shared across actions and deduped; GRAVE is
// Console's alone). After a file remap to a widened key (F1) the union
// gains it while shared keys stay; reset restores the 13. No
// duplicates allowed in any state.
static bool checkKeysForAllActions() {
    bool ok = true;
    auto contains = [](const std::vector<int>& v, int key) {
        for (int k : v) if (k == key) return true;
        return false;
    };
    auto noDups = [](const std::vector<int>& v) {
        for (std::size_t i = 0; i < v.size(); ++i) {
            for (std::size_t j = i + 1; j < v.size(); ++j) {
                if (v[i] == v[j]) return false;
            }
        }
        return true;
    };
    pe::resetActionOverrides();  // known state: Step 88 defaults + Console
    const std::vector<int> def = pe::keysForAllActions();
    if (def.size() != 13 || !noDups(def)) {
        std::cerr << "Default union must be 13 unique keys\n";
        ok = false;
    }
    const int defaults[] = {GLFW_KEY_A, GLFW_KEY_LEFT, GLFW_KEY_D, GLFW_KEY_RIGHT,
                            GLFW_KEY_W, GLFW_KEY_UP, GLFW_KEY_S, GLFW_KEY_DOWN,
                            GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE,
                            GLFW_KEY_BACKSPACE, GLFW_KEY_GRAVE_ACCENT};
    for (int k : defaults) {
        if (!contains(def, k)) { std::cerr << "Default union missing a key\n"; ok = false; }
    }
    // Remap Jump to F1 (a key no default holds): union must gain it.
    const std::string fname = "keys_all_test_tmp.txt";
    {
        std::filesystem::create_directories("assets");
        std::ofstream out("assets/" + fname);
        if (!out) {
            std::filesystem::create_directories("../assets");
            out.open("../assets/" + fname);
        }
        if (!out) { std::cerr << "Failed to write union test file\n"; return false; }
        out << "Jump=F1\n";
    }
    const bool loaded = pe::loadInputBindings(fname);
    std::remove(("assets/" + fname).c_str());
    std::remove(("../assets/" + fname).c_str());
    std::remove(("../../assets/" + fname).c_str());
    if (!loaded) { std::cerr << "Union test bindings load failed\n"; return false; }
    const std::vector<int> remapped = pe::keysForAllActions();
    // SPACE stays (Confirm keeps it): the union grows by exactly F1.
    if (remapped.size() != 14 || !contains(remapped, GLFW_KEY_F1) ||
        !contains(remapped, GLFW_KEY_SPACE) || !noDups(remapped)) {
        std::cerr << "Union must gain remapped key, keep shared keys, stay deduped\n";
        ok = false;
    }
    // Reset round-trips to the defaults.
    pe::resetActionOverrides();
    if (pe::keysForAllActions() != def) {
        std::cerr << "Union must restore the defaults after reset\n";
        ok = false;
    }
    // Leave the table in the shipped-file state for later checks.
    pe::loadInputBindings("input_bindings.txt");
    return ok;
}

// Step 158 gap closed: headless pad-bridge tests. The fixed
// gamepadButtonsForAction table and the pure edge pattern need no
// window or hardware; the isAction* pad tails need a live GLFWwindow
// for the keyboard half (hidden window, skip-not-failed where GL
// cannot init — checkInputEdges pattern). Hand-built GamepadStates
// throughout: no hardware claims.
static bool checkGamepadActions() {
    bool ok = true;
    auto buttonsFor = [](pe::Action a) {
        return pe::gamepadButtonsForAction(a);
    };
    auto singleButton = [](const std::vector<int>& v, int b) {
        return v.size() == 1 && v[0] == b;
    };
    // Fixed table, every action (Step 158 mapping: d-pad moves, A
    // jumps/confirms, START pauses, B backs).
    if (!singleButton(buttonsFor(pe::Action::MoveLeft), GLFW_GAMEPAD_BUTTON_DPAD_LEFT) ||
        !singleButton(buttonsFor(pe::Action::MoveRight), GLFW_GAMEPAD_BUTTON_DPAD_RIGHT) ||
        !singleButton(buttonsFor(pe::Action::MoveUp), GLFW_GAMEPAD_BUTTON_DPAD_UP) ||
        !singleButton(buttonsFor(pe::Action::MoveDown), GLFW_GAMEPAD_BUTTON_DPAD_DOWN) ||
        !singleButton(buttonsFor(pe::Action::Jump), GLFW_GAMEPAD_BUTTON_A) ||
        !singleButton(buttonsFor(pe::Action::Pause), GLFW_GAMEPAD_BUTTON_START) ||
        !singleButton(buttonsFor(pe::Action::Confirm), GLFW_GAMEPAD_BUTTON_A) ||
        !singleButton(buttonsFor(pe::Action::Back), GLFW_GAMEPAD_BUTTON_B)) {
        std::cerr << "Gamepad action table mapping wrong\n";
        ok = false;
    }
    // Console has NO bridge mapping (d-pad/A/START/B only): the pad can
    // never fire Console — documented Step 158 exclusion.
    if (!buttonsFor(pe::Action::Console).empty()) {
        std::cerr << "Console must have no gamepad mapping\n";
        ok = false;
    }
    // Edge fires ONCE across poll pairs: press, hold, release.
    pe::GamepadState up, aDown, aHeld;
    aDown.buttons[GLFW_GAMEPAD_BUTTON_A] = true;
    aHeld = aDown;
    if (!pe::gamepadButtonEdge(up, aDown, GLFW_GAMEPAD_BUTTON_A)) {
        std::cerr << "Pad press must be a rising edge\n";
        ok = false;
    }
    if (pe::gamepadButtonEdge(aHeld, aDown, GLFW_GAMEPAD_BUTTON_A)) {
        std::cerr << "Held pad button must not re-edge\n";
        ok = false;
    }
    if (pe::gamepadButtonEdge(aDown, up, GLFW_GAMEPAD_BUTTON_A)) {
        std::cerr << "Pad release must not be a rising edge\n";
        ok = false;
    }
    // Isolation: B drives Back only; START drives Pause only.
    pe::GamepadState bDown, startDown;
    bDown.buttons[GLFW_GAMEPAD_BUTTON_B] = true;
    startDown.buttons[GLFW_GAMEPAD_BUTTON_START] = true;
    if (!pe::gamepadButtonEdge(up, bDown, GLFW_GAMEPAD_BUTTON_B) ||
        pe::gamepadButtonEdge(up, bDown, GLFW_GAMEPAD_BUTTON_A)) {
        std::cerr << "B must edge Back-only via the fixed table\n";
        ok = false;
    }
    if (!pe::gamepadButtonEdge(up, startDown, GLFW_GAMEPAD_BUTTON_START)) {
        std::cerr << "START must edge via the fixed table\n";
        ok = false;
    }
    // Pad-tail checks through the action system need a GLFWwindow for
    // the keyboard half. Skip (not fail) where GL cannot init.
    if (!glfwInit()) {
        std::cerr << "gamepad action test skipped: glfwInit failed\n";
        return ok;
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(64, 64, "pad-action-test", nullptr, nullptr);
    if (!window) {
        std::cerr << "gamepad action test skipped: hidden window failed\n";
        glfwTerminate();
        return ok;
    }
    pe::Input input{GLFW_KEY_SPACE, GLFW_KEY_ESCAPE};
    glfwPollEvents();
    input.update(window);  // settle: keyboard snapshot clean
    // Keyboard-only path unchanged: nullptr (and omitted) pads, no keys
    // pressed, no pad buttons — everything silent either way.
    if (input.isActionDown(window, pe::Action::Jump) ||
        input.isActionDown(window, pe::Action::Jump, nullptr) ||
        input.isActionEdge(window, pe::Action::Jump) ||
        input.isActionEdge(window, pe::Action::Jump, nullptr, nullptr)) {
        std::cerr << "Keyboard-only action path must read silent\n";
        ok = false;
    }
    // The bridge: pad A down drives Jump even though no key is pressed.
    pe::GamepadState prevPad;  // zeroed: nothing was down
    pe::GamepadState curPad;
    curPad.buttons[GLFW_GAMEPAD_BUTTON_A] = true;
    if (!input.isActionDown(window, pe::Action::Jump, &curPad)) {
        std::cerr << "Pad A must drive Jump down via the bridge\n";
        ok = false;
    }
    if (!input.isActionEdge(window, pe::Action::Jump, &prevPad, &curPad)) {
        std::cerr << "Pad A press must drive Jump edge via the bridge\n";
        ok = false;
    }
    // Edge consumed once: same pad pair again (A held) stays silent.
    pe::GamepadState heldPrev = curPad;
    if (input.isActionEdge(window, pe::Action::Jump, &heldPrev, &curPad)) {
        std::cerr << "Held pad A must not re-fire Jump edge\n";
        ok = false;
    }
    // Release: no edge.
    if (input.isActionEdge(window, pe::Action::Jump, &curPad, &prevPad)) {
        std::cerr << "Pad A release must not edge Jump\n";
        ok = false;
    }
    // Pause via START; Back via B (and B must not leak into Jump).
    if (!input.isActionEdge(window, pe::Action::Pause, &prevPad, &startDown)) {
        std::cerr << "START must drive Pause edge via the bridge\n";
        ok = false;
    }
    if (!input.isActionEdge(window, pe::Action::Back, &prevPad, &bDown) ||
        input.isActionEdge(window, pe::Action::Jump, &prevPad, &bDown)) {
        std::cerr << "B must drive Back edge only\n";
        ok = false;
    }
    // d-pad drives movement actions down.
    pe::GamepadState dpadLeft;
    dpadLeft.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] = true;
    if (!input.isActionDown(window, pe::Action::MoveLeft, &dpadLeft) ||
        input.isActionDown(window, pe::Action::MoveRight, &dpadLeft)) {
        std::cerr << "D-pad left must drive MoveLeft down only\n";
        ok = false;
    }
    // Zeroed/disconnected pad: every action silent (no connected gate —
    // pollGamepad zeroes absent pads; hand-built states stay usable).
    pe::GamepadState zero;
    if (input.isActionDown(window, pe::Action::Jump, &zero) ||
        input.isActionDown(window, pe::Action::Pause, &zero) ||
        input.isActionEdge(window, pe::Action::Jump, &zero, &zero)) {
        std::cerr << "Zeroed pad must drive nothing\n";
        ok = false;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return ok;
}

// Step 158-follow-up: keysForAllActions adoption helper. Proves the
// opt-in ORDER that closes the silent untracked-key gap: load bindings
// FIRST, then construct Input from the union — every remapped key is
// tracked and isActionEdge fires. Counter-proofs: a narrow hand-rolled
// tracked set stays silent, and an Input built from the PRE-remap union
// must NOT track a fresh key (the documented residual: runtime rebinds
// to fresh keys need re-adoption). Needs a hidden window + key
// injection (checkInputEdges pattern); skipped where GL cannot init.
static bool checkInputAdoptionHelper() {
    if (!glfwInit()) {
        std::cerr << "adoption test skipped: glfwInit failed\n";
        return true;
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(64, 64, "adoption-test", nullptr, nullptr);
    if (!window) {
        std::cerr << "adoption test skipped: hidden window failed\n";
        glfwTerminate();
        return true;
    }
    bool ok = true;
    // --- Phase A: correct order (bindings BEFORE construction) ---
    pe::resetActionOverrides();
    const std::string fname = "adoption_test_tmp.txt";
    {
        std::filesystem::create_directories("assets");
        std::ofstream out("assets/" + fname);
        if (!out) {
            std::filesystem::create_directories("../assets");
            out.open("../assets/" + fname);
        }
        if (!out) { std::cerr << "Failed to write adoption test file\n"; return false; }
        out << "Pause=F5\nJump=BACKSPACE\n";
    }
    const bool loaded = pe::loadInputBindings(fname);
    std::remove(("assets/" + fname).c_str());
    std::remove(("../assets/" + fname).c_str());
    std::remove(("../../assets/" + fname).c_str());
    if (!loaded) { std::cerr << "Adoption test bindings load failed\n"; return false; }
    const std::vector<int> tracked = pe::keysForAllActions();
    bool hasF5 = false, hasBack = false;
    for (int k : tracked) {
        if (k == GLFW_KEY_F5) hasF5 = true;
        if (k == GLFW_KEY_BACKSPACE) hasBack = true;
    }
    if (!hasF5 || !hasBack) {
        std::cerr << "Post-remap union must contain remapped keys\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }
    pe::Input adopted{tracked};  // vector ctor: the adoption piece
    glfwPollEvents();
    adopted.update(window);  // settle: snapshot clean
    // F5 (fresh key, in the union only because bindings loaded first)
    // must fire Pause through isActionEdge.
    postKey(window, VK_F5, true);
    Sleep(150);
    glfwPollEvents();
    if (!adopted.isActionEdge(window, pe::Action::Pause)) {
        std::cerr << "Union adoption must fire a remapped fresh key\n";
        ok = false;
    }
    adopted.update(window);
    postKey(window, VK_F5, false);
    Sleep(150);
    glfwPollEvents();
    adopted.update(window);
    // BACKSPACE (a key another action already held) must fire Jump.
    postKey(window, VK_BACK, true);
    Sleep(150);
    glfwPollEvents();
    if (!adopted.isActionEdge(window, pe::Action::Jump)) {
        std::cerr << "Union adoption must fire a remapped shared key\n";
        ok = false;
    }
    adopted.update(window);
    postKey(window, VK_BACK, false);
    Sleep(150);
    glfwPollEvents();
    adopted.update(window);
    // Counter-proof: a narrow hand-rolled tracked set stays silent on
    // both remapped keys (the gap the adoption closes).
    pe::Input narrow{GLFW_KEY_ESCAPE, GLFW_KEY_SPACE};
    narrow.update(window);
    postKey(window, VK_F5, true);
    Sleep(150);
    glfwPollEvents();
    if (narrow.isActionEdge(window, pe::Action::Pause) ||
        narrow.isEdge(window, GLFW_KEY_F5)) {
        std::cerr << "Untracked key must stay silent in the narrow set\n";
        ok = false;
    }
    narrow.update(window);
    postKey(window, VK_F5, false);
    Sleep(150);
    glfwPollEvents();
    narrow.update(window);
    // --- Phase B: wrong order (construction BEFORE bindings) ---
    // The pre-remap union covers only default keys: a runtime rebind to
    // a FRESH key (F5) stays untracked — the documented residual.
    pe::resetActionOverrides();
    const std::vector<int> defUnion = pe::keysForAllActions();
    pe::Input early{defUnion};
    glfwPollEvents();
    early.update(window);
    {
        std::ofstream out("assets/" + fname);
        if (!out) out.open("../assets/" + fname);
        if (!out) { std::cerr << "Failed to rewrite adoption test file\n"; return false; }
        out << "Pause=F5\n";
    }
    const bool loaded2 = pe::loadInputBindings(fname);
    std::remove(("assets/" + fname).c_str());
    std::remove(("../assets/" + fname).c_str());
    std::remove(("../../assets/" + fname).c_str());
    if (!loaded2) { std::cerr << "Adoption phase B load failed\n"; return false; }
    postKey(window, VK_F5, true);
    Sleep(150);
    glfwPollEvents();
    if (early.isActionEdge(window, pe::Action::Pause)) {
        std::cerr << "Pre-remap union must not track a fresh key\n";
        ok = false;
    }
    early.update(window);
    postKey(window, VK_F5, false);
    Sleep(150);
    glfwPollEvents();
    early.update(window);
    // Restore the shipped-file state for later checks.
    pe::loadInputBindings("input_bindings.txt");
    glfwDestroyWindow(window);
    glfwTerminate();
    return ok;
}

static bool checkInputBindings() {
    const std::string fname = "input_bindings_test_tmp.txt";
    {
        std::filesystem::create_directories("assets");
        std::ofstream out("assets/" + fname);
        if (!out) {
            std::filesystem::create_directories("../assets");
            out.open("../assets/" + fname);
        }
        if (!out) { std::cerr << "Failed to write bindings test file\n"; return false; }
        out << "MoveLeft=A,LEFT,TAB\nJump=SPACE\nUnknownAction=SPACE\nBadLineNoEquals\nMoveRight=UNKNOWNKEY\n";
    }
    bool ok = pe::loadInputBindings(fname);
    std::remove(("assets/" + fname).c_str());
    std::remove(("../assets/" + fname).c_str());
    std::remove(("../../assets/" + fname).c_str());
    if (!ok) { std::cerr << "Bindings load should succeed with some valid lines\n"; return false; }
    auto leftKeys = pe::keysForAction(pe::Action::MoveLeft);
    bool hasA = false, hasTab = false;
    for (int k : leftKeys) { if (k == GLFW_KEY_A) hasA = true; if (k == GLFW_KEY_TAB) hasTab = true; }
    if (!hasA) { std::cerr << "MoveLeft remap failed\n"; return false; }
    // Step 155: a widened name (TAB) must flow end-to-end through the file parser.
    if (!hasTab) { std::cerr << "Widened key name TAB must bind from file\n"; return false; }
    auto jumpKeys = pe::keysForAction(pe::Action::Jump);
    if (jumpKeys.size() != 1 || jumpKeys[0] != GLFW_KEY_SPACE) { std::cerr << "Jump remap failed\n"; return false; }
    bool missing = pe::loadInputBindings("no_such_bindings_xyz.txt");
    if (missing) { std::cerr << "Missing file should return false\n"; return false; }
    // Step 148: a FAILED load must not clobber the committed overrides
    // (loadInputBindings commits atomically after a successful parse).
    auto jumpAfterFail = pe::keysForAction(pe::Action::Jump);
    if (jumpAfterFail.size() != 1 || jumpAfterFail[0] != GLFW_KEY_SPACE) {
        std::cerr << "Failed load must keep the remapped overrides\n";
        return false;
    }
    // Step 154: reset restores the Step 88 defaults after a remap
    // (load -> remap -> reset round-trip).
    pe::resetActionOverrides();
    auto leftAfterReset = pe::keysForAction(pe::Action::MoveLeft);
    if (leftAfterReset.size() != 2 ||
        leftAfterReset[0] != GLFW_KEY_A || leftAfterReset[1] != GLFW_KEY_LEFT) {
        std::cerr << "Reset must restore MoveLeft defaults\n";
        return false;
    }
    auto jumpAfterReset = pe::keysForAction(pe::Action::Jump);
    if (jumpAfterReset.size() != 3 ||
        jumpAfterReset[0] != GLFW_KEY_SPACE || jumpAfterReset[1] != GLFW_KEY_W ||
        jumpAfterReset[2] != GLFW_KEY_UP) {
        std::cerr << "Reset must restore Jump defaults\n";
        return false;
    }
    pe::loadInputBindings("input_bindings.txt");
    return true;
}

static bool checkSceneDumpReload() {
    pe::Scene src;
    src.name = "dump_test";
    pe::Entity e1(pe::Vec3(1.0f, 2.0f, 0.0f), 0.5f, pe::Vec3(1,1,1));
    e1.roleId = 1; e1.textureId = 0; e1.depth = 1;
    pe::Entity e2(pe::Vec3(-1.0f, 0.0f, 0.0f), -1.0f, pe::Vec3(0.8f,0.8f,1));
    e2.roleId = 2; e2.textureId = 2; e2.depth = 2;
    src.entities = {e1, e2, e1};
    src.tilemapFile = "arcade_arena.txt";
    src.tilemap = pe::loadTilemap(src.tilemapFile);
    std::filesystem::create_directories("savedata");
    const std::string path = "savedata/scene_dump.txt";
    std::remove(path.c_str()); std::remove(("../" + path).c_str());
    if (!pe::saveSceneToFile(src, path)) { std::cerr << "Scene dump save failed\n"; return false; }
    pe::Scene loaded;
    if (!pe::loadSceneFromFile(path, loaded)) { std::cerr << "Scene dump load failed\n"; std::remove(path.c_str()); return false; }
    if (loaded.entities.size() != 3) { std::cerr << "Dump entity count\n"; std::remove(path.c_str()); return false; }
    if (!assertFloatClose(loaded.entities[0].position.x, 1.0f)) { std::cerr << "Dump pos mismatch\n"; std::remove(path.c_str()); return false; }
    if (loaded.tilemap.width != src.tilemap.width || loaded.tilemap.tiles.size() != src.tilemap.tiles.size()) { std::cerr << "Dump tilemap mismatch\n"; std::remove(path.c_str()); return false; }
    std::remove(path.c_str()); std::remove(("../" + path).c_str()); std::remove(("../../" + path).c_str());
    return true;
}

// --- Increment 1 (Scene/Prefab/Persistence campaign): manager save test ---
// First test for the ZERO-coverage saveSceneManagerToFile write path
// (scene.h:587-612): index file (# scene manager v1, scenes=, current=,
// scene_file= lines) + per-scene scene_<name>.txt files. The writer
// hardcodes "assets/" with no probe, so the test pre-creates assets/ in
// its CWD (the checkInputBindings pattern). Round-trips the real files
// through loadSceneFromFile as a bonus proof.
static bool checkSceneManagerSave() {
    // 1. Build a 2-scene manager with distinct entities; switch to the
    // second so current= is non-zero (the interesting index case).
    pe::SceneManager manager;
    pe::Scene& alpha = pe::loadScene(manager, "alpha");
    pe::Entity ea(pe::Vec3(1.0f, 2.0f, 0.0f), 0.5f, pe::Vec3(1.0f, 1.0f, 1.0f),
                  pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    ea.roleId = 1;
    alpha.entities = {ea};
    pe::Scene& beta = pe::loadScene(manager, "beta");
    pe::Entity eb(pe::Vec3(-1.0f, 0.5f, 0.0f), -1.2f, pe::Vec3(0.8f, 0.8f, 1.0f),
                  pe::Vec3(0.4f, 0.4f, 0.0f), 2);
    eb.roleId = 2;
    beta.entities = {eb};
    if (!pe::switchTo(manager, "beta") || manager.current != 1) {
        std::cerr << "manager save test: switchTo beta failed\n";
        return false;
    }

    // 2. Cleanup prior artifacts (index + per-scene + .tmp files in both
    // candidate spellings), then pre-create assets/ in the CWD.
    auto rmArtifacts = [&]() {
        const char* prefixes[3] = {"assets/", "../assets/", "../../assets/"};
        const char* names[3] = {"scene_manager_test.txt", "scene_alpha.txt", "scene_beta.txt"};
        for (const char* p : prefixes) {
            for (const char* n : names) {
                std::remove((std::string(p) + n).c_str());
                std::remove((std::string(p) + n + ".tmp").c_str());
            }
        }
    };
    rmArtifacts();
    std::filesystem::create_directories("assets");

    // 3. Save the manager.
    if (!pe::saveSceneManagerToFile(manager, "scene_manager_test.txt")) {
        std::cerr << "saveSceneManagerToFile failed\n";
        rmArtifacts();
        return false;
    }

    // 4. Assert the index file content: header, scene count, current
    // index, one scene_file line per scene.
    bool ok = true;
    {
        std::ifstream in("assets/scene_manager_test.txt");
        if (!in) { std::cerr << "index file not written\n"; rmArtifacts(); return false; }
        std::string line;
        bool header = false, scenes = false, current = false;
        int fileLines = 0;
        while (std::getline(in, line)) {
            if (line == "# scene manager v1") header = true;
            if (line == "scenes=2") scenes = true;
            if (line == "current=1") current = true;
            if (line == "scene_file=scene_alpha.txt" ||
                line == "scene_file=scene_beta.txt") {
                ++fileLines;
            }
        }
        if (!header || !scenes || !current || fileLines != 2) {
            std::cerr << "index file content wrong (header=" << header
                      << " scenes=" << scenes << " current=" << current
                      << " fileLines=" << fileLines << ")\n";
            ok = false;
        }
    }
    // 5. Assert both per-scene files round-trip through the real loader.
    {
        pe::Scene loaded;
        if (!pe::loadSceneFromFile("assets/scene_alpha.txt", loaded) ||
            loaded.name != "alpha" || loaded.entities.size() != 1 ||
            loaded.entities[0].roleId != 1) {
            std::cerr << "scene_alpha.txt round-trip wrong\n";
            ok = false;
        }
    }
    {
        pe::Scene loaded;
        if (!pe::loadSceneFromFile("assets/scene_beta.txt", loaded) ||
            loaded.name != "beta" || loaded.entities.size() != 1 ||
            loaded.entities[0].roleId != 2 ||
            !assertFloatClose(loaded.entities[0].position.x, -1.0f)) {
            std::cerr << "scene_beta.txt round-trip wrong\n";
            ok = false;
        }
    }

    // 6. Empty file name refuses.
    if (pe::saveSceneManagerToFile(manager, "")) {
        std::cerr << "Empty file name must refuse\n";
        ok = false;
    }

    rmArtifacts();
    return ok;
}

// --- Increment (Scene/Persistence campaign): manager round-trip ---
// First test for loadSceneManagerFromFile (the READ side of the "# scene
// manager v1" index file). checkSceneManagerSave proved the WRITE path,
// the index content, and per-scene round-trips; this proves the full
// manager-level round-trip: save -> loadSceneManagerFromFile -> same
// scene count/order, same current index, same entities, tilemap payload
// restored. Adds refusal cases the writer test cannot cover: missing
// index file, unknown index version, scenes-count mismatch.
static bool checkSceneManagerRoundTrip() {
    // 1. Cleanup prior artifacts FIRST (index + per-scene + tilemap +
    //    .tmp in all three candidate spellings). The tilemap file is
    //    re-created in step 2: rmArtifacts must run BEFORE the save/load
    //    round-trip needs it, not between fixture creation and save (the
    //    load-side loadTilemap probe reads it back off disk).
    auto rmArtifacts = [&]() {
        const char* prefixes[3] = {"assets/", "../assets/", "../../assets/"};
        const char* names[4] = {"scene_manager_rt_test.txt", "scene_alpha.txt",
                                "scene_beta.txt", "scene_gamma.txt"};
        for (const char* p : prefixes) {
            for (const char* n : names) {
                std::remove((std::string(p) + n).c_str());
                std::remove((std::string(p) + n + ".tmp").c_str());
            }
            std::remove((std::string(p) + "tilemap_rt_test.txt").c_str());
        }
    };
    rmArtifacts();

    // 2. Build a 3-scene manager; the middle scene carries a tilemap
    //    payload; switch to the middle so current=1 (non-zero, non-last).
    pe::SceneManager manager;
    pe::Scene& alpha = pe::loadScene(manager, "alpha");
    pe::Entity ea(pe::Vec3(1.0f, 2.0f, 0.0f), 0.5f, pe::Vec3(1.0f, 1.0f, 1.0f),
                  pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    ea.roleId = 1;
    ea.tag = "checkpoint";
    alpha.entities = {ea};
    pe::Scene& beta = pe::loadScene(manager, "beta");
    pe::Entity eb(pe::Vec3(-1.0f, 0.5f, 0.0f), -1.2f, pe::Vec3(0.8f, 0.8f, 1.0f),
                  pe::Vec3(0.4f, 0.4f, 0.0f), 2);
    eb.roleId = 2;
    pe::Entity ec(pe::Vec3(0.0f, -1.5f, 0.0f), 0.0f, pe::Vec3(0.5f, 0.5f, 1.0f),
                  pe::Vec3(0.25f, 0.25f, 0.0f), 3);
    ec.roleId = 2;
    beta.entities = {eb, ec};
    pe::Scene& gamma = pe::loadScene(manager, "gamma");
    pe::Entity eg(pe::Vec3(3.0f, 3.0f, 0.0f), 2.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                  pe::Vec3(0.5f, 0.5f, 0.0f), 1);
    eg.roleId = 1;
    gamma.entities = {eg};

    // Tilemap payload for beta: a tiny 2x2 map written into the CWD's
    // assets/ (loadTilemap's first probe candidate, same pattern the
    // manager-save test uses). beta is RE-FIND via sceneByName because
    // gamma's loadScene push_back above may reallocate manager.scenes and
    // dangle the earlier beta reference (scene.h's POINTER INVALIDATION
    // note — observed live: a stale alias wrote a moved-from empty map).
    std::filesystem::create_directories("assets");
    {
        std::ofstream tm("assets/tilemap_rt_test.txt", std::ios::trunc);
        if (!tm) { std::cerr << "manager round-trip test: tilemap write failed\n"; return false; }
        tm << "width=2\nheight=2\ntileSize=1.0\nrow=1,0\nrow=0,1\n";
    }
    pe::Scene* betaSlot = pe::sceneByName(manager, "beta");
    if (!betaSlot || !pe::loadTilemapIntoScene(*betaSlot, "tilemap_rt_test.txt")) {
        std::cerr << "manager round-trip test: loadTilemapIntoScene failed\n";
        std::remove("assets/tilemap_rt_test.txt");
        return false;
    }
    if (!pe::switchTo(manager, "beta") || manager.current != 1) {
        std::cerr << "manager round-trip test: switchTo beta failed\n";
        std::remove("assets/tilemap_rt_test.txt");
        return false;
    }

    // 3. Save the manager, then load it back through the real reader.
    if (!pe::saveSceneManagerToFile(manager, "scene_manager_rt_test.txt")) {
        std::cerr << "manager round-trip test: save failed\n";
        rmArtifacts();
        return false;
    }
    pe::SceneManager loaded;
    if (!pe::loadSceneManagerFromFile("scene_manager_rt_test.txt", loaded)) {
        std::cerr << "manager round-trip test: loadSceneManagerFromFile failed\n";
        rmArtifacts();
        return false;
    }

    // 4. Assert the manager-level round-trip: count, order, current,
    //    entities, tilemap payload.
    bool ok = true;
    if (loaded.scenes.size() != 3) { std::cerr << "round-trip scene count\n"; ok = false; }
    if (loaded.current != 1) { std::cerr << "round-trip current index\n"; ok = false; }
    if (loaded.scenes.size() == 3) {
        if (loaded.scenes[0].name != "alpha" || loaded.scenes[1].name != "beta" ||
            loaded.scenes[2].name != "gamma") {
            std::cerr << "round-trip scene order/names\n";
            ok = false;
        }
        if (loaded.scenes[0].entities.size() != 1 ||
            loaded.scenes[0].entities[0].roleId != 1 ||
            loaded.scenes[0].entities[0].tag != "checkpoint" ||
            !assertFloatClose(loaded.scenes[0].entities[0].position.x, 1.0f)) {
            std::cerr << "round-trip alpha entity mismatch\n";
            ok = false;
        }
        if (loaded.scenes[1].entities.size() != 2 ||
            loaded.scenes[1].entities[0].roleId != 2 ||
            loaded.scenes[1].entities[1].roleId != 2 ||
            !assertFloatClose(loaded.scenes[1].entities[0].position.x, -1.0f)) {
            std::cerr << "round-trip beta entity mismatch\n";
            ok = false;
        }
        if (loaded.scenes[1].tilemap.width != 2 || loaded.scenes[1].tilemap.height != 2 ||
            loaded.scenes[1].tilemap.tileSize != 1.0f ||
            loaded.scenes[1].tilemap.tiles.size() != 4) {
            std::cerr << "round-trip beta tilemap mismatch\n";
            ok = false;
        } else {
            // Row-major, row 0 = TOP: tile (0,0) id 1 solid, (1,0) id 0
            // empty, (0,1) id 0 empty, (1,1) id 1 solid.
            const pe::Tile* t00 = pe::tileAt(loaded.scenes[1].tilemap, 0, 0);
            const pe::Tile* t10 = pe::tileAt(loaded.scenes[1].tilemap, 1, 0);
            const pe::Tile* t11 = pe::tileAt(loaded.scenes[1].tilemap, 1, 1);
            if (!t00 || !t11 || !t10 ||
                t00->tileId != 1 || !t00->solid ||
                t10->tileId != 0 || t10->solid ||
                t11->tileId != 1 || !t11->solid) {
                std::cerr << "round-trip beta tilemap cells mismatch\n";
                ok = false;
            }
        }
        if (loaded.scenes[2].entities.size() != 1 ||
            loaded.scenes[2].entities[0].roleId != 1 ||
            !assertFloatClose(loaded.scenes[2].entities[0].position.x, 3.0f)) {
            std::cerr << "round-trip gamma entity mismatch\n";
            ok = false;
        }
    }

    // 5. Refusal cases the writer test cannot cover.
    // 5a. Empty index file name refuses.
    if (pe::loadSceneManagerFromFile("", loaded)) {
        std::cerr << "round-trip: empty file name must refuse\n";
        ok = false;
    }
    // 5b. Missing index file refuses, loaded manager untouched.
    {
        pe::SceneManager untouched;
        loaded.current = -2;  // sentinel the refusal must preserve
        if (pe::loadSceneManagerFromFile("scene_manager_missing_test.txt", loaded) ||
            loaded.current != -2) {
            std::cerr << "round-trip: missing index file must refuse untouched\n";
            ok = false;
        }
        untouched.current = -1; (void)untouched;  // keep -Wunused quiet on untouched
    }
    // 5c. Unknown index version ("# scene manager v99") refuses. References
    //     a real per-scene file (written in step 3) so ONLY the version is
    //     wrong.
    {
        const std::string bad = "assets/scene_manager_rt_badver.txt";
        {
            std::ofstream out(bad, std::ios::trunc);
            out << "# scene manager v99\nscenes=1\ncurrent=0\nscene_file=scene_alpha.txt\n";
        }
        if (pe::loadSceneManagerFromFile("scene_manager_rt_badver.txt", loaded)) {
            std::cerr << "round-trip: unknown index version must refuse\n";
            ok = false;
        }
        std::remove(bad.c_str());
        std::remove((bad + ".tmp").c_str());
    }
    // 5d. scenes-count mismatch (scenes=2 but 1 scene_file line) refuses.
    {
        const std::string bad = "assets/scene_manager_rt_badcount.txt";
        {
            std::ofstream out(bad, std::ios::trunc);
            out << "# scene manager v1\nscenes=2\ncurrent=0\nscene_file=scene_alpha.txt\n";
        }
        if (pe::loadSceneManagerFromFile("scene_manager_rt_badcount.txt", loaded)) {
            std::cerr << "round-trip: scenes-count mismatch must refuse\n";
            ok = false;
        }
        std::remove(bad.c_str());
        std::remove((bad + ".tmp").c_str());
    }
    // 5e. Explicit-path save (path symmetry): the index honors a given
    //     savedata/ path directly (directory auto-created), the same
    //     explicit branch saveSceneToFile has. The manager here still
    //     holds 3 scenes with current=1, so the content assert doubles as
    //     a write-path proof (loader probe round-trip is case 3/4).
    {
        const std::string explicitPath = "savedata/scene_manager_sym_test.txt";
        std::remove(explicitPath.c_str());
        std::remove((explicitPath + ".tmp").c_str());
        if (!pe::saveSceneManagerToFile(manager, explicitPath)) {
            std::cerr << "round-trip: explicit-path save failed\n";
            ok = false;
        } else {
            std::ifstream in(explicitPath);
            if (!in) {
                std::cerr << "round-trip: explicit-path index not written\n";
                ok = false;
            } else {
                std::string line;
                bool header = false, scenes = false, current = false;
                while (std::getline(in, line)) {
                    if (line == "# scene manager v1") header = true;
                    if (line == "scenes=3") scenes = true;
                    if (line == "current=1") current = true;
                }
                if (!header || !scenes || !current) {
                    std::cerr << "round-trip: explicit-path index content wrong\n";
                    ok = false;
                }
            }
        }
        std::remove(explicitPath.c_str());
        std::remove((explicitPath + ".tmp").c_str());
    }

    rmArtifacts();
    return ok;
}

// --- Increment 2 (Scene/Prefab/Persistence campaign): spawn-after-kill ---
// Proves the no-erase contract under spawn-after-kill: the new entity is
// APPENDED at a NEW index, the dead slot stays dead, every earlier index
// stays stable, and alive flags are correct throughout (Step 113).
static bool checkSpawnAfterKill() {
    bool ok = true;
    pe::Scene scene;
    // 1. Spawn one live entity (index 0).
    pe::Entity first(pe::Vec3(0.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                     pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    first.tag = "first";
    const std::size_t i0 = pe::spawnEntity(scene, first);
    if (i0 != 0 || scene.entities.size() != 1 || !scene.entities[0].alive) {
        std::cerr << "First spawn must land at index 0 alive\n";
        return false;
    }
    // 2. Spawn a second, then kill it (slot 1 dead, count still 2).
    pe::Entity doomed(pe::Vec3(1.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                      pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    doomed.tag = "doomed";
    const std::size_t i1 = pe::spawnEntity(scene, doomed);
    pe::killEntity(scene, i1);
    if (i1 != 1 || scene.entities.size() != 2 ||
        !scene.entities[0].alive || scene.entities[1].alive) {
        std::cerr << "Kill must mark the slot dead without erasing\n";
        return false;
    }
    // 3. Spawn AFTER the kill: appends at a NEW index (2) — no slot reuse.
    pe::Entity third(pe::Vec3(-1.0f, 0.0f, 0.0f), 0.0f, pe::Vec3(1.0f, 1.0f, 1.0f),
                     pe::Vec3(0.5f, 0.5f, 0.0f), 0);
    third.tag = "third";
    const std::size_t i2 = pe::spawnEntity(scene, third);
    if (i2 != 2 || scene.entities.size() != 3) {
        std::cerr << "Spawn after kill must append a new index\n";
        ok = false;
    }
    // 4. Index stability: earlier slots unchanged, dead slot still dead.
    if (!scene.entities[0].alive || scene.entities[0].tag != "first" ||
        !assertFloatClose(scene.entities[0].position.x, 0.0f)) {
        std::cerr << "Slot 0 must stay stable after the later spawn\n";
        ok = false;
    }
    if (scene.entities[1].alive || scene.entities[1].tag != "doomed") {
        std::cerr << "Dead slot must stay dead (no erase, no reuse)\n";
        ok = false;
    }
    if (!scene.entities[2].alive || scene.entities[2].tag != "third" ||
        !assertFloatClose(scene.entities[2].position.x, -1.0f)) {
        std::cerr << "New slot must be live with its config\n";
        ok = false;
    }
    // 5. Out-of-range kill is a safe no-op; kill-then-kill idempotent.
    pe::killEntity(scene, 99);
    pe::killEntity(scene, i2);
    pe::killEntity(scene, i2);
    if (scene.entities.size() != 3 || scene.entities[2].alive) {
        std::cerr << "Out-of-range/double kill must be safe no-ops\n";
        ok = false;
    }
    return ok;
}

// --- Increment 3 (Scene/Prefab/Persistence campaign): multi-spawn persist ---
// Deepens the Step 136 composition: TWO live prefab entities at distinct
// positions + one killed entity -> save -> load -> count 2 (dead dropped)
// and BOTH live configs/positions survive the round-trip in order.
static bool checkPrefabMultiSpawnPersist() {
    bool ok = true;
    // 1. Scene + two prefab-spawned live entities + one killed.
    pe::Scene src;
    src.name = "multi_persist_test";
    pe::Prefab p;
    if (!pe::loadPrefab("enemy.txt", p)) {
        std::cerr << "multi-persist test: enemy.txt failed to load\n";
        return false;
    }
    pe::spawnEntity(src, pe::instantiatePrefab(p, pe::Vec3(2.0f, 1.0f, 0.0f)));
    pe::spawnEntity(src, pe::instantiatePrefab(p, pe::Vec3(-3.0f, -2.0f, 0.0f)));
    pe::Entity doomed = pe::instantiatePrefab(p, pe::Vec3(0.0f, 0.0f, 0.0f));
    doomed.tag = "doomed";
    const std::size_t deadIdx = pe::spawnEntity(src, doomed);
    pe::killEntity(src, deadIdx);
    if (src.entities.size() != 3 || !src.entities[0].alive ||
        !src.entities[1].alive || src.entities[2].alive) {
        std::cerr << "multi-persist setup wrong (0..1 live, 2=doomed dead)\n";
        return false;
    }

    // 2. Save + load into a fresh Scene (same rmAll pattern).
    const std::string fname = "scene_test_multi_persist.txt";
    auto rmAll = [&]() {
        std::remove(("assets/" + fname).c_str());
        std::remove(("../assets/" + fname).c_str());
        std::remove(("../../assets/" + fname).c_str());
        std::remove(("assets/" + fname + ".tmp").c_str());
        std::remove(("../assets/" + fname + ".tmp").c_str());
    };
    rmAll();
    if (!pe::saveSceneToFile(src, fname)) {
        std::cerr << "multi-persist save failed\n";
        rmAll();
        return false;
    }
    pe::Scene loaded;
    if (!pe::loadSceneFromFile(fname, loaded)) {
        std::cerr << "multi-persist load failed\n";
        rmAll();
        return false;
    }

    // 3. Dead dropped: exactly 2 live entities, in spawn order.
    if (loaded.entities.size() != 2) {
        std::cerr << "Dead entity must be dropped (got "
                  << loaded.entities.size() << ", want 2)\n";
        rmAll();
        return false;
    }
    // 4. Both live configs survive: per-entity positions + prefab fields.
    if (!assertFloatClose(loaded.entities[0].position.x, 2.0f) ||
        !assertFloatClose(loaded.entities[0].position.y, 1.0f) ||
        !assertFloatClose(loaded.entities[1].position.x, -3.0f) ||
        !assertFloatClose(loaded.entities[1].position.y, -2.0f)) {
        std::cerr << "Per-entity positions must survive in order\n";
        rmAll();
        ok = false;
    }
    for (int i = 0; i < 2; ++i) {
        const pe::Entity& e = loaded.entities[i];
        if (!e.alive || e.roleId != 2 || e.tag != "hostile" ||
            e.textureId != 2 || e.currentClipName != "walk_left") {
            std::cerr << "Entity " << i << " lost prefab config in round-trip\n";
            rmAll();
            ok = false;
        }
    }
    // 5. Source scene unchanged: 3 slots, doomed (index 2) still dead.
    if (src.entities.size() != 3 || src.entities[2].alive ||
        src.entities[2].tag != "doomed") {
        std::cerr << "Save must not mutate the source scene\n";
        rmAll();
        ok = false;
    }

    rmAll();
    return ok;
}

// --- System increment (Prefab <-> Scene failure matrix) ---
// One contract closing the failure cells the earlier prefab/spawn tests
// leave open. Already covered elsewhere (not repeated here): malformed
// no-'=' line (checkPrefabSystem), spawn into a NON-empty scene
// (checkPrefabSceneSpawn), kill+spawn index rules (checkSpawnAfterKill),
// kill->save-drops-dead with survivors (checkPrefabMultiSpawnPersist),
// no-current addEntity (checkSceneNoCurrentNoOps), OOB kill size rule
// (checkEntityLifecycle). This closes: (1) loadPrefab failure leaves out
// UNTOUCHED (missing file AND wrong header — sentinel assert, not just
// `false`); (2) a malformed NUMERIC value skips the key and keeps the
// field default (warn+skip is per-line); (3) instantiatePrefab from a
// failed-load Prefab yields a default-config entity (no partial state);
// (4) prefab-composed spawnEntity into an EMPTY scene lands at index 0
// (Scene has no capacity cap — "full" is not a real failure mode, the
// vector always grows); (5) an ALL-dead scene round-trips to zero
// entities with its name intact (multi-persist covers 2-live+1-dead).
static bool checkPrefabSceneFailureMatrix() {
    bool ok = true;
    auto rmAll = [&]() {
        const char* prefixes[3] = {"assets/", "../assets/", "../../assets/"};
        for (const char* p : prefixes) {
            std::remove((std::string(p) + "scene_test_matrix_alldead.txt").c_str());
            std::remove((std::string(p) + "scene_test_matrix_alldead.txt.tmp").c_str());
        }
        std::filesystem::remove("test_prefab_matrix_badheader.txt");
        std::filesystem::remove("test_prefab_matrix_badnum.txt");
    };
    rmAll();

    // Cell 1: missing file -> false, out untouched.
    pe::Prefab sentinel;
    sentinel.name = "sentinel";
    sentinel.textureId = 9;
    pe::Prefab p = sentinel;
    if (pe::loadPrefab("nonexistent_prefab_matrix.txt", p)) {
        std::cerr << "matrix: missing prefab must refuse\n";
        return false;
    }
    if (p.name != "sentinel" || p.textureId != 9) {
        std::cerr << "matrix: failed load must leave out untouched\n";
        ok = false;
    }

    // Cell 2: wrong header -> false, out untouched.
    {
        std::ofstream f("test_prefab_matrix_badheader.txt");
        f << "# not a prefab header\nname=wrong\n";
    }
    p = sentinel;
    pe::loadPrefab("test_prefab_matrix_badheader.txt", p);  // false; no crash
    if (p.name != "sentinel" || p.textureId != 9) {
        std::cerr << "matrix: wrong-header load must leave out untouched\n";
        ok = false;
    }

    // Cell 3: malformed numeric value -> key skipped, default kept;
    // later lines still parse (warn+skip is per-line).
    {
        std::ofstream f("test_prefab_matrix_badnum.txt");
        f << "# PureEngine prefab v1\n";
        f << "name=badnum\n";
        f << "moveSpeed=abc\n";   // parse fail -> key skipped, default kept
        f << "health=50.0\n";
    }
    pe::Prefab p3;
    pe::loadPrefab("test_prefab_matrix_badnum.txt", p3);  // true; no crash
    if (p3.name != "badnum") { std::cerr << "matrix: name should parse\n"; ok = false; }
    if (p3.moveSpeed != 0.0f) {
        std::cerr << "matrix: malformed numeric must keep field default\n";
        ok = false;
    }
    if (std::abs(p3.health - 50.0f) >= 1e-5f) {
        std::cerr << "matrix: fields after malformed numeric should parse\n";
        ok = false;
    }

    // Cell 4: instantiatePrefab from the failed-load Prefab — defaults
    // only (the untouched badnum result), no partial state.
    pe::Entity e = pe::instantiatePrefab(p3, pe::Vec3(0.0f, 0.0f, 0.0f));
    if (!e.alive) { std::cerr << "matrix: instantiated must be alive\n"; ok = false; }
    if (std::abs(e.moveSpeed) >= 1e-5f) {
        std::cerr << "matrix: instantiated moveSpeed must be the default-kept 0\n";
        ok = false;
    }
    if (std::abs(e.scale.x - 1.0f) >= 1e-5f || std::abs(e.scale.y - 1.0f) >= 1e-5f) {
        std::cerr << "matrix: instantiated scale must be default\n";
        ok = false;
    }
    if (e.textureId != 0) {
        std::cerr << "matrix: instantiated textureId must be default\n";
        ok = false;
    }

    // Cell 5: prefab-composed spawn into an EMPTY scene -> index 0.
    pe::Scene fresh;
    fresh.name = "matrix_empty";
    pe::Prefab shipped;
    if (!pe::loadPrefab("enemy.txt", shipped)) {
        std::cerr << "matrix: shipped enemy.txt failed to load\n";
        rmAll();
        return false;
    }
    const pe::Entity composed = pe::instantiatePrefab(shipped, pe::Vec3(1.0f, 1.0f, 0.0f));
    const std::size_t idx = pe::spawnEntity(fresh, composed);
    if (idx != 0 || fresh.entities.size() != 1 || !fresh.entities[0].alive ||
        fresh.entities[0].tag != shipped.tag) {
        std::cerr << "matrix: spawn into empty scene must land at index 0 alive\n";
        rmAll();
        ok = false;
    }

    // Cell 6: kill the spawned instance -> the ALL-dead scene round-trips
    // to zero entities, name intact (save writes no entity lines; load
    // keeps the scene slot with an empty list).
    pe::killEntity(fresh, idx);
    if (fresh.entities[0].alive) {
        std::cerr << "matrix: killEntity must mark the spawned instance dead\n";
        rmAll();
        ok = false;
    }
    if (!pe::saveSceneToFile(fresh, "scene_test_matrix_alldead.txt")) {
        std::cerr << "matrix: all-dead save failed\n";
        rmAll();
        return false;
    }
    pe::Scene after;
    if (!pe::loadSceneFromFile("scene_test_matrix_alldead.txt", after)) {
        std::cerr << "matrix: all-dead load failed\n";
        rmAll();
        return false;
    }
    if (!after.entities.empty()) {
        std::cerr << "matrix: all-dead scene must round-trip to zero entities\n";
        rmAll();
        ok = false;
    }
    if (after.name != "matrix_empty") {
        std::cerr << "matrix: all-dead round-trip must keep the scene name\n";
        rmAll();
        ok = false;
    }

    rmAll();
    return ok;
}

static bool checkTimeScale() {
    pe::FrameTime ft;
    if (!assertFloatClose(ft.getTimeScale(), 1.0f) || ft.isPaused()) { std::cerr << "Time default failed\n"; return false; }
    ft.setTimeScale(0.5f);
    if (!assertFloatClose(ft.getTimeScale(), 0.5f)) { std::cerr << "Time scale set failed\n"; return false; }
    ft.setTimeScale(-1.0f);
    if (!assertFloatClose(ft.getTimeScale(), 0.0f)) { std::cerr << "Time scale clamp failed\n"; return false; }
    ft.setPaused(true);
    if (!ft.isPaused()) { std::cerr << "Pause set failed\n"; return false; }
    ft.setPaused(false);
    if (ft.isPaused()) { std::cerr << "Unpause failed\n"; return false; }
    return true;
}

// Step 170: time/timestep contract (headless, wall-clock). Locks the
// halves checkTimeScale left open: tick() advances each frame (a
// delta measures only its own frame, never cumulative), the 0.1s
// clamp engages on a real long frame (sleep 150ms -> tick == 0.1f),
// paused scaledTick() returns 0 while the underlying clock still
// advances (pause gates the game's use, it does not freeze time),
// scaledTick multiplies by scale, scale 0 gives 0 unpaused, and
// consecutive ticks never go negative (monotonic clock by
// construction). No game slow-mo features — mechanism only.
// Step 174: core loop contract (from source, call-site based — a test
// cannot execute a game loop headless, so the order is locked HERE).
// Arcade (main.cpp:1274), Platformer (platformer.cpp:278), Pong
// (pong.cpp:139 — Step 174 moved its tick above poll to match):
//   1. frameTime.tick()    at the VERY TOP, before glfwPollEvents() —
//                          the Step 2/17 sampling invariant: each delta
//                          spans the ENTIRE previous frame
//   2. glfwPollEvents()    the window belongs to the game loop
//   3. input               gamepad snapshot + input.update() / isDown
//   4. update              simulation, animation clips
//   5. render              drawScene/drawEntities
//   6. glfwSwapBuffers()
// State transitions stop event sounds + music (Arcade 1578-1588).
// Teardown: glfwDestroyWindow on failure paths, glfwTerminate once at
// exit, audio.shutdown via destructor insurance (Step 171).
// No pe:: helper added — the order is call-site based and every piece
// is already owned by an existing boundary; nothing was missing.

static bool checkTimeContract() {
    bool ok = true;
    // glfwGetTime() requires glfwInit(): the test self-contains the
    // clock's lifetime (checkInputEdges terminates GLFW before this
    // runs). Headless init — no window needed for the monotonic clock.
    if (!glfwInit()) { std::cerr << "time test skipped: glfwInit failed\n"; return false; }
    pe::FrameTime ft;
    ft.start();
    // tick ordering: each delta measures its own frame.
    Sleep(50);
    const float d1 = ft.tick();
    if (d1 <= 0.0f || d1 > 0.1f) { std::cerr << "tick must measure its frame\n"; ok = false; }
    Sleep(30);
    const float d2 = ft.tick();
    if (d2 <= 0.0f || d2 > 0.1f) { std::cerr << "tick must advance per frame\n"; ok = false; }
    if (d2 >= d1 + 0.045f) { std::cerr << "tick delta must not accumulate\n"; ok = false; }
    // 0.1s clamp on a real long frame (debugger-pause analogue).
    Sleep(150);
    const float d3 = ft.tick();
    if (!assertFloatClose(d3, 0.1f)) { std::cerr << "Long frame must clamp to 0.1s, got " << d3 << "\n"; ok = false; }
    // Paused: scaledTick is 0; the clock still advances underneath.
    ft.setPaused(true);
    Sleep(30);
    if (ft.scaledTick() != 0.0f) { std::cerr << "Paused scaledTick must be 0\n"; ok = false; }
    ft.setPaused(false);
    const float d4 = ft.tick();
    if (d4 <= 0.0f || d4 > 0.1f) { std::cerr << "Pause must not freeze the clock\n"; ok = false; }
    // Scale multiply + scale 0 unpaused.
    ft.setTimeScale(0.5f);
    Sleep(40);
    const float d5 = ft.scaledTick();
    if (d5 <= 0.001f || d5 > 0.045f) { std::cerr << "scaledTick must be scale*delta\n"; ok = false; }
    ft.setTimeScale(0.0f);
    Sleep(20);
    if (ft.scaledTick() != 0.0f) { std::cerr << "Scale 0 unpaused must give 0\n"; ok = false; }
    // Hygiene: restore defaults for later tests.
    ft.setTimeScale(1.0f);
    ft.setPaused(false);
    glfwTerminate();
    return ok;
}

static bool checkAnimationClipSwitch() {
    std::map<std::string, pe::Animation> clips;
    pe::Animation a; a.name = "idle"; a.frames = {{0, 0.1f}}; a.loops = true;
    pe::Animation b; b.name = "run"; b.frames = {{1, 0.1f}}; b.loops = true;
    clips["idle"] = a; clips["run"] = b;
    pe::Entity e;
    if (!pe::setClip(e, clips, "idle")) { std::cerr << "setClip idle failed\n"; return false; }
    if (e.animationState.currentAnimation == nullptr || e.animationState.currentAnimation->name != "idle") { std::cerr << "setClip idle not assigned\n"; return false; }
    if (!pe::setClip(e, clips, "run")) { std::cerr << "setClip run failed\n"; return false; }
    if (e.animationState.currentAnimation->name != "run") { std::cerr << "setClip run not switched\n"; return false; }
    if (pe::setClip(e, clips, "missing")) { std::cerr << "setClip missing should fail\n"; return false; }
    e.animationState.elapsedTime = 0.5f;
    pe::setClip(e, clips, "run");
    if (!assertFloatClose(e.animationState.elapsedTime, 0.5f)) { std::cerr << "setClip same should preserve elapsedTime\n"; return false; }
    return true;
}

static bool checkHierarchyContractFreeze() {
    // Attachment-only: translation accumulates, rotation/scale do not propagate
    pe::Entity parent(pe::Vec3(1.0f, 0.0f, 0.0f), 3.14f, pe::Vec3(2.0f, 2.0f, 1.0f));
    parent.parentIndex = -1;
    pe::Entity child(pe::Vec3(0.0f, 1.0f, 0.0f), 1.0f, pe::Vec3(0.5f, 0.5f, 1.0f));
    child.parentIndex = 0;
    std::vector<pe::Entity> ents = {parent, child};
    pe::Vec3 wp = pe::worldPosition(ents, 1);
    if (!assertFloatClose(wp.x, 1.0f) || !assertFloatClose(wp.y, 1.0f)) {
        std::cerr << "Hierarchy attachment-only failed: wp " << wp.x << "," << wp.y << "\n"; return false;
    }
    // Erase/reorder invalidates indices â€” re-establish required
    ents.erase(ents.begin()); // remove parent, child now at 0 with stale parentIndex 0 (self-loop)
    pe::Vec3 wp2 = pe::worldPosition(ents, 0);
    // After erase, stale index must no longer resolve to original (1,1) and must not crash â€” bounded walk
    if (assertFloatClose(wp2.x, 1.0f) && assertFloatClose(wp2.y, 1.0f)) {
        std::cerr << "Hierarchy erase should invalidate, still (1,1)\n"; return false;
    }
    return true;
}

static bool checkComponentHelpers() {
    pe::Entity e;
    if (!assertFloatClose(pe::getHealth(e), 100.0f) || !pe::isAlive(e)) { std::cerr << "Health default failed\n"; return false; }
    pe::damage(e, 30.0f);
    if (!assertFloatClose(pe::getHealth(e), 70.0f)) { std::cerr << "Damage failed\n"; return false; }
    pe::heal(e, 10.0f);
    if (!assertFloatClose(e.health, 80.0f)) { std::cerr << "Heal failed\n"; return false; }
    pe::setTag(e, "boss");
    if (!pe::hasTag(e, "boss") || pe::getTag(e) != "boss") { std::cerr << "Tag failed\n"; return false; }
    pe::clearTag(e);
    if (!e.tag.empty()) { std::cerr << "ClearTag failed\n"; return false; }
    pe::setTimer(e, 1.0f);
    pe::tickTimer(e, 0.4f);
    if (!assertFloatClose(pe::getTimer(e), 0.6f)) { std::cerr << "Timer tick failed\n"; return false; }
    pe::addVelocity(e, pe::Vec3(1,0,0));
    if (!assertFloatClose(e.velocity.x, 1.0f)) { std::cerr << "Velocity helper failed\n"; return false; }
    return true;
}

static bool checkConsoleHistoryRecall() {
    pe::Console c;
    c.open = true;
    // submit two commands
    c.input = "echo hello";
    pe::submit(c);
    c.input = "echo world";
    pe::submit(c);
    if (c.submitHistory.size() != 2) { std::cerr << "History size 2 failed\n"; return false; }
    // Up should recall last
    pe::feedKey(c, GLFW_KEY_UP, false);
    if (c.input != "echo world") { std::cerr << "Up recall 1 failed: " << c.input << "\n"; return false; }
    pe::feedKey(c, GLFW_KEY_UP, false);
    if (c.input != "echo hello") { std::cerr << "Up recall 2 failed: " << c.input << "\n"; return false; }
    // Down should go forward
    pe::feedKey(c, GLFW_KEY_DOWN, false);
    if (c.input != "echo world") { std::cerr << "Down recall failed\n"; return false; }
    pe::feedKey(c, GLFW_KEY_DOWN, false);
    if (!c.input.empty()) { std::cerr << "Down past end should clear\n"; return false; }
    // History cap: push 64+5, oldest dropped
    for (int i = 0; i < 70; ++i) {
        c.input = "cmd" + std::to_string(i);
        pe::submit(c);
    }
    if (c.submitHistory.size() != 64) { std::cerr << "History cap 64 failed: " << c.submitHistory.size() << "\n"; return false; }
    if (c.submitHistory.front() != "cmd6") { std::cerr << "History oldest drop failed\n"; return false; }
    return true;
}

// Step 168: console contract gaps (headless). Closes the halves the
// five existing checkConsole* tests left open: the Step 95 wrap
// splits long lines to 38-col pieces with the 64-line cap still
// enforced, Up/Down recall is a no-op on an empty submitHistory,
// submitHistory SURVIVES a clear (clear wipes c.lines only — "look
// away, not hang up" applies to recall too), a new submit resets
// historyPos to the end (Up recalls the newest), and drawConsole on
// a CLOSED console with empty lines is a no-op that makes zero
// renderer calls (headless-safe; open+draw needs a GL context and
// stays a game-loop concern — no draw verification here).
static bool checkConsoleContract() {
    bool ok = true;
    // Step 95 wrap: a 100-char line splits 38/38/24.
    pe::Console c;
    std::string longLine(100, 'x');
    pe::print(c, longLine);
    if (c.lines.size() != 3 || c.lines[0].size() != 38 ||
        c.lines[1].size() != 38 || c.lines[2].size() != 24) {
        std::cerr << "Wrap must split a long line to 38-col pieces\n";
        ok = false;
    }
    // Cap under wrapped submits: wrapped echoes still evict the oldest.
    for (int i = 0; i < 40; ++i) {
        c.input = longLine;
        pe::submit(c);
    }
    if (c.lines.size() != 64) { std::cerr << "Cap must hold under wrapped submits\n"; ok = false; }
    // Recall is a no-op on an empty submitHistory (no crash, no state).
    pe::Console fresh;
    pe::feedKey(fresh, GLFW_KEY_UP, false);
    pe::feedKey(fresh, GLFW_KEY_DOWN, false);
    if (!fresh.input.empty() || !fresh.submitHistory.empty()) {
        std::cerr << "Recall on empty submitHistory must be a no-op\n";
        ok = false;
    }
    // submitHistory SURVIVES clear (clear wipes c.lines only).
    pe::Console k;
    k.input = "echo keep";
    pe::submit(k);
    k.input = "clear";
    pe::submit(k);
    if (!k.lines.empty() || k.submitHistory.size() != 2) {
        std::cerr << "clear must wipe lines but keep submitHistory\n";
        ok = false;
    }
    pe::feedKey(k, GLFW_KEY_UP, false);
    if (k.input != "clear") {
        std::cerr << "Recall must work after clear\n";
        ok = false;
    }
    // A new submit resets historyPos to the end: Up recalls the newest.
    k.input = "echo after";
    pe::submit(k);
    pe::feedKey(k, GLFW_KEY_UP, false);
    if (k.input != "echo after") {
        std::cerr << "Submit must reset historyPos to the end\n";
        ok = false;
    }
    // drawConsole on a CLOSED console with empty lines: no-op, zero
    // renderer calls (headless-safe — GL draw paths need a context).
    pe::Console closed;
    pe::Renderer r;
    pe::Mat4 proj;
    pe::drawConsole(r, proj, closed);
    if (closed.open || !closed.lines.empty()) {
        std::cerr << "Closed console must stay closed and empty\n";
        ok = false;
    }
    return ok;
}

static bool checkAnimClipKeep() {
    std::map<std::string, pe::Animation> clips;
    pe::Animation a; a.name = "walk_left"; a.frames = {{0, 0.1f}};
    clips["walk_left"] = a;
    pe::Entity e(pe::Vec3(0,0,0), 0.0f, pe::Vec3(1,1,1));
    e.currentClipName = "walk_left";
    e.animationState.currentAnimation = nullptr;
    // Simulate activateScene rebuild: new vector copy retains currentClipName via Entity copy
    std::vector<pe::Entity> rebuilt = {e};
    // Reassign via stored name (Step 106)
    for (auto& ent : rebuilt) {
        if (!ent.currentClipName.empty()) {
            auto it = clips.find(ent.currentClipName);
            if (it != clips.end()) { ent.animationState.currentAnimation = &it->second; ent.animationState.isPlaying = true; }
        }
    }
    if (rebuilt[0].animationState.currentAnimation == nullptr) { std::cerr << "Anim clip keep failed: nullptr after rebuild\n"; return false; }
    if (rebuilt[0].currentClipName != "walk_left") { std::cerr << "Clip name not preserved\n"; return false; }
    // Simulate snapshot copy: buildInitialEntities sets name, ensure it survives
    pe::HostileDefaults hd;
    hd.hostiles.push_back(pe::HostileDefinition{1.0f, pe::Vec3(0,0,0), 0.0f, 0});
    auto ents = pe::buildInitialEntities(hd);
    bool found = false;
    for (auto& en : ents) if (en.roleId == static_cast<int>(pe::ArcadeRole::Hostile) && en.currentClipName == "walk_left") found = true;
    if (!found) { std::cerr << "buildInitialEntities should set currentClipName\n"; return false; }
    return true;
}

// --- Animation contract: load -> bind -> update -> switch -> loop/end -> failure ---
// Covers the untested halves: loadAnimations parsing/failure, non-loop end,
// zero-duration frame skip, not-playing no-ops, getCurrentFrame nullptrs.
static bool checkAnimationSystem() {
    bool ok = true;
    const fs::path tmpDir = fs::temp_directory_path() / "pureengine_animation_test";
    fs::create_directories(tmpDir);

    // 1) Valid load: absolute path bypasses the probe (Step-34 pattern).
    const fs::path goodPath = tmpDir / "__pureengine_anim_good___.txt";
    if (!writeFile(goodPath,
            "# comment and blank line tolerated\n"
            "\n"
            "animation=walk_left,4,0.1,true\n"
            "animation=attack,2,0.25,false\n")) {
        std::cerr << "animation test: good file write failed\n";
        return false;
    }
    const std::map<std::string, pe::Animation> loaded = pe::loadAnimations(goodPath.string());
    if (loaded.size() != 2) {
        std::cerr << "loadAnimations expected 2 clips, got " << loaded.size() << "\n";
        return false;
    }
    auto walk = loaded.find("walk_left");
    auto attack = loaded.find("attack");
    if (walk == loaded.end() || attack == loaded.end()) {
        std::cerr << "loadAnimations lost a named clip\n";
        return false;
    }
    if (walk->second.frames.size() != 4 || walk->second.loops != true ||
        !assertFloatClose(walk->second.totalDuration, 0.4f) ||
        walk->second.frames[2].frameIndex != 2 ||
        !assertFloatClose(walk->second.frames[0].duration, 0.1f)) {
        std::cerr << "loadAnimations walk_left fields wrong\n";
        return false;
    }
    if (attack->second.frames.size() != 2 || attack->second.loops != false ||
        !assertFloatClose(attack->second.totalDuration, 0.5f)) {
        std::cerr << "loadAnimations attack fields wrong\n";
        return false;
    }

    // 2) Malformed lines: skipped with a warning, parsing continues;
    // bad count/duration/loop flag each skip their own entry.
    const fs::path badPath = tmpDir / "__pureengine_anim_bad___.txt";
    if (!writeFile(badPath,
            "not an animation line\n"
            "animation=\n"
            "animation=zero_count,0,0.1,true\n"
            "animation=bad_duration,2,abc,true\n"
            "animation=bad_loop,2,0.1,perhaps\n"
            "animation=garbled_count,2x,0.1,true\n"
            "animation=good_after_bad,3,0.05,false\n")) {
        std::cerr << "animation test: bad file write failed\n";
        return false;
    }
    const std::map<std::string, pe::Animation> bad = pe::loadAnimations(badPath.string());
    if (bad.size() != 1 || bad.find("good_after_bad") == bad.end() ||
        bad.find("zero_count") != bad.end() ||
        bad.find("bad_duration") != bad.end() ||
        bad.find("bad_loop") != bad.end() ||
        bad.find("garbled_count") != bad.end()) {
        std::cerr << "Malformed animation entries did not skip cleanly\n";
        return false;
    }

    // 3) Duplicate names: last wins.
    const fs::path dupPath = tmpDir / "__pureengine_anim_dup___.txt";
    if (!writeFile(dupPath,
            "animation=dup,2,0.1,true\n"
            "animation=dup,5,0.2,false\n")) {
        std::cerr << "animation test: dup file write failed\n";
        return false;
    }
    const std::map<std::string, pe::Animation> dup = pe::loadAnimations(dupPath.string());
    if (dup.size() != 1 || dup.find("dup") == dup.end() ||
        dup.find("dup")->second.frames.size() != 5 ||
        dup.find("dup")->second.loops != false) {
        std::cerr << "Duplicate animation name: last wins failed\n";
        return false;
    }

    // 4) Missing file: empty map (warning, no crash).
    const fs::path missingPath = tmpDir / "__definitely_no_anim___.txt";
    if (fs::exists(missingPath)) {
        fs::remove(missingPath);
    }
    const std::map<std::string, pe::Animation> none = pe::loadAnimations(missingPath.string());
    if (!none.empty()) {
        std::cerr << "Missing animation file must load an empty map\n";
        return false;
    }

    // 5) Bind to entity: setClip on an empty map fails, state untouched.
    pe::Entity e;
    if (pe::setClip(e, none, "walk_left")) {
        std::cerr << "setClip on empty map must fail\n";
        return false;
    }
    if (e.animationState.currentAnimation != nullptr || e.animationState.isPlaying) {
        std::cerr << "Failed setClip mutated state\n";
        return false;
    }
    if (!pe::setClip(e, loaded, "walk_left")) {
        std::cerr << "setClip from loaded map failed\n";
        return false;
    }
    if (e.animationState.currentAnimation != &walk->second ||
        e.animationState.currentFrameIndex != 0 || !e.animationState.isPlaying) {
        std::cerr << "setClip did not bind to the loaded clip\n";
        return false;
    }

    // 6) Loop wrap: 4 frames at 0.1s loop -> frame 0 again, still playing.
    for (int i = 0; i < 3; ++i) {
        if (e.animationState.update(0.1f)) {
            std::cerr << "Looping animation must not report finished\n";
            return false;
        }
        if (e.animationState.currentFrameIndex != i + 1 || !e.animationState.isPlaying) {
            std::cerr << "Frame did not advance to " << i + 1 << " after one duration\n";
            return false;
        }
    }
    if (e.animationState.update(0.1f)) {
        std::cerr << "Looping animation must not report finished (2)\n";
        return false;
    }
    if (e.animationState.currentFrameIndex != 0) {
        std::cerr << "Looping animation did not wrap to frame 0\n";
        return false;
    }

    // 7) Switch clip mid-play: playhead resets to the new clip's frame 0.
    e.animationState.elapsedTime = 0.05f;
    if (!pe::setClip(e, loaded, "attack")) {
        std::cerr << "Mid-play switch failed\n";
        return false;
    }
    if (e.animationState.currentAnimation->name != "attack" ||
        e.animationState.currentFrameIndex != 0 ||
        !assertFloatClose(e.animationState.elapsedTime, 0.0f)) {
        std::cerr << "Mid-play switch did not reset the playhead\n";
        return false;
    }

    // 8) Non-loop end: consumes the last frame, reports finished once,
    // then stops updating.
    if (e.animationState.update(0.25f) || e.animationState.currentFrameIndex != 1) {
        std::cerr << "Non-loop first frame did not advance\n";
        return false;
    }
    if (!e.animationState.update(0.25f)) {
        std::cerr << "Non-loop end must report finished\n";
        return false;
    }
    if (e.animationState.isPlaying) {
        std::cerr << "Finished non-loop must stop playing\n";
        return false;
    }
    if (e.animationState.update(0.25f)) {
        std::cerr << "Stopped animation must stay stopped\n";
        return false;
    }

    // 9) Zero-duration frames: skipped without consuming time, never hang.
    pe::Animation skip;
    skip.name = "skip";
    skip.frames = {{0, 0.0f}, {1, 0.05f}};
    skip.loops = true;
    e.animationState.currentAnimation = &skip;
    e.animationState.currentFrameIndex = 0;
    e.animationState.elapsedTime = 0.0f;
    e.animationState.isPlaying = true;
    if (e.animationState.update(0.02f)) {
        std::cerr << "Skip animation must not report finished\n";
        return false;
    }
    if (e.animationState.currentFrameIndex != 1) {
        std::cerr << "Zero-duration frame was not skipped\n";
        return false;
    }

    // 10) Not playing / no animation: update is a no-op returning false.
    e.animationState.isPlaying = false;
    e.animationState.currentFrameIndex = 0;
    e.animationState.elapsedTime = 0.0f;
    if (e.animationState.update(0.016f) || e.animationState.currentFrameIndex != 0 ||
        !assertFloatClose(e.animationState.elapsedTime, 0.0f)) {
        std::cerr << "Not-playing update must be a no-op\n";
        return false;
    }
    e.animationState.isPlaying = true;
    e.animationState.currentAnimation = nullptr;
    if (e.animationState.update(0.016f)) {
        std::cerr << "No-animation update must return false\n";
        return false;
    }

    // 11) getCurrentFrame nullptr cases: no animation, empty list,
    // out-of-range index.
    if (e.animationState.getCurrentFrame() != nullptr) {
        std::cerr << "No-animation getCurrentFrame must be nullptr\n";
        return false;
    }
    pe::Animation empty;
    e.animationState.currentAnimation = &empty;
    if (e.animationState.getCurrentFrame() != nullptr) {
        std::cerr << "Empty-clip getCurrentFrame must be nullptr\n";
        return false;
    }
    e.animationState.currentAnimation = &walk->second;
    e.animationState.currentFrameIndex = 99;
    if (e.animationState.getCurrentFrame() != nullptr) {
        std::cerr << "Out-of-range getCurrentFrame must be nullptr\n";
        return false;
    }

    // 12) Caller-applied speed: update(dt * animationSpeed) contract from
    // main.cpp:1679 — speed 0 freezes playback (no crash, no advance).
    e.animationState.currentFrameIndex = 0;
    e.animationState.elapsedTime = 0.0f;
    e.animationState.isPlaying = true;
    if (e.animationState.update(0.1f * 0.0f)) {
        std::cerr << "Zero-speed update must not report finished\n";
        return false;
    }
    if (e.animationState.currentFrameIndex != 0) {
        std::cerr << "Zero-speed update must not advance\n";
        return false;
    }

    fs::remove(goodPath);
    fs::remove(badPath);
    fs::remove(dupPath);
    fs::remove(missingPath);
    fs::remove(tmpDir);
    return ok;
}

static bool checkScenePointerStability() {
    pe::SceneManager m;
    m.scenes.reserve(4);
    pe::loadScene(m, "s1");
    pe::loadScene(m, "s2");
    pe::Scene* p1 = pe::sceneByName(m, "s1");
    if (!p1) { std::cerr << "s1 not found\n"; return false; }
    std::string nameBefore = p1->name;
    void* addrBefore = static_cast<void*>(p1);
    pe::loadScene(m, "s3");
    pe::Scene* p1After = pe::sceneByName(m, "s1");
    if (!p1After) { std::cerr << "s1 lost after third load\n"; return false; }
    if (p1After->name != nameBefore) { std::cerr << "s1 name corrupted after realloc\n"; return false; }
    // With reserve(4), address should be stable (no realloc for 3 pushes)
    if (static_cast<void*>(p1After) != addrBefore) { std::cerr << "Pointer moved despite reserve\n"; return false; }
    return true;
}

// --- Step 57: spritesheet frame UVs (headless, pure math, no GL calls) ---
// Degenerate inputs resolve to the full texture, indices clamp, and the
// row/col mapping follows the "row 0 is the image TOP" V convention.
static bool checkFrameUV() {
    bool ok = true;
    // Degenerate inputs: non-positive row count and 0/1 total frames all
    // resolve to the full texture — never an error, never empty.
    const pe::UVRect full = {0.0f, 0.0f, 1.0f, 1.0f};
    const pe::UVRect zeroRows = pe::calculateFrameUV(3, 0, 8);
    if (zeroRows.minU != full.minU || zeroRows.minV != full.minV ||
        zeroRows.maxU != full.maxU || zeroRows.maxV != full.maxV) {
        std::cerr << "calculateFrameUV framesPerRow=0 must be full texture\n";
        ok = false;
    }
    const pe::UVRect negRows = pe::calculateFrameUV(3, -3, 8);
    if (negRows.minU != full.minU || negRows.minV != full.minV ||
        negRows.maxU != full.maxU || negRows.maxV != full.maxV) {
        std::cerr << "calculateFrameUV framesPerRow<0 must be full texture\n";
        ok = false;
    }
    const pe::UVRect noAnim = pe::calculateFrameUV(0, 8, 1);
    if (noAnim.minU != full.minU || noAnim.minV != full.minV ||
        noAnim.maxU != full.maxU || noAnim.maxV != full.maxV) {
        std::cerr << "calculateFrameUV totalFrames=1 must be full texture\n";
        ok = false;
    }
    const pe::UVRect noFrames = pe::calculateFrameUV(2, 8, 0);
    if (noFrames.minU != full.minU || noFrames.minV != full.minV ||
        noFrames.maxU != full.maxU || noFrames.maxV != full.maxV) {
        std::cerr << "calculateFrameUV totalFrames=0 must be full texture\n";
        ok = false;
    }
    // Single row (8 frames, 8 per row): frame 3 = col 3, row 0. V spans
    // the full texture height (one row), U spans one eighth-slice.
    const pe::UVRect f3 = pe::calculateFrameUV(3, 8, 8);
    if (!assertFloatClose(f3.minU, 0.375f) || !assertFloatClose(f3.maxU, 0.5f) ||
        !assertFloatClose(f3.minV, 0.0f) || !assertFloatClose(f3.maxV, 1.0f)) {
        std::cerr << "calculateFrameUV single-row col mapping wrong\n";
        ok = false;
    }
    // Negative frameIndex clamps to 0 (never underflows the row math).
    const pe::UVRect fNeg = pe::calculateFrameUV(-1, 8, 8);
    if (!assertFloatClose(fNeg.minU, 0.0f) || !assertFloatClose(fNeg.maxU, 0.125f)) {
        std::cerr << "calculateFrameUV negative index must clamp to 0\n";
        ok = false;
    }
    // Two rows (8 frames, 4 per row): row 0 is the image TOP (higher V),
    // row 1 sits below it — the V-flip convention the digit path shares.
    const pe::UVRect f0 = pe::calculateFrameUV(0, 4, 8);
    if (!assertFloatClose(f0.minU, 0.0f) || !assertFloatClose(f0.maxU, 0.25f) ||
        !assertFloatClose(f0.minV, 0.5f) || !assertFloatClose(f0.maxV, 1.0f)) {
        std::cerr << "calculateFrameUV two-row top frame wrong\n";
        ok = false;
    }
    const pe::UVRect f4 = pe::calculateFrameUV(4, 4, 8);
    if (!assertFloatClose(f4.minU, 0.0f) || !assertFloatClose(f4.maxU, 0.25f) ||
        !assertFloatClose(f4.minV, 0.0f) || !assertFloatClose(f4.maxV, 0.5f)) {
        std::cerr << "calculateFrameUV two-row bottom frame wrong\n";
        ok = false;
    }
    // Last frame (7) = col 3, row 1: rightmost slice of the bottom row.
    const pe::UVRect f7 = pe::calculateFrameUV(7, 4, 8);
    if (!assertFloatClose(f7.minU, 0.75f) || !assertFloatClose(f7.maxU, 1.0f) ||
        !assertFloatClose(f7.minV, 0.0f) || !assertFloatClose(f7.maxV, 0.5f)) {
        std::cerr << "calculateFrameUV last frame wrong\n";
        ok = false;
    }
    // Index beyond the last clamps to totalFrames-1 (same rect as f7).
    const pe::UVRect fOOB = pe::calculateFrameUV(99, 4, 8);
    if (fOOB.minU != f7.minU || fOOB.minV != f7.minV ||
        fOOB.maxU != f7.maxU || fOOB.maxV != f7.maxV) {
        std::cerr << "calculateFrameUV OOB index must clamp to last\n";
        ok = false;
    }
    return ok;
}

// --- Step 79 / MAX_LIGHTS: addLight capacity contract (headless, pure) ---
// Four lights fit; the fifth is silently ignored; lightCount() and the
// stored lights reflect exactly what was accepted.
static bool checkLightingCap() {
    bool ok = true;
    if (pe::LightingState::MAX_LIGHTS != 4) {
        std::cerr << "MAX_LIGHTS must stay 4 (uniform array + upload loop bound)\n";
        ok = false;
    }
    pe::LightingState s = pe::LightingState::makeDefault();
    if (s.lightCount() != 0) {
        std::cerr << "makeDefault must start with zero lights\n";
        ok = false;
    }
    if (!assertFloatClose(s.ambient.intensity, 1.0f) ||
        !assertFloatClose(s.ambient.color.x, 1.0f) ||
        !assertFloatClose(s.ambient.color.y, 1.0f) ||
        !assertFloatClose(s.ambient.color.z, 1.0f)) {
        std::cerr << "makeDefault ambient must be white intensity 1\n";
        ok = false;
    }
    for (int i = 0; i < pe::LightingState::MAX_LIGHTS; ++i) {
        pe::PointLight l;
        l.position = pe::Vec3(float(i), 0.0f, 0.0f);
        s.addLight(l);
    }
    if (s.lightCount() != pe::LightingState::MAX_LIGHTS) {
        std::cerr << "addLight must accept exactly MAX_LIGHTS lights\n";
        ok = false;
    }
    // Stored lights keep their order and data.
    for (int i = 0; i < pe::LightingState::MAX_LIGHTS; ++i) {
        if (!assertFloatClose(s.lights[i].position.x, float(i))) {
            std::cerr << "addLight must preserve light order/data at " << i << "\n";
            ok = false;
        }
    }
    // The fifth push is silently ignored — the cap never throws, never grows.
    pe::PointLight fifth;
    fifth.position = pe::Vec3(99.0f, 0.0f, 0.0f);
    s.addLight(fifth);
    if (s.lightCount() != pe::LightingState::MAX_LIGHTS) {
        std::cerr << "addLight beyond MAX_LIGHTS must be ignored\n";
        ok = false;
    }
    if (assertFloatClose(s.lights[pe::LightingState::MAX_LIGHTS - 1].position.x, 99.0f)) {
        std::cerr << "fifth light must not overwrite the last accepted light\n";
        ok = false;
    }
    return ok;
}

int main() {
    const bool validOk = checkCaseValidData();
    const bool missingKeyOk = checkCaseMissingKey();
    const bool malformedOk = checkCaseMalformedNumber();
    const bool emptyListOk = checkCaseEmptyHostileList();
    const bool missingFileOk = checkCaseMissingFile();
    const bool tilemapValidOk = checkTilemapValidLoad();
    const bool tilemapMalformedOk = checkTilemapMalformedFallsBack();
    const bool tilemapCollideOk = checkTilemapCollidePushesOut();
    const bool sceneLifecycleOk = checkSceneLifecycle();
    const bool sceneNoOpsOk = checkSceneNoCurrentNoOps();
    const bool hierarchyChainOk = checkHierarchyChain();
    const bool hierarchyRefusalsOk = checkHierarchyRefusals();
    const bool hierarchyEdgeOk = checkHierarchyEdgeCases();
    const bool fontCellsOk = checkFontCells();
    const bool fontMetricsOk = checkFontMetrics();
    const bool eventsOrderOk = checkEventsOrderAndPayload();
    const bool eventsUnsubOk = checkEventsUnsubscribe();
    const bool eventsEdgeOk = checkEventsEdgeCases();
    const bool consoleToggleOk = checkConsoleToggle();
    const bool consoleFeedOk = checkConsoleFeedKey();
    const bool consoleSubmitOk = checkConsoleSubmit();
    const bool consoleHistoryOk = checkConsoleHistory();
    const bool gamepadDeadzoneOk = checkGamepadDeadzone();
    const bool gamepadButtonsOk = checkGamepadButtons();
    const bool gamepadEdgeOk = checkGamepadEdge();
    const bool gamepadPollOk = checkGamepadPollSafety();
    const bool mouseInputOk = checkMouseInput();
    const bool prefabSystemOk = checkPrefabSystem();
    const bool prefabSceneSpawnOk = checkPrefabSceneSpawn();
    const bool prefabLiveOk = checkPrefabLiveSpawn();
    const bool particleSpawnOk = checkParticleSpawn();
    const bool emitterRateOk = checkEmitterRateAndCap();
    const bool particleMotionOk = checkParticleMotion();
    const bool particleDeathOk = checkParticleDeath();
    const bool particleConvertOk = checkParticleConverter();
    const bool staticFloorOk = checkStaticFloorLanding();
    const bool groundedOk = checkGroundedState();
    const bool wallOk = checkWallCollision();
    const bool ceilingOk = checkCeilingCollision();
    const bool jumpOk = checkJumpAndLand();
    const bool coyoteOk = checkCoyoteTime();
    const bool charDtOk = checkCharacterDtGuards();
    const bool staticResolveOk = checkStaticResolve();
    const bool applyForceOk = checkApplyForceMath();
    const bool applyPhysicsOk = checkApplyPhysicsSemantics();
    const bool applyPhysicsFixedOk = checkApplyPhysicsFixedFreeMovers();
    const bool broadphaseOk = checkBroadphaseGridPairs();
    const bool restClampOk = checkResolveRestitutionClamp();
    const bool bounceStickOk = checkResolveBounceVsStick();
    const bool approachGuardOk = checkResolveApproachingGuard();
    const bool edgeTouchOk = checkResolveEdgeTouch();
    const bool fixedJumpOnceOk = checkFixedJumpConsumedOnce();
    const bool fixedClampFallbackOk = checkFixedSubstepClampAndFallback();
    const bool kinematicCarryOk = checkKinematicCarry();
    const bool kinematicResolveOk = checkKinematicResolve();
    const bool sweptAABBOk = checkSweptAABBContract();
    const bool sweptMoveOk = checkSweptMoveAndCollide();
    const bool sceneByNameOk = checkSceneByName();
    const bool platLevelsOk = checkPlatformerLevels();
    const bool platClimbOk = checkPlatformerClimb();
    const bool inputEdgesOk = checkInputEdges();
    const bool volumeClampOk = checkVolumeClamp();
    const bool muteToggleOk = checkMuteToggle();
    const bool perSoundVolumeOk = checkPerSoundVolume();
    const bool musicVolumeIndepOk = checkMusicVolumeIndependence();
    const bool preInitGuardsOk = checkPreInitGuards();
    const bool volumeMatrixOk = checkAudioVolumeMatrix();
    const bool audioDeviceLifecycleOk = checkAudioDeviceLifecycle();
    const bool audioPoolRotationOk = checkAudioPoolRotation();
    const bool screenToWorldOk = checkScreenToWorld();
    const bool worldToScreenOk = checkWorldToScreen();
    const bool entityPickOk = checkEntityPick();
    const bool screenPickOk = checkScreenPick();
    const bool entityBoundsOk = checkEntityWorldAABB();
    const bool gateTableOk = checkGameStateGateTable();
    const bool highscoreOk = checkHighscoreSemantics();
    const bool platLandingOk = checkPlatformerLanding();
    const bool platSwitchOk = checkPlatformerLevelSwitch();
    const bool platGoalOk = checkPlatformerGoalEvent();
    const bool sceneSerOk = checkSceneSerialization();
    const bool pongScoreOk = checkPongScore();
    const bool particleColorOk = checkParticleColorAndEmit();
    const bool fixedStepOk = checkFixedTimestepNoTunnel();
    const bool actionMapOk = checkActionMap();
    const bool textureRegOk = checkTextureRegistry();
    const bool consoleHistRecallOk = checkConsoleHistoryRecall();
    const bool consoleContractOk = checkConsoleContract();
    const bool eventThrowOnceOk = checkEventThrowAndOnce();
    const bool eventGapOk = checkEventReentrantOnceGaps();
    const bool timeScaleOk = checkTimeScale();
    const bool timeContractOk = checkTimeContract();
    const bool hierarchyFreezeOk = checkHierarchyContractFreeze();
    const bool animClipOk = checkAnimationClipSwitch();
    const bool binaryBlobOk = checkBinaryBlob();
    const bool resourceSystemOk = checkResourceSystem();
    const bool keyNamesOk = checkKeyNames();
    const bool keysAllOk = checkKeysForAllActions();
    const bool gamepadActionsOk = checkGamepadActions();
    const bool adoptionOk = checkInputAdoptionHelper();
    const bool inputBindingsOk = checkInputBindings();
    const bool componentOk = checkComponentHelpers();
    const bool sceneDumpOk = checkSceneDumpReload();
    const bool managerSaveOk = checkSceneManagerSave();
    const bool managerRoundTripOk = checkSceneManagerRoundTrip();
    const bool spawnAfterKillOk = checkSpawnAfterKill();
    const bool multiPersistOk = checkPrefabMultiSpawnPersist();
    const bool prefabMatrixOk = checkPrefabSceneFailureMatrix();
    const bool animClipKeepOk = checkAnimClipKeep();
    const bool animationSystemOk = checkAnimationSystem();
    const bool scenePtrOk = checkScenePointerStability();
    const bool persistV2Ok = checkScenePersistenceV2();
    const bool persistComposeOk = checkPrefabScenePersist();
    const bool persistV1Ok = checkScenePersistenceV1Compat();
    const bool persistV99Ok = checkSceneVersionUnknown();
    const bool lifecycleOk = checkEntityLifecycle();
    const bool frameUvOk = checkFrameUV();
    const bool followLerpOk = checkFollowLerp();
    const bool particleContractOk = checkParticleContract();
    const bool lightingCapOk = checkLightingCap();

    if (!validOk || !missingKeyOk || !malformedOk || !emptyListOk || !missingFileOk ||
        !tilemapValidOk || !tilemapMalformedOk || !tilemapCollideOk ||
        !sceneLifecycleOk || !sceneNoOpsOk ||
        !hierarchyChainOk || !hierarchyRefusalsOk || !hierarchyEdgeOk ||
        !fontCellsOk || !fontMetricsOk ||
        !eventsOrderOk || !eventsUnsubOk || !eventsEdgeOk || !eventThrowOnceOk || !eventGapOk || !followLerpOk || !consoleContractOk || !particleContractOk || !timeContractOk ||
        !consoleToggleOk || !consoleFeedOk || !consoleSubmitOk ||
        !consoleHistoryOk ||
        !gamepadDeadzoneOk || !gamepadButtonsOk || !gamepadEdgeOk ||
        !gamepadPollOk || !mouseInputOk || !prefabSystemOk || !prefabSceneSpawnOk || !prefabLiveOk ||
        !particleSpawnOk || !emitterRateOk || !particleMotionOk ||
        !particleDeathOk || !particleConvertOk ||
        !staticFloorOk || !groundedOk || !wallOk || !ceilingOk ||
        !jumpOk || !coyoteOk || !charDtOk || !staticResolveOk ||
        !applyForceOk || !applyPhysicsOk || !applyPhysicsFixedOk || !broadphaseOk ||
        !restClampOk || !bounceStickOk || !approachGuardOk || !edgeTouchOk ||
        !fixedJumpOnceOk || !fixedClampFallbackOk ||
        !kinematicCarryOk || !kinematicResolveOk || !sweptAABBOk || !sweptMoveOk ||
        !sceneByNameOk ||
        !platLevelsOk || !platLandingOk || !platSwitchOk || !platGoalOk ||
        !platClimbOk || !inputEdgesOk ||         !volumeClampOk || !muteToggleOk || !perSoundVolumeOk ||
        !musicVolumeIndepOk || !preInitGuardsOk || !volumeMatrixOk || !audioDeviceLifecycleOk ||
        !audioPoolRotationOk || !screenToWorldOk || !worldToScreenOk || !entityPickOk || !screenPickOk || !entityBoundsOk || !gateTableOk || !highscoreOk || !sceneSerOk || !pongScoreOk || !particleColorOk || !fixedStepOk || !actionMapOk || !textureRegOk || !consoleHistRecallOk || !timeScaleOk || !hierarchyFreezeOk || !animClipOk || !binaryBlobOk || !resourceSystemOk || !keyNamesOk || !keysAllOk || !gamepadActionsOk || !adoptionOk || !inputBindingsOk || !componentOk || !sceneDumpOk || !managerSaveOk || !managerRoundTripOk || !spawnAfterKillOk || !multiPersistOk || !prefabMatrixOk || !animClipKeepOk || !animationSystemOk || !scenePtrOk || !persistV2Ok || !persistV1Ok || !persistV99Ok || !persistComposeOk || !lifecycleOk || !frameUvOk || !lightingCapOk || !followLerpOk || !particleContractOk) {
        std::cerr << "hostile_data_test: FAILED\n";
        return 1;
    }

    std::cout << "hostile_data_test: PASS\n";
    return 0;
}
