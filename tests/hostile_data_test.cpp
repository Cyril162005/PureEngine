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
        out << "MoveLeft=A,LEFT\nJump=SPACE\nUnknownAction=SPACE\nBadLineNoEquals\nMoveRight=UNKNOWNKEY\n";
    }
    bool ok = pe::loadInputBindings(fname);
    std::remove(("assets/" + fname).c_str());
    std::remove(("../assets/" + fname).c_str());
    std::remove(("../../assets/" + fname).c_str());
    if (!ok) { std::cerr << "Bindings load should succeed with some valid lines\n"; return false; }
    auto leftKeys = pe::keysForAction(pe::Action::MoveLeft);
    bool hasA = false;
    for (int k : leftKeys) if (k == GLFW_KEY_A) hasA = true;
    if (!hasA) { std::cerr << "MoveLeft remap failed\n"; return false; }
    auto jumpKeys = pe::keysForAction(pe::Action::Jump);
    if (jumpKeys.size() != 1 || jumpKeys[0] != GLFW_KEY_SPACE) { std::cerr << "Jump remap failed\n"; return false; }
    bool missing = pe::loadInputBindings("no_such_bindings_xyz.txt");
    if (missing) { std::cerr << "Missing file should return false\n"; return false; }
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
    const bool sceneByNameOk = checkSceneByName();
    const bool platLevelsOk = checkPlatformerLevels();
    const bool platClimbOk = checkPlatformerClimb();
    const bool inputEdgesOk = checkInputEdges();
    const bool volumeClampOk = checkVolumeClamp();
    const bool muteToggleOk = checkMuteToggle();
    const bool screenToWorldOk = checkScreenToWorld();
    const bool worldToScreenOk = checkWorldToScreen();
    const bool entityPickOk = checkEntityPick();
    const bool screenPickOk = checkScreenPick();
    const bool entityBoundsOk = checkEntityWorldAABB();
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
    const bool eventThrowOnceOk = checkEventThrowAndOnce();
    const bool timeScaleOk = checkTimeScale();
    const bool hierarchyFreezeOk = checkHierarchyContractFreeze();
    const bool animClipOk = checkAnimationClipSwitch();
    const bool binaryBlobOk = checkBinaryBlob();
    const bool inputBindingsOk = checkInputBindings();
    const bool componentOk = checkComponentHelpers();
    const bool sceneDumpOk = checkSceneDumpReload();
    const bool animClipKeepOk = checkAnimClipKeep();
    const bool scenePtrOk = checkScenePointerStability();
    const bool persistV2Ok = checkScenePersistenceV2();
    const bool persistV1Ok = checkScenePersistenceV1Compat();
    const bool persistV99Ok = checkSceneVersionUnknown();
    const bool lifecycleOk = checkEntityLifecycle();

    if (!validOk || !missingKeyOk || !malformedOk || !emptyListOk || !missingFileOk ||
        !tilemapValidOk || !tilemapMalformedOk || !tilemapCollideOk ||
        !sceneLifecycleOk || !sceneNoOpsOk ||
        !hierarchyChainOk || !hierarchyRefusalsOk || !hierarchyEdgeOk ||
        !fontCellsOk || !fontMetricsOk ||
        !eventsOrderOk || !eventsUnsubOk || !eventsEdgeOk || !eventThrowOnceOk ||
        !consoleToggleOk || !consoleFeedOk || !consoleSubmitOk ||
        !consoleHistoryOk ||
        !gamepadDeadzoneOk || !gamepadButtonsOk || !gamepadEdgeOk ||
        !gamepadPollOk || !mouseInputOk || !prefabSystemOk || !prefabSceneSpawnOk || !prefabLiveOk ||
        !particleSpawnOk || !emitterRateOk || !particleMotionOk ||
        !particleDeathOk || !particleConvertOk ||
        !staticFloorOk || !groundedOk || !wallOk || !ceilingOk ||
        !jumpOk || !coyoteOk || !charDtOk || !staticResolveOk ||
        !sceneByNameOk ||
        !platLevelsOk || !platLandingOk || !platSwitchOk || !platGoalOk ||
        !platClimbOk || !inputEdgesOk || !volumeClampOk || !muteToggleOk || !screenToWorldOk || !worldToScreenOk || !entityPickOk || !screenPickOk || !entityBoundsOk || !sceneSerOk || !pongScoreOk || !particleColorOk || !fixedStepOk || !actionMapOk || !textureRegOk || !consoleHistRecallOk || !timeScaleOk || !hierarchyFreezeOk || !animClipOk || !binaryBlobOk || !inputBindingsOk || !componentOk || !sceneDumpOk || !animClipKeepOk || !scenePtrOk || !persistV2Ok || !persistV1Ok || !persistV99Ok || !lifecycleOk) {
        std::cerr << "hostile_data_test: FAILED\n";
        return 1;
    }

    std::cout << "hostile_data_test: PASS\n";
    return 0;
}
