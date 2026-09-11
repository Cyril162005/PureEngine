/**
 * PureEngine — Minimum Platformer Proof (Session 4)
 * File: games/platformer/platformer.cpp
 *
 * An ENGINE VALIDATION GAME, not a product: the smallest platformer that
 * proves the engine without bypassing it. Two tiny tile levels (TITLE ->
 * LEVEL 1 -> LEVEL 2 -> WIN) played through SceneManager; the existing
 * character controller, tilemap, event bus, particles, audio, console,
 * text, gamepad, and hierarchy systems do all the work — no second
 * physics, no UI framework, no state framework beyond gamestate.h.
 *
 * Controls: arrows move, SPACE/W/Up jump, ESC pause, ` console,
 * gamepad stick + A where practical. Goal: walk into the goal zone.
 */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../../src/entity.h"
#include "../../src/collision.h"
#include "../../src/renderer.h"
#include "../../src/camera.h"
#include "../../src/input.h"
#include "../../src/time.h"
#include "../../src/gamestate.h"
#include "../../src/animation_data.h"
#include "../../src/tilemap.h"
#include "../../src/scene.h"
#include "../../src/physics.h"
#include "../../src/events.h"
#include "../../src/console.h"
#include "../../src/gamepad.h"
#include "../../src/particles.h"
#include "../../src/font.h"
#include "../../src/audio.h"

// --- Level data (game-side constants; geometry lives in the files) ---
// Goal rects are world-space zones, not tiles: tiles are all solid, so a
// walk-through goal tile cannot exist in the current format. Tile centers:
// x = col - 4.5, y = 3.5 - row (10x8, size 1.0).
static const float kMoveSpeed = 4.5f;
static const float kJumpImpulse = 7.0f;  // apex ~2.5: clears every rise below
// Spawn y is the exact measured rest height (floor top + world half-height):
// grounded on frame 0 via the tolerance band, zero penetration so no
// first-frame correction. Keeps opening jumps responsive.
static const pe::Vec3 kSpawn1(-3.0f, -2.68f, 0.0f);
static const pe::Vec3 kSpawn2(-3.0f, -2.68f, 0.0f);
static const float kGoal1[4] = {2.25f, -2.25f, 0.75f, 0.75f};  // cx,cy,hw,hh
static const float kGoal2[4] = {2.0f, -1.0f, 0.75f, 0.75f};

static pe::Entity makePlayer(const pe::Vec3& spawn) {
    pe::Entity p(spawn, 0.0f, pe::Vec3(0.8f, 0.8f, 1.0f),
                 pe::Vec3(0.4f, 0.4f, 0.0f), 4);
    p.depth = 3;
    p.roleId = 0;
    p.cols = 8;
    p.rows = 1;  // paddle sheet frames (existing asset, reused honestly)
    p.gravityScale = 1.0f;
    p.jumpImpulse = kJumpImpulse;
    return p;
}

static pe::Entity makeCompanion() {
    // Visual child proving hierarchy render (arcade pattern): role 99 is
    // opaque to every scan; parent 0 is the player by construction order.
    pe::Entity c(pe::Vec3(0.9f, 0.5f, 0.0f), 2.0f,
                 pe::Vec3(0.4f, 0.4f, 1.0f),
                 pe::Vec3(0.3f, 0.3f, 0.0f), 0);
    c.depth = 3;
    c.roleId = 99;
    c.parentIndex = 0;
    return c;
}

static bool overlapsGoal(const pe::Entity& p, const float g[4]) {
    const float dx = p.position.x - g[0];
    const float dy = p.position.y - g[1];
    const float adx = dx >= 0.0f ? dx : -dx;
    const float ady = dy >= 0.0f ? dy : -dy;
    return adx < g[2] + p.halfExtents.x * p.scale.x &&
           ady < g[3] + p.halfExtents.y * p.scale.y;
}

static void spawnBurst(std::vector<pe::Particle>& out, const pe::Vec3& at,
                       int count, float speed, float life) {
    for (int s = 0; s < count; ++s) {
        const float ang =
            static_cast<float>(s) * 2.0f * 3.14159265f / static_cast<float>(count);
        pe::spawnParticle(out, at,
                          pe::Vec3(std::cos(ang) * speed, std::sin(ang) * speed, 0.0f),
                          life, 0.35f, pe::Vec3(1.0f, 1.0f, 1.0f));
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "platformer: glfwInit failed" << std::endl;
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window =
        glfwCreateWindow(800, 600, "PureEngine - Platformer", nullptr, nullptr);
    if (!window) {
        std::cerr << "platformer: window failed" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "platformer: glad failed" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    pe::Audio audio;
    if (!audio.init()) {
        std::cerr << "platformer: audio failed" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    const std::map<std::string, pe::Animation> animations =
        pe::loadAnimations("paddle_animations.txt");
    const auto moveClip = animations.find("paddle_move");
    const auto idleClip = animations.find("paddle_idle");

    // --- Scenes own everything (level entity lists + tilemaps) ---
    // Creation calls come FIRST and bare: each loadScene may reallocate
    // the vector, so NO reference is held across them (a held ref would
    // dangle into moved-from storage — this exact bug ate level1's map
    // print during development). Refs below are taken after all scenes
    // exist, when loadScene is find-only and never reallocates.
    pe::SceneManager scenes;
    pe::loadScene(scenes, "level1");
    pe::loadScene(scenes, "level2");
    // Re-find (never hold across creation): both exist, so these pointers
    // are stable from here on — no further loadScene runs anywhere below.
    pe::Scene* level1 = pe::sceneByName(scenes, "level1");
    pe::Scene* level2 = pe::sceneByName(scenes, "level2");
    if (!level1 || !level2) {
        std::cerr << "platformer: level scenes missing" << std::endl;
        audio.shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    level1->entities = {makePlayer(kSpawn1), makeCompanion()};
    pe::loadTilemapIntoScene(*level1, "platformer_level1.txt");
    level2->entities = {makePlayer(kSpawn2), makeCompanion()};
    pe::loadTilemapIntoScene(*level2, "platformer_level2.txt");
    std::cout << "platformer: level1 ("
              << level1->tilemap.tiles.size() << " cells), level2 ("
              << level2->tilemap.tiles.size() << " cells)" << std::endl;
    pe::switchTo(scenes, "level1");
    pe::Scene* activeScene = pe::currentScene(scenes);
    int levelIndex = 0;
    std::vector<pe::Entity> initial1 = level1->entities;
    std::vector<pe::Entity> initial2 = level2->entities;
    auto resetLevel = [&](int level) {
        levelIndex = level;
        pe::switchTo(scenes, level == 0 ? "level1" : "level2");
        activeScene = pe::currentScene(scenes);
        activeScene->entities = (level == 0 ? initial1 : initial2);
    };

    // --- Events: landing dust + goal fanfare through the bus ---
    // Collision (landing edge) -> dust burst at the feet + soft beep.
    // SceneChanged (goal) -> win sound; the game positions bursts and
    // switches scenes itself (bus payloads are opaque ints by design).
    std::vector<pe::Particle> burst;
    pe::EventBus eventBus;
    eventBus.subscribe(pe::EventType::Collision, [&](const pe::GameEvent&) {
        audio.playNext();
        for (auto& e : activeScene->entities) {
            if (e.roleId == 0) {
                spawnBurst(burst,
                           pe::Vec3(e.position.x, e.position.y - 0.5f, 0.0f),
                           6, 1.5f, 0.4f);
                break;
            }
        }
    });
    eventBus.subscribe(pe::EventType::SceneChanged, [&](const pe::GameEvent& e) {
        audio.playNewHighScore();
        std::cout << "platformer: scene event " << e.a << " -> " << e.b << std::endl;
    });

    pe::Renderer renderer;
    if (!renderer.init()) {
        std::cerr << "platformer: renderer failed" << std::endl;
        audio.shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    pe::Camera camera;
    pe::Input input{GLFW_KEY_ESCAPE, GLFW_KEY_SPACE, GLFW_KEY_GRAVE_ACCENT,
                    GLFW_KEY_ENTER, GLFW_KEY_BACKSPACE, GLFW_KEY_W, GLFW_KEY_UP,
                    GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C, GLFW_KEY_D, GLFW_KEY_E,
                    GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_I, GLFW_KEY_J,
                    GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_M, GLFW_KEY_N, GLFW_KEY_O,
                    GLFW_KEY_P, GLFW_KEY_Q, GLFW_KEY_R, GLFW_KEY_S, GLFW_KEY_T,
                    GLFW_KEY_U, GLFW_KEY_V, GLFW_KEY_X, GLFW_KEY_Y, GLFW_KEY_Z,
                    GLFW_KEY_0, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4,
                    GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8, GLFW_KEY_9,
                    GLFW_KEY_PERIOD, GLFW_KEY_MINUS};
    pe::FrameTime frameTime;
    frameTime.start();
    pe::GameState currentState = pe::GameState::MENU;
    pe::Console console;
    pe::GamepadState prevPad;
    std::vector<char> colliding;
    bool wasInGoal = false;
    bool wasGrounded = false;

    pe::registerCommand(console, "entities", [&](const std::vector<std::string>&) {
        return std::to_string(activeScene->entities.size()) + " entities";
    });
    pe::registerCommand(console, "reset", [&](const std::vector<std::string>&) {
        resetLevel(levelIndex);
        return std::string("level reset");
    });
    pe::registerCommand(console, "pos", [&](const std::vector<std::string>&) {
        for (const auto& e : activeScene->entities) {
            if (e.roleId == 0) {
                return activeScene->name + " " + std::to_string(e.position.x) +
                       " " + std::to_string(e.position.y);
            }
        }
        return std::string("no player");
    });

    while (!glfwWindowShouldClose(window)) {
        const float dt = frameTime.tick();
        glfwPollEvents();

        const bool escDown = pe::Input::isDown(window, GLFW_KEY_ESCAPE);
        const pe::GamepadState curPad = pe::pollGamepad();
        const bool padStart =
            pe::gamepadButtonEdge(prevPad, curPad, GLFW_GAMEPAD_BUTTON_START);
        const bool padJump =
            pe::gamepadButtonEdge(prevPad, curPad, GLFW_GAMEPAD_BUTTON_A);
        bool escEdge = input.isEdge(window, GLFW_KEY_ESCAPE) || padStart;
        bool spaceEdge = input.isEdge(window, GLFW_KEY_SPACE);
        const bool playingLike =
            currentState == pe::GameState::PLAYING ||
            currentState == pe::GameState::PLAYING_ALT ||
            currentState == pe::GameState::PAUSED;
        const bool consoleWasOpen = playingLike && console.open;
        bool consoleAteFrame = false;
        auto pumpConsole = [&]() {
            if (input.isEdge(window, GLFW_KEY_GRAVE_ACCENT)) {
                pe::toggle(console);
            }
            if (!consoleWasOpen) {
                return;
            }
            consoleAteFrame = true;
            if (console.open && escEdge) {
                pe::toggle(console);
            } else if (console.open) {
                const bool shift = pe::Input::isDown(window, GLFW_KEY_LEFT_SHIFT) ||
                                   pe::Input::isDown(window, GLFW_KEY_RIGHT_SHIFT);
                if (spaceEdge) {
                    pe::feedKey(console, GLFW_KEY_SPACE, false);
                }
                if (input.isEdge(window, GLFW_KEY_ENTER)) {
                    const std::string out = pe::submit(console);
                    if (!out.empty()) {
                        std::cout << out << std::endl;
                    }
                }
                if (input.isEdge(window, GLFW_KEY_BACKSPACE)) {
                    pe::feedKey(console, GLFW_KEY_BACKSPACE, false);
                }
                for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; ++key) {
                    if (input.isEdge(window, key)) {
                        pe::feedKey(console, key, shift);
                    }
                }
                for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; ++key) {
                    if (input.isEdge(window, key)) {
                        pe::feedKey(console, key, shift);
                    }
                }
                if (input.isEdge(window, GLFW_KEY_PERIOD)) {
                    pe::feedKey(console, GLFW_KEY_PERIOD, false);
                }
                if (input.isEdge(window, GLFW_KEY_MINUS)) {
                    pe::feedKey(console, GLFW_KEY_MINUS, false);
                }
            }
        };

        switch (currentState) {
        case pe::GameState::MENU:
            if (escDown) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            } else if (spaceEdge) {
                resetLevel(0);
                wasInGoal = false;
                wasGrounded = false;
                currentState = pe::GameState::PLAYING;
                std::cout << "platformer: started level1" << std::endl;
            }
            break;
        case pe::GameState::PLAYING:
        case pe::GameState::PLAYING_ALT:
            pumpConsole();
            if (escEdge && !consoleAteFrame) {
                currentState = pe::GameState::PAUSED;
                std::cout << "platformer: paused" << std::endl;
            }
            break;
        case pe::GameState::PAUSED:
            pumpConsole();
            if (escEdge && !consoleAteFrame) {
                currentState = pe::GameState::PLAYING;
                std::cout << "platformer: resumed" << std::endl;
            } else if (spaceEdge && !consoleAteFrame) {
                currentState = pe::GameState::MENU;
            }
            break;
        case pe::GameState::GAME_OVER:
            if (spaceEdge) {
                currentState = pe::GameState::MENU;
            }
            break;
        case pe::GameState::WIN:
            if (spaceEdge) {
                audio.stopEventSounds();
                currentState = pe::GameState::MENU;
            }
            break;
        }

        if (pe::simulates(currentState)) {
            pe::Entity* player = nullptr;
            for (auto& e : activeScene->entities) {
                if (e.roleId == 0) {
                    player = &e;
                    break;
                }
            }
            if (player) {
                // Horizontal intent: arrows, then stick adds (clamped).
                // Arrows + WASD drive the same axis (A/Left, D/Right);
                // the stick adds below. S/Down has no player action.
                float move = 0.0f;
                if (pe::Input::isDown(window, GLFW_KEY_LEFT) ||
                    pe::Input::isDown(window, GLFW_KEY_A)) {
                    move -= 1.0f;
                }
                if (pe::Input::isDown(window, GLFW_KEY_RIGHT) ||
                    pe::Input::isDown(window, GLFW_KEY_D)) {
                    move += 1.0f;
                }
                if (curPad.connected) {
                    move += curPad.leftX;
                    if (move > 1.0f) {
                        move = 1.0f;
                    }
                    if (move < -1.0f) {
                        move = -1.0f;
                    }
                }
                player->velocity.x = move * kMoveSpeed;
                const bool jumpEdge =
                    input.isEdge(window, GLFW_KEY_SPACE) ||
                    input.isEdge(window, GLFW_KEY_W) ||
                    input.isEdge(window, GLFW_KEY_UP) || padJump;
                if (jumpEdge && player->wasGrounded) {
                    audio.playNext();
                    std::cout << "platformer: jump" << std::endl;
                }
                // Tiles become static bodies for the controller (no logic
                // duplicated: converter builds them, one flag marks them).
                std::vector<pe::Entity> tileStatics =
                    pe::tilemapToEntities(activeScene->tilemap, 0.0f, 3);
                for (auto& t : tileStatics) {
                    t.isStatic = true;
                }
                const bool grounded = pe::updateCharacterController(
                    *player, tileStatics, dt, jumpEdge);
                if (grounded && !wasGrounded) {
                    eventBus.emit(pe::GameEvent{pe::EventType::Collision, 0, -1});
                }
                wasGrounded = grounded;
                // Animation follows motion (Pong pattern).
                const bool moving =
                    player->velocity.x > 0.5f || player->velocity.x < -0.5f;
                const auto& want = moving ? moveClip : idleClip;
                if (want != animations.end() &&
                    player->animationState.currentAnimation != &want->second) {
                    player->animationState.currentAnimation = &want->second;
                    player->animationState.elapsedTime = 0.0f;
                    player->animationState.isPlaying = true;
                }
                if (player->animationState.isPlaying) {
                    player->animationState.update(dt);
                }
                camera.follow(player->position);
                // Goal edge: fanfare + advance (subscriber plays the sound).
                const float* goal = (levelIndex == 0 ? kGoal1 : kGoal2);
                const bool inGoal = overlapsGoal(*player, goal);
                if (inGoal && !wasInGoal) {
                    const int from = levelIndex;
                    if (levelIndex == 0) {
                        levelIndex = 1;
                        pe::switchTo(scenes, "level2");
                        activeScene = pe::currentScene(scenes);
                        camera.follow(activeScene->entities[0].position);
                        eventBus.emit(pe::GameEvent{pe::EventType::SceneChanged, from, 1});
                        std::cout << "platformer: goal level1 -> level2" << std::endl;
                        wasInGoal = false;  // fresh level, fresh edge
                    } else {
                        eventBus.emit(pe::GameEvent{pe::EventType::SceneChanged, from, 2});
                        std::cout << "platformer: YOU WIN" << std::endl;
                        currentState = pe::GameState::WIN;
                        wasInGoal = inGoal;
                    }
                    spawnBurst(burst, player->position, 24, 3.0f, 0.8f);
                } else {
                    wasInGoal = inGoal;
                }
            }
        }

        // Frame-end snapshots, through the boundaries (input.h contract):
        // AFTER every edge read of the frame — dispatch AND simulation.
        // (Once lived between dispatch and sim, which silently killed every
        // sim-block edge: pre-switch reads saw them, post-snapshot reads
        // never did. The spc=1/jmp=0 diagnostic proved it.)
        input.update(window);
        prevPad = curPad;

        if (pe::drawsWorld(currentState) && currentState != pe::GameState::PAUSED) {
            pe::updateParticles(burst, dt);
        }

        pe::ClearColor frameClear = pe::clearColorFor(currentState, false);
        renderer.clear(frameClear.r, frameClear.g, frameClear.b);
        if (pe::drawsWorld(currentState)) {
            // No tint feedback in this game: flags rebuilt zeroed, sized to
            // the scene (entity count never changes — no spawn/remove).
            colliding.assign(activeScene->entities.size(), 0);
            renderer.drawWorld(camera.projection(), camera.view(),
                               activeScene->entities, colliding);
            {
                const std::vector<pe::Entity> tileEntities =
                    pe::tilemapToEntities(activeScene->tilemap, 0.0f, 3);
                if (!tileEntities.empty()) {
                    const std::vector<char> tileClear(tileEntities.size(), 0);
                    renderer.drawWorld(camera.projection(), camera.view(),
                                       tileEntities, tileClear);
                }
            }
            if (!burst.empty()) {
                const std::vector<pe::Entity> sparkEntities =
                    pe::particlesToEntities(burst, 2, 4, 99);
                const std::vector<char> sparkClear(sparkEntities.size(), 0);
                renderer.drawWorld(camera.projection(), camera.view(),
                                   sparkEntities, sparkClear);
            }
        }
        // Labels + console (extended text in-game).
        if (currentState == pe::GameState::MENU) {
            renderer.drawTextString("PLATFORMER", 0.0f, 1.0f, camera.projection(),
                                    pe::TextAlign::Center);
            renderer.drawTextString("ARROWS MOVE SPACE JUMP", 0.0f, 0.0f,
                                    camera.projection(), pe::TextAlign::Center);
        } else if (currentState == pe::GameState::PLAYING ||
                   currentState == pe::GameState::PLAYING_ALT) {
            renderer.drawTextString(levelIndex == 0 ? "LEVEL 1" : "LEVEL 2", -5.75f,
                                    4.05f, camera.projection(), pe::TextAlign::Left);
            pe::drawConsole(renderer, camera.projection(), console);
        } else if (currentState == pe::GameState::PAUSED) {
            renderer.drawTextString("PAUSED", 0.0f, 0.5f, camera.projection(),
                                    pe::TextAlign::Center);
            pe::drawConsole(renderer, camera.projection(), console);
        } else if (currentState == pe::GameState::WIN) {
            renderer.drawTextString("YOU WIN", 0.0f, 0.5f, camera.projection(),
                                    pe::TextAlign::Center);
        }
        glfwSwapBuffers(window);
    }

    renderer.shutdown();
    audio.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
