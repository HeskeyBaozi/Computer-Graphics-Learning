/**
 * CG - HW3
 * @author 何志宇<hezhiyu233@foxmail.com> 15331097
 */

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <cstdio>
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Bresenham.h"

#define V_GLSL "D:\\CLionProjects\\hw3\\src\\shaders\\v.glsl"
#define F_GLSL "D:\\CLionProjects\\hw3\\src\\shaders\\f.glsl"


int main() {

    /************************************************************
     *                  OpenGL & ImGUI Setup
     ************************************************************/

    Window window(800, 600, "HW3 - Hezhiyu 15331097");
    gl3wInit();
    if (!window.isSuccessful) {
        std::cout << "Create Window Object Failed" << std::endl;
        return -1;
    }

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window.getWindow(), true);

    // Setup style
    ImGui::StyleColorsDark();
    ImVec4 clear_color = ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.00f);


    ImVec2 lineStart = ImVec2(-0.5f, -0.5f);
    ImVec2 lineEnd = ImVec2(0.0f, 0.5f);

    ImVec2 CircleCenter = ImVec2(0.0f, 0.0f);
    float CircleRadius = 0.3f;


    /************************************************************
     *                         Render
     ************************************************************/

    while (!window.isWindowClosed()) {

        int count = Bresenham::getInstance()->drawLine(static_cast<int>(lineStart.x * 100),
                                                       static_cast<int>(lineStart.y * 100),
                                                       static_cast<int>(lineEnd.x * 100),
                                                       static_cast<int>(lineEnd.y * 100));

        int count2 = Bresenham::getInstance()->drawCircle(static_cast<int>(CircleCenter.x * 100),
                                                          static_cast<int>(CircleCenter.y * 100),
                                                          static_cast<int>(CircleRadius * 100));

        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Text("Change Color");
            ImGui::ColorEdit3("Background Color", (float *) &clear_color);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
        }



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
