/**
 * CG - HW7
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


// 阴影映射着色器
const char *SMVS = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "layout (location = 2) in vec2 aTexCoords;\n"
        "\n"
        "out vec2 TexCoords;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec3 FragPos;\n"
        "    vec3 Normal;\n"
        "    vec2 TexCoords;\n"
        "    vec4 FragPosLightSpace;\n"
        "} vs_out;\n"
        "\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"
        "uniform mat4 lightSpaceMatrix;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));\n"
        "    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;\n"
        "    vs_out.TexCoords = aTexCoords;\n"
        "    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}";

const char *SMFS = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in VS_OUT {\n"
        "    vec3 FragPos;\n"
        "    vec3 Normal;\n"
        "    vec2 TexCoords;\n"
        "    vec4 FragPosLightSpace;\n"
        "} fs_in;\n"
        "\n"
        "uniform sampler2D diffuseTexture;\n"
        "uniform sampler2D shadowMap;\n"
        "\n"
        "uniform vec3 objectColor;\n"
        "uniform vec3 lightPos;\n"
        "uniform vec3 viewPos;\n"
        "\n"
        "float ShadowCalculation(vec4 fragPosLightSpace)\n"
        "{\n"
        "    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
        "    projCoords = projCoords * 0.5 + 0.5;\n"
        "    float closestDepth = texture(shadowMap, projCoords.xy).r; \n"
        "    float currentDepth = projCoords.z;\n"
        "    vec3 normal = normalize(fs_in.Normal);\n"
        "    vec3 lightDir = normalize(lightPos - fs_in.FragPos);\n"
        "    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);\n"
        "    float shadow = 0.0;\n"
        "    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);\n"
        "    for(int x = -1; x <= 1; ++x)\n"
        "    {\n"
        "        for(int y = -1; y <= 1; ++y)\n"
        "        {\n"
        "            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; \n"
        "            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        \n"
        "        }    \n"
        "    }\n"
        "    shadow /= 9.0;\n"
        "    \n"
        "    if(projCoords.z > 1.0)\n"
        "        shadow = 0.0;\n"
        "        \n"
        "    return shadow;\n"
        "}\n"
        "\n"
        "void main()\n"
        "{           \n"
        "    vec3 color = objectColor;\n"
        "    vec3 normal = normalize(fs_in.Normal);\n"
        "    vec3 lightColor = vec3(0.3);\n"
        "    // ambient\n"
        "    vec3 ambient = 0.3 * color;\n"
        "    // diffuse\n"
        "    vec3 lightDir = normalize(lightPos - fs_in.FragPos);\n"
        "    float diff = max(dot(lightDir, normal), 0.0);\n"
        "    vec3 diffuse = diff * lightColor;\n"
        "    // specular\n"
        "    vec3 viewDir = normalize(viewPos - fs_in.FragPos);\n"
        "    vec3 reflectDir = reflect(-lightDir, normal);\n"
        "    float spec = 0.0;\n"
        "    vec3 halfwayDir = normalize(lightDir + viewDir);  \n"
        "    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);\n"
        "    vec3 specular = spec * lightColor;    \n"
        "    // calculate shadow\n"
        "    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      \n"
        "    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    \n"
        "    \n"
        "    FragColor = vec4(lighting, 1.0);\n"
        "}";

const char *DepthVS = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "\n"
        "uniform mat4 lightSpaceMatrix;\n"
        "uniform mat4 model;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);\n"
        "}";

const char *DepthFS = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec3 objectColor;\n"
        "\n"
        "void main()\n"
        "{             \n"
        "    gl_FragDepth = gl_FragCoord.z;\n"
        "    vec3 result = objectColor;\n"
        "    FragColor = vec4(result, 1.0);\n"
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

// 全局网格meshes
unsigned int planeVAO;

// GUI 切换
bool isFPS = true;

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void renderScene(const Shader &shader, const glm::vec3 &objectColor);

void renderCube();

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
    Shader shader(SMVS, SMFS);
    Shader depthShader(DepthVS, DepthFS);

    std::vector<GLfloat> planeVertices = {
            25.0f, -0.5f, 25.0f, 0.0f, 0.0f, 0.0f, 25.0f, 0.0f,
            -25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,

            25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
            -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
            25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f
    };

    // 配置平面
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * planeVertices.size(), planeVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glBindVertexArray(0);


    // 配置深度贴图 和 帧缓冲对象
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("shadowMap", 1);

    // 灯光位置参数
    glm::vec3 lightPosition(-2.0f, 4.0f, -1.0f);

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
//        ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.0f, 1.0f);
//        ImGui::SliderFloat("diffuseStrength", &diffuseStrength, 0.0f, 1.0f);
//        ImGui::SliderFloat("specularStrength", &specularStrength, 0.0f, 1.0f);
//        ImGui::SliderInt("shininess", &shininess, 2, 256);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    };


    // 主循环
    while (!glfwWindowShouldClose(window)) {

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(113 / 255.0f, 181 / 255.0f, 241 / 255.0f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 渲染场景深度贴图
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderScene(depthShader, objectColor);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 场景透视变换
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // 光照配置
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPosition);
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderScene(shader, objectColor);

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


void renderScene(const Shader &shader, const glm::vec3 &objectColor) {

    glm::mat4 model;
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    model = glm::mat4();
    model = glm::translate(model, glm::vec3(0.5f, 1.5f, 1.5f));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", objectColor);
    renderCube();
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", objectColor);
    renderCube();
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.8f));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", objectColor);
    renderCube();
}


// 绘制立方体
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

void renderCube() {
    if (cubeVAO == 0) {
        float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
                1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
                1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
                1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
                -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
                -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
                1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
                1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
                // bottom face
                -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
                1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
                1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
                1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
                -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}