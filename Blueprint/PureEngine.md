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
**Status:** Not started
**Goal:** Loop that clears screen + swaps buffers every frame. Track delta time.
**Definition of done:** Window shows a solid clear color, updates every frame, closes cleanly.
**Notes:**
-

### Step 3 — Input Handling
**Status:** Not started
**Goal:** Poll keyboard/mouse inside the loop.
**Definition of done:** Pressing a key does something visible (e.g. changes clear color).
**Notes:**
-

### Step 4 — Draw One Shape
**Status:** Not started
**Goal:** Triangle or quad on screen via a basic shader.
**Definition of done:** Shape renders correctly, proves GPU pipeline works end to end.
**Notes:**
-

### Step 5 — Math Layer
**Status:** Not started
**Goal:** Vectors, matrices, transforms.
**Definition of done:** Can translate/rotate/scale the shape from Step 4 using your own math code (or a math lib you understand).
**Notes:**
-

### Step 6 — Sprite/Mesh Rendering + Camera
**Status:** Not started
**Notes:**
-

### Step 7 — Basic Object/ECS System
**Status:** Not started
**Notes:**
-

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