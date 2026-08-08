#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

// --- Step 5: The Math Layer ---
// Our OWN math code (src/math/), not an external library. Header-only:
// all logic lives in these two files, so the CMake build is unchanged.
#include "math/vec3.h"
#include "math/mat4.h"

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
 *
 * Step 4: Draw One Shape
 * Goal: Render a triangle through a basic shader program, proving
 *       the full GPU pipeline works: vertex data -> vertex shader ->
 *       rasterization -> fragment shader -> framebuffer.
 *
 * Step 5: Math Layer
 * Goal: Transform geometry with our own math code. A Mat4 built on the
 *       CPU (rotation driven by deltaTime) is uploaded to the vertex
 *       shader as a uniform every frame, and the triangle visibly rotates.
 */

// --- Step 5: Compile-time sanity tests for the math layer ---
// static_assert conditions are evaluated BY THE COMPILER. If any line of
// our math were wrong in a way visible at compile time, the build would
// fail with a clear message — math errors caught before the program even
// runs. These are tests that cost nothing at runtime.
static_assert(pe::Vec3(1.0f, 2.0f, 3.0f) + pe::Vec3(4.0f, 5.0f, 6.0f) == pe::Vec3(5.0f, 7.0f, 9.0f),
              "Vec3 addition is broken");
static_assert(pe::Vec3(1.0f, 0.0f, 0.0f).dot(pe::Vec3(0.0f, 1.0f, 0.0f)) == 0.0f,
              "Vec3 dot product is broken");
// The 4th column of a translation matrix must hold the offset itself.
static_assert(pe::Mat4::translation(pe::Vec3(3.0f, 4.0f, 5.0f)).m[3][1] == 4.0f,
              "Mat4 translation builder is broken");
// Identity must transform a point into exactly itself.
static_assert(pe::Mat4().transformPoint(pe::Vec3(2.0f, 3.0f, 4.0f)) == pe::Vec3(2.0f, 3.0f, 4.0f),
              "Mat4 identity/transformPoint is broken");
// A translation applied through transformPoint must shift the point.
static_assert(pe::Mat4::translation(pe::Vec3(1.0f, 1.0f, 1.0f)).transformPoint(pe::Vec3(1.0f, 2.0f, 3.0f)) == pe::Vec3(2.0f, 3.0f, 4.0f),
              "Mat4 translation transform is broken");
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
        // The window already exists here, so clean it up before exiting —
        // same consistent cleanup as every other error path below.
        glfwDestroyWindow(window);
        glfwTerminate();
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

    // --- Step 4: Shader Setup (before the loop) ---
    // Shaders are tiny programs that run ON THE GPU. OpenGL's fixed-function
    // pipeline is gone in core profile 3.3 — we MUST supply at least a vertex
    // shader (positions vertices) and a fragment shader (colors pixels).
    // They are written in GLSL and compiled at runtime from these strings.
    // "#version 330 core" matches the OpenGL 3.3 Core context from Step 1.
    //
    // VERTEX SHADER: runs once per vertex. "layout (location = 0)" binds the
    // input attribute aPos to attribute index 0 — the same index we will use
    // when describing our vertex data with glVertexAttribPointer below.
    //
    // STEP 5 ADDITION — "uniform mat4 transform;": a uniform is a global
    // INPUT to the shader, identical for every vertex in the draw call, set
    // from C++ with glUniformMatrix4fv. Every vertex is multiplied by it
    // (w = 1 makes translation take effect), so ONE matrix transforms the
    // whole triangle: rotate it, move it, scale it — from CPU-side math.
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 transform;\n"
        "void main() {\n"
        "    gl_Position = transform * vec4(aPos, 1.0);\n"
        "}\n";

    // FRAGMENT SHADER: runs once per pixel covered by the shape. Its output
    // (FragColor, an RGBA vec4) becomes that pixel's color. Solid orange here.
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n";

    // Compile the vertex shader. glCreateShader creates an empty shader object
    // of the given type; glShaderSource attaches our GLSL text; glCompileShader
    // compiles it on the GPU driver.
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Error check: if compilation failed (typo in GLSL, unsupported feature),
    // the info log contains the driver's error message. Without this check a
    // broken shader fails SILENTLY and you just get a black screen.
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
        // Consistent cleanup on error: destroy the window and terminate GLFW
        // before exiting, matching the window-creation error path.
        // glfwTerminate() also destroys the OpenGL context and every GPU
        // object owned by it (the compiled shader included), so nothing leaks.
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Compile the fragment shader — same three calls, same error check.
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
        // Same consistent cleanup; glfwTerminate() reclaims both shader
        // objects through context destruction.
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Link both compiled shaders into one shader PROGRAM: the complete
    // executable pipeline the GPU will run. Linking can also fail (e.g. the
    // vertex shader's outputs don't match the fragment shader's inputs), so
    // we check GL_LINK_STATUS with glGetProgramiv the same way.
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
        // Same consistent cleanup; the unfinished program and both shaders
        // are reclaimed when glfwTerminate() destroys the context.
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    // The individual shader objects are now baked into the program; delete
    // them to free resources. The program itself stays alive until cleanup.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- Step 5: Locate the transform uniform (after linking) ---
    // glGetUniformLocation asks the linked program for the storage location
    // of the uniform named "transform" and returns its handle. We need the
    // handle to set its value per frame. -1 means it does not exist —
    // usually a misspelled name, or the compiler optimized the uniform away
    // because nothing feeds gl_Position through it. Without this check the
    // upload below would fail SILENTLY and the triangle would sit still.
    GLint transformLocation = glGetUniformLocation(shaderProgram, "transform");
    if (transformLocation < 0) {
        std::cerr << "Uniform 'transform' not found in shader program" << std::endl;
        // Same consistent cleanup as the other error paths.
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // --- Step 5: Rotation State (before the loop) ---
    // Current rotation angle in RADIANS. It accumulates every frame:
    // angle += speed * deltaTime. Because deltaTime is in seconds, the
    // factor below is a SPEED in radians per second — the triangle spins
    // at the same rate whether the machine runs 30 FPS or 300 FPS.
    // 0.9 rad/s ≈ one full revolution every 7 seconds: slow enough to see.
    float rotationAngle = 0.0f;
    const float rotationSpeed = 0.9f;

    // --- Step 4: Vertex Data + VAO/VBO (before the loop) ---
    // Three vertices (x, y, z) forming a triangle centered on screen.
    // NDC coordinates: x and y run from -1 (left/bottom) to +1 (right/top).
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,   // bottom-left
         0.5f, -0.5f, 0.0f,   // bottom-right
         0.0f,  0.5f, 0.0f    // top
    };

    // VBO (Vertex Buffer Object): a buffer that lives in GPU memory.
    // We upload our vertex data once so the GPU doesn't need it re-sent
    // every frame.
    // VAO (Vertex Array Object): a small state container that REMEMBERS how
    // to interpret the VBO (which attribute index, how many floats, stride,
    // offset). Binding the VAO later restores all of that in one call.
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);   // generate names/IDs for 1 VAO and 1 VBO
    glGenBuffers(1, &VBO);

    // In core-profile OpenGL, most calls act on whatever object is currently
    // BOUND. Bind the VAO first: everything we configure now is recorded
    // inside it.
    glBindVertexArray(VAO);
    // Bind the VBO as the current GL_ARRAY_BUFFER target, then upload data.
    // GL_STATIC_DRAW hints the data is set once and drawn many times.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Tell OpenGL how to read the raw floats: attribute index 0 (matches
    // "layout (location = 0)" in the vertex shader), 3 floats per vertex,
    // no normalization, stride = byte distance to the next vertex
    // (3 floats), offset 0 (data starts at the buffer's beginning).
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Attribute arrays are disabled by default — enable index 0.
    glEnableVertexAttribArray(0);
    // Unbind the VBO (optional safety measure); the VAO remembers the setup.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

        // --- Step 5: Update the rotation angle (per-frame simulation) ---
        // deltaTime (Step 2) finally gets a job: advance the angle by
        // speed * elapsed-seconds. This is the universal game-loop pattern
        // — state += rate * deltaTime — that movement and physics will use.
        rotationAngle += rotationSpeed * static_cast<float>(deltaTime);

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

        // --- Step 4: Draw the Triangle (after clear, before swap) ---
        // Activate our shader program: all following draw calls use it.
        glUseProgram(shaderProgram);

        // --- Step 5: Build this frame's transform and upload it ---
        // Compose the matrix from our OWN math builders. Order matters —
        // (rotation * scale) applies scale to the vertices FIRST, then
        // rotation (matrices act right-to-left on the vertex). Scale is
        // neutral (1,1,1) for now; translation could join the chain the
        // same way — the builders are proven by the static_asserts above.
        pe::Mat4 transform = pe::Mat4::rotationZ(rotationAngle)
                           * pe::Mat4::scale(pe::Vec3(1.0f, 1.0f, 1.0f));
        // Upload the 16 floats to the uniform. Arguments: the uniform's
        // location, number of matrices (1), transpose flag (GL_FALSE — our
        // Mat4 is ALREADY column-major, the layout OpenGL expects, so no
        // conversion needed), and a pointer to the 16 floats.
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &transform.m[0][0]);

        // Bind the VAO: restores the vertex data layout we configured once.
        glBindVertexArray(VAO);
        // Draw: GL_TRIANGLES = interpret vertices as triangles, start at
        // index 0, use 3 vertices. This is THE call that feeds the GPU
        // pipeline: vertices -> vertex shader -> rasterizer -> fragment
        // shader -> pixels in the back buffer.
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // C. Swap buffers
        // GLFW uses double buffering. This swaps the front buffer (what we see)
        // with the back buffer (what we just drew to).
        glfwSwapBuffers(window);
    }

    // 7. Cleanup
    // Free GPU resources in reverse order of creation, before GLFW teardown.
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
