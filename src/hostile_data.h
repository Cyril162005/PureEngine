#ifndef PUREENGINE_HOSTILE_DATA_H
#define PUREENGINE_HOSTILE_DATA_H

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "entity.h"

namespace pe {

struct HostileDefaults {
    std::array<float, 3> baseSpeeds{1.8f, 1.6f, 1.5f};
    std::array<Vec3, 3> spawnPositions{
        Vec3(0.0f, -2.0f, 0.0f),
        Vec3(3.0f, 2.0f, 0.0f),
        Vec3(-3.0f, 2.0f, 0.0f)
    };
    std::array<float, 3> rotationSpeeds{1.8f, -1.2f, 2.2f};
    float difficultyRate = 0.01f;
    float maxDifficultyScale = 1.33f;
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

inline bool parseSpawnPositions(const std::string& text, std::vector<Vec3>& out) {
    std::stringstream stream(text);
    std::string token;
    while (std::getline(stream, token, ';')) {
        const std::string trimmed = trim(token);
        if (trimmed.empty()) {
            return false;
        }
        std::stringstream pairStream(trimmed);
        std::string item;
        std::vector<float> pair;
        while (std::getline(pairStream, item, ',')) {
            const std::string trimmedItem = trim(item);
            if (trimmedItem.empty()) {
                return false;
            }
            std::stringstream valueStream(trimmedItem);
            float value = 0.0f;
            valueStream >> value;
            if (valueStream.fail() || !valueStream.eof()) {
                return false;
            }
            pair.push_back(value);
        }
        if (pair.size() != 2) {
            return false;
        }
        out.push_back(Vec3(pair[0], pair[1], 0.0f));
    }
    return !out.empty();
}

inline std::vector<std::string> hostileDataCandidates() {
    return {
        "assets/hostile_default.txt",
        "../assets/hostile_default.txt",
        "../../assets/hostile_default.txt"
    };
}

inline HostileDefaults loadHostileDefaults() {
    HostileDefaults defaults;

    for (const std::string& candidate : hostileDataCandidates()) {
        std::ifstream in(candidate);
        if (!in) {
            continue;
        }

        std::array<float, 3> parsedSpeeds = defaults.baseSpeeds;
        std::array<float, 3> parsedRotations = defaults.rotationSpeeds;
        std::array<Vec3, 3> parsedSpawns = defaults.spawnPositions;
        float parsedRate = defaults.difficultyRate;
        float parsedCap = defaults.maxDifficultyScale;
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

            if (key == "base_speed" || key == "base_speeds") {
                std::vector<float> vals;
                if (!parseFloatList(value, vals) || vals.size() != 3) {
                    std::cerr << "Warning: hostile defaults file contained malformed base speeds; using built-in defaults." << std::endl;
                    ok = false;
                    break;
                }
                parsedSpeeds = {vals[0], vals[1], vals[2]};
            } else if (key == "rotation_speed" || key == "rotation_speeds") {
                std::vector<float> vals;
                if (!parseFloatList(value, vals) || vals.size() != 3) {
                    std::cerr << "Warning: hostile defaults file contained malformed rotation speeds; using built-in defaults." << std::endl;
                    ok = false;
                    break;
                }
                parsedRotations = {vals[0], vals[1], vals[2]};
            } else if (key == "spawn_points") {
                std::vector<Vec3> points;
                if (!parseSpawnPositions(value, points) || points.size() != 3) {
                    std::cerr << "Warning: hostile defaults file contained malformed spawn_points; using built-in defaults." << std::endl;
                    ok = false;
                    break;
                }
                parsedSpawns = {points[0], points[1], points[2]};
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
            } else {
                std::cerr << "Warning: hostile defaults file contained an unknown key; using built-in defaults." << std::endl;
                ok = false;
                break;
            }
        }

        if (ok) {
            defaults.baseSpeeds = parsedSpeeds;
            defaults.rotationSpeeds = parsedRotations;
            defaults.spawnPositions = parsedSpawns;
            defaults.difficultyRate = parsedRate;
            defaults.maxDifficultyScale = parsedCap;
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
