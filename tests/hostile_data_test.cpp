#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "../src/hostile_data.h"
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

int main() {
    const bool validOk = checkCaseValidData();
    const bool missingKeyOk = checkCaseMissingKey();
    const bool malformedOk = checkCaseMalformedNumber();
    const bool emptyListOk = checkCaseEmptyHostileList();
    const bool missingFileOk = checkCaseMissingFile();
    const bool tilemapValidOk = checkTilemapValidLoad();
    const bool tilemapMalformedOk = checkTilemapMalformedFallsBack();
    const bool tilemapCollideOk = checkTilemapCollidePushesOut();

    if (!validOk || !missingKeyOk || !malformedOk || !emptyListOk || !missingFileOk ||
        !tilemapValidOk || !tilemapMalformedOk || !tilemapCollideOk) {
        std::cerr << "hostile_data_test: FAILED\n";
        return 1;
    }

    std::cout << "hostile_data_test: PASS\n";
    return 0;
}
