/**
 * CG - HW5
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
    ImVec4 clear_color = ImVec4(93.0f / 255.0f, 101.0f / 255.0f, 131.0f / 255.0f, 1.00f);

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

    glm::vec3 cubePosition = glm::vec3(-1.5f, 0.5f, -1.5f);
    float angle = 0.0f;
    float scale = 45.0f;
    bool isAnimate = false;
    bool isPerspective = true;


    float lookAtRadius = 10.0f;

    // ortho arguments
    // glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f);
    float left = -2.0f;
    float right = 2.0f;
    float bottom = -2.0f;
    float top = 2.0f;
    float nearArgs = 0.1f;
    float farArgs = 10.0f;

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
            ImGui::Checkbox("View Changing", &isAnimate);
            ImGui::SliderFloat("Look At Radius", &lookAtRadius, 1.0f, 100.0f);
            ImGui::Checkbox("isPerspective", &isPerspective);
            ImGui::SliderFloat("Angle", &angle, 0, 360);
            ImGui::SliderFloat("Scale", &scale, 1, 180);
            if (!isPerspective) {
                ImGui::SliderFloat("left", &left, -5, 5);
                ImGui::SliderFloat("right", &right, -5, 5);
                ImGui::SliderFloat("bottom", &bottom, -5, 5);
                ImGui::SliderFloat("top", &top, -5, 5);
                ImGui::SliderFloat("near", &nearArgs, -5, 5);
                ImGui::SliderFloat("far", &farArgs, -10, 10);
            }
        }

        ourShader.use();

        // transformations
        glm::mat4 view;
        if (isAnimate) {
            auto camX = static_cast<float>(sin(glfwGetTime()) * lookAtRadius);
            auto camZ = static_cast<float>(cos(glfwGetTime()) * lookAtRadius);
            view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        } else {
            view = glm::translate(view, glm::vec3(0.0f, 1.0f, -5.0f));
        }


        glm::mat4 projection;
        if (isPerspective) {
            projection = glm::perspective(glm::radians(scale), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, 0.1f,
                                          100.0f);
        } else {
            projection = glm::ortho(left, right, bottom, top, nearArgs, farArgs);
        }


        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glBindVertexArray(VAO);

        {
            glm::mat4 model;
            if (!isAnimate) {
                model = glm::translate(model, cubePosition);
            }
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
