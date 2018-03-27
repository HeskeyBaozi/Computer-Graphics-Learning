/**
 * CG - HW3
 * @author 何志宇<hezhiyu233@foxmail.com> 15331097
 */

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <cstdio>
#include <iostream>
#include "Window.h"
#include "Shader.h"

#define VERTEX_SHADER_URL "D:/CLionProjects/hw3/src/shaders/v.glsl"
#define FRAGMENT_SHADER_URL "D:/CLionProjects/hw3/src/shaders/f.glsl"

int main() {

    /************************************************************
     *                  OpenGL & ImGUI Setup
     ************************************************************/

    Window window(800, 600, "HW3 - Hezhiyu 15331097");
    if (!window.isSuccessful) {
        std::cout << "Create Window Object Failed" << std::endl;
        return -1;
    }

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window.getWindow(), true);

    // Setup style
    ImGui::StyleColorsDark();
    ImVec4 clear_color = ImVec4(206.0f / 255.0f, 161.0f / 255.0f, 200.0f / 255.0f, 1.00f);





    /************************************************************
     *                  Triangle Setup
     ************************************************************/

    ImVec4 triangleVertex1 = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // default: Red
    ImVec4 triangleVertex2 = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // default: Green
    ImVec4 triangleVertex3 = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // default: Blue

    // vertices: [x, y, z, r, g, b]
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            triangleVertex1.x, triangleVertex1.y, triangleVertex1.z,  // 左下

            0.5f, -0.5f, 0.0f,
            triangleVertex2.x, triangleVertex2.y, triangleVertex2.z,  // 右下

            0.0f, 0.5f, 0.0f,
            triangleVertex3.x, triangleVertex3.y, triangleVertex3.z   // 正上
    };

    // VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // location === 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    // location === 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /************************************************************
    *                  Shader Setup
    ************************************************************/

    Shader shader(VERTEX_SHADER_URL, FRAGMENT_SHADER_URL);

    shader.use();


    /************************************************************
     *                         Render
     ************************************************************/

    while (!window.isWindowClosed()) {
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Text("Change Color");
            ImGui::ColorEdit3("Background Color", (float *) &clear_color);
            ImGui::ColorEdit3("Left Color", (float *) &triangleVertex1);
            ImGui::ColorEdit3("Right Color", (float *) &triangleVertex2);
            ImGui::ColorEdit3("Top Color", (float *) &triangleVertex3);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
        }

        float currentVertices[] = {
                -0.5f, -0.5f, 0.0f,
                triangleVertex1.x, triangleVertex1.y, triangleVertex1.z,  // 左下

                0.5f, -0.5f, 0.0f,
                triangleVertex2.x, triangleVertex2.y, triangleVertex2.z,  // 右下

                0.0f, 0.5f, 0.0f,
                triangleVertex3.x, triangleVertex3.y, triangleVertex3.z   // 正上
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(currentVertices), currentVertices, GL_STATIC_DRAW);
        glBindVertexArray(VAO);

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window.getWindow(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        window.update();
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    window.terminate();

    return 0;
}
