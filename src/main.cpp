#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>   // Step 7: entities live in a std::vector

// --- Step 5: The Math Layer ---
// Our OWN math code (src/math/), not an external library. Header-only:
// all logic lives in these two files, so the CMake build is unchanged.
#include "math/vec3.h"
#include "math/mat4.h"

// --- Step 7: The Object System ---
// Entities as data: one struct per game object, stored in a vector.
// Header-only like the math layer, so the CMake build stays unchanged.
#include "entity.h"

// --- Step 8: Collision Detection (AABB) ---
// The overlap ALGORITHM lives apart from the entity DATA (same
// data/system split as Step 7, at file level). Header-only: no
// CMakeLists.txt change.
#include "collision.h"

// --- Step 11: Scene/Level Structure ---
// The game-state enum: pure logic, header-only like entity.h and
// collision.h — no CMakeLists.txt change. The DISPATCH that uses it
// lives in main.cpp, because it needs every piece of per-frame state
// (input flags, entities, camera, audio) in one place.
#include "gamestate.h"

// --- Step 9: Audio Playback ---
// miniaudio — a single-file audio library fetched by CMake via
// FetchContent (same pattern as GLFW). Unlike our math/entity/
// collision code, audio touches OS audio devices and hardware
// buffers; that is the one layer we deliberately do NOT write
// ourselves. The CMake target 'miniaudio' compiles the library as
// a static lib and exports its include path, so a plain include
// here is all main.cpp needs — no MINIAUDIO_IMPLEMENTATION define.
#include <miniaudio.h>

// --- Step 10: Asset Loading ---
// stb_image — declarations ONLY. The actual implementation compiles in
// exactly one translation unit: src/stb_impl.cpp (added to CMake's
// source list), which defines STB_IMAGE_IMPLEMENTATION. Same
// declare-everywhere / define-once pattern the C runtime uses, and it
// keeps ~8,000 lines of generated code out of this file.
#include <stb_image.h>

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
 *
 * Step 6: Sprite/Mesh Rendering + Camera
 * Goal: Objects live in WORLD space and a camera decides what is visible.
 *       Each instance gets its own model matrix; a view matrix (camera
 *       position, moved with WASD) and an orthographic projection matrix
 *       are combined as projection * view * model and uploaded per draw.
 *
 * Step 7: Basic Object/ECS System
 * Goal: Entities are DATA, not hardcoded per-object blocks. Each triangle
 *       instance is a pe::Entity struct (position, rotation, scale) in a
 *       std::vector; ONE update loop and ONE draw loop process all of
 *       them — adding an entity is a push_back, never new code.
 *
 * Step 8: Collision Detection (AABB)
 * Goal: Detect when two entities overlap using axis-aligned bounding
 *       boxes. The player drives entities[0] with the ARROW keys into
 *       the others; overlapping entities turn red via a per-draw color
 *       uniform. The overlap test is constexpr and static_assert-proven.
 *
 * Step 9: Audio Playback
 * Goal: A sound FILE loads and plays on trigger — audible proof. One
 *       short WAV (assets/beep.wav) is loaded at startup through
 *       miniaudio; it plays exactly on the EDGE where collision starts
 *       (not-colliding -> colliding), never every frame while overlap
 *       continues — the same edge-detection principle as Step 3's SPACE.
 *
 * Step 10: Asset Loading
 * Goal: Textures load from FILES instead of hardcoded data. A PNG
 *       (assets/checker.png) is decoded by stb_image, uploaded to a GL
 *       texture object, and sampled in the fragment shader through UV
 *       coordinates added to the vertex data. Two Step 9 seams are
 *       refactored first: collision-edge detection becomes per-entity
 *       (previous colliding VECTOR, not a scalar), and playback uses a
 *       round-robin POOL of ma_sound instances instead of one.
 *
 * Step 11: Scene/Level Structure
 * Goal: The engine can LOAD, HOLD, and SWITCH between distinct game
 *       states. A pe::GameState enum (MENU / PLAYING / PAUSED) and ONE
 *       current-state variable drive a single dispatch point: what a
 *       key MEANS, whether the world SIMULATES, and what gets DRAWN
 *       all branch on the state. MENU is a plain dark-purple screen
 *       (no text system exists yet) — SPACE starts, ESC quits.
 *       PLAYING is Steps 1-10 untouched; ESC pauses. PAUSED freezes
 *       the scene (drawn every frame, simulated on none); ESC resumes,
 *       SPACE returns to the menu. Starting from the menu RESETS the
 *       world to its initial data snapshot.
 *
 * Step 12: Game Logic Layer
 * Goal: Franchise gameplay on top of everything — the v1 game loop.
 *       A fifth entity, the HOSTILE, chases the player every PLAYING
 *       frame (normalize(player - hostile) * speed * dt, reusing only
 *       Step 5 math). Touching it ends the run: a new GameState
 *       GAME_OVER freezes the scene on a dark-red screen and prints
 *       the survival time (deltaTime accumulation, Step 2's pattern)
 *       to the console. SPACE returns to the menu; the next SPACE
 *       resets everything via Step 11's resetGame(). All Steps 1-11
 *       behavior — spinning triangles, camera, tint, beep, pause — is
 *       additive-adjacent and untouched.
 *
 * Game Build Phase 1: Multiple Hostiles
 * Goal: Three hostiles instead of one — added purely as DATA (two more
 *       push_backs before the initial snapshot), each with its own
 *       chase speed and spin as per-entity data (Step 7's pattern).
 *       The chase becomes a loop over the hostile range; the catch
 *       test becomes a loop where ANY overlap ends the run identically
 *       to v1. The 3-entity scenery loop and resetGame() are untouched
 *       — the snapshot/restore pattern absorbs the new entities for
 *       free. Reuse before you build new.
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
// --- Step 6 additions: prove the orthographic projection at compile time ---
// The corners of the ortho box must land EXACTLY on the clip-cube corners.
// The box below was chosen so every intermediate value is exactly
// representable in float (scales 0.5 and 1.0) — static_assert compares
// floats with ==, so the test values must be exact, no rounding luck.
// transformPoint is safe to use here: orthographic preserves w = 1, so the
// skipped perspective divide is a genuine no-op for this matrix.
static_assert(pe::Mat4::orthographic(-2.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f).transformPoint(pe::Vec3(2.0f, 1.0f, 0.0f)) == pe::Vec3(1.0f, 1.0f, 0.0f),
              "Mat4 orthographic projection is broken");
static_assert(pe::Mat4::orthographic(-2.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f).transformPoint(pe::Vec3(-2.0f, -1.0f, 0.0f)) == pe::Vec3(-1.0f, -1.0f, 0.0f),
              "Mat4 orthographic projection is broken");
// --- Step 8 additions: prove the AABB overlap test at compile time ---
// Two identical unit boxes at the same center: maximally overlapping.
static_assert(pe::aabbOverlap(pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f),
                              pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f)),
              "AABB overlap: concentric boxes must collide");
// Gap on the X axis alone kills the collision — even with Y ranges
// fully shared. This is the test that catches the classic AND->OR bug.
static_assert(!pe::aabbOverlap(pe::Vec3(3.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f),
                               pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f)),
              "AABB overlap: X gap must prevent collision");
// Mirror image: gap on the Y axis alone must also prevent it.
static_assert(!pe::aabbOverlap(pe::Vec3(0.0f, 3.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f),
                               pe::Vec3(0.0f, 0.0f, 0.0f), pe::Vec3(1.0f, 1.0f, 0.0f)),
              "AABB overlap: Y gap must prevent collision");
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

    // --- Step 9: Audio engine + sound loading (one-time setup) ---
    // ma_engine_init starts miniaudio's high-level engine: it opens the
    // OS playback device (WASAPI on Windows), creates the mixing thread,
    // and gives us one object that owns every sound we play. NULL = use
    // the default config (default device, default sample rate). It
    // returns MA_SUCCESS (0) on success — checked like every other init
    // in this program, with the same cleanup-then-exit pattern.
    ma_engine audioEngine;
    if (ma_engine_init(NULL, &audioEngine) != MA_SUCCESS) {
        std::cerr << "Failed to initialize the audio engine (miniaudio)" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Locate assets/beep.wav. Relative paths resolve against the CURRENT
    // WORKING DIRECTORY, which depends on how the exe is launched (from
    // the repo root, from build/, or by double-clicking it next to the
    // exe). We try candidates one level deeper each time so the same
    // binary works in all three cases.
    const char* soundPathCandidates[] = {
        "assets/beep.wav",       // run from the repo root (d:\PureEngine)
        "../assets/beep.wav",    // run from build/
        "../../assets/beep.wav"  // run from build/Release/ (exe's own folder)
    };
    // --- Step 10 REFACTOR (Step 9 seam #2): a POOL of sound instances ---
    // Step 9 used ONE ma_sound for every trigger, so two sounds that
    // overlapped in time fought over the same playback slot (the restart
    // branch had to rewind it). A pool fixes this the same way Step 7
    // fixed per-object state: sounds become DATA in a container. Each
    // pool slot is its own independent ma_sound decoding the same file;
    // a trigger takes the NEXT slot (round-robin), so up to POOL_SIZE
    // beeps can be audible simultaneously. 4 slots for a one-shot 150 ms
    // beep is comfortably more than any realistic trigger rate.
    // std::vector<ma_sound>(N) VALUE-initializes the C structs to zero,
    // which is exactly the state ma_sound_init_from_file expects.
    const size_t SOUND_POOL_SIZE = 4;
    std::vector<ma_sound> collisionSounds(SOUND_POOL_SIZE);
    // Round-robin cursor: which pool slot the NEXT trigger claims.
    size_t nextCollisionSound = 0;
    // ma_sound_init_from_file DECODES the file (miniaudio has a built-in
    // WAV decoder) and registers the sound with the engine, ready to be
    // started with one call later. 0 = flags: default settings — no
    // looping (one shot), no 3D spatialization. The two NULLs skip an
    // optional resource-manager group and fence. The probe loop finds the
    // first loadable path and initializes pool slot 0 with it; the
    // remaining slots are then filled from that known-good path.
    bool soundLoaded = false;
    const char* loadedSoundPath = NULL;
    for (const char* candidate : soundPathCandidates) {
        if (ma_sound_init_from_file(&audioEngine, candidate, 0, NULL, NULL,
                                    &collisionSounds[0]) == MA_SUCCESS) {
            loadedSoundPath = candidate;
            soundLoaded = true;
            break;
        }
    }
    if (!soundLoaded) {
        std::cerr << "Failed to load assets/beep.wav (tried: assets/, ../assets/, ../../assets/)" << std::endl;
        // The engine initialized, so it must be uninitialized before exit.
        ma_engine_uninit(&audioEngine);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    // Fill pool slots 1..N-1 from the path that just worked. If any slot
    // fails to init (out of resources, etc.), uninit every slot that DID
    // succeed and abort — a half-built pool is a bug factory.
    for (size_t i = 1; i < collisionSounds.size(); ++i) {
        if (ma_sound_init_from_file(&audioEngine, loadedSoundPath, 0, NULL, NULL,
                                    &collisionSounds[i]) != MA_SUCCESS) {
            std::cerr << "Failed to initialize collision sound pool slot " << i << std::endl;
            for (size_t j = 0; j < i; ++j) {
                ma_sound_uninit(&collisionSounds[j]);
            }
            ma_engine_uninit(&audioEngine);
            glfwDestroyWindow(window);
            glfwTerminate();
            return -1;
        }
    }

    // --- Step 10 REFACTOR (Step 9 seam #1): per-entity edge state ---
    // Step 9 collapsed every collision into ONE scalar flag, so a NEW
    // overlap starting while another was already active fired no sound
    // (the global level never dropped to false). The fix tracks the
    // previous frame's full collision VECTOR — one entry per entity —
    // so each entity's 0->1 transition is its own edge. Declared empty
    // here (entities don't exist yet); sized on first use in the loop.
    std::vector<char> wasColliding;

    // --- Step 11: Game state (before the loop) ---
    // ONE variable holds which state the engine is in. Every per-frame
    // decision — what input means, whether the world updates, what gets
    // drawn — dispatches on it from a single point below. The engine
    // boots to the MENU, not straight into gameplay.
    pe::GameState currentState = pe::GameState::MENU;
    // ESC edge state — same pattern as SPACE. Steps 1-10 used LEVEL
    // polling for ESC (hold = close), fine when ESC only ever quits.
    // Now ESC also PAUSES/RESUMES, where a held key must not flip the
    // state 60 times a second — so ESC gets edge detection too.
    bool escWasPressedLastFrame = false;

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
        // STEP 10 ADDITION: a second vertex ATTRIBUTE carrying the UV
        // texture coordinate of this vertex. 'location = 1' matches the
        // glVertexAttribPointer index below. The shader does nothing with
        // it except hand it to the fragment stage — rasterization
        // INTERPOLATES it across the triangle automatically, so every
        // pixel gets a smoothly blended UV between the three corners.
        "layout (location = 1) in vec2 aTexCoord;\n"
        "uniform mat4 transform;\n"
        "out vec2 TexCoord;\n"
        "void main() {\n"
        "    gl_Position = transform * vec4(aPos, 1.0);\n"
        "    TexCoord = aTexCoord;\n"
        "}\n";

    // FRAGMENT SHADER: runs once per pixel covered by the shape. Its output
    // (FragColor, an RGBA vec4) becomes that pixel's color.
    //
    // STEP 10 — the flat color is replaced by a TEXTURE LOOKUP:
    // 'uniform sampler2D tex' is a handle to the texture bound on a
    // texture unit (set from C++ with glUniform1i); texture(tex, TexCoord)
    // fetches the image color at the INTERPOLATED UV for this pixel.
    // The Step 8 'color' uniform survives as a TINT multiplier: white
    // (1,1,1) leaves the texture untouched, red (1,0,0) multiplies the
    // green and blue channels to zero — the collision feedback is now a
    // red-tinted texture instead of flat red. Same uniform, evolved role.
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "uniform sampler2D tex;\n"
        "uniform vec3 color;\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    FragColor = vec4(texture(tex, TexCoord).rgb * color, 1.0f);\n"
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

    // --- Step 8: Locate the color uniform (after linking) ---
    // Same pattern as 'transform' above: get the handle once, check it,
    // upload a per-draw value every frame below. Since Step 10 the color
    // is a TINT multiplied with the texture — white normally, red when
    // colliding — but the lookup and check are identical.
    GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
    if (colorLocation < 0) {
        std::cerr << "Uniform 'color' not found in shader program" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // --- Step 7: Entities as DATA (before the loop) ---
    // Every triangle instance is one entry in this vector. The Step 5/6
    // globals rotationAngle/rotationSpeed no longer exist — that state
    // now lives INSIDE each entity, per instance. Adding an entity is
    // one push_back; the update loop and draw loop below never change.
    std::vector<pe::Entity> entities;
    // Instances 1 & 2 — the Step 6 pair, exactly preserved: same world
    // positions, same 0.9 rad/s counter-clockwise spin (~7 s/revolution),
    // same unit scale.
    entities.push_back(pe::Entity(pe::Vec3(-1.5f, 0.0f, 0.0f), 0.9f,
                                  pe::Vec3(1.0f, 1.0f, 1.0f)));
    entities.push_back(pe::Entity(pe::Vec3( 1.5f, 0.0f, 0.0f), 0.9f,
                                  pe::Vec3(1.0f, 1.0f, 1.0f)));
    // Instance 3 — the proof that the loop scales without code
    // duplication: new position, new speed AND DIRECTION (-1.4 rad/s =
    // clockwise, ~4.5 s/revolution), new scale (60% size). None of this
    // required a single new rendering line — behavior comes from DATA.
    entities.push_back(pe::Entity(pe::Vec3(0.0f, 1.5f, 0.0f), -1.4f,
                                  pe::Vec3(0.6f, 0.6f, 1.0f)));
    // Instance 4 — Step 12: the HOSTILE. Same pe::Entity type, same
    // vector, added purely as DATA (Step 7's ruling): the existing
    // update loop spins it (1.8 rad/s CCW), the existing draw loop
    // renders it, its halfExtents come from the same Step 8 bounds.
    // Starts at (0, -2) — bottom-center, away from the player's start
    // at (-1.5, 0) — unit scale like entities 1 & 2, so its collider
    // matches its rendered size exactly. It ignores the spinning
    // triangles (scenery is not solid in v1) and hunts only the
    // player — that chase and its catch test live in the simulation
    // section below, deliberately OUTSIDE the pair loop so the Step 8
    // scenery-collision system stays byte-identical in behavior.
    entities.push_back(pe::Entity(pe::Vec3(0.0f, -2.0f, 0.0f), 1.8f,
                                  pe::Vec3(1.0f, 1.0f, 1.0f)));
    // Instances 5 & 6 — Game Build Phase 1: TWO MORE hostiles, added
    // the only way this project adds behavior — as DATA (two push_backs,
    // Step 7's ruling). Each carries its OWN personality through the
    // exact same fields Step 7 gave every triangle: position = spawn
    // point, rotationSpeed = visual spin, scale = size (and collider).
    // Spawn points split the compass around the player's (-1.5, 0)
    // start — bottom (existing hostile), top-right, top-left — so the
    // opening seconds are a genuine three-direction read, not an
    // instant surround. Spins differ (one clockwise, like entity 3;
    // one faster CCW) so the three threats are visually distinct.
    // Both sit BEFORE the initialEntities snapshot below, which is the
    // entire reason resetGame() needs NO changes: the snapshot simply
    // contains all six entities at their start positions.
    entities.push_back(pe::Entity(pe::Vec3(3.0f, 2.0f, 0.0f), -1.2f,
                                  pe::Vec3(1.0f, 1.0f, 1.0f)));
    entities.push_back(pe::Entity(pe::Vec3(-3.0f, 2.0f, 0.0f), 2.2f,
                                  pe::Vec3(1.0f, 1.0f, 1.0f)));
    // --- Step 11: the INITIAL world, kept as DATA ---
    // A snapshot of the fresh entity list. Starting a game from the
    // menu restores it — reset is an ASSIGNMENT, not new code, which
    // is exactly what 'entities as data' (Step 7) was buying.
    const std::vector<pe::Entity> initialEntities = entities;
    // Collision flags hoisted OUT of the loop body (they lived inside
    // it in Steps 8-10). The PAUSED state still DRAWS the scene but
    // stops SIMULATING, so the last PLAYING frame's flags must survive
    // the pause (frozen tint). Rebuilt from zero every PLAYING frame.
    std::vector<char> colliding(entities.size(), 0);

    // --- Step 6: Camera + Projection State (before the loop) ---
    // The camera's position IN WORLD SPACE. WASD shifts it every frame and
    // the view matrix is rebuilt from it, so the whole scene appears to
    // slide the opposite way — that IS camera movement.
    pe::Vec3 cameraPos(0.0f, 0.0f, 0.0f);
    // Camera speed in WORLD UNITS PER SECOND. Multiplied by deltaTime in
    // the loop, so panning is frame-rate independent — same pattern as
    // Step 5's rotation.
    const float cameraSpeed = 3.0f;
    // The PROJECTION matrix: maps the visible slice of world space onto the
    // clip cube. An 8 x 6 world-unit box matches the 800x600 window's 4:3
    // aspect ratio, so shapes keep their proportions (no stretching).
    // Orthographic: apparent size never changes with depth — right for this
    // flat scene. Built ONCE: nothing about it changes per frame (yet).
    const pe::Mat4 projection = pe::Mat4::orthographic(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);

    // --- Step 8: Player movement speed (before the loop) ---
    // World units per second for the ARROW-key-driven entity
    // (entities[0]). Deliberately a bit slower than the camera's 3.0
    // so driving into another triangle feels controlled, not twitchy.
    const float entityMoveSpeed = 2.5f;

    // --- Step 12 / Phase 1: hostile chase speeds (before the loop) ---
    // World units per second, ONE PER HOSTILE, parallel to the hostile
    // range of the entity vector: index 0 -> entities[3], index 1 ->
    // entities[4], index 2 -> entities[5]. Data in a container — the
    // same pattern as soundPathCandidates and the collision vectors,
    // and the same per-entity-variance idea Step 7 gave the triangles'
    // rotation speeds. Every value stays SLOWER than the player's 2.5:
    // against pure pursuers a straight-line escape always wins, so
    // being caught remains a positioning mistake, not a script — the
    // Step 12 'avoidable by construction' principle, now for three
    // threats. The spread (1.8 / 1.6 / 1.5) means they straggle rather
    // than arrive as a wall — the player faces a pincer, not a fence.
    const float hostileSpeeds[] = { 1.8f, 1.6f, 1.5f };

    // --- Step 4: Vertex Data + VAO/VBO (before the loop) ---
    // Three vertices forming a triangle centered on screen.
    // STEP 10: each vertex now carries TWO attributes interleaved in the
    // same array — position (x, y, z) followed by its UV texture
    // coordinate (u, v). UV space: (0,0) is the texture's BOTTOM-LEFT,
    // (1,1) its top-right — the whole image maps into the 0..1 square
    // regardless of the image's pixel size. The bottom corners get the
    // bottom UV corners; the apex gets (0.5, 1), so the triangle shows
    // the top-center of the image, upright.
    float vertices[] = {
        // position              // UV
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,   // bottom-left
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f,   // bottom-right
         0.0f,  0.5f, 0.0f,     0.5f, 1.0f    // top
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
    // no normalization, stride = byte distance to the NEXT vertex — now
    // 5 floats (3 position + 2 UV) because the attributes are interleaved.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // Attribute arrays are disabled by default — enable index 0.
    glEnableVertexAttribArray(0);
    // STEP 10: attribute index 1 = the UV pair. Same buffer, same stride,
    // but the pointer starts 3 floats in (right after each position).
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Unbind the VBO (optional safety measure); the VAO remembers the setup.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // --- Step 10: Texture loading (stb_image) + GL texture object ---
    // Same 3-candidate CWD strategy as the sound file in Step 9.
    const char* texturePathCandidates[] = {
        "assets/checker.png",       // run from the repo root
        "../assets/checker.png",    // run from build/
        "../../assets/checker.png"  // run from build/Release/
    };
    // stbi_load DECODES the image file (PNG here; it also reads JPG/BMP/
    // TGA) into a raw pixel array in memory. The three ints receive the
    // decoded width, height, and channel count; the final argument 3
    // FORCES the output to 3-channel RGB even if the file stores alpha.
    // It returns NULL on failure — and it is OUR job to check, because a
    // failed load followed by a silent black texture is the audio-missing
    // problem all over again, visually.
    int texWidth = 0, texHeight = 0, texChannels = 0;
    unsigned char* pixels = NULL;
    for (const char* candidate : texturePathCandidates) {
        pixels = stbi_load(candidate, &texWidth, &texHeight, &texChannels, 3);
        if (pixels) {
            break;
        }
    }
    if (!pixels) {
        std::cerr << "Failed to load assets/checker.png (tried: assets/, ../assets/, ../../assets/)" << std::endl;
        for (ma_sound& sound : collisionSounds) {
            ma_sound_uninit(&sound);
        }
        ma_engine_uninit(&audioEngine);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Upload the decoded pixels into a GPU TEXTURE OBJECT.
    // glGenTextures creates the object name; glBindTexture makes it the
    // current target so the following calls configure IT.
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Sampling parameters, part of the object's state:
    //  WRAP_S/WRAP_T: what happens for UVs outside 0..1 — CLAMP_TO_EDGE
    //    freezes the border pixels (correct for our exactly-0..1 UVs;
    //    GL_REPEAT would tile the image instead).
    //  MIN/MAG_FILTER: how a pixel is computed when the texel-to-pixel
    //    ratio is not 1:1 — LINEAR blends the 4 nearest texels, which
    //    keeps a spinning, scaled triangle smooth instead of blocky.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // The actual upload: level 0 (no mipmaps yet), internal format RGB,
    // dimensions from stbi_load, border 0, source format RGB of unsigned
    // bytes — our 64x64x3 pixel array moves to GPU memory in one call.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, pixels);
    // The CPU copy has served its purpose — the GPU owns the data now.
    stbi_image_free(pixels);

    // --- Step 10: Bind the sampler uniform to TEXTURE UNIT 0 ---
    // A sampler2D uniform does NOT hold image data — it holds the INDEX
    // of a texture unit. Units are GL's binding slots (GL_TEXTURE0..N);
    // each frame we bind a texture to a unit, and the shader's sampler
    // must be told WHICH unit to read. Setting the uniform once is
    // enough: uniform values persist in the program object. (Requires
    // the program to be current — hence the glUseProgram first.)
    glUseProgram(shaderProgram);
    GLint texLocation = glGetUniformLocation(shaderProgram, "tex");
    if (texLocation < 0) {
        std::cerr << "Uniform 'tex' not found in shader program" << std::endl;
        glDeleteTextures(1, &texture);
        for (ma_sound& sound : collisionSounds) {
            ma_sound_uninit(&sound);
        }
        ma_engine_uninit(&audioEngine);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    glUniform1i(texLocation, 0);   // sampler reads from GL_TEXTURE0

    // --- Step 12: survival timer ---
    // Seconds survived in the current run. Accumulated INSIDE the
    // PLAYING gate only — pausing stops the clock, which is correct:
    // the timer measures played time, not wall time. Same pattern as
    // every other deltaTime consumer since Step 2: state += rate * dt,
    // here with a rate of 1 second per second.
    float survivalTime = 0.0f;

    // --- Step 11: reset the world to its initial data ---
    // Called when a game STARTS from the menu. Every piece of play
    // state returns to its pre-game value: the entity snapshot (which
    // INCLUDES the hostile at its start position — the snapshot was
    // taken AFTER its push_back), the camera home, the clear-color
    // toggle off, BOTH collision histories empty (nothing was colliding
    // before the game began), and the survival timer back to zero.
    auto resetGame = [&]() {
        entities = initialEntities;
        cameraPos = pe::Vec3(0.0f, 0.0f, 0.0f);
        clearColorIsBlue = false;
        wasColliding.clear();
        colliding.assign(entities.size(), 0);
        survivalTime = 0.0f;
    };

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
        // --- Step 11: one shared float conversion for every consumer ---
        // (input rates in the state dispatch below, entity updates in
        // the simulation branch). Declared at loop scope on purpose.
        float dt = static_cast<float>(deltaTime);

        // A. Poll for events (input, window resize, etc.)
        glfwPollEvents();

        // --- Step 11: Input dispatch — what a KEY MEANS depends on the STATE ---
        // The engine's first real input FORK: the same physical key has
        // different semantics per state. ESC quits from MENU, PAUSES in
        // PLAYING, resumes from PAUSED. SPACE starts from MENU, toggles
        // the clear color in PLAYING, returns to MENU from PAUSED.
        // Both keys are read ONCE here as level + EDGE (Step 3's
        // pattern — ESC gains an edge detector now that it toggles),
        // then the state switch below consumes the results. Every
        // transition is a one-line assignment: states are values, and
        // switching is nothing more dramatic than storing a new one.
        bool escIsPressedNow = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
        bool escEdge = escIsPressedNow && !escWasPressedLastFrame;
        bool spaceIsPressedNow = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
        bool spaceEdge = spaceIsPressedNow && !spaceWasPressedLastFrame;

        switch (currentState) {
        case pe::GameState::MENU:
            // ESC quits. LEVEL polling is fine here: the only effect is
            // setting the close flag — idempotent even while held, and
            // glfwSetWindowShouldClose destroys nothing immediately (the
            // loop condition checks it, cleanup runs as usual).
            if (escIsPressedNow) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            // SPACE starts a game: reset the world to its initial data,
            // then enter PLAYING. EDGE — one start per press.
            } else if (spaceEdge) {
                resetGame();
                currentState = pe::GameState::PLAYING;
            }
            break;
        case pe::GameState::PLAYING:
            // ESC pauses. EDGE — a held key must not flip pause on and
            // off 60 times a second.
            if (escEdge) {
                currentState = pe::GameState::PAUSED;
            } else {
                // --- Step 3: SPACE clear-color toggle (EDGE, exactly one
                // toggle per physical press; held key stays false after
                // the first frame). Scope of PLAYING only now.
                if (spaceEdge) {
                    // Flip the flag: black becomes blue, blue becomes black.
                    clearColorIsBlue = !clearColorIsBlue;
                }
                // --- Step 6: Camera Movement (WASD, polled every frame) ---
                // Movement is a RATE, not a toggle: while a key is held it
                // must act on EVERY frame. No previous-frame comparison —
                // speed * deltaTime turns a per-frame key state into
                // frame-rate-independent units per second.
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                    cameraPos.y += cameraSpeed * dt;   // camera up    -> scene slides down
                }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                    cameraPos.y -= cameraSpeed * dt;   // camera down  -> scene slides up
                }
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    cameraPos.x -= cameraSpeed * dt;   // camera left  -> scene slides right
                }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    cameraPos.x += cameraSpeed * dt;   // camera right -> scene slides left
                }
                // --- Step 8: Player entity movement (ARROW keys) ---
                // WASD belongs to the CAMERA (established Step 6 behavior,
                // kept untouched). The ARROW keys move entities[0] through
                // the world, so the player can drive it into the other two
                // triangles and trigger a collision on demand. Same RATE
                // pattern as camera panning. The reference below points
                // INTO the vector — writes land in the real entity.
                pe::Entity& player = entities[0];
                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                    player.position.y += entityMoveSpeed * dt;
                }
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                    player.position.y -= entityMoveSpeed * dt;
                }
                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                    player.position.x -= entityMoveSpeed * dt;
                }
                if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                    player.position.x += entityMoveSpeed * dt;
                }
            }
            break;
        case pe::GameState::PAUSED:
            // ESC resumes; SPACE gives up the run and returns to the
            // menu. The abandoned world stays in memory as-is; the NEXT
            // start resets it via resetGame().
            if (escEdge) {
                currentState = pe::GameState::PLAYING;
            } else if (spaceEdge) {
                currentState = pe::GameState::MENU;
            }
            break;
        case pe::GameState::GAME_OVER:
            // --- Step 12: the run is over ---
            // SPACE returns to the menu — Step 11's navigation pattern
            // verbatim, one-line assignment. ESC is deliberately DEAD
            // here: ESC means pause/resume, and there is nothing to
            // resume from a finished run. The frozen scene stays on
            // screen (drawn below, never simulated) until SPACE moves
            // us back to the purple menu.
            if (spaceEdge) {
                currentState = pe::GameState::MENU;
            }
            break;
        }
        // Store this frame's key states so the NEXT frame can detect edges.
        escWasPressedLastFrame = escIsPressedNow;
        spaceWasPressedLastFrame = spaceIsPressedNow;

        // --- Step 11: Simulation runs ONLY in PLAYING ---
        // PAUSED holds the world still — drawn every frame (below),
        // simulated on none of them. MENU has no world to simulate.
        // Everything Steps 7-10 do per frame is unchanged INSIDE this
        // branch: the state machine WRAPS the simulation, it never
        // touches it. That is why all Step 1-10 behavior survives.
        if (currentState == pe::GameState::PLAYING) {
            // --- Step 12: survival timer accumulates ---
            // deltaTime integration, Step 2's pattern: count up at a
            // constant rate of 1 second per second of PLAYING time.
            survivalTime += dt;

            // --- Step 7: Update every entity (per-frame simulation) ---
            // One loop replaces the old global rotationAngle update from
            // Steps 5/6. Each entity carries its OWN speed (and even its own
            // direction), so each advances at its own rate — the same
            // state += rate * deltaTime pattern, now per-entity data.
            for (pe::Entity& entity : entities) {
                entity.update(dt);
            }

            // --- Step 12 / Phase 1: EVERY hostile chases the player ---
            // The v1 chase, unchanged per hostile, wrapped in the only
            // thing Phase 1 adds: a loop over the hostile range
            // (index 3 to the end of the vector). direction = player -
            // hostile (Vec3 operator-, Step 5), normalized() to a unit
            // vector, scaled by THIS hostile's speed from the parallel
            // hostileSpeeds array and deltaTime, assigned back. ZERO
            // new math — the same four Step 5 Vec3 operations as v1,
            // executed per hostile. Each hostile picks its own pursuit
            // vector every frame (pure pursuers — no prediction, no
            // flanking; the difficulty comes from NUMBERS, not AI).
            // The zero-length guard keeps intent honest per hostile:
            // a hostile sitting exactly ON the player has no direction
            // to move in — the catch test ends the run this frame.
            for (size_t h = 3; h < entities.size(); ++h) {
                pe::Entity& hostile = entities[h];
                const pe::Vec3 toPlayer = entities[0].position - hostile.position;
                if (toPlayer.length() > 0.0f) {
                    hostile.position = hostile.position + toPlayer.normalized() * hostileSpeeds[h - 3] * dt;
                }
            }

            // --- Step 8: Collision pass (after movement, before drawing) ---
            // One flag per entity, rebuilt from ZERO every frame: collision
            // state is derived fresh from positions, never remembered. A
            // 'sticky' flag would need explicit reset logic and drift out of
            // sync; deriving it needs nothing. char over bool: zero-init is
            // unambiguous and it reads fine as a flag. (Step 11: the
            // vector itself is hoisted to loop scope so PAUSED can keep
            // drawing the last frame's tint; contents rebuilt here.)
            colliding.assign(entities.size(), 0);
            // Test every UNIQUE pair exactly once: i runs each entity,
            // j only the ones AFTER it. With N entities that is N*(N-1)/2
            // tests — 3 for N = 3. Testing i == j would self-collide
            // (always true — useless); testing both orders doubles the work
            // for identical results. Overlap is symmetric: set BOTH flags.
            // Step 12: the loop bound is the ORIGINAL THREE, not
            // entities.size() — the hostiles are deliberately excluded
            // so this loop stays exactly the Steps 8-11 scenery system
            // (tint + beep between the three triangles). Phase 1 added
            // two more hostiles at indices 4-5; this line did NOT
            // change, which is the point — the hostiles pass through
            // scenery, and their only interaction remains the player
            // catch test immediately after the audio block.
            for (size_t i = 0; i < 3; ++i) {
                for (size_t j = i + 1; j < 3; ++j) {
                    if (pe::aabbOverlap(entities[i], entities[j])) {
                        colliding[i] = 1;
                        colliding[j] = 1;
                    }
                }
            }

            // --- Step 10: per-entity collision EDGE detection + sound pool ---
            // Step 9's scalar OR-flag is gone. Now the previous frame's full
            // collision VECTOR is compared entry by entry: an entity whose
            // flag goes 0 -> 1 is a fresh edge EVEN IF other entities were
            // already colliding. (First frame: the vector is sized here with
            // all-zero entries — before the program starts, nothing collides.)
            if (wasColliding.size() != colliding.size()) {
                wasColliding.assign(colliding.size(), 0);
            }
            bool anyNewCollision = false;
            for (size_t i = 0; i < colliding.size(); ++i) {
                if (colliding[i] && !wasColliding[i]) {
                    anyNewCollision = true;
                    break;
                }
            }
            if (anyNewCollision) {
                // Take the NEXT pool slot (round-robin) so a beep still
                // ringing from the previous trigger keeps playing untouched.
                ma_sound& sound = collisionSounds[nextCollisionSound];
                nextCollisionSound = (nextCollisionSound + 1) % collisionSounds.size();
                // A slot recycled while still audible gets rewound first.
                if (ma_sound_is_playing(&sound)) {
                    ma_sound_seek_to_pcm_frame(&sound, 0);
                }
                // ma_sound_start hands the sound to the engine's mixer; the
                // mixing THREAD plays it from here — this call returns
                // immediately and never blocks the render loop.
                ma_sound_start(&sound);
            }
            // Store THIS frame's vector for the next frame's edge test.
            wasColliding = colliding;

            // --- Step 12 / Phase 1: the catch test — player vs ANY hostile ---
            // v1 tested ONE pair; Phase 1 generalizes it the minimal
            // way: a loop over the hostile range (index 3 onward)
            // OR-ing Step 8's exact AABB test into a single bool.
            // Touching ANY hostile ends the run — the lose condition
            // does not care which one caught you. Everything ELSE is
            // the identical Step 12 block: reuse Step 8's aabbOverlap
            // machinery, print the survival time (console is the only
            // output channel — no font system exists), reuse the sound
            // POOL for the audible end-of-run signal (round-robin,
            // rewind-if-busy, Steps 9/10 pattern), and flip the state
            // LAST so every per-frame system above ran exactly once on
            // the final frame. Same frame, same death, three threats.
            bool caught = false;
            for (size_t h = 3; h < entities.size(); ++h) {
                if (pe::aabbOverlap(entities[0], entities[h])) {
                    caught = true;
                    break;   // one catcher is enough — stop testing
                }
            }
            if (caught) {
                std::cout << "GAME OVER — survived "
                          << survivalTime << " seconds" << std::endl;
                ma_sound& endSound = collisionSounds[nextCollisionSound];
                nextCollisionSound = (nextCollisionSound + 1) % collisionSounds.size();
                if (ma_sound_is_playing(&endSound)) {
                    ma_sound_seek_to_pcm_frame(&endSound, 0);
                }
                ma_sound_start(&endSound);
                currentState = pe::GameState::GAME_OVER;
            }
        }

        // B. Clear the screen
        // --- Step 11: clear color is per-state ---
        if (currentState == pe::GameState::MENU) {
            // MENU: a fixed dark PURPLE — deliberately a color outside
            // gameplay's black/dark-blue palette, so the menu can never
            // be mistaken for a paused or toggled game screen. With no
            // text system yet, the color IS the menu; the paint job is
            // Step 12 territory.
            glClearColor(0.16f, 0.0f, 0.24f, 1.0f);
        } else if (currentState == pe::GameState::GAME_OVER) {
            // GAME_OVER (Step 12): a fixed DARK RED — the engine's
            // established danger channel (collision tint), distinct from
            // menu purple and gameplay black/blue, so a loss reads at a
            // glance. The frozen death scene renders on top (the draw
            // gate below excludes only MENU), exactly like PAUSED.
            glClearColor(0.28f, 0.0f, 0.0f, 1.0f);
        } else if (clearColorIsBlue) {
            // Dark blue (Step 3 toggle target)
            glClearColor(0.0f, 0.0f, 0.25f, 1.0f);
        } else {
            // Original black — same values as Step 1/2
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        }
        // glClear actually performs the clear operation on the color buffer
        glClear(GL_COLOR_BUFFER_BIT);

        // --- Step 11: gameplay rendering happens OUTSIDE the MENU ---
        // MENU draws nothing but the clear color. PLAYING, PAUSED, and
        // (Step 12) GAME_OVER share this EXACT draw path — the only
        // difference between them is whether the simulation branch
        // above advanced the data being drawn here (frozen spin, frozen
        // camera, frozen tint, frozen hostile).
        if (currentState != pe::GameState::MENU) {
            // --- Step 4: Draw the Triangle (after clear, before swap) ---
            // Activate our shader program: all following draw calls use it.
            glUseProgram(shaderProgram);

            // --- Step 6: Build the VIEW matrix from the camera position ---
            // lookAt constructs the camera's INVERSE transform directly: the
            // camera sits at cameraPos, aims straight down -Z (orientation is
            // fixed for now), with world +Y as up. Because the view matrix is
            // the inverse of the camera transform, shifting cameraPos moves
            // every rendered vertex by the OPPOSITE amount — the camera pans
            // across the world, the geometry stays put.
            const pe::Mat4 view = pe::Mat4::lookAt(cameraPos,
                                                   cameraPos + pe::Vec3(0.0f, 0.0f, -1.0f),
                                                   pe::Vec3(0.0f, 1.0f, 0.0f));

            // Bind the VAO ONCE: every entity shares this exact vertex data —
            // only the transform differs per instance.
            glBindVertexArray(VAO);

            // --- Step 10: bind the texture to unit 0 for this frame ---
            // glActiveTexture SELECTS the unit; glBindTexture attaches our
            // texture object to it. The sampler uniform was told once at
            // startup to read unit 0 — so every draw below samples the
            // checkerboard. (Step 11: the bind moved INSIDE the gameplay
            // draw path — the menu never touches a texture. Per-entity
            // textures remain Step 12's seam, deliberately deferred.)
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            // --- Step 7: ONE draw loop for ALL entities ---
            // This replaces Step 6's copy-pasted model1/mvp1/draw,
            // model2/mvp2/draw blocks. The pipeline math is unchanged —
            // projection * view * model, acting RIGHT-TO-LEFT on the vertex:
            //   model      : local coords  -> WORLD coords  (from entity DATA)
            //   view       : world coords  -> CAMERA coords
            //   projection : camera coords -> clip coords (-1..1 cube)
            // The loop neither knows nor cares how many entities exist:
            // three, thirty, or three hundred — same code, same three GL
            // calls per entity. THAT is what "entities as data" buys you.
            // (Step 8: the same index also selects the draw color, so a
            // colliding entity turns red — the visible proof of detection.)
            for (size_t i = 0; i < entities.size(); ++i) {
                const pe::Entity& entity = entities[i];
                // Build this entity's MVP from its own data.
                pe::Mat4 mvp = projection * view * entity.modelMatrix();
                // Upload to the same 'transform' uniform (GL_FALSE: our Mat4
                // is already column-major, the layout OpenGL expects).
                glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &mvp.m[0][0]);
                // Step 10: per-draw TINT — white (1,1,1) leaves the texture
                // color untouched; red (1,0,0) zeroes its green and blue
                // channels, so a colliding entity renders as a red-tinted
                // checkerboard. The Step 8 collision feedback survives the
                // switch from flat color to textured rendering.
                if (colliding[i]) {
                    glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
                } else {
                    glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
                }
                // One draw call for this entity.
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }

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
    glDeleteTextures(1, &texture);   // Step 10: the GPU texture object
    // --- Step 9/10: audio cleanup, reverse creation order ---
    // Every pool slot first (each registered WITH the engine), then the
    // engine itself — which stops the mixing thread and closes the OS
    // audio device. Audio is independent of OpenGL, so its teardown
    // order relative to the GL calls does not matter.
    for (ma_sound& sound : collisionSounds) {
        ma_sound_uninit(&sound);
    }
    ma_engine_uninit(&audioEngine);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
