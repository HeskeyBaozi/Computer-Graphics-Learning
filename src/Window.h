#ifndef HW3_WINDOW_H
#define HW3_WINDOW_H

/**
 * 封装OpenGL Window窗口
 * @author 何志宇<hezhiyu233@foxmail.com> 15331097
 */

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

class Window {
public:
    Window(int width, int height, const char *name);

    ~Window();

    void update();

    void drawPoints(std::vector<std::pair<GLfloat, GLfloat>> &pixi);

    bool isWindowClosed();

    void terminate();

    inline GLFWwindow *getWindow() const {
        return this->window;
    }

    bool isSuccessful = true;

private:
    void initialize();

    GLFWwindow *window;
};


#endif //HW3_WINDOW_H
