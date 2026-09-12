/**
 * PureEngine — Step 58: Data-Driven Animation Loading
 * File: animation_data.h
 *
 * Loads named animation definitions from a flat text file into
 * std::map<std::string, Animation> (see animation.h for the types).
 * Mirrors hostile_data.h's discipline, not its code: strict,
 * exception-free parsing (no stoi/stof — malformed numbers are
 * warnings, never throws), trimmed comment/blank handling, the same
 * 3-candidate asset probe plus the Step-34 absolute-path guard, and
 * stderr warnings naming the exact problem.
 *
 * Failure contract: missing file -> empty map (warning, no crash);
 * malformed line -> warning, that line skipped, parsing continues.
 * There are no built-in fallback animations: an empty map simply
 * means no entity is assigned a clip.
 *
 * Ownership: the returned map is owned by the CALLER (the game keeps
 * it alive as long as any AnimationState points into it — see the
 * non-owning-pointer rule in animation.h).
 *
 * Header-only, same discipline as every project module: no
 * animation_data.cpp, no CMakeLists.txt change.
 */
#ifndef PUREENGINE_ANIMATION_DATA_H
#define PUREENGINE_ANIMATION_DATA_H
// Include guard, same pattern as every other project header.

#include <filesystem>  // absolute-path guard (Step-34 pattern)
#include <fstream>     // the candidate files
#include <iostream>    // malformed-line / missing-file warnings
#include <map>         // name -> Animation lookup owned by the caller
#include <sstream>     // strict token parsing without exceptions
#include <string>      // names, lines, tokens

#include "animation.h"  // Animation / AnimationFrame (data only)
#include "entity.h"   // Step 93: setClip helper needs Entity

namespace pe {

// --- Local trim (mirrors hostile_data.h; duplicated deliberately) ---
// Including hostile_data.h for two string helpers would couple animation
// loading to hostile data — the worse evil. Whitespace skipped: space,
// tab, CR, LF (files edited on Windows carry \r).
inline std::string animTrim(const std::string& value) {
    const std::size_t first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

// --- Candidate search paths (mirrors hostileDataCandidates) ---
// Bare filename -> assets/, ../assets/, ../../assets/ (same probe every
// asset uses, so the exe works from the repo root, build/, or
// build/Release/). Absolute paths bypass the probe and are used
// directly (Step-34 pattern: temp-dir test files must resolve).
inline std::vector<std::string> animDataCandidates(const std::string& fileName) {
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

// --- Strict int parse without exceptions (mirrors hostile_data.h) ---
// Whole token must convert: trailing garbage ("12x") fails instead of
// silently parsing 12.
inline bool animParseInt(const std::string& text, int& out) {
    std::stringstream stream(text);
    int value = 0;
    stream >> value;
    if (stream.fail() || !stream.eof()) {
        return false;
    }
    out = value;
    return true;
}

// --- Strict float parse without exceptions (mirrors hostile_data.h) ---
inline bool animParseFloat(const std::string& text, float& out) {
    std::stringstream stream(text);
    float value = 0.0f;
    stream >> value;
    if (stream.fail() || !stream.eof()) {
        return false;
    }
    out = value;
    return true;
}

// --- Load named animations: animation=name,count,duration,loops ---
// Example: animation=walk_left,8,0.1,true
// loops accepts true/1 (loop) or false/0 (one-shot); anything else is
// malformed. frameCount must be >= 1; durations may be any float
// (AnimationState::update skips non-positive durations safely).
// Free function (resources.h/lifecycle.h precedent), not a class:
// there is no state to own.
inline std::map<std::string, Animation> loadAnimations(const std::string& fileName = "animation_default.txt") {
    std::map<std::string, Animation> animations;

    for (const std::string& candidate : animDataCandidates(fileName)) {
        std::ifstream in(candidate);
        if (!in) {
            continue;
        }

        std::string line;
        while (std::getline(in, line)) {
            const std::string trimmed = animTrim(line);
            if (trimmed.empty() || trimmed[0] == '#') {
                continue;
            }

            const std::string prefix = "animation=";
            if (trimmed.compare(0, prefix.size(), prefix) != 0) {
                std::cerr << "Warning: animation file contained an unknown entry; skipping line." << std::endl;
                continue;
            }

            std::stringstream fields(trimmed.substr(prefix.size()));
            std::string name, countText, durationText, loopsText;
            if (!std::getline(fields, name, ',') ||
                !std::getline(fields, countText, ',') ||
                !std::getline(fields, durationText, ',') ||
                !std::getline(fields, loopsText, ',')) {
                std::cerr << "Warning: animation file contained a malformed entry; skipping line." << std::endl;
                continue;
            }
            name = animTrim(name);
            if (name.empty()) {
                std::cerr << "Warning: animation file contained an entry with no name; skipping line." << std::endl;
                continue;
            }

            int frameCount = 0;
            float frameDuration = 0.0f;
            if (!animParseInt(animTrim(countText), frameCount) || frameCount < 1) {
                std::cerr << "Warning: animation '" << name << "' has a bad frame count; skipping entry." << std::endl;
                continue;
            }
            if (!animParseFloat(animTrim(durationText), frameDuration)) {
                std::cerr << "Warning: animation '" << name << "' has a bad frame duration; skipping entry." << std::endl;
                continue;
            }
            const std::string loops = animTrim(loopsText);
            bool loop = false;
            if (loops == "true" || loops == "1") {
                loop = true;
            } else if (loops == "false" || loops == "0") {
                loop = false;
            } else {
                std::cerr << "Warning: animation '" << name << "' has a bad loop flag; skipping entry." << std::endl;
                continue;
            }

            Animation anim;
            anim.name = name;
            anim.loops = loop;
            anim.totalDuration = 0.0f;
            for (int i = 0; i < frameCount; ++i) {
                AnimationFrame frame;
                frame.frameIndex = i;
                frame.duration = frameDuration;
                anim.frames.push_back(frame);
                anim.totalDuration += frameDuration;
            }
            animations[anim.name] = anim;  // duplicate names: last wins
        }
        return animations;
    }

    std::cerr << "Warning: animation file not found; no animations loaded." << std::endl;
    return animations;
}

// Step 93: safe clip switch helper — lookup by name, assign without dangling.
// Returns true on switch (or already on that clip), false if name not found.
inline bool setClip(Entity& entity, const std::map<std::string, Animation>& clips, const std::string& name) {
    auto it = clips.find(name);
    if (it == clips.end()) return false;
    if (entity.animationState.currentAnimation == &it->second) return true;
    entity.animationState.currentAnimation = &it->second;
    entity.animationState.currentFrameIndex = 0;
    entity.animationState.elapsedTime = 0.0f;
    entity.animationState.isPlaying = true;
    return true;
}

} // namespace pe

#endif // PUREENGINE_ANIMATION_DATA_H
