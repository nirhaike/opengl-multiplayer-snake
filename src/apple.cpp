#include "apple.h"

color_t kAppleColor = {1, 0, 0};

Apple::Apple(int x, int y) {
    this->x = x;
    this->y = y;
}

bool Apple::canEat(int x, int y) {
    return (this->x == x && this->y == y);
}

void Apple::movePosition(int x, int y) {
    this->x = x;
    this->y = y;
}

void Apple::draw(int camX, int camY) {
    glColor3f(kAppleColor.r, kAppleColor.g, kAppleColor.b);
    glRectd(x * BLOCK_SIZE, y * BLOCK_SIZE, (x+1) * BLOCK_SIZE, (y+1) * BLOCK_SIZE);
}
