#include "Bresenham.h"

using namespace std;

Bresenham *Bresenham::getInstance() {
    static Bresenham *instance = nullptr;
    if (instance == nullptr) {
        instance = new Bresenham();
    }
    return instance;
}

int Bresenham::drawLine(int x0, int y0, int x1, int y1) {

    auto dx = static_cast<int>(std::abs(x1 - x0));
    auto dy = static_cast<int>(std::abs(y1 - y0));

    vector<pair<float, float>> pixi;

    bool isKBiggerThan1 = false;

    if (dy > dx) {
        isKBiggerThan1 = true;
        this->swap(&x0, &y0);
        this->swap(&x1, &y1);
        this->swap(&dx, &dy);
    }

    int cx = (x1 - x0) > 0 ? 1 : -1;
    int cy = (y1 - y0) > 0 ? 1 : -1;

    int x = x0;
    int y = y0;

    int p = 2 * dy - dx;
    int pointCounter = 0; // 点个数计数器



    do {
        float normalizedX = float(isKBiggerThan1 ? y : x) / 100;
        float normalizedY = float(isKBiggerThan1 ? x : y) / 100;
        pixi.emplace_back(normalizedX, normalizedY);
        pointCounter++;

        if (p < 0) {
            p += 2 * dy;
        } else {
            y += cy;
            p += 2 * (dy - dx);
        }

        x += cx;
    } while (x <= x1);

    glColor4f(this->color[0], this->color[1], this->color[2], this->color[3]);
    glBegin(GL_POINTS);

    for (const auto &item:pixi) {
        glVertex2f(item.first, item.second);
    }
    glEnd();
    return pointCounter;
}

int Bresenham::drawCircle(int x0, int y0, int r) {
    int x = -r, y = 0;
    int pointCounter = 0; // 点个数计数器
    int error = 2 - 2 * r;

    glColor4f(this->color[0], this->color[1], this->color[2], this->color[3]);
    glBegin(GL_POINTS);


    do {
        glVertex2f(float(x0 - x) / 100, float(y0 + y) / 100); // 第一象限, 初值为最右的一点, 向上拓展
        glVertex2f(float(x0 - y) / 100, float(y0 - x) / 100); // 第二象限, 初值为最上一点, 向左拓展
        glVertex2f(float(x0 + x) / 100, float(y0 - y) / 100); // 第三象限, 初值为最左一点, 向下拓展
        glVertex2f(float(x0 + y) / 100, float(y0 + x) / 100); // 第四象限, 初值为最下一点, 向右拓展
        pointCounter += 4;

        int currentError = error;
        if (currentError <= y) {
            y++;
            error += 2 * y + 1;
        }

        if (currentError > x || currentError > y) {
            x++;
            error += 2 * x + 1;
        }
    } while (x < 0);

    glEnd();

    return pointCounter;
}
