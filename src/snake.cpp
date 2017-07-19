#include "snake.h"

Snake::Snake(int capacity, int x, int y)
{
    this->capacity = capacity;
    this->tailX = new int[capacity+1];
    this->tailY = new int[capacity+1];
    this->len = 1;
    this->tailX[0] = x;
    this->tailY[0] = y;
    this->dir = DOWN;
}

Snake::~Snake()
{
    delete this->tailX;
    delete this->tailY;
}

void Snake::update() {
    switch (dir) {
    case DOWN:
        moveAll(0, 1);
        break;
    case UP:
        moveAll(0, -1);
        break;
    case LEFT:
        moveAll(-1, 0);
        break;
    case RIGHT:
        moveAll(1, 0);
        break;
    default:
        break;
    }
}

void Snake::draw(int camX, int camY) {
    int i;
    for (i = 0; i < this->len; i++) {
        int x = this->tailX[i] - camX;
        int y = this->tailY[i] - camY;
        glColor3f(0.0, 1.0, 0.0);
        glRectd(x * BLOCK_SIZE, y * BLOCK_SIZE, (x+1) * BLOCK_SIZE, (y+1) * BLOCK_SIZE);
    }
}

void Snake::enlarge() {
    if (len >= capacity) {
        return;
    }
    int lastX = tailX[this->len-1];
    int lastY = tailY[this->len-1];
    update();
    this->len++;
    tailX[this->len-1] = lastX;
    tailY[this->len-1] = lastY;
}

void Snake::moveAll(int dx, int dy) {
    int i;
    for (i = this->len-1; i >= 0; i--) {
        this->tailX[i+1] = this->tailX[i];
        this->tailY[i+1] = this->tailY[i];
    }
    this->tailX[0] = this->tailX[1] + dx;
    this->tailY[0] = this->tailY[1] + dy;
}

void Snake::setDirection(direction dir) {
    this->dir = dir;
}

bool Snake::collides(int start, int len, int *x, int *y) {
    for (int i = start; i < len; i++) {
        if (this->tailX[0] == x[i] && this->tailY[0] == y[i]) {
            return true;
        }
    }
    return false;
}

int *Snake::getX() {
    return this->tailX;
}

int *Snake::getY() {
    return this->tailY;
}

int Snake::getLength() {
    return this->len;
}
