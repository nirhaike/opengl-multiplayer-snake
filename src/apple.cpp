#include "apple.h"

color_t kAppleColor = {1, 0, 0};
color_t kSparkleColor = {1, 1, 1};

Apple::Apple(int x, int y) {
    this->x = x;
    this->y = y;
    this->sparkle = false;
}

int Apple::getX() {
    return this->x;
}

int Apple::getY() {
    return this->y;
}

void Apple::toSparkle(int snake, int id) {
    this->sparkle = true;
    this->sparkleSnakeId = snake;
    this->sparkleIndex = id;
}

int Apple::getSparkleSnake() {
    return this->sparkleSnakeId;
}

int Apple::getSparkleIndex() {
    return this->sparkleIndex;
}

bool Apple::canEat(int x, int y) {
    return (this->x == x && this->y == y);
}

void Apple::movePosition(int x, int y) {
    this->x = x;
    this->y = y;
}

void Apple::draw(int camX, int camY) {
    color_t col = kAppleColor;
    if (sparkle) {
        col = kSparkleColor;
    }
    glColor3f(col.r, col.g, col.b);
    glRectd(x * BLOCK_SIZE, y * BLOCK_SIZE, (x+1) * BLOCK_SIZE, (y+1) * BLOCK_SIZE);
}
