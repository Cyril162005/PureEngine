#ifndef PUREENGINE_INPUT_H
#define PUREENGINE_INPUT_H

// =====================================================================
//  PureEngine — Step 16: Input Module Boundary (src/input.h)
// =====================================================================
//  The engine's FOURTH system boundary (after the renderer in Step 13,
//  resource loading in Step 14, and the camera in Step 15). This file
//  OWNS two responsibilities, and nothing else:
//
//    1. KEYBOARD KEY-STATE POLLING — the raw "is this key down RIGHT
//       NOW?" reads (glfwGetKey), relocated whole from main.cpp.
//    2. EDGE DETECTION — the Step 3 pattern (the exact instant a key
//       goes DOWN), which needs memory: the PREVIOUS frame's state for
//       each tracked key. That memory lives here, with a single owner.
//
//  What deliberately does NOT live here:
//    - what a key MEANS in MENU / PLAYING / PAUSED / GAME_OVER —
//      state-specific meaning stays in main.cpp's state switch;
//    - game-state transitions, camera behavior, player movement,
//      deltaTime, gameplay decisions of any kind;
//    - window lifecycle: glfwPollEvents(), glfwWindowShouldClose(),
//      and glfwSetWindowShouldClose() all stay in main.cpp;
//    - any action-mapping system. Keys are identified by their raw
//      GLFW key codes (GLFW_KEY_ESCAPE, GLFW_KEY_SPACE, ...): the
//      boundary hands main.cpp plain booleans, and main.cpp decides.
//
//  The polling model is unchanged: no callbacks, just glfwGetKey
//  queries once per frame. The TEMPORAL ORDER that makes edge
//  detection work is a contract between this class and the frame
//  loop, preserved exactly as Steps 3 and 11 established it:
//
//      glfwPollEvents()            (main.cpp, frame start)
//      isDown / isEdge reads       (this class, no side effects)
//      state switch consumes them  (main.cpp)
//      update()                    (this class, frame end)
//
//  isEdge() READS the previous-frame snapshot but never writes it;
//  update() is the ONLY writer, called once per frame AFTER all edge
//  consumption. That separation is why a held key produces exactly
//  ONE edge event per physical press.
//
//  Header-only, like every project module: no CMakeLists.txt change.
// =====================================================================

#include <GLFW/glfw3.h>   // glfwGetKey, GLFW_PRESS, raw GLFW key codes
#include <vector>         // the tracked-key list and previous-frame state

namespace pe {

class Input {
public:
    // Construct with the keys that need EDGE detection (their
    // previous-frame snapshots are tracked from this moment, all
    // initialized to "not pressed": before the program starts, no
    // key is down). Level-only keys (WASD, arrows) need no tracking
    // at all and are simply not listed.
    explicit Input(std::initializer_list<int> edgeTrackedKeys)
        : trackedKeys(edgeTrackedKeys),
          wasDownLastFrame(edgeTrackedKeys.size(), 0) {}

    // --- Level read: the key's state RIGHT NOW, no memory ---
    // The Step 3/6/8 pattern, relocated whole. Static because it is
    // stateless: two parallel vectors of level reads would agree.
    static bool isDown(GLFWwindow* window, int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    // --- Edge read: pressed NOW and NOT pressed last frame ---
    // READ-only against the previous-frame snapshot: calling this
    // never advances the snapshot (update() alone does that), so the
    // frame loop may read any edge at any point before update().
    // A key that was not registered for edge tracking reports false.
    bool isEdge(GLFWwindow* window, int key) const {
        for (std::size_t i = 0; i < trackedKeys.size(); ++i) {
            if (trackedKeys[i] == key) {
                return isDown(window, key) && !wasDownLastFrame[i];
            }
        }
        return false;
    }

    // --- Frame-end snapshot update ---
    // Call ONCE per frame, AFTER the state switch has consumed the
    // edges. Stores every tracked key's CURRENT level as the next
    // frame's "previous" state. This is the relocated tail of
    // main.cpp's old escWasPressedLastFrame / spaceWasPressedLastFrame
    // bookkeeping, now with a single owner.
    void update(GLFWwindow* window) {
        for (std::size_t i = 0; i < trackedKeys.size(); ++i) {
            wasDownLastFrame[i] = isDown(window, trackedKeys[i]) ? 1 : 0;
        }
    }

private:
    // The keys under edge surveillance (GLFW key codes, C2 ruling:
    // no enum, no mapping — raw codes flow straight through).
    std::vector<int> trackedKeys;
    // Previous-frame state, index-aligned with trackedKeys. char
    // keeps the storage trivial (0/1), matching the engine's
    // collision-flag convention.
    std::vector<char> wasDownLastFrame;
};

} // namespace pe

#endif // PUREENGINE_INPUT_H
