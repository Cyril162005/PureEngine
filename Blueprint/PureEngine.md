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
**Status:** Not started
**Notes:**
-

### Step 9 — Audio Playback
**Status:** Not started
**Notes:**
-

### Step 10 — Asset Loading
**Status:** Not started
**Notes:**
-

### Step 11 — Scene/Level Structure
**Status:** Not started
**Notes:**
-

### Step 12 — Game Logic Layer (your franchise)
**Status:** Not started
**Notes:**
-

## Kill Criteria
If you're stuck on one step for 2+ weeks with no forward progress and no clear next action —
stop, come back here, defend/fix/kill the step before moving on. Don't skip ahead.