/**
 * PureEngine — Step 56: Sprite Animation Data (data structures only)
 * File: animation.h
 *
 * Frame metadata (AnimationFrame), named frame sequences (Animation),
 * and per-entity playback state (AnimationState). DATA ONLY in this
 * step: nothing here touches the renderer, and no Entity member or
 * render path consumes these types yet — that wiring is a later,
 * separately reviewed step.
 *
 * Ownership rule: AnimationState holds a NON-OWNING pointer to its
 * Animation. The game owns Animation objects (globals, level data,
 * or profile structs) and must guarantee two things: (1) an Animation
 * outlives every AnimationState playing it; (2) an Animation's frames
 * vector is never mutated while any state plays it (getCurrentFrame
 * hands out pointers into that vector).
 *
 * Header-only, same discipline as every project module: no
 * animation.cpp, no CMakeLists.txt change.
 */
#ifndef PUREENGINE_ANIMATION_H
#define PUREENGINE_ANIMATION_H
// Include guard, same pattern as every other project header.

#include <cstddef>   // std::size_t — frame counts and loop bound
#include <string>    // Animation::name
#include <vector>    // Animation::frames

namespace pe {

// --- One frame of an animation: which spritesheet cell, for how long ---
struct AnimationFrame {
    int frameIndex;   // spritesheet cell (0, 1, 2, ...)
    float duration;   // display time in seconds; MUST be > 0 — a
                      // zero/negative duration is skipped by update()
                      // (never hangs: the advance loop below is bounded)
};

// --- A named frame sequence (e.g. "walk_left", "jump", "idle") ---
struct Animation {
    std::string name;                 // looked up by the game, never by the engine
    std::vector<AnimationFrame> frames;
    bool loops = true;                // repeat after the last frame?
    float totalDuration = 0.0f;       // sum of frame durations, maintained
                                      // by whoever builds the Animation —
                                      // nothing reads it yet (data step)
};

// --- Runtime playback state for one entity ---
struct AnimationState {
    int currentFrameIndex = 0;              // position inside frames
    float elapsedTime = 0.0f;               // time spent on the current frame
    const Animation* currentAnimation = nullptr;  // non-owning (see rule above)
    bool isPlaying = false;

    // The frame under the playhead, or nullptr when there is nothing
    // playable (no animation, empty list, or index out of range).
    const AnimationFrame* getCurrentFrame() const {
        if (!currentAnimation || currentAnimation->frames.empty()) return nullptr;
        if (currentFrameIndex < 0 ||
            currentFrameIndex >= static_cast<int>(currentAnimation->frames.size())) {
            return nullptr;
        }
        return &currentAnimation->frames[static_cast<std::size_t>(currentFrameIndex)];
    }

    // Advance playback by dt. Returns true only when a NON-looping
    // animation just finished (or had nothing playable, stopping it).
    // The advance loop is bounded to one full pass over the frames per
    // call, so zero-duration frames move the index without hanging.
    bool update(float dt) {
        if (!isPlaying || !currentAnimation) return false;
        if (currentAnimation->frames.empty()) { isPlaying = false; return true; }

        elapsedTime += dt;
        const std::size_t count = currentAnimation->frames.size();
        for (std::size_t steps = 0; steps <= count; ++steps) {
            const AnimationFrame* frame = getCurrentFrame();
            if (!frame) { isPlaying = false; return true; }
            if (frame->duration <= 0.0f) {
                if (advance()) return true;   // no time consumed
                continue;
            }
            if (elapsedTime < frame->duration) return false;
            elapsedTime -= frame->duration;
            if (advance()) return true;
        }
        return false;
    }

private:
    // Step the index once, wrapping or stopping at the end.
    // Returns true when a non-looping animation just finished.
    bool advance() {
        ++currentFrameIndex;
        const std::size_t count = currentAnimation->frames.size();
        if (currentFrameIndex >= static_cast<int>(count)) {
            if (currentAnimation->loops) {
                currentFrameIndex = 0;
                return false;
            }
            isPlaying = false;
            return true;
        }
        return false;
    }
};

} // namespace pe

#endif // PUREENGINE_ANIMATION_H
