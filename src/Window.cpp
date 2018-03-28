
#include "Window.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

Window::Window(const int width, const int height, const char *name) {
    this->initialize();
    this->window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!this->window) {
        this->isSuccessful = false;
        std::cout << "Create Window Failed." << std::endl;
        return;
    }

    glfwMakeContextCurrent(this->window);
//    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
//        this->isSuccessful = false;
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return;
//    }
    gl3wInit();


    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard
    // Controls  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable
    // Gamepad Controls
    ImGui_ImplGlfwGL3_Init(this->window, true);

    // Setup style
    ImGui::StyleColorsDark();
    glViewport(0, 0, width, height);
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

void Window::drawPoints(std::vector<std::pair<GLfloat, GLfloat>> &pixi) {
    for (auto &p:pixi) {

    }
}




