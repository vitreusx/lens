#pragma once

class Window {
    private:
    GLFWwindow *window;

    static void resetViewport(GLFWwindow*, int w, int h) {
        glViewport(0, 0, w, h);
    }

    public:
    Window() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(800, 600, "lens", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, resetViewport);

        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

    ~Window() {
        glfwTerminate();
    }

    operator GLFWwindow*() {
        return window;
    }
};