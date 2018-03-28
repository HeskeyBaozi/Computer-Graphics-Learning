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
#include "Bresenham.h"

const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos, 1.0);\n"
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
    ImVec4 draw_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    /************************************************************
    *                  Shader Setup
    ************************************************************/

    // Compile Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // test
    int isSuccessful = 0;
    char infoLog[512] = "";
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isSuccessful);
    if (!isSuccessful) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Fragment Shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // test
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isSuccessful);
    if (!isSuccessful) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Link
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // test
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isSuccessful);
    if (!isSuccessful) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    /************************************************************
     *                  Triangle Setup
     ************************************************************/

    ImVec2 triangleVertex1 = ImVec2(-0.5f, -0.5f); // default: 左下
    ImVec2 triangleVertex2 = ImVec2(0.5f, -0.5f); // default: 右下
    ImVec2 triangleVertex3 = ImVec2(0.0f, 0.5f); // default: 正上

    // vertices: [x, y, z, r, g, b]
    std::vector<GLfloat> vertices = {
            triangleVertex1.x, triangleVertex1.y, 0.0f,
            0.0f, 0.0f, 0.0f,

            triangleVertex2.x, triangleVertex2.y, 0.0f,
            0.0f, 0.0f, 0.0f,

            triangleVertex3.x, triangleVertex3.y, 0.0f,
            0.0f, 0.0f, 0.0f
    };

    /************************************************************
    *                  Circle Setup
    ************************************************************/

    bool isShowCircle = false;
    ImVec2 circleCenter = ImVec2(0.0f, 0.0f);
    float circleRadius = 0.3f;

    // VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // use shader
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

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


    /************************************************************
     *                         Render
     ************************************************************/

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::ColorEdit3("Background Color", (float *) &clear_color);
            ImGui::ColorEdit3("Draw Color", (float *) &draw_color);
            ImGui::Checkbox(isShowCircle ? "show Circle" : "show Triple", &isShowCircle);
            if (!isShowCircle) {
                ImGui::SliderFloat2("Vertex 1", (float *) &triangleVertex1, -1, 1);
                ImGui::SliderFloat2("Vertex 2", (float *) &triangleVertex2, -1, 1);
                ImGui::SliderFloat2("Vertex 3", (float *) &triangleVertex3, -1, 1);
            } else {
                ImGui::SliderFloat2("Circle Center", (float *) &circleCenter, -1, 1);
                ImGui::SliderFloat("Circle Radius", &circleRadius, 0, 1);
            }
        }

        std::vector<GLfloat> triplePoints; // 三角形点集合

        Bresenham::getInstance()->color = {
                draw_color.x, draw_color.y, draw_color.z, draw_color.w
        };

        int count1 = Bresenham::getInstance()
                ->drawLine(triplePoints,
                           static_cast<int>(triangleVertex1.x * 1000),
                           static_cast<int>(triangleVertex1.y * 1000),
                           static_cast<int>(triangleVertex2.x * 1000),
                           static_cast<int>(triangleVertex2.y * 1000));
        int count2 = Bresenham::getInstance()
                ->drawLine(triplePoints,
                           static_cast<int>(triangleVertex1.x * 1000),
                           static_cast<int>(triangleVertex1.y * 1000),
                           static_cast<int>(triangleVertex3.x * 1000),
                           static_cast<int>(triangleVertex3.y * 1000));
        int count3 = Bresenham::getInstance()
                ->drawLine(triplePoints,
                           static_cast<int>(triangleVertex2.x * 1000),
                           static_cast<int>(triangleVertex2.y * 1000),
                           static_cast<int>(triangleVertex3.x * 1000),
                           static_cast<int>(triangleVertex3.y * 1000));

        std::vector<GLfloat> circlePoints; // 圆点集合

        int count4 = Bresenham::getInstance()
                ->drawCircle(circlePoints,
                             static_cast<int>(circleCenter.x * 1000),
                             static_cast<int>(circleCenter.y * 1000),
                             static_cast<int>(circleRadius * 1000));

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        if (!isShowCircle) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * triplePoints.size(), triplePoints.data(), GL_STATIC_DRAW);
        } else {
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * circlePoints.size(), circlePoints.data(), GL_STATIC_DRAW);
        }
        glBindVertexArray(VAO);

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);


        glDrawArrays(GL_POINTS, 0, !isShowCircle ? count1 + count2 + count3 : count4);
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
