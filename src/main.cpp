/**
 * CG - HW3
 * @author 何志宇<hezhiyu233@foxmail.com> 15331097
 */

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include "Window.h"
#include "Bresenham.h"
#include "Shader.h"

#define V_GLSL "D:\\CLionProjects\\hw3\\shaders\\v.glsl"
#define F_GLSL "D:\\CLionProjects\\hw3\\shaders\\f.glsl"


int main() {

    /************************************************************
     *                  OpenGL & ImGUI Setup
     ************************************************************/

    Window window(800, 600, "HW3 - Hezhiyu 15331097");
    if (!window.isSuccessful) {
        std::cout << "Create Window Object Failed" << std::endl;
        return -1;
    }

    std::vector<GLfloat> points{};


    Shader shader(V_GLSL, F_GLSL);
    shader.use();

    // VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points.data()), points.data(), GL_STATIC_DRAW);

    // VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points.data()), points.data(), GL_STATIC_DRAW);

    // location === 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    // location === 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    ImVec4 clear_color = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.00f);


    GLfloat vertex1[] = {
            -0.5f, 0.0f
    };
    GLfloat vertex2[] = {
            0.5f, 0.0f
    };
    GLfloat vertex3[] = {
            0.0f, 0.5f
    };

    GLfloat CircleCenter[] = {
            0.0f, 0.0f
    };
    GLfloat CircleRadius = 0.3f;


    /************************************************************
     *                         Render
     ************************************************************/

    while (!window.isWindowClosed()) {

        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Begin("HW3");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::ColorEdit3("Background Color", (float *) &clear_color);

            ImGui::SliderFloat2("TripleVertex 1", vertex1, -1, 1);
            ImGui::SliderFloat2("TripleVertex 2", vertex2, -1, 1);
            ImGui::SliderFloat2("TripleVertex 3", vertex3, -1, 1);
            ImGui::End();
        }

        points.clear();
        glBindVertexArray(VAO);

        int count1 = Bresenham::getInstance()
                ->drawLine(points,
                           static_cast<int>(vertex1[0] * 100),
                           static_cast<int>(vertex1[1] * 100),
                           static_cast<int>(vertex2[0] * 100),
                           static_cast<int>(vertex2[1] * 100));
        int count2 = Bresenham::getInstance()
                ->drawLine(points,
                           static_cast<int>(vertex2[0] * 100),
                           static_cast<int>(vertex2[1] * 100),
                           static_cast<int>(vertex3[0] * 100),
                           static_cast<int>(vertex3[1] * 100));
        int count3 = Bresenham::getInstance()
                ->drawLine(points,
                           static_cast<int>(vertex1[0] * 100),
                           static_cast<int>(vertex1[1] * 100),
                           static_cast<int>(vertex3[0] * 100),
                           static_cast<int>(vertex3[1] * 100));

        int baseCount = static_cast<int>(points.size() / 6);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points.data()), points.data(), GL_STATIC_DRAW);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, count1 + count2 + count3);

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window.getWindow(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
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
