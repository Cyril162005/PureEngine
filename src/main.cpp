#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

/**
 * Step 1: Window + Context Creation
 * Goal: Open a window and create a valid OpenGL context.
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

    // 6. The Main Loop
    while (!glfwWindowShouldClose(window)) {
        // A. Poll for events (input, window resize, etc.)
        glfwPollEvents();

        // B. Clear the screen to black
        // glClearColor sets the color to clear to (R, G, B, A)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
