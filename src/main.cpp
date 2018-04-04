/**
 * CG - HW4
 * @author 何志宇<hezhiyu233@foxmail.com> 15331097
 */

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <cstdio>
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Shader.h"

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 800;

const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "   ourColor = aColor;\n"
        "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(ourColor, 1.0f);\n"
        "}\n\0";

int main() {

    /************************************************************
     *                  OpenGL & ImGUI Setup
     ************************************************************/

    // glfw window
    glfwSetErrorCallback([](int error, const char *description) {
        fprintf(stderr, "Error %d: %s\n", error, description);
    });
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window = glfwCreateWindow(800, 800, "He Zhiyu 15331097", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gl3wInit();

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsDark();
    ImVec4 clear_color = ImVec4(49.0f / 255.0f, 57.0f / 255.0f, 88.0f / 255.0f, 1.00f);

    glEnable(GL_DEPTH_TEST);
    Shader ourShader(vertexShaderSource, fragmentShaderSource);

    std::vector<GLfloat> vertices = {
            -0.5f, -0.5f, -0.5f, 0.6f, 0.6f, 0.6f,
            0.5f, -0.5f, -0.5f, 0.6f, 0.6f, 0.6f,
            0.5f, 0.5f, -0.5f, 0.6f, 0.6f, 0.6f,
            0.5f, 0.5f, -0.5f, 0.6f, 0.6f, 0.6f,
            -0.5f, 0.5f, -0.5f, 0.6f, 0.6f, 0.6f,
            -0.5f, -0.5f, -0.5f, 0.6f, 0.6f, 0.6f,

            -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
            0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,

            -0.5f, 0.5f, 0.5f, 0.4f, 0.4f, 0.4f,
            -0.5f, 0.5f, -0.5f, 0.4f, 0.4f, 0.4f,
            -0.5f, -0.5f, -0.5f, 0.4f, 0.4f, 0.4f,
            -0.5f, -0.5f, -0.5f, 0.4f, 0.4f, 0.4f,
            -0.5f, -0.5f, 0.5f, 0.4f, 0.4f, 0.4f,
            -0.5f, 0.5f, 0.5f, 0.4f, 0.4f, 0.4f,

            0.5f, 0.5f, 0.5f, 0.3f, 0.3f, 0.3f,
            0.5f, 0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
            0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
            0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
            0.5f, -0.5f, 0.5f, 0.3f, 0.3f, 0.3f,
            0.5f, 0.5f, 0.5f, 0.3f, 0.3f, 0.3f,

            -0.5f, -0.5f, -0.5f, 0.2f, 0.2f, 0.2f,
            0.5f, -0.5f, -0.5f, 0.2f, 0.2f, 0.2f,
            0.5f, -0.5f, 0.5f, 0.2f, 0.2f, 0.2f,
            0.5f, -0.5f, 0.5f, 0.2f, 0.2f, 0.2f,
            -0.5f, -0.5f, 0.5f, 0.2f, 0.2f, 0.2f,
            -0.5f, -0.5f, -0.5f, 0.2f, 0.2f, 0.2f,

            -0.5f, 0.5f, -0.5f, 0.1f, 0.1f, 0.1f,
            0.5f, 0.5f, -0.5f, 0.1f, 0.1f, 0.1f,
            0.5f, 0.5f, 0.5f, 0.1f, 0.1f, 0.1f,
            0.5f, 0.5f, 0.5f, 0.1f, 0.1f, 0.1f,
            -0.5f, 0.5f, 0.5f, 0.1f, 0.1f, 0.1f,
            -0.5f, 0.5f, -0.5f, 0.1f, 0.1f, 0.1f,
    };

    glm::vec3 cubePosition = glm::vec3(0.0f, 0.0f, 0.0f);
    float angle = 0.0f;
    float scale = 45.0f;
    bool isAnimate = false;
    int frameCount = 0;

    // VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // location === 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    // location === 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    ourShader.use();

    /************************************************************
     *                         Render
     ************************************************************/

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::ColorEdit3("Background Color", (float *) &clear_color);
            ImGui::SliderFloat3("Cube Position", (float *) &cubePosition, -1, 1);
            ImGui::SliderFloat("Angle", &angle, 0, 360);
            ImGui::SliderFloat("Scale", &scale, 1, 180);
            ImGui::Checkbox("Animate Rotate", &isAnimate);
        }

        ourShader.use();

        glm::mat4 view;
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(scale), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, 0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // animate
        if (isAnimate) {
            angle = (frameCount++) % 360;
            if (frameCount > 360) {
                frameCount = 0;
            }
        }

        glBindVertexArray(VAO);

        {
            glm::mat4 model;
            model = glm::translate(model, cubePosition);
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 1.0f));
            ourShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}
