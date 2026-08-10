# PureEngine — Build Log

## Goal
Learn engine architecture by building from scratch. No Frankenstein. No skipping steps.
Target: i3 / 8GB RAM. Language: C/C++. Libs: GLFW/SDL + OpenGL (no Vulkan yet).

## Rule
Do not start a step until the previous one runs and you understand every line you wrote.
No copy-pasted code you can't explain.

## Steps

### Step 1 — Window + Context Creation
**Status:** Completed
**Goal:** Open a window, get a valid render surface. Nothing else can exist without this.
**Definition of done:** A blank window opens, stays open until you close it, no crash.
**Notes:**
- Used GLFW for windowing and context creation.
- Targeting OpenGL 3.3 Core Profile.
- CMake configured with FetchContent for zero-install dependency management.
- FIXED: Added GLAD loader (bundled with GLFW) to handle OpenGL function pointers.
- FIXED: Added glClearColor, glClear, and glfwSwapBuffers to the render loop to ensure a solid black window.
- VERIFIED: Window renders solid black and closes cleanly on the 'X' button.

### Step 2 — Render Loop
**Status:** Completed
**Goal:** Loop that clears screen + swaps buffers every frame. Track delta time.
**Definition of done:** Window shows a solid clear color, updates every frame, closes cleanly.
**Notes:**
- Added delta time tracking using glfwGetTime(): lastFrameTime initialized before the loop; deltaTime recalculated at the top of each frame iteration (seconds per frame).
- VERIFIED: Window renders solid black, updates every frame, closes cleanly on 'X'.

### Step 3 — Input Handling
**Status:** Completed
**Goal:** Poll keyboard/mouse inside the loop.
**Definition of done:** Pressing a key does something visible (e.g. changes clear color).
**Notes:**
- Added keyboard polling with glfwGetKey() inside the render loop: ESC sets glfwSetWindowShouldClose; SPACE toggles the clear color between black and dark blue using edge detection (previous-frame vs current-frame state) so a held key toggles exactly once per press.
- VERIFIED: ESC closes the window cleanly; SPACE toggles black/dark blue exactly once per press with no flicker while held.

### Step 4 — Draw One Shape
**Status:** Completed
**Goal:** Triangle or quad on screen via a basic shader.
**Definition of done:** Shape renders correctly, proves GPU pipeline works end to end.
**Notes:**
- Rendered a triangle via a basic shader program: inline GLSL strings (vertex shader passes position through to gl_Position; fragment shader outputs solid orange), compiled and linked with explicit compile/link error checking, vertex data uploaded once through a VBO with layout described by a VAO (attribute 0, 3 floats), drawn every frame with glDrawArrays after the clear and before the swap.
- VERIFIED: Solid orange triangle renders centered on the black background; Step 3 ESC close and SPACE background toggle still work.

### Step 5 — Math Layer
**Status:** Completed
**Goal:** Vectors, matrices, transforms.
**Definition of done:** Can translate/rotate/scale the shape from Step 4 using your own math code (or a math lib you understand).
**Notes:**
- Wrote our own header-only math layer (no external math library): src/math/vec3.h (Vec3: dot, cross, length, normalized, + - * operators) and src/math/mat4.h (Mat4: column-major m[col][row] storage matching OpenGL, identity/translation/scale/rotationZ builders, matrix multiplication, transformPoint).
- Wired into the vertex shader as 'uniform mat4 transform', uploaded each frame via glUniformMatrix4fv (transpose = GL_FALSE, storage already matches OpenGL); rotation angle advanced per frame by deltaTime at 0.9 rad/s (state += rate * deltaTime).
- Five static_assert compile-time tests prove the math before the program runs.
- No CMakeLists.txt change needed: header-only math resolves relative to main.cpp.
- VERIFIED: Triangle rotates continuously counter-clockwise at constant speed (~one full revolution per 7 seconds); ESC close and SPACE background toggle still work.

### Step 6 — Sprite/Mesh Rendering + Camera
**Status:** Completed
**Notes:**
- Added orthographic() and lookAt() builders to the own math layer (src/math/mat4.h): ortho maps a world box to the clip cube (constexpr, static_assert-proven); lookAt builds the view matrix as the camera's inverse directly from orthonormal axes (forward/right/trueUp via Vec3 cross/dot).
- Combined projection * view * model per instance and uploaded via glUniformMatrix4fv; shader unchanged (single 'transform' uniform now carries the full MVP).
- Two triangle instances drawn from identical vertex data at world positions (-1.5, 0) and (+1.5, 0), each spinning around its own center (translation * rotationZ order).
- WASD pans the camera at 3 world units/s via deltaTime (rate polling, no edge detection — movement is a rate, not a toggle); perspective divide deliberately left out of transformPoint (ortho preserves w = 1; GPU divides in hardware).
- No CMakeLists.txt change needed (no new files).
- VERIFIED: Two triangles side by side spinning independently; camera movement correctly inverted (D->scene left, A->right, W->down, S->up); ESC/SPACE intact.

### Step 7 — Basic Object/ECS System
**Status:** Completed
**Notes:**
- Design ruling: simple data-driven array of structs, NOT a full ECS — at three instances with one behavior, entity-ID registries / component pools / system schedulers would be indirection with zero payoff. Kept the one ECS idea that earns its place: entities are plain data in a contiguous container, processed by generic loops.
- New header-only file src/entity.h: pe::Entity struct (position, rotationAngle, rotationSpeed, scale) with update(deltaTime) owning the state += rate * deltaTime pattern per entity, and modelMatrix() building translation * rotationZ * scale (spin-in-place order).
- main.cpp: old global rotationAngle/rotationSpeed deleted; entities live in std::vector<pe::Entity>; ONE update loop and ONE draw loop (projection * view * entity.modelMatrix()) replaced the copy-pasted model1/mvp1/draw, model2/mvp2/draw blocks.
- Third instance added purely as DATA — position (0, 1.5), speed -1.4 rad/s (clockwise, ~4.5 s/rev), scale 0.6 — proving the loop scales with zero new rendering code.
- No CMakeLists.txt change needed (header-only; git status proved only src/main.cpp + src/entity.h changed).
- VERIFIED: Three triangles — two original spin counter-clockwise together, smaller third one spins clockwise and faster (per-entity state proven independent); WASD camera pan, SPACE toggle, ESC close all intact.

### Step 8 — Collision Detection (AABB)
**Status:** Completed
**Notes:**
- Design rulings: AABB over circles (canonical first primitive, axis-aligned math) and over exact per-triangle tests (AABB is the permanent broadphase first gate in real engines). Bounds live DIRECTLY on Entity as halfExtents — same 'don't over-engineer for 3 objects' rule as Step 7. Algorithm lives in new header-only src/collision.h (data vs. algorithm split), not in entity.h.
- Overlap test: |dx| < sum-of-half-extents AND on BOTH axes (separating axis principle; OR is the classic bug), strict '<' = touching is not colliding. constexpr + 3 new static_asserts prove it at compile time, including the X-gap-only case that catches an AND->OR mistake.
- Entity.halfExtents = (0.7071, 0.7071, 0): distance to the triangle's farthest vertex — the tightest box correct at EVERY rotation angle for spinning geometry; collision multiplies by scale so the collider matches what renders (default constructor argument keeps Step 7 call sites unchanged).
- Movement ruling: player-driven over automatic oscillation — ARROW keys move entities[0] at 2.5 units/s (WASD stays the camera's); collision becomes an interaction, and it's the engine's first real entity-movement code.
- Visible feedback: fragment shader gained 'uniform vec3 color'; colliding entities draw red per frame via glUniform3f, all others keep the Step 4 orange. Collision flags rebuilt from zero every frame (derived state, never sticky); unique pairs tested once via j = i + 1.
- No CMakeLists.txt change needed (header-only; git status proved only the three Step 8 files changed).
- VERIFIED: Driving into another triangle turns BOTH red instantly, backing off clears both to orange immediately; the red-trigger gap matches the rotation-safe AABB square; WASD camera, arrow movement, SPACE toggle, ESC close all work independently.

### Step 9 — Audio Playback
**Status:** Completed
**Notes:**
- Library ruling: miniaudio 0.11.25 (single source file, zero deps, built-in WAV decoder, readable implementation) over OpenAL-soft (heavier infra), FMOD/Wwise (proprietary black boxes), SDL_mixer (imports SDL redundantly). Fetched via FetchContent exactly like GLFW — first REAL CMakeLists.txt change since Step 4 (Declare + MakeAvailable + added to target_link_libraries).
- Build-it-ourselves exception clarified and recorded: own the logic, outsource the hardware. Math/entity/collision are provable pure logic (ours); audio terminates in WASAPI/device buffers and needs a real-time mixing thread (library, same role GLFW plays for windowing).
- Asset: assets/beep.wav generated in-tree by make_beep.ps1 (0.15 s 880 Hz sine, 44.1 kHz 16-bit mono, 5 ms fade-in / 20 ms fade-out to kill click artifacts) — reproducible, no downloaded binary of unknown provenance.
- Playback wiring: ma_engine_init after GL setup; sound loaded with a 3-candidate relative-path search (repo root / build/ / build/Release CWD) and HARD FAIL if none loads; trigger is the EDGE into collision (anyCollidingNow && !wasCollidingLastFrame — Step 3's SPACE edge pattern reused), never per-frame while overlapping; in-flight restart rewinds via ma_sound_seek_to_pcm_frame(0); cleanup in reverse creation order (sound then engine).
- Build note: first fresh configure failed with 'Could not resolve host: github.com' (sandboxed git subprocess) — succeeded on re-run outside the sandbox; first compile caught two author errors (identifier typo, ma_sound_seek_to_pcm_frames vs the real singular form), both fixed; final build clean, exe links glfw3.lib + miniaudio.lib.
- VERIFIED: silent on launch; exactly ONE beep on the frame both entities turn red; no repeat beeping while staying inside the overlap (edge detection confirmed); silence on separation; WASD/SPACE/ESC all intact with audio running.

### Step 10 — Asset Loading
**Status:** Completed
**Notes:**
- Step 9 seams fixed FIRST (refactors, not features): (1) collision-sound triggering moved from a global-OR scalar to per-entity edge detection — previous frame's full colliding VECTOR is compared entry by entry, so a new overlap starting while another is already active fires its own beep (the scalar level never dropped to false before, silencing the second event); (2) playback moved from ONE shared ma_sound to a round-robin POOL of 4 slots (Step 7's data-in-a-container pattern), each decoding the same beep.wav, so simultaneous triggers no longer fight over one playback slot.
- Library ruling: stb_image (single header, public domain, one stbi_load call to raw pixels, readable implementation) over libpng (zlib dep + chunk-level ceremony), SDL_image (drags in SDL), FreeImage/DevIL (heavy frameworks, heavier licensing). stb publishes NO version tags, so FetchContent pins exact commit SHA 2c980bb59875b0d32144a71867fbdebb2f77cd20; the stb repo has no root CMakeLists, so MakeAvailable only populates headers.
- Declare-everywhere/define-once: implementation compiles in dedicated src/stb_impl.cpp (STB_IMAGE_IMPLEMENTATION), keeping ~8,000 lines out of main.cpp — first new source file since Step 4, hence a real CMakeLists.txt change (FetchContent block + stb_impl.cpp in sources + ${stb_SOURCE_DIR} include dir).
- Asset: assets/checker.png generated in-tree by make_checker.ps1 (64x64 RGB, 8x8-block checkerboard of Step 4 orange + Step 3 dark blue; PNG written byte-by-byte — signature, IHDR, zlib-wrapped deflate IDAT, IEND, big-endian fields, CRC32 per chunk). Loaded via 3-candidate relative-path search with HARD FAIL if missing.
- Texture pipeline: interleaved vertex data (3 position + 2 UV floats, stride 5); vertex shader passes aTexCoord (location 1) to the fragment stage where rasterization interpolates it; fragment shader samples uniform sampler2D tex and multiplies by the Step 8 color uniform, now a TINT (white = untouched, red = zeroes G+B — collision feedback survives the switch to textures). Sampler holds a texture-UNIT index, set once with glUniform1i(texLocation, 0); per frame the texture binds to GL_TEXTURE0. CLAMP_TO_EDGE + LINEAR filters; glTexImage2D upload; stbi_image_free; glDeleteTextures in cleanup.
- Build note: full build-folder delete + fresh configure (348 s, stb cloned) + build succeeded FIRST TRY, zero warnings in engine code; 5 s smoke test with empty stderr.
- VERIFIED: checkerboard renders on all three triangles (upright, LINEAR-smoothed while spinning); collision shows red-TINTED checkerboard, not flat red; per-entity fix confirmed — second beep fires when a new collision starts while another is already active; pool lets two rapid beeps overlap; ESC/SPACE/WASD/arrows all intact.

### Step 11 — Scene/Level Structure
**Status:** Completed
**Notes:**
- Design ruling: enum class pe::GameState { MENU, PLAYING, PAUSED } + ONE currentState variable + single dispatch points in main.cpp (a switch for input semantics, state gates for simulation and drawing). A push-down state stack or scene-graph was rejected on the same 'don't over-engineer' principle as Step 7's ECS ruling — three states with linear transitions need no dynamic nesting or history; PAUSED already holds the world data intact, so a stack buys nothing.
- Pure logic, built ourselves: new header-only src/gamestate.h (31 lines, scoped enum in namespace pe), same pattern as entity.h/collision.h. NO new external dependency — git diff proved CMakeLists.txt byte-identical to Step 10.
- Input fork by state (first time a key's MEANING depends on state): MENU — ESC quits (level poll), SPACE starts (edge, calls resetGame). PLAYING — ESC pauses (NEW edge detector for ESC, same pattern as Step 3's SPACE), SPACE toggles clear color, WASD camera + arrow player movement gated to this state. PAUSED — ESC resumes, SPACE abandons the run back to MENU.
- Simulation gated: entity update + collision + audio edge detection run ONLY in PLAYING — the state machine WRAPS the Step 7-10 simulation, never touches it, which is why all prior behavior survives. Collision flags hoisted to loop scope so PAUSED keeps drawing the frozen tint.
- resetGame lambda restores the world to a pre-loop const initialEntities data snapshot (entities, camera home, clear color, both collision histories) — reset is an assignment, exactly what Step 7's 'entities as data' bought.
- MENU rendering: plain dark-purple clear color (0.16, 0.0, 0.24) — deliberately outside gameplay's black/blue palette since no text system exists yet. Step 10 seam half-closed: the texture bind moved INSIDE the gameplay draw path so MENU never touches a texture unit; per-entity textures deferred to Step 12.
- Build note: full build-folder delete + fresh configure (260.6 s, all three deps re-cloned) + Release build clean FIRST TRY; one upstream warning inside miniaudio's own C code, zero in engine code. Engine exe links glfw3.lib + miniaudio.lib as before.
- VERIFIED by the user directly, full checklist passed: MENU shows dark purple with no triangles; SPACE starts correctly; WASD/arrows/collision/red tint/beep all identical to Steps 1-10 inside PLAYING; ESC pauses with the scene frozen including frozen collision tint; ESC resumes exactly where it left off; SPACE from PAUSED returns to MENU; fresh SPACE from MENU resets entities to original positions; ESC from MENU closes cleanly.

### Step 12 — Game Logic Layer (your franchise)
**Status:** Not started
**Notes:**
-

## Kill Criteria
If you're stuck on one step for 2+ weeks with no forward progress and no clear next action —
stop, come back here, defend/fix/kill the step before moving on. Don't skip ahead.