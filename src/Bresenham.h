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

    int drawCircle(std::vector<GLfloat> &pixi, int x0, int y0, int r);

    std::vector<GLfloat> color = {
            0.0f, 0.0f, 0.0f, 1.0f // black
    };

    int scale = 1000;

private:

    void swap(int *a, int *b) {
        int tmp = *a;
        *a = *b;
        *b = tmp;
    }

private:
    Bresenham() = default;

    void addPoints(std::vector<GLfloat> &pixi, int x, int y);

    float normalize(int target);

};


#endif //HW3_BRESENHAM_H
