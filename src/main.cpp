#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

/**
 * Step 1: Window + Context Creation
 * Goal: Open a window and create a valid OpenGL context.
 *
 * Step 2: Render Loop — Delta Time Tracking
 * Goal: Measure how long each frame takes (delta time), so future
 *       movement and physics systems can run at a consistent speed
 *       regardless of the frame rate.
 *
 * Step 3: Input Handling
 * Goal: Poll the keyboard inside the loop. ESC closes the window;
 *       SPACE toggles the clear color between black and dark blue
 *       (one toggle per press, not per held frame).
 */
int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 2. Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 3. Create the Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "PureEngine - Step 1", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 4. Make the Window's Context Current
    glfwMakeContextCurrent(window);

    // 5. Initialize GLAD
    // We use the GLAD loader to get OpenGL function pointers.
    // This must be done AFTER the context is created and made current.
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // --- Step 2: Delta Time Setup (before the loop) ---
    // glfwGetTime() returns the number of seconds (as a high-resolution double)
    // that have elapsed since glfwInit() was called. It is a monotonic clock,
    // perfect for measuring intervals between frames.
    // We initialize lastFrameTime ONCE, before the loop starts, so that the
    // very first frame has a valid reference point to subtract from. Without
    // this initialization, the first frame's delta would be garbage
    // (uninitialized memory), potentially producing a huge or negative value.
    double lastFrameTime = glfwGetTime();

    // --- Step 3: Input State Setup (before the loop) ---
    // Tracks which clear color is currently active. false = black (the
    // Step 1/2 default), true = dark blue. SPACE toggles this flag.
    bool clearColorIsBlue = false;
    // Remembers whether SPACE was already held down on the PREVIOUS frame.
    // glfwGetKey() only tells us the key's state RIGHT NOW (pressed or not),
    // so on its own a held key would look "pressed" on every single frame and
    // would toggle the color hundreds of times per second. By comparing the
    // previous frame's state to the current one we can detect the exact
    // instant the key goes DOWN (edge detection) and toggle once per press.
    // Initialized to false: before the program starts, SPACE is not pressed.
    bool spaceWasPressedLastFrame = false;

    // 6. The Main Loop
    while (!glfwWindowShouldClose(window)) {
        // --- Step 2: Delta Time Calculation (top of the frame) ---
        // Placed at the very top of the loop body so the timing covers the
        // entire frame: events, rendering, and buffer swap.
        // Grab the current timestamp for this frame.
        double currentFrameTime = glfwGetTime();
        // Subtract the previous frame's timestamp from the current one.
        // The result, deltaTime, is the duration of the last frame in SECONDS.
        // Future movement/physics systems will multiply velocities by this
        // value so objects move the same distance per second whether the
        // game runs at 30 FPS or 300 FPS.
        double deltaTime = currentFrameTime - lastFrameTime;
        // Update lastFrameTime so the NEXT frame can compute its own delta
        // relative to this frame. If we skipped this, every frame would be
        // measured against the original start time instead of the previous frame.
        lastFrameTime = currentFrameTime;

        // A. Poll for events (input, window resize, etc.)
        glfwPollEvents();

        // --- Step 3: Input Handling (polled every frame, after events) ---
        // ESC: glfwGetKey() queries the current state of one key and returns
        // GLFW_PRESS or GLFW_RELEASE. If ESC is currently pressed, we ask GLFW
        // to flag the window for closing. glfwSetWindowShouldClose() does NOT
        // destroy anything immediately — it just sets the flag that our while
        // loop condition (!glfwWindowShouldClose(window)) checks, so the loop
        // exits cleanly after this frame and the normal cleanup code runs.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        // SPACE: read the key's state for THIS frame into a local variable.
        bool spaceIsPressedNow = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
        // Edge detection: toggle ONLY on the frame where the key transitions
        // from "not pressed" (previous frame) to "pressed" (this frame).
        // While the key is held, both values are true, so this stays false
        // after the first frame — exactly one toggle per physical press.
        if (spaceIsPressedNow && !spaceWasPressedLastFrame) {
            // Flip the flag: black becomes blue, blue becomes black.
            clearColorIsBlue = !clearColorIsBlue;
        }
        // Store this frame's state so the NEXT frame can compare against it.
        spaceWasPressedLastFrame = spaceIsPressedNow;

        // B. Clear the screen
        // glClearColor sets the color to clear to (R, G, B, A).
        // The values used depend on the toggle flag flipped by SPACE above.
        if (clearColorIsBlue) {
            // Dark blue (Step 3 toggle target)
            glClearColor(0.0f, 0.0f, 0.25f, 1.0f);
        } else {
            // Original black — same values as Step 1/2
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        }
        // glClear actually performs the clear operation on the color buffer
        glClear(GL_COLOR_BUFFER_BIT);

        // C. Swap buffers
        // GLFW uses double buffering. This swaps the front buffer (what we see)
        // with the back buffer (what we just drew to).
        glfwSwapBuffers(window);
    }

    // 7. Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
