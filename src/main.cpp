/**
 * CG - HW6
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
#include "Camera.h"

// 屏幕设置
const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 800;

// 光照颜色着色器 - 冯氏光照模型
const char *lightVertexColors = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "\n"
        "out vec3 FragPos;\n"
        "out vec3 Normal;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragPos = vec3(model * vec4(aPos, 1.0));\n"
        "    Normal = mat3(transpose(inverse(model))) * aNormal;  \n"
        "    \n"
        "    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
        "}";

// 光照颜色着色器 - 冯氏光照模型
const char *lightFragmentColors = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in vec3 Normal;  \n"
        "in vec3 FragPos;  \n"
        "  \n"
        "uniform vec3 lightPos; \n"
        "uniform vec3 viewPos; \n"
        "uniform vec3 lightColor;\n"
        "uniform vec3 objectColor;\n"
        "uniform float ambientStrength;\n"
        "uniform float diffuseStrength;\n"
        "uniform float specularStrength;\n"
        "uniform int shininess;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // ambient\n"
        "    vec3 ambient = ambientStrength * lightColor;\n"
        "  \t\n"
        "    // diffuse \n"
        "    vec3 norm = normalize(Normal);\n"
        "    vec3 lightDir = normalize(lightPos - FragPos);\n"
        "    float diff = max(dot(norm, lightDir), 0.0);\n"
        "    vec3 diffuse = diffuseStrength * diff * lightColor;\n"
        "    \n"
        "    // specular\n"
        "    vec3 viewDir = normalize(viewPos - FragPos);\n"
        "    vec3 reflectDir = reflect(-lightDir, norm);  \n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);\n"
        "    vec3 specular = specularStrength * spec * lightColor;  \n"
        "        \n"
        "    vec3 result = (ambient + diffuse + specular) * objectColor;\n"
        "    FragColor = vec4(result, 1.0);\n"
        "} ";

// 光照颜色着色器 - Gouraud着色
const char *lightVertexColorsG = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "\n"
        "out vec3 LightingColor;\n"
        "\n"
        "uniform vec3 lightPos;\n"
        "uniform vec3 viewPos;\n"
        "uniform vec3 lightColor;\n"
        "uniform float ambientStrength;\n"
        "uniform float diffuseStrength;\n"
        "uniform float specularStrength;\n"
        "uniform int shininess;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "    vec3 Position = vec3(model * vec4(aPos, 1.0));\n"
        "    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;\n"
        "    vec3 ambient = ambientStrength * lightColor;\n"
        "    vec3 norm = normalize(Normal);\n"
        "    vec3 lightDir = normalize(lightPos - Position);\n"
        "    float diff = max(dot(norm, lightDir), 0.0);\n"
        "    vec3 diffuse = diffuseStrength * diff * lightColor;\n"
        "    vec3 viewDir = normalize(viewPos - Position);\n"
        "    vec3 reflectDir = reflect(-lightDir, norm);  \n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);\n"
        "    vec3 specular = specularStrength * spec * lightColor;      \n"
        "\n"
        "    LightingColor = ambient + diffuse + specular;\n"
        "}";

const char *lightFragmentColorsG = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in vec3 LightingColor; \n"
        "\n"
        "uniform vec3 objectColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(LightingColor * objectColor, 1.0);\n"
        "}";

// 光源颜色着色器
const char *lampVertexColors = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main()\n"
        "{\n"
        "\tgl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}";

const char *lampFragmentColors = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0);\n"
        "}";

// 镜头配置
glm::vec3 cameraPosition(0.0f, 0.0f, 3.0f);
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;
Camera camera(cameraPosition);

// 全局时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// GUI 切换
bool isFPS = true;
bool isPhone = true;

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

int main() {

    // 初始化GLFW
    glfwSetErrorCallback([](int error, const char *description) {
        fprintf(stderr, "Error %d: %s\n", error, description);
    });
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建GLFW窗口
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "He Zhiyu 15331097", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    });
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // 初始化GL3W
    gl3wInit();

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 构建和编译着色器
    Shader lightShader(lightVertexColors, lightFragmentColors); // Phone
    Shader lightShaderG(lightVertexColorsG, lightFragmentColorsG); // Gouraud
    Shader lampShader(lampVertexColors, lampFragmentColors);

    // 渲染初始场景的顶点数据，一个立方体
    std::vector<GLfloat> vertices = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
    };

    // 构造立方体
    unsigned int VBO, targetVAO;
    glGenVertexArrays(1, &targetVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(targetVAO);

    // 立方体的位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void *) (3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // 构造灯光
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void *) 0);
    glEnableVertexAttribArray(0);

    // 灯光位置参数
    glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);

    // 冯氏参数
    float ambientStrength = 0.1f; // 环境光照系数
    float diffuseStrength = 1.0f; // 漫反射光照系数
    float specularStrength = 0.5f; // 镜面光照系数
    int shininess = 32;

    // 物体参数
    glm::vec3 objectColor = glm::vec3(175.0f, 215.0f, 58.0f) / 255.0f;

    // GUI配置
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();
    auto renderGUI = [&]() {
        // 绘制GUI
        ImGui_ImplGlfwGL3_NewFrame();

        // FPS 信息
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::ColorEdit3("Object Color", (float *) &objectColor);
        ImGui::SliderFloat3("Light Position", (float *) &lightPosition, -5, 5);
        ImGui::SliderFloat3("Camera Position", (float *) &camera.Position, -5, 5);
        ImGui::Checkbox("is FPS Mode", &isFPS);
        ImGui::Text("Current Model: %s", isPhone ? "Phone" : "Gouraud");
        ImGui::Checkbox("Change Model", &isPhone);
        ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("diffuseStrength", &diffuseStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("specularStrength", &specularStrength, 0.0f, 1.0f);
        ImGui::SliderInt("shininess", &shininess, 2, 256);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    };


    // 主循环
    while (!glfwWindowShouldClose(window)) {

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader target = isPhone ? lightShader : lightShaderG;

        target.use();
        target.setVec3("objectColor", objectColor[0], objectColor[1], objectColor[2]);
        target.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        target.setVec3("lightPos", lightPosition);
        target.setVec3("viewPos", camera.Position);
        target.setFloat("ambientStrength", ambientStrength);
        target.setFloat("diffuseStrength", diffuseStrength);
        target.setFloat("specularStrength", specularStrength);
        target.setInt("shininess", shininess);

        // 视图投影变换
        glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT,
                0.1f, 100.0f
        );
        glm::mat4 view = camera.GetViewMatrix();
        target.setMat4("projection", projection);
        target.setMat4("view", view);

        // 世界视图变换
        glm::mat4 model;
        target.setMat4("model", model);

        // 绘制立方体, 使用三角形来拼接
        glBindVertexArray(targetVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制灯源
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        model = glm::mat4();
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.2f));
        lampShader.setMat4("model", model);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // GUI
        renderGUI();

        // 处理事件等其他
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        isFPS = !isFPS;
        if (isFPS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }


}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (isFPS) {
        if (firstMouse) {
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
            firstMouse = false;
        }

        auto xoffset = static_cast<float>(xpos - lastX);
        auto yoffset = static_cast<float>(lastY - ypos); // reversed since y-coordinates go from bottom to top

        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}