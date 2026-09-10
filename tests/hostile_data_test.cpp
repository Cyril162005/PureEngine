#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "../src/events.h"
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

    if (!validOk || !missingKeyOk || !malformedOk || !emptyListOk || !missingFileOk ||
        !tilemapValidOk || !tilemapMalformedOk || !tilemapCollideOk ||
        !sceneLifecycleOk || !sceneNoOpsOk ||
        !hierarchyChainOk || !hierarchyRefusalsOk || !hierarchyEdgeOk ||
        !fontCellsOk || !fontMetricsOk ||
        !eventsOrderOk || !eventsUnsubOk || !eventsEdgeOk) {
        std::cerr << "hostile_data_test: FAILED\n";
        return 1;
    }

    std::cout << "hostile_data_test: PASS\n";
    return 0;
}
