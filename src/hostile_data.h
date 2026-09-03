#ifndef PUREENGINE_HOSTILE_DATA_H
#define PUREENGINE_HOSTILE_DATA_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "entity.h"

namespace pe {

struct HostileDefinition {
    float baseSpeed;
    Vec3 spawnPosition;
    float rotationSpeed;
    int textureId = 0;
};

struct HostileDefaults {
    std::vector<HostileDefinition> hostiles{
        {1.8f, Vec3(0.0f, -2.0f, 0.0f), 1.8f, 0},
        {1.6f, Vec3(3.0f, 2.0f, 0.0f), -1.2f, 0},
        {1.5f, Vec3(-3.0f, 2.0f, 0.0f), 2.2f, 0}
    };
    float difficultyRate = 0.01f;
    float maxDifficultyScale = 1.33f;
    float winTime = 120.0f;
};

inline std::string trim(const std::string& value) {
    const std::size_t first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

inline bool parseFloatList(const std::string& text, std::vector<float>& out) {
    std::stringstream stream(text);
    std::string token;
    while (std::getline(stream, token, ',')) {
        const std::string trimmed = trim(token);
        if (trimmed.empty()) {
            return false;
        }
        std::stringstream tokenStream(trimmed);
        float value = 0.0f;
        tokenStream >> value;
        if (tokenStream.fail() || !tokenStream.eof()) {
            return false;
        }
        out.push_back(value);
    }
    return !out.empty();
}

inline std::vector<std::string> hostileDataCandidates(const std::string& fileName) {
    const std::filesystem::path path(fileName);
    if (path.is_absolute()) {
        return { fileName };
    }

    return {
        "assets/" + fileName,
        "../assets/" + fileName,
        "../../assets/" + fileName
    };
}

inline HostileDefaults loadHostileDefaults(const std::string& fileName = "hostile_default.txt") {
    HostileDefaults defaults;

    for (const std::string& candidate : hostileDataCandidates(fileName)) {
        std::ifstream in(candidate);
        if (!in) {
            continue;
        }

        std::vector<HostileDefinition> parsedHostiles;
        float parsedRate = defaults.difficultyRate;
        float parsedCap = defaults.maxDifficultyScale;
        float parsedWinTime = defaults.winTime;
        bool ok = true;

        std::string line;
        while (std::getline(in, line)) {
            const std::string trimmed = trim(line);
            if (trimmed.empty() || trimmed[0] == '#') {
                continue;
            }

            const std::size_t equalsPos = trimmed.find('=');
            if (equalsPos == std::string::npos) {
                std::cerr << "Warning: hostile defaults file contained a malformed line; using built-in defaults." << std::endl;
                ok = false;
                break;
            }

            const std::string key = trim(trimmed.substr(0, equalsPos));
            const std::string value = trim(trimmed.substr(equalsPos + 1));

            if (key == "hostile") {
                std::vector<float> vals;
                if (!parseFloatList(value, vals) || (vals.size() != 4 && vals.size() != 5)) {
                    std::cerr << "Warning: hostile defaults file contained malformed hostile entry; using built-in defaults." << std::endl;
                    ok = false;
                    break;
                }
                const int textureId = (vals.size() == 5) ? static_cast<int>(vals[4]) : 0;
                parsedHostiles.push_back({vals[0], Vec3(vals[1], vals[2], 0.0f), vals[3], textureId});
            } else if (key == "difficulty_rate") {
                std::stringstream stream(value);
                float parsed = 0.0f;
                stream >> parsed;
                if (stream.fail() || !stream.eof()) {
                    std::cerr << "Warning: hostile defaults file contained malformed difficulty_rate; using built-in defaults." << std::endl;
                    ok = false;
                    break;
                }
                parsedRate = parsed;
            } else if (key == "max_difficulty_scale") {
                std::stringstream stream(value);
                float parsed = 0.0f;
                stream >> parsed;
                if (stream.fail() || !stream.eof()) {
                    std::cerr << "Warning: hostile defaults file contained malformed max_difficulty_scale; using built-in defaults." << std::endl;
                    ok = false;
                    break;
                }
                parsedCap = parsed;
            } else if (key == "win_time") {
                std::stringstream stream(value);
                float parsed = 0.0f;
                stream >> parsed;
                if (stream.fail() || !stream.eof()) {
                    std::cerr << "Warning: hostile defaults file contained malformed win_time; using built-in defaults." << std::endl;
                    ok = false;
                    break;
                }
                parsedWinTime = parsed;
            } else {
                std::cerr << "Warning: hostile defaults file contained an unknown key; using built-in defaults." << std::endl;
                ok = false;
                break;
            }
        }

        if (ok && parsedHostiles.empty()) {
            std::cerr << "Warning: hostile defaults file contained no hostile entries; using built-in defaults." << std::endl;
            return defaults;
        }

        if (ok) {
            defaults.hostiles = parsedHostiles;
            defaults.difficultyRate = parsedRate;
            defaults.maxDifficultyScale = parsedCap;
            defaults.winTime = parsedWinTime;
            return defaults;
        }

        std::cerr << "Warning: hostile defaults file was malformed; using built-in defaults." << std::endl;
        return defaults;
    }

    std::cerr << "Warning: hostile defaults file not found; using built-in defaults." << std::endl;
    return defaults;
}

} // namespace pe

#endif // PUREENGINE_HOSTILE_DATA_H
