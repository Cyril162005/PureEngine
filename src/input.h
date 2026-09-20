/**
 * =====================================================================
 *  PureEngine — Step 16: Input Module Boundary (src/input.h)
 * =====================================================================
 *  The engine's FOURTH system boundary (after the renderer in Step 13,
 *  resource loading in Step 14, and the camera in Step 15). This file
 *  OWNS two responsibilities, and nothing else:
 *
 *    1. KEYBOARD KEY-STATE POLLING — the raw "is this key down RIGHT
 *       NOW?" reads (glfwGetKey), relocated whole from main.cpp.
 *    2. EDGE DETECTION — the Step 3 pattern (the exact instant a key
 *       goes DOWN), which needs memory: the PREVIOUS frame's state for
 *       each tracked key. That memory lives here, with a single owner.
 *
 *  What deliberately does NOT live here:
 *    - what a key MEANS in MENU / PLAYING / PAUSED / GAME_OVER —
 *      state-specific meaning stays in main.cpp's state switch;
 *    - game-state transitions, camera behavior, player movement,
 *      deltaTime, gameplay decisions of any kind;
 *    - window lifecycle: glfwPollEvents(), glfwWindowShouldClose(),
 *      and glfwSetWindowShouldClose() all stay in main.cpp;
 *    - any action-mapping system. Keys are identified by their raw
 *      GLFW key codes (GLFW_KEY_ESCAPE, GLFW_KEY_SPACE, ...): the
 *      boundary hands main.cpp plain booleans, and main.cpp decides.
 *
 *  The polling model is unchanged: no callbacks, just glfwGetKey
 *  queries once per frame. The TEMPORAL ORDER that makes edge
 *  detection work is a contract between this class and the frame
 *  loop, preserved exactly as Steps 3 and 11 established it:
 *
 *      glfwPollEvents()            (main.cpp, frame start)
 *      isDown / isEdge reads       (this class, no side effects)
 *      state switch consumes them  (main.cpp)
 *      update()                    (this class, frame end)
 *
 *  isEdge() READS the previous-frame snapshot but never writes it;
 *  update() is the ONLY writer, called once per frame AFTER all edge
 *  consumption. That separation is why a held key produces exactly
 *  ONE edge event per physical press.
 *
 *  Header-only, like every project module: no CMakeLists.txt change.
 *
 *  Step 117 adds a THIRD responsibility on the same pattern: MOUSE
 *  SNAPSHOT POLLING (position + left/right buttons), following the
 *  gamepad.h pure-helper style so the edge logic is testable without
 *  hardware. Games opt in; keyboard/action paths are untouched.
 * =====================================================================
 */

#ifndef PUREENGINE_INPUT_H
#define PUREENGINE_INPUT_H

#include <GLFW/glfw3.h>   // glfwGetKey, GLFW_PRESS, raw GLFW key codes
#include <vector>         // the tracked-key list and previous-frame state
#include <string>         // Step 105: action/key names
#include <map>            // Step 105: override table (ordered, no hash needed)
#include <unordered_map>  // Step 115: parsed table in loadInputBindings (was transitive-only via MSVC)
#include <cctype>         // Step 105: key name parsing
#include <fstream>        // Step 105: bindings file probe
#include <iostream>       // Step 105: unknown action/key warnings
#include <sstream>        // Step 105: line parsing

namespace pe {

// Step 88: minimal action map — thin layer over raw GLFW codes.
// Raw isDown/isEdge remain; games may opt into isAction* for cleaner code.
enum class Action {
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Jump,
    Pause,
    Confirm,
    Back
};

inline std::map<Action, std::vector<int>>& actionOverrides() {
    static std::map<Action, std::vector<int>> m;
    return m;
}

inline std::vector<int> keysForAction(Action a) {
    auto& ov = actionOverrides();
    auto it = ov.find(a);
    if (it != ov.end()) return it->second;
    switch (a) {
        case Action::MoveLeft:  return {GLFW_KEY_A, GLFW_KEY_LEFT};
        case Action::MoveRight: return {GLFW_KEY_D, GLFW_KEY_RIGHT};
        case Action::MoveUp:    return {GLFW_KEY_W, GLFW_KEY_UP};
        case Action::MoveDown:  return {GLFW_KEY_S, GLFW_KEY_DOWN};
        case Action::Jump:      return {GLFW_KEY_SPACE, GLFW_KEY_W, GLFW_KEY_UP};
        case Action::Pause:     return {GLFW_KEY_ESCAPE};
        case Action::Confirm:   return {GLFW_KEY_SPACE, GLFW_KEY_ENTER};
        case Action::Back:      return {GLFW_KEY_ESCAPE, GLFW_KEY_BACKSPACE};
        default: return {};
    }
}

// Step 157: every key any action can hold (defaults + overrides),
// deduplicated, first-seen order. Rebind-safety for edge tracking:
// isActionEdge only fires for keys registered at Input construction,
// so a game that edge-tracks keysForAllActions() keeps firing after
// any loadInputBindings remap — the silent untracked-key gap closes.
// Pure table read: no GLFW, no file I/O, no mutation.
inline std::vector<int> keysForAllActions() {
    static const Action all[] = {
        Action::MoveLeft, Action::MoveRight, Action::MoveUp, Action::MoveDown,
        Action::Jump, Action::Pause, Action::Confirm, Action::Back
    };
    std::vector<int> out;
    for (Action a : all) {
        for (int k : keysForAction(a)) {
            bool seen = false;
            for (int e : out) { if (e == k) { seen = true; break; } }
            if (!seen) out.push_back(k);
        }
    }
    return out;
}

inline int keyNameToGLFW(const std::string& name) {
    std::string n;
    n.reserve(name.size());
    for (char c : name) n.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    if (n == "A") return GLFW_KEY_A; if (n == "B") return GLFW_KEY_B; if (n == "C") return GLFW_KEY_C;
    if (n == "D") return GLFW_KEY_D; if (n == "E") return GLFW_KEY_E; if (n == "F") return GLFW_KEY_F;
    if (n == "G") return GLFW_KEY_G; if (n == "H") return GLFW_KEY_H; if (n == "I") return GLFW_KEY_I;
    if (n == "J") return GLFW_KEY_J; if (n == "K") return GLFW_KEY_K; if (n == "L") return GLFW_KEY_L;
    if (n == "M") return GLFW_KEY_M; if (n == "N") return GLFW_KEY_N; if (n == "O") return GLFW_KEY_O;
    if (n == "P") return GLFW_KEY_P; if (n == "Q") return GLFW_KEY_Q; if (n == "R") return GLFW_KEY_R;
    if (n == "S") return GLFW_KEY_S; if (n == "T") return GLFW_KEY_T; if (n == "U") return GLFW_KEY_U;
    if (n == "V") return GLFW_KEY_V; if (n == "W") return GLFW_KEY_W; if (n == "X") return GLFW_KEY_X;
    if (n == "Y") return GLFW_KEY_Y; if (n == "Z") return GLFW_KEY_Z;
    if (n == "0") return GLFW_KEY_0; if (n == "1") return GLFW_KEY_1; if (n == "2") return GLFW_KEY_2;
    if (n == "3") return GLFW_KEY_3; if (n == "4") return GLFW_KEY_4; if (n == "5") return GLFW_KEY_5;
    if (n == "6") return GLFW_KEY_6; if (n == "7") return GLFW_KEY_7; if (n == "8") return GLFW_KEY_8;
    if (n == "9") return GLFW_KEY_9;
    if (n == "LEFT") return GLFW_KEY_LEFT; if (n == "RIGHT") return GLFW_KEY_RIGHT;
    if (n == "UP") return GLFW_KEY_UP; if (n == "DOWN") return GLFW_KEY_DOWN;
    if (n == "SPACE") return GLFW_KEY_SPACE; if (n == "ESCAPE") return GLFW_KEY_ESCAPE; if (n == "ESC") return GLFW_KEY_ESCAPE;
    if (n == "ENTER") return GLFW_KEY_ENTER; if (n == "GRAVE") return GLFW_KEY_GRAVE_ACCENT; if (n == "GRAVE_ACCENT") return GLFW_KEY_GRAVE_ACCENT;
    if (n == "BACKSPACE") return GLFW_KEY_BACKSPACE; if (n == "PERIOD") return GLFW_KEY_PERIOD; if (n == "MINUS") return GLFW_KEY_MINUS;
    // Step 155: additive widening — modifiers, whitespace, punctuation,
    // function keys. Every name above is unchanged byte-for-byte; only a
    // name that previously returned -1 can now map to a real key.
    if (n == "TAB") return GLFW_KEY_TAB;
    if (n == "SHIFT") return GLFW_KEY_LEFT_SHIFT; if (n == "LEFT_SHIFT") return GLFW_KEY_LEFT_SHIFT; if (n == "RIGHT_SHIFT") return GLFW_KEY_RIGHT_SHIFT;
    if (n == "CTRL") return GLFW_KEY_LEFT_CONTROL; if (n == "CONTROL") return GLFW_KEY_LEFT_CONTROL;
    if (n == "LEFT_CTRL") return GLFW_KEY_LEFT_CONTROL; if (n == "RIGHT_CTRL") return GLFW_KEY_RIGHT_CONTROL;
    if (n == "LEFT_CONTROL") return GLFW_KEY_LEFT_CONTROL; if (n == "RIGHT_CONTROL") return GLFW_KEY_RIGHT_CONTROL;
    if (n == "ALT") return GLFW_KEY_LEFT_ALT; if (n == "LEFT_ALT") return GLFW_KEY_LEFT_ALT; if (n == "RIGHT_ALT") return GLFW_KEY_RIGHT_ALT;
    if (n == "COMMA") return GLFW_KEY_COMMA; if (n == "SLASH") return GLFW_KEY_SLASH; if (n == "BACKSLASH") return GLFW_KEY_BACKSLASH;
    if (n == "SEMICOLON") return GLFW_KEY_SEMICOLON; if (n == "APOSTROPHE") return GLFW_KEY_APOSTROPHE;
    if (n == "EQUAL") return GLFW_KEY_EQUAL; if (n == "LEFT_BRACKET") return GLFW_KEY_LEFT_BRACKET; if (n == "RIGHT_BRACKET") return GLFW_KEY_RIGHT_BRACKET;
    if (n == "CAPS_LOCK") return GLFW_KEY_CAPS_LOCK;
    if (n == "F1") return GLFW_KEY_F1; if (n == "F2") return GLFW_KEY_F2; if (n == "F3") return GLFW_KEY_F3;
    if (n == "F4") return GLFW_KEY_F4; if (n == "F5") return GLFW_KEY_F5; if (n == "F6") return GLFW_KEY_F6;
    if (n == "F7") return GLFW_KEY_F7; if (n == "F8") return GLFW_KEY_F8; if (n == "F9") return GLFW_KEY_F9;
    if (n == "F10") return GLFW_KEY_F10; if (n == "F11") return GLFW_KEY_F11; if (n == "F12") return GLFW_KEY_F12;
    return -1;
}

inline bool loadInputBindings(const std::string& filename) {
    const std::string candidates[3] = {std::string("assets/") + filename, std::string("../assets/") + filename, std::string("../../assets/") + filename};
    std::ifstream in;
    for (int k = 0; k < 3; ++k) { in.open(candidates[k]); if (in) break; in.clear(); }
    if (!in) return false; // missing → keep defaults
    auto trim = [](std::string s) -> std::string {
        std::size_t a = s.find_first_not_of(" \t\r\n");
        if (a == std::string::npos) return "";
        std::size_t b = s.find_last_not_of(" \t\r\n");
        return s.substr(a, b - a + 1);
    };
    auto toLower = [](std::string s) {
        for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    };
    std::string line;
    bool anyOk = false;
    std::unordered_map<Action, std::vector<int>> parsed;
    while (std::getline(in, line)) {
        std::string t = trim(line);
        if (t.empty() || t[0] == '#') continue;
        auto eq = t.find('=');
        if (eq == std::string::npos) { std::cerr << "input_bindings: malformed line (no '='): " << t << "\n"; continue; }
        std::string actName = trim(t.substr(0, eq));
        std::string keysStr = trim(t.substr(eq + 1));
        if (actName.empty() || keysStr.empty()) { std::cerr << "input_bindings: malformed line (empty action/keys): " << t << "\n"; continue; }
        std::string lowerAct;
        lowerAct.reserve(actName.size());
        for (char c : actName) lowerAct.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        Action act;
        bool known = true;
        if (lowerAct == "moveleft") act = Action::MoveLeft;
        else if (lowerAct == "moveright") act = Action::MoveRight;
        else if (lowerAct == "moveup") act = Action::MoveUp;
        else if (lowerAct == "movedown") act = Action::MoveDown;
        else if (lowerAct == "jump") act = Action::Jump;
        else if (lowerAct == "pause") act = Action::Pause;
        else if (lowerAct == "confirm") act = Action::Confirm;
        else if (lowerAct == "back") act = Action::Back;
        else if (lowerAct == "console") act = Action::Back; // alias
        else { std::cerr << "input_bindings: unknown action '" << actName << "'\n"; continue; }
        std::vector<int> keys;
        std::stringstream ss(keysStr);
        std::string tok;
        while (std::getline(ss, tok, ',')) {
            std::string kn = trim(tok);
            if (kn.empty()) continue;
            int k = keyNameToGLFW(kn);
            if (k < 0) { std::cerr << "input_bindings: unknown key '" << kn << "'\n"; continue; }
            keys.push_back(k);
        }
        if (keys.empty()) { std::cerr << "input_bindings: no valid keys for action '" << actName << "'\n"; continue; }
        parsed[act] = keys;
        anyOk = true;
    }
    if (!anyOk) return false;
    // Commit parsed overrides atomically
    auto& ov = actionOverrides();
    for (auto& kv : parsed) ov[kv.first] = kv.second;
    return true;
}

// Step 154: reset half of the rebind lifecycle (load -> remap -> reset).
// Clears the override table so keysForAction hands back the Step 88
// defaults again. Pure table clear: no GLFW calls, no file I/O. Games
// that never rebind never need this; tests use it to prove the
// lifecycle round-trips.
inline void resetActionOverrides() {
    actionOverrides().clear();
}

// --- Step 117: mouse snapshot (gamepad.h pattern) ---
// Plain snapshot struct: window-coordinate position plus left/right
// button levels. Left + right only — no middle/other buttons, no
// scroll, no cursor management. Games opt in; existing keyboard /
// action / gamepad paths are untouched.
struct MouseState {
    float x = 0.0f, y = 0.0f;      // window coords (GLFW cursor position)
    bool left = false;             // GLFW_MOUSE_BUTTON_LEFT
    bool right = false;            // GLFW_MOUSE_BUTTON_RIGHT
};

// Pure level read of one button in a snapshot (gamepadButton
// precedent). Only GLFW_MOUSE_BUTTON_LEFT and GLFW_MOUSE_BUTTON_RIGHT
// are meaningful; every other button code reports false. Directly
// testable without GLFW hardware.
inline bool mouseButtonDown(const MouseState& state, int button) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) return state.left;
    if (button == GLFW_MOUSE_BUTTON_RIGHT) return state.right;
    return false;
}

// Pure rising-edge read across two snapshots: down NOW and NOT down
// in prev. The owner rotates prev/cur once per update(); this helper
// never mutates anything.
inline bool mouseButtonEdge(const MouseState& prev,
                            const MouseState& curr, int button) {
    return mouseButtonDown(curr, button) && !mouseButtonDown(prev, button);
}

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

    // --- Step 88: action reads (thin mapping over raw keys) ---
    // isActionDown: any mapped key down now. isActionEdge: any mapped
    // key edge this frame. Raw isDown/isEdge remain unchanged.
    bool isActionDown(GLFWwindow* window, Action a) const {
        for (int k : keysForAction(a)) if (isDown(window, k)) return true;
        return false;
    }
    bool isActionEdge(GLFWwindow* window, Action a) const {
        for (int k : keysForAction(a)) if (isEdge(window, k)) return true;
        return false;
    }

    // --- Step 117: mouse snapshot poll (gamepad.h precedent) ---
    // One live poll of position + left/right levels. Static because it
    // is stateless: the caller decides what to do with the snapshot.
    // Never crashes on any window state GLFW accepts.
    static MouseState pollMouse(GLFWwindow* window) {
        MouseState s;
        double px = 0.0, py = 0.0;
        glfwGetCursorPos(window, &px, &py);
        s.x = static_cast<float>(px);
        s.y = static_cast<float>(py);
        s.left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        s.right = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        return s;
    }

    // --- Step 117: mouse reads (snapshot cadence, gamepad-style) ---
    // These reflect the most recent update() poll, not this frame's
    // hardware state — the same expressive power as the gamepad path.
    // For a same-frame live read, use pollMouse(window) directly.
    bool mouseDown(int button) const {
        return mouseButtonDown(curMouse, button);
    }
    bool mouseEdge(int button) const {
        return mouseButtonEdge(prevMouse, curMouse, button);
    }
    float mouseX() const { return curMouse.x; }
    float mouseY() const { return curMouse.y; }

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
        // Step 117: rotate the mouse snapshot after the keyboard
        // snapshot. curMouse becomes this frame's poll; prevMouse the
        // one before it — mouseEdge() reads that pair.
        prevMouse = curMouse;
        curMouse = pollMouse(window);
    }

private:
    // The keys under edge surveillance (GLFW key codes, C2 ruling:
    // no enum, no mapping — raw codes flow straight through).
    std::vector<int> trackedKeys;
    // Previous-frame state, index-aligned with trackedKeys. char
    // keeps the storage trivial (0/1), matching the engine's
    // collision-flag convention.
    std::vector<char> wasDownLastFrame;
    // Step 117: mouse snapshots, rotated once per update() call.
    MouseState curMouse;
    MouseState prevMouse;
};

} // namespace pe

#endif // PUREENGINE_INPUT_H
