#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "../src/console.h"
#include "../src/events.h"
#include "../src/gamepad.h"
#include "../src/input.h"
#include "../src/particles.h"
#include "../src/physics.h"
#include "../src/font.h"
#include "../src/hostile_data.h"
#include "../src/scene.h"
#include "../src/tilemap.h"

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
    // Hand-written cycle (bypasses setParent — field is public): the
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
    // floor, land grounded at rest height (-2.5). No logic duplicated —
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
    // reallocate the vector and invalidate s1 (documented scene caveat —
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
        // the single jump on an airborne frame — that exact mistake failed
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
    const bool sceneByNameOk = checkSceneByName();
    const bool platLevelsOk = checkPlatformerLevels();
    const bool platClimbOk = checkPlatformerClimb();
    const bool inputEdgesOk = checkInputEdges();
    const bool platLandingOk = checkPlatformerLanding();
    const bool platSwitchOk = checkPlatformerLevelSwitch();
    const bool platGoalOk = checkPlatformerGoalEvent();

    if (!validOk || !missingKeyOk || !malformedOk || !emptyListOk || !missingFileOk ||
        !tilemapValidOk || !tilemapMalformedOk || !tilemapCollideOk ||
        !sceneLifecycleOk || !sceneNoOpsOk ||
        !hierarchyChainOk || !hierarchyRefusalsOk || !hierarchyEdgeOk ||
        !fontCellsOk || !fontMetricsOk ||
        !eventsOrderOk || !eventsUnsubOk || !eventsEdgeOk ||
        !consoleToggleOk || !consoleFeedOk || !consoleSubmitOk ||
        !consoleHistoryOk ||
        !gamepadDeadzoneOk || !gamepadButtonsOk || !gamepadEdgeOk ||
        !gamepadPollOk ||
        !particleSpawnOk || !emitterRateOk || !particleMotionOk ||
        !particleDeathOk || !particleConvertOk ||
        !staticFloorOk || !groundedOk || !wallOk || !ceilingOk ||
        !jumpOk || !coyoteOk || !charDtOk || !staticResolveOk ||
        !sceneByNameOk ||
        !platLevelsOk || !platLandingOk || !platSwitchOk || !platGoalOk ||
        !platClimbOk || !inputEdgesOk) {
        std::cerr << "hostile_data_test: FAILED\n";
        return 1;
    }

    std::cout << "hostile_data_test: PASS\n";
    return 0;
}
