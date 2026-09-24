/**
 * PureEngine — resources session: resource system contract test (headless)
 * File: resources_test.cpp
 *
 * Dedicated CTest target for the pe::resources boundary (src/resources.h).
 * hostile_data_test already locks the blob/cache/pack half (Step 163
 * checkResourceSystem) alongside its parser focus; this target owns the
 * WHOLE contract as one coherent system, and adds the coverage that was
 * still open there:
 *   - the texture load FAILURE path is headless-safe: all three decodes
 *     missed -> GL name 0, and that path makes NO GL calls (so the test
 *     needs no GL context or window);
 *   - probe-path behavior: a file present at candidate 1 (assets/ of the
 *     CWD) wins; a miss falls through to the deeper candidates (the real
 *     repo assets resolve that way from the build CWD); a directory is
 *     never a successful load (MSVC ifstream.open on a directory fails,
 *     verified 2026-09-22);
 *   - a 0-byte file is a successful load with an empty out;
 *   - a failed load empties `out` even when the caller passed prior
 *     content;
 *   - the cache snapshot contract: cached bytes are not re-read after a
 *     file change until clearBinaryCache(); pack loads never touch the
 *     cache.
 * Cross-candidate probe ORDER (candidate 2 before candidate 3) is
 * guaranteed by construction — every loader shares the same literal
 * candidate list — and is not observable headlessly without writing
 * outside the repo.
 *
 * No game content, no level art: fixtures are runtime-written temp files
 * (same pattern as checkKeysForAllActions) plus the two committed assets
 * the blob tests read (beep.wav, prefabs/enemy.txt).
 */
#include "../src/resources.h"
#include "../src/renderer.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Write a fixture into the CWD's assets/ (candidate 1), falling back to
// ../assets/ (candidate 2) if that directory does not exist yet — same
// pattern as checkKeysForAllActions in hostile_data_test.
static std::string writeFixture(const std::string& name, const std::string& content) {
    std::filesystem::create_directories("assets");
    std::ofstream out("assets/" + name, std::ios::binary);
    if (!out) {
        std::filesystem::create_directories("../assets");
        out.open("../assets/" + name, std::ios::binary);
    }
    if (!out) {
        std::cerr << "Failed to write fixture " << name << "\n";
        return std::string();
    }
    out << content;
    return std::string("assets/") + name;
}

// Remove a fixture from every candidate location the probe could have
// written it to (CWD-relative), so no temp file survives the test.
static void removeFixture(const std::string& name) {
    std::error_code ec;
    std::filesystem::remove("assets/" + name, ec);
    std::filesystem::remove("../assets/" + name, ec);
    std::filesystem::remove("../../assets/" + name, ec);
}

// --- Blob failure: missing file, directory-as-file, out emptied even
// when the caller passed prior content. ---
static bool checkBlobFailure() {
    bool ok = true;
    std::vector<uint8_t> out;
    if (pe::loadBinaryBlob("res_test_no_such.bin", out)) {
        std::cerr << "Missing blob must fail\n";
        ok = false;
    }
    if (!out.empty()) {
        std::cerr << "Failed blob load must leave out empty\n";
        ok = false;
    }
    // Directory instead of file: the probe must treat it as a miss
    // (MSVC ifstream.open on a directory fails — verified), not a
    // successful load of garbage.
    std::error_code ec;
    std::filesystem::create_directories("assets/res_test_dir.bin");
    std::vector<uint8_t> dirOut;
    if (pe::loadBinaryBlob("res_test_dir.bin", dirOut)) {
        std::cerr << "Directory-as-file must fail\n";
        ok = false;
    }
    if (!dirOut.empty()) {
        std::cerr << "Directory-as-file must leave out empty\n";
        ok = false;
    }
    std::filesystem::remove_all("assets/res_test_dir.bin", ec);
    // Prior content in out: a failed load still empties it.
    out.push_back('X');
    out.push_back('Y');
    if (pe::loadBinaryBlob("res_test_no_such.bin", out)) {
        std::cerr << "Missing blob must fail\n";
        ok = false;
    }
    if (!out.empty()) {
        std::cerr << "Failed blob load must empty prior out content\n";
        ok = false;
    }
    return ok;
}

// --- Blob success: real committed assets via the probe fallthrough,
// full-read byte count + content prefix, 0-byte file = success + empty. ---
static bool checkBlobSuccess() {
    // Real asset: whatever CWD the test runs from, the 3-candidate probe
    // must fall through to the committed assets bundle.
    std::vector<uint8_t> txt;
    if (!pe::loadBinaryBlob("prefabs/enemy.txt", txt) || txt.size() < 16) {
        std::cerr << "Blob prefabs/enemy.txt failed\n";
        return false;
    }
    if (std::string(reinterpret_cast<const char*>(txt.data()), 16) != "# PureEngine pre") {
        std::cerr << "Blob full-read prefix mismatch\n";
        return false;
    }
    std::vector<uint8_t> wav;
    if (!pe::loadBinaryBlob("beep.wav", wav) || wav.empty()) {
        std::cerr << "Blob beep.wav failed\n";
        return false;
    }
    // 0-byte file: opens good, tellg()==0, stream stays good -> a
    // successful load with an empty out (MSVC verified).
    std::ofstream empty("assets/res_test_empty.tmp", std::ios::binary);
    if (!empty) {
        std::cerr << "Failed to create 0-byte fixture\n";
        return false;
    }
    empty.close();
    std::vector<uint8_t> zero;
    if (!pe::loadBinaryBlob("res_test_empty.tmp", zero)) {
        std::cerr << "0-byte file must load successfully\n";
        return false;
    }
    if (!zero.empty()) {
        std::cerr << "0-byte file must give empty out\n";
        return false;
    }
    removeFixture("res_test_empty.tmp");
    return true;
}

// --- Probe priority: candidate 1 (assets/ of the CWD) wins when the
// file exists there; a miss falls through (the load then fails, since
// no deeper candidate has the fixture). ---
static bool checkProbePriority() {
    const std::string path = writeFixture("res_test_probe.tmp", "C1");
    if (path.empty()) return false;
    std::vector<uint8_t> out;
    if (!pe::loadBinaryBlob("res_test_probe.tmp", out) || out.size() != 2 ||
        out[0] != 'C' || out[1] != '1') {
        std::cerr << "Candidate 1 (assets/ of CWD) must win the probe\n";
        return false;
    }
    removeFixture("res_test_probe.tmp");
    // Miss on candidate 1: the probe falls through to candidates 2/3,
    // which lack the fixture -> false, out emptied.
    if (pe::loadBinaryBlob("res_test_probe.tmp", out)) {
        std::cerr << "Missing fixture must fail after candidate 1 removal\n";
        return false;
    }
    if (!out.empty()) {
        std::cerr << "Failed probe must empty out\n";
        return false;
    }
    return true;
}

// --- Texture failure path is headless-safe: all three decodes missed
// -> GL name 0, NO GL calls on that path (so this runs with no GL
// context). Both the RGB and RGBA variants honor it. ---
static bool checkTextureFailureHeadless() {
    const char* missing[3] = {
        "res_test_missing_a.png", "res_test_missing_b.png", "res_test_missing_c.png"
    };
    if (pe::loadRgbTexture(missing) != 0) {
        std::cerr << "loadRgbTexture all-miss must return 0\n";
        return false;
    }
    if (pe::loadRgbaTexture(missing) != 0) {
        std::cerr << "loadRgbaTexture all-miss must return 0\n";
        return false;
    }
    return true;
}

// --- Cache: empty start, holds exactly the loaded entries, failure is
// never cached (retry stays possible), returned copy is independent,
// bytes are a SNAPSHOT (not re-read after a file change until clear),
// pack loads never touch the cache, clear empties it. ---
static bool checkCache() {
    bool ok = true;
    pe::clearBinaryCache();
    if (pe::binaryCache().size() != 0) {
        std::cerr << "Cache must start empty after clear\n";
        ok = false;
    }
    std::vector<uint8_t> txt;
    if (!pe::loadBinaryBlob("prefabs/enemy.txt", txt) || txt.empty()) {
        std::cerr << "Blob enemy.txt failed\n";
        return false;
    }
    std::vector<uint8_t> c1;
    if (!pe::loadBinaryBlobCached("prefabs/enemy.txt", c1) || c1 != txt) {
        std::cerr << "Cached enemy.txt failed\n";
        ok = false;
    }
    if (pe::binaryCache().size() != 1) {
        std::cerr << "Cache must hold exactly 1 entry\n";
        ok = false;
    }
    // Failure is never cached: size stays 1, retry stays possible.
    std::vector<uint8_t> miss;
    if (pe::loadBinaryBlobCached("res_test_no_such.bin", miss)) {
        std::cerr << "Cached missing blob must fail\n";
        ok = false;
    }
    if (pe::binaryCache().size() != 1) {
        std::cerr << "Failed load must not enter the cache\n";
        ok = false;
    }
    // Copy independence: mutating the returned copy must not corrupt
    // the cache.
    if (!c1.empty()) c1[0] = 'X';
    std::vector<uint8_t> c2;
    if (!pe::loadBinaryBlobCached("prefabs/enemy.txt", c2) || c2.empty() || c2[0] != '#') {
        std::cerr << "Cache copy must be independent of caller mutation\n";
        ok = false;
    }
    // Snapshot: cached bytes are NOT re-read after a file change until
    // clearBinaryCache().
    const std::string path = writeFixture("res_test_snap.tmp", "V1");
    if (path.empty()) return false;
    std::vector<uint8_t> s1;
    if (!pe::loadBinaryBlobCached("res_test_snap.tmp", s1) || s1.size() != 2 || s1[0] != 'V') {
        std::cerr << "Snapshot first load failed\n";
        return false;
    }
    {
        std::ofstream overwrite(path, std::ios::binary | std::ios::trunc);
        overwrite << "V2-longer";
    }
    std::vector<uint8_t> s2;
    if (!pe::loadBinaryBlobCached("res_test_snap.tmp", s2) || s2 != s1) {
        std::cerr << "Cached bytes must be a snapshot until clear\n";
        ok = false;
    }
    pe::clearBinaryCache();
    std::vector<uint8_t> s3;
    if (!pe::loadBinaryBlobCached("res_test_snap.tmp", s3) || s3 == s1 ||
        std::string(reinterpret_cast<const char*>(s3.data()), s3.size()) != "V2-longer") {
        std::cerr << "After clear, cache must re-read the changed file\n";
        ok = false;
    }
    removeFixture("res_test_snap.tmp");
    // Pack loads never touch the cache.
    pe::clearBinaryCache();
    std::vector<uint8_t> packOut;
    if (!pe::loadPackEntry("res_test_no_such_pack.bin", "prefabs/enemy.txt", packOut) || packOut.empty()) {
        std::cerr << "Pack fallback failed\n";
        return false;
    }
    if (pe::binaryCache().size() != 0) {
        std::cerr << "Pack loads must not enter the cache\n";
        ok = false;
    }
    pe::clearBinaryCache();
    if (pe::binaryCache().size() != 0) {
        std::cerr << "clearBinaryCache must empty the cache\n";
        ok = false;
    }
    return ok;
}

// --- Pack v1: hit = the pack file's own bytes (entryName ignored),
// fallback = entryName as a direct file, both missing = false + empty. ---
static bool checkPack() {
    std::vector<uint8_t> wav;
    if (!pe::loadBinaryBlob("beep.wav", wav) || wav.empty()) {
        std::cerr << "Blob beep.wav failed\n";
        return false;
    }
    std::vector<uint8_t> txt;
    if (!pe::loadBinaryBlob("prefabs/enemy.txt", txt) || txt.empty()) {
        std::cerr << "Blob enemy.txt failed\n";
        return false;
    }
    std::vector<uint8_t> packHit;
    if (!pe::loadPackEntry("beep.wav", "prefabs/enemy.txt", packHit) || packHit != wav) {
        std::cerr << "Pack hit must return the pack file's bytes\n";
        return false;
    }
    std::vector<uint8_t> packFall;
    if (!pe::loadPackEntry("res_test_no_such_pack.bin", "prefabs/enemy.txt", packFall) || packFall != txt) {
        std::cerr << "Pack fallback must return the entry's bytes\n";
        return false;
    }
    std::vector<uint8_t> none;
    if (pe::loadPackEntry("res_test_no_such_pack.bin", "res_test_no_such_entry.bin", none) || !none.empty()) {
        std::cerr << "Pack both-missing must fail with empty out\n";
        return false;
    }
    return true;
}

// --- Texture lifetime contract, headless GL-free half (renderer.h
// registry side). A DEFAULT-CONSTRUCTED pe::Renderer (no init(), no GL
// context — GLAD pointers are unloaded) may safely exercise the public
// paths that make no GL calls: register failure (all-miss -> -1,
// nothing registered) and unloadNonCoreTextures on an empty registry
// (its guards and its clearNonCoreTextures loop over no slots return
// before any GL call). releaseTexture/clearNonCoreTextures are private
// — their core-slot guards are enforced inside those paths and covered
// by the console textures command (main.cpp); the live-context half
// (register success, release of a real slot, destroyAll, the draw-path
// checker fallback for stale ids) is covered by that command plus the
// alive probe.
static bool checkTextureLifetime() {
    // Default construction is GL-free: all names default to 0.
    pe::Renderer renderer;
    if (renderer.textureCount() != 0) {
        std::cerr << "Fresh renderer registry must be empty\n";
        return false;
    }
    // Register failure: all three probes miss -> -1, and the failure
    // path makes NO GL calls (loadRgbTexture all-miss is GL-free —
    // locked above). Nothing enters the registry.
    if (renderer.registerNonCoreTexture("res_test_missing_lifetime.png") != -1) {
        std::cerr << "Register with missing asset must return -1\n";
        return false;
    }
    if (renderer.textureCount() != 0) {
        std::cerr << "Failed register must not grow the registry\n";
        return false;
    }
    // Unload on an empty registry: every guard (core ids, negative,
    // out-of-range) returns BEFORE any GL call — 0 released, and this
    // is safe with no context.
    if (renderer.unloadNonCoreTextures() != 0) {
        std::cerr << "Unload on empty registry must release 0\n";
        return false;
    }
    if (renderer.textureCount() != 0) {
        std::cerr << "Unload must not grow the registry\n";
        return false;
    }
    return true;
}

int main() {
    const bool blobFailureOk = checkBlobFailure();
    const bool blobSuccessOk = checkBlobSuccess();
    const bool probePriorityOk = checkProbePriority();
    const bool textureFailureOk = checkTextureFailureHeadless();
    const bool cacheOk = checkCache();
    const bool packOk = checkPack();
    const bool textureLifetimeOk = checkTextureLifetime();
    pe::clearBinaryCache();

    if (!blobFailureOk || !blobSuccessOk || !probePriorityOk ||
        !textureFailureOk || !cacheOk || !packOk || !textureLifetimeOk) {
        std::cerr << "resources_test: FAILED\n";
        return 1;
    }

    std::cout << "resources_test: PASS\n";
    return 0;
}
