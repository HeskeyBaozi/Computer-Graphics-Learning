#include "Bresenham.h"

using namespace std;

Bresenham *Bresenham::getInstance() {
    static Bresenham *instance = nullptr;
    if (instance == nullptr) {
        instance = new Bresenham();
    }
    return instance;
}

int Bresenham::drawLine(vector<GLfloat> &pixi, int x0, int y0, int x1, int y1) {

    auto dx = static_cast<int>(std::abs(x1 - x0));
    auto dy = static_cast<int>(std::abs(y1 - y0));


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
        this->addPoints(pixi,
                        isKBiggerThan1 ? y : x,
                        isKBiggerThan1 ? x : y);
        pointCounter++;

        if (p < 0) {
            p += 2 * dy;
        } else {
            y += cy;
            p += 2 * (dy - dx);
        }

        x += cx;
    } while (x <= x1);
    return pointCounter;
}

int Bresenham::drawCircle(vector<GLfloat> &pixi, int x0, int y0, int r) {
    int x = -r, y = 0;
    int pointCounter = 0; // 点个数计数器
    int error = 2 - 2 * r;

    do {
        this->addPoints(pixi, x0 - x, y0 + y); // 第一象限, 初值为最右的一点, 向上拓展
        this->addPoints(pixi, x0 - y, y0 - x); // 第二象限, 初值为最上一点, 向左拓展
        this->addPoints(pixi, x0 + x, y0 - y); // 第三象限, 初值为最左一点, 向下拓展
        this->addPoints(pixi, x0 + y, y0 + x); // 第四象限, 初值为最下一点, 向右拓展
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

    return pointCounter;
}

void Bresenham::addPoints(std::vector<GLfloat> &pixi, int x, int y) {
    pixi.emplace_back(this->normalize(x));
    pixi.emplace_back(this->normalize(y));
    pixi.emplace_back(0.0f);
    pixi.emplace_back(this->color[0]);
    pixi.emplace_back(this->color[1]);
    pixi.emplace_back(this->color[2]);
}

float Bresenham::normalize(int target) {
    return float(target) / this->scale;
}
