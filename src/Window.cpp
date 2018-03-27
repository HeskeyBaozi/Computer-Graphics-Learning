
#include "Window.h"

Window::Window(const int width, const int height, const char *name) {
    this->initialize();
    this->window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!this->window) {
        this->isSuccessful = false;
        std::cout << "Create Window Failed." << std::endl;
        return;
    }

    glfwMakeContextCurrent(this->window);
    gl3wInit();
}

Window::~Window() = default;

void Window::initialize() {
    if (!glfwInit()) {
        this->isSuccessful = false;
        std::cout << "Initialize Failed." << std::endl;
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void Window::update() {
    glfwSwapBuffers(this->window);
    glfwPollEvents();
}

bool Window::isWindowClosed() {
    return static_cast<bool>(glfwWindowShouldClose(this->window));
}

void Window::terminate() {
    glfwTerminate();
}




