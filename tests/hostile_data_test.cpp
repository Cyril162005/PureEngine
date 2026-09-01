#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "../src/hostile_data.h"

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

int main() {
    const bool validOk = checkCaseValidData();
    const bool missingKeyOk = checkCaseMissingKey();
    const bool malformedOk = checkCaseMalformedNumber();
    const bool emptyListOk = checkCaseEmptyHostileList();
    const bool missingFileOk = checkCaseMissingFile();

    if (!validOk || !missingKeyOk || !malformedOk || !emptyListOk || !missingFileOk) {
        std::cerr << "hostile_data_test: FAILED\n";
        return 1;
    }

    std::cout << "hostile_data_test: PASS\n";
    return 0;
}
