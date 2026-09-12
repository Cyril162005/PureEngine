/**
 * Pong API-gate scaffold — NOT a game yet.
 *
 * Purpose: prove a second game builds and runs using ONLY public engine
 * headers. Public set used here: math (via entity.h), entity.h,
 * renderer.h, camera.h, input.h. Deliberately NOT included (game-side):
 * lifecycle.h, hostile_data.h, gamestate.h, simulation.h, audio.h, ui.h.
 *
 * Includes are relative paths, never -Isrc: adding src/ to the include
 * path risks src/time.h shadowing the CRT time.h (see CMakeLists.txt
 * note at the hostile_data_test target).
 *
 * Current scope: window + renderer init + two paddles + ball with
 * wall/paddle bounce + ESC to quit. No score yet — arrives as its own
 * reviewed step.
 */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>      // Pong clip library (name -> Animation)
#include <vector>

#include "../../src/entity.h"
#include "../../src/collision.h"
#include "../../src/renderer.h"
#include "../../src/camera.h"
#include "../../src/input.h"
#include "../../src/time.h"
#include "../../src/animation_data.h"

int main() {
    // 1. Initialize GLFW (window lifecycle stays game-side per input.h contract)
    if (!glfwInit()) {
        std::cerr << "Pong: Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 2. Configure GLFW (mirrors main.cpp: OpenGL 3.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    // 3. Create the window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Pong - PureEngine API gate", NULL, NULL);
    if (!window) {
        std::cerr << "Pong: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 4. Context + GLAD loader
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Pong: Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // 5. Renderer boundary (public API): init or die
    pe::Renderer renderer;
    if (!renderer.init()) {
        std::cerr << "Pong: renderer init failed" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // 6. Camera boundary (public API): default 12x9 ortho view
    pe::Camera camera;

    // 7. Pong entities: two paddles + one ball. Game-owned vector and
    //    game-owned indices — the engine never interprets positions.
    //    roleIds are distinct but unused by the engine (drawWorld takes
    //    no roles since Step 55); textureIds pick renderer slots.
    std::vector<pe::Entity> entities;

    // --- Step 59B: paddle clip library (game-owned, outlives entities) ---
    // Bare filename: loadAnimations() probes assets/ itself.
    const std::map<std::string, pe::Animation> paddleClips =
        pe::loadAnimations("paddle_animations.txt");

    pe::Entity paddleL;
    paddleL.position = pe::Vec3(-5.0f, 0.0f, 0.0f);   // inside the +/-6 view box
    paddleL.scale = pe::Vec3(0.5f, 2.0f, 1.0f);       // thin tall paddle
    paddleL.roleId = 0;
    paddleL.textureId = 4;   // slot 4 = paddle sheet (NOT slots 0/1:
                             // 16px singles would slice into slivers)
    paddleL.cols = 8;        // 8x1 sheet grid
    paddleL.rows = 1;
    entities.push_back(paddleL);

    pe::Entity paddleR;
    paddleR.position = pe::Vec3(5.0f, 0.0f, 0.0f);
    paddleR.scale = pe::Vec3(0.5f, 2.0f, 1.0f);
    paddleR.roleId = 1;
    paddleR.textureId = 4;   // slot 4 = paddle sheet
    paddleR.cols = 8;
    paddleR.rows = 1;
    entities.push_back(paddleR);

    pe::Entity ball;
    ball.position = pe::Vec3(0.0f, 0.0f, 0.0f);
    ball.scale = pe::Vec3(0.3f, 0.3f, 1.0f);
    ball.roleId = 2;
    ball.textureId = 2;      // slot 2 = crimson
    entities.push_back(ball);
    std::vector<char> colliding(entities.size(), 0);

    pe::Vec3 ballVelocity(3.0f, 2.0f, 0.0f);   // world units/s, game-side state
    const float paddleSpeed = 3.0f;
    const float paddleLimitY = 3.0f;           // keeps paddles on screen
    int leftScore = 0, rightScore = 0;
    const int winScore = 5;
    bool win = false;
    int winner = 0; // 0 none, 1 left, 2 right

    // 8. Main loop: poll -> input -> simulate -> draw -> swap
    pe::FrameTime frameTime;
    frameTime.start();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (pe::Input::isDown(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        // Win reset: SPACE or R restarts after win
        if (win && (pe::Input::isDown(window, GLFW_KEY_SPACE) || pe::Input::isDown(window, GLFW_KEY_R))) {
            leftScore = 0; rightScore = 0; win = false; winner = 0;
            entities[2].position = pe::Vec3(0.0f, 0.0f, 0.0f);
            ballVelocity = pe::Vec3(3.0f, 2.0f, 0.0f);
        }
        const float dt = frameTime.tick();

        if (!win) {
        // Paddles (game-owned indices 0/1), clamped to the view
        if (pe::Input::isDown(window, GLFW_KEY_W)) { entities[0].position.y += paddleSpeed * dt; }
        if (pe::Input::isDown(window, GLFW_KEY_S)) { entities[0].position.y -= paddleSpeed * dt; }
        if (pe::Input::isDown(window, GLFW_KEY_UP)) { entities[1].position.y += paddleSpeed * dt; }
        if (pe::Input::isDown(window, GLFW_KEY_DOWN)) { entities[1].position.y -= paddleSpeed * dt; }
        for (int i = 0; i < 2; ++i) {
            if (entities[i].position.y > paddleLimitY) { entities[i].position.y = paddleLimitY; }
            if (entities[i].position.y < -paddleLimitY) { entities[i].position.y = -paddleLimitY; }
        }
        // --- Step 59B: paddle clips follow paddle motion ---
        // Moving paddle -> paddle_move clip; still paddle -> paddle_idle.
        // Single find per paddle (no double lookup); re-pointing at the
        // same clip preserves elapsedTime (no restart jitter).
        {
            const bool leftMoves = pe::Input::isDown(window, GLFW_KEY_W) ||
                                   pe::Input::isDown(window, GLFW_KEY_S);
            const auto moveClip = paddleClips.find("paddle_move");
            const auto idleClip = paddleClips.find("paddle_idle");
            const auto* want = leftMoves
                ? (moveClip != paddleClips.end() ? &moveClip->second : nullptr)
                : (idleClip != paddleClips.end() ? &idleClip->second : nullptr);
            if (want) {
                entities[0].animationState.currentAnimation = want;
                entities[0].animationState.isPlaying = true;
            }
        }
        {
            const bool rightMoves = pe::Input::isDown(window, GLFW_KEY_UP) ||
                                    pe::Input::isDown(window, GLFW_KEY_DOWN);
            const auto moveClip = paddleClips.find("paddle_move");
            const auto idleClip = paddleClips.find("paddle_idle");
            const auto* want = rightMoves
                ? (moveClip != paddleClips.end() ? &moveClip->second : nullptr)
                : (idleClip != paddleClips.end() ? &idleClip->second : nullptr);
            if (want) {
                entities[1].animationState.currentAnimation = want;
                entities[1].animationState.isPlaying = true;
            }
        }

        // Ball: discrete integrate, wall bounce, paddle bounce (game-side
        // response via public aabbOverlap), reset past the side edges
        entities[2].position = entities[2].position + ballVelocity * dt;
        if (entities[2].position.y > 4.3f || entities[2].position.y < -4.3f) {
            ballVelocity.y *= -1.0f;
        }
        if (pe::aabbOverlap(entities[2], entities[0]) && ballVelocity.x < 0.0f) {
            ballVelocity.x *= -1.0f;
            entities[2].position.x = entities[0].position.x + 0.6f;   // clear overlap
        }
        if (pe::aabbOverlap(entities[2], entities[1]) && ballVelocity.x > 0.0f) {
            ballVelocity.x *= -1.0f;
            entities[2].position.x = entities[1].position.x - 0.6f;   // clear overlap
        }
        // Score: ball past side edge → point for opposite side, reset ball, win check
        if (entities[2].position.x < -7.0f) {
            ++rightScore;
            entities[2].position = pe::Vec3(0.0f, 0.0f, 0.0f);
            ballVelocity.x = 3.0f; ballVelocity.y = 2.0f;
            if (rightScore >= winScore) { win = true; winner = 2; }
        } else if (entities[2].position.x > 7.0f) {
            ++leftScore;
            entities[2].position = pe::Vec3(0.0f, 0.0f, 0.0f);
            ballVelocity.x = -3.0f; ballVelocity.y = 2.0f;
            if (leftScore >= winScore) { win = true; winner = 1; }
        }
        } // end !win

        // --- Step 59B: tick playing clips (ball idles on frame 0: not playing) ---
        for (pe::Entity& entity : entities) {
            if (entity.animationState.isPlaying) {
                entity.animationState.update(dt * entity.animationSpeed);
            }
        }

        renderer.clear(0.0f, 0.0f, 0.0f);
        renderer.drawWorld(camera.projection(), camera.view(), entities, colliding);
        // Scores HUD (existing digit path) + win label
        {
            std::string ls = std::to_string(leftScore);
            std::string rs = std::to_string(rightScore);
            renderer.drawDigitString(ls, -2.0f, 4.0f, camera.projection());
            renderer.drawDigitString(rs, 1.5f, 4.0f, camera.projection());
            if (win) {
                std::string msg = (winner == 1 ? "LEFT WINS" : "RIGHT WINS");
                renderer.drawTextString(msg, 0.0f, 0.5f, camera.projection(), pe::TextAlign::Center);
                renderer.drawTextString("SPACE/R TO RESTART", 0.0f, -0.5f, camera.projection(), pe::TextAlign::Center);
            }
        }
        glfwSwapBuffers(window);
    }

    // 9. Teardown, reverse creation order
    renderer.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
