#include "Bresenham.h"

using namespace std;

/**
 * 算法采用单例模式
 * @return 算法实例
 */
Bresenham *Bresenham::getInstance() {
    static Bresenham *instance = nullptr;
    if (instance == nullptr) {
        instance = new Bresenham();
    }
    return instance;
}

/**
 * 收集直线信息
 * @param pixi 像素信息，每个像素数据结构为[ x, y, z, r, g, b]
 * @param x0 起始点横坐标，默认范围 [ -1000, 1000 ]
 * @param y0 起始点纵坐标，默认范围 [ -1000, 1000 ]
 * @param x1 终点横坐标，默认范围 [ -1000, 1000 ]
 * @param y1 终点纵坐标，默认范围 [ -1000, 1000 ]
 * @return 收集的点的个数
 */
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

/**
 * 收集圆信息
 * @param pixi 像素信息，每个像素数据结构为[ x, y, z, r, g, b]
 * @param x0 中心点横坐标，默认范围 [ -1000, 1000 ]
 * @param y0 中心点纵坐标，默认范围 [ -1000, 1000 ]
 * @param r 半径，默认范围 [ 0, 1000 ]
 * @return 收集的点的个数
 */
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

/**
 * 添加点到收集集合中，帮助函数
 * @param pixi 像素信息，每个像素数据结构为[ x, y, z, r, g, b]
 * @param x 要加入的点的横坐标, 默认范围 [ -1000, 1000 ]
 * @param y 要加入的点的纵坐标, 默认范围 [ -1000, 1000 ]
 */
void Bresenham::addPoints(std::vector<GLfloat> &pixi, int x, int y) {
    pixi.emplace_back(this->normalize(x)); // x
    pixi.emplace_back(this->normalize(y)); // y
    pixi.emplace_back(0.0f); // z
    pixi.emplace_back(this->color[0]); // r
    pixi.emplace_back(this->color[1]); // g
    pixi.emplace_back(this->color[2]); // b
}

/**
 * 规格化
 * @param target 输入整数
 * @return 规格化后的数（单精度浮点数）
 */
float Bresenham::normalize(int target) {
    return float(target) / this->scale;
}
