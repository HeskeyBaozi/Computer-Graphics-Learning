#ifndef HW3_BRESENHAM_H
#define HW3_BRESENHAM_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>


class Bresenham {
public:
    static Bresenham *getInstance();

    int drawLine(std::vector<GLfloat> &pixi, int x0, int y0, int x1, int y1);

    int drawCircle(int x0, int y0, int r);

private:
    GLfloat color[4] = {
            0.0f, 0.0f, 0.0f, 1.0f // black
    };

    void swap(int *a, int *b) {
        int tmp = *a;
        *a = *b;
        *b = tmp;
    }

private:
    Bresenham() = default;

};


#endif //HW3_BRESENHAM_H
