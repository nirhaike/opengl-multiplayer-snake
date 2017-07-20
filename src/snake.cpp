#include "snake.h"

Snake::Snake(char *name, int capacity, int x, int y, int len)
{
    this->name = name;
    this->capacity = capacity;
    this->tailX = new int[capacity+1];
    this->tailY = new int[capacity+1];
    this->len = len;
    // initialize the position
    for (int i = 1; i < len; i++) {
        this->tailX[i] = x;
        this->tailY[i] = y-1;
    }
    this->tailX[0] = x;
    this->tailY[0] = y;
    // default direction
    this->dir = DOWN;
    this->nextDir = DOWN;
    // initialize the color
    this->color = new color_t;
    color->r = 0;
    color->g = 1;
    color->b = 0;
}

Snake::~Snake()
{
    delete this->tailX;
    delete this->tailY;
    delete this->color;
    delete this->name;
}

void Snake::update() {
    this->dir = this->nextDir;
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
        glColor3f(color->r, color->g, color->b);
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
    this->tailX[0] = (this->tailX[1] + dx + WIDTH) % (WIDTH/BLOCK_SIZE);
    this->tailY[0] = (this->tailY[1] + dy + HEIGHT) % (HEIGHT/BLOCK_SIZE);
}

void Snake::setDirection(direction dir) {
    if ((this->dir + dir) % 2 == 1) this->nextDir = dir;
}

bool Snake::collides(int start, int len, int *x, int *y) {
    for (int i = start; i < len; i++) {
        if (this->tailX[0] == x[i] && this->tailY[0] == y[i]) {
            return true;
        }
    }
    return false;
}

char *Snake::getName() {
    return this->name;
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

void Snake::setColor(float r, float g, float b) {
    this->color->r = r;
    this->color->g = g;
    this->color->b = b;
}

color_t *Snake::getColor() {
    return this->color;
}
