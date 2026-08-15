/**
 * PureEngine — Step 20: Audio Boundary
 * File: audio.h
 *
 * The engine's seventh SYSTEM boundary. Before Step 20 every miniaudio
 * call lived directly in main.cpp: engine init, the 3-candidate probe
 * for assets/beep.wav, the four-slot pool clone, both round-robin
 * trigger sites, and three copies of the teardown pattern. Step 20
 * gathers the MECHANISM — device, decode, pool, cursor, playback,
 * cleanup — into pe::Audio, and leaves main.cpp the DECISIONS: when a
 * beep should happen (the collision-edge test, the catch), whether a
 * failed init is fatal (it is — same policy as renderer.init()), and
 * all teardown ordering relative to non-audio systems.
 *
 * Trigger semantics, relocated byte-equivalent from Steps 9/10/12:
 *   - ONE round-robin cursor shared by EVERY trigger site — the
 *     collision beep and the GAME_OVER beep advance the SAME cursor,
 *     exactly as the single nextCollisionSound variable always did;
 *   - the claimed slot is rewound to frame 0 only if still playing,
 *     so a ringing beep keeps sounding until its slot comes around
 *     again;
 *   - ma_sound_start hands the sound to the engine's mixing thread
 *     and returns immediately — playback never blocks the frame.
 *
 * What it deliberately is NOT:
 *   - no volume or mixing controls, no streaming, no second asset,
 *     no sound identity/naming system, no per-event sound types.
 *     One asset, four slots, one verb: playNext().
 *
 * Ownership rule (renderer.h precedent): the Audio object owns the
 * ma_engine and every pool slot it created. init() returns bool; on
 * ANY failure everything it allocated is already cleaned up before it
 * returns false (a half-built pool is a bug factory), and calling
 * shutdown() afterwards is a safe no-op. The CALLER decides whether
 * false is fatal and owns the window/GLFW teardown — audio never
 * touches it.
 *
 * Header-only, same discipline as every project module: no audio.cpp,
 * no CMakeLists.txt change. miniaudio stays a static library target —
 * this header only ever calls its API.
 */
#ifndef PUREENGINE_AUDIO_H
#define PUREENGINE_AUDIO_H
// Include guard, same pattern as every other project header.

#include <cstddef>      // std::size_t — pool size and cursor
#include <iostream>     // the slot-clone failure warning (matches the
                        // console-message discipline the init path had)
#include <miniaudio.h>  // the static-lib API — the one layer we do not
                        // write ourselves (Step 9's ruling).

namespace pe {

class Audio {
public:
    // The pool size Steps 9/10 established: 4 independent slots for a
    // one-shot 150 ms beep — comfortably more than any realistic
    // trigger rate.
    static constexpr std::size_t POOL_SIZE = 4;

    Audio() = default;
    // No copies: each Audio owns registered ma_sound slots and an
    // engine — duplicating the owner would double-uninit them. The
    // game holds exactly one instance for its whole lifetime.
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;
    // Teardown insurance: a forgotten shutdown() still releases the
    // device before the object dies.
    ~Audio() { shutdown(); }

    // --- One-time setup: engine + probe + pool ---
    // Starts the miniaudio engine (default device, default sample
    // rate), locates assets/beep.wav through the SAME 3-candidate CWD
    // probe every asset uses (assets/, ../assets/, ../../assets/),
    // decodes it into pool slot 0, and clones the known-good path
    // into slots 1..POOL_SIZE-1. Returns true when the full pool is
    // ready. On ANY failure every partial allocation is cleaned up
    // before returning false — the object is left exactly as if
    // init() had never run, and the caller owns the fatal-exit
    // decision plus all non-audio teardown.
    bool init() {
        if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
            return false;
        }
        engineIsValid = true;

        // Locate the asset: relative paths resolve against the CURRENT
        // WORKING DIRECTORY, which depends on how the exe is launched
        // (repo root, build/, or build/Release/). One level deeper per
        // candidate, same order as every other asset probe.
        const char* candidates[] = {
            "assets/beep.wav",       // run from the repo root (d:\PureEngine)
            "../assets/beep.wav",    // run from build/
            "../../assets/beep.wav"  // run from build/Release/ (exe's own folder)
        };
        // ma_sound_init_from_file DECODES the file (miniaudio's built-in
        // WAV decoder) and registers the sound with the engine, ready
        // for a one-call start later. 0 = flags: default settings — no
        // looping (one shot), no 3D spatialization. The two NULLs skip
        // the optional resource-manager group and fence.
        const char* loadedPath = NULL;
        for (const char* candidate : candidates) {
            if (ma_sound_init_from_file(&engine, candidate, 0, NULL, NULL,
                                        &sounds[0]) == MA_SUCCESS) {
                loadedPath = candidate;
                ++slotsValid;
                break;
            }
        }
        if (loadedPath == NULL) {
            shutdown();
            return false;
        }
        // Fill slots 1..POOL_SIZE-1 from the path that just worked. If
        // any slot fails to init (out of resources, etc.), tear down
        // everything and abort — a half-built pool is a bug factory.
        for (std::size_t i = 1; i < POOL_SIZE; ++i) {
            if (ma_sound_init_from_file(&engine, loadedPath, 0, NULL, NULL,
                                        &sounds[i]) != MA_SUCCESS) {
                std::cerr << "Failed to initialize collision sound pool slot "
                          << i << std::endl;
                shutdown();
                return false;
            }
            ++slotsValid;
        }
        return true;
    }

    // --- Trigger: the ONE shared round-robin playback path ---
    // Claim the NEXT slot, advance the cursor, rewind the slot only if
    // a beep is still ringing in it, then start it. Both game events —
    // the collision edge and the catch — come through THIS call, so
    // they keep rotating through the same four slots exactly as the
    // single pre-Step-20 cursor made them. Non-blocking: the mixing
    // thread plays the sound from here.
    void playNext() {
        if (slotsValid == 0) {
            return;
        }
        ma_sound& sound = sounds[nextSlot];
        nextSlot = (nextSlot + 1) % slotsValid;
        if (ma_sound_is_playing(&sound)) {
            ma_sound_seek_to_pcm_frame(&sound, 0);
        }
        ma_sound_start(&sound);
    }

    // --- Teardown, reverse creation order ---
    // Every initialized pool slot first (each registered WITH the
    // engine), then the engine itself — which stops the mixing thread
    // and closes the OS audio device. Idempotent: safe to call after
    // a failed init(), safe to call twice, and the destructor calls it
    // as insurance. Audio is independent of OpenGL, so its teardown
    // order relative to GL calls does not matter.
    void shutdown() {
        for (std::size_t i = 0; i < slotsValid; ++i) {
            ma_sound_uninit(&sounds[i]);
        }
        slotsValid = 0;
        nextSlot = 0;
        if (engineIsValid) {
            ma_engine_uninit(&engine);
            engineIsValid = false;
        }
    }

private:
    ma_engine engine = {};
    ma_sound sounds[POOL_SIZE] = {};
    // slotsValid is BOTH the count of initialized slots and the
    // round-robin modulus — playNext can never touch an uninitialized
    // slot, even after a failed init().
    std::size_t slotsValid = 0;
    std::size_t nextSlot = 0;
    bool engineIsValid = false;
};

} // namespace pe

#endif // PUREENGINE_AUDIO_H
