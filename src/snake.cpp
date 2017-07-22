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
    color->r = 0.45f;
    color->g = 0.95f;
    color->b = 0.45f;
    // the snake is alive by default
    this->fainted = false;
}

Snake::~Snake()
{
    delete this->tailX;
    delete this->tailY;
    delete this->color;
    delete this->name;
}

/// <summary>updates the snake's position.
/// called on every turn in which the snake does not becomes larger.<summary>
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

/// <summary>draws the snake on the screen.</summary>
/// <param name="camX"> the game camera's X coordinate. </param>
/// <param name="camY"> the game camera's Y coordinate. </param>
void Snake::draw(int camX, int camY) {
    int i;
    for (i = 0; i < this->len; i++) {
        int x = this->tailX[i] - camX;
        int y = this->tailY[i] - camY;
        glColor3f(color->r, color->g, color->b);
        glRectd(x * BLOCK_SIZE, y * BLOCK_SIZE, (x+1) * BLOCK_SIZE, (y+1) * BLOCK_SIZE);
    }
}

/// <summary>makes this snake larger by 1</summary>
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

/// <summary>moves this snake by the specified delta</summary>
void Snake::moveAll(int dx, int dy) {
    int i;
    for (i = this->len-1; i >= 0; i--) {
        this->tailX[i+1] = this->tailX[i];
        this->tailY[i+1] = this->tailY[i];
    }
    this->tailX[0] = (this->tailX[1] + dx + WIDTH) % (WIDTH/BLOCK_SIZE);
    this->tailY[0] = (this->tailY[1] + dy + HEIGHT) % (HEIGHT/BLOCK_SIZE);
}
/// <summary>sets this snake's direction</summary>
void Snake::setDirection(direction dir) {
    if ((this->dir + dir) % 2 == 1) this->nextDir = dir;
}

/// <summary>returns this snake's name</summary>
/// <param name="start">the list index to start counting from</param>
/// <param name="len">the list's total length</param>
/// <param name="x">the object's x coordinates list</param>
/// <param name="y">the object's y coordinates list</param>
/// <returns>true iff the snake's head collides one of these points</returns>
bool Snake::collides(int start, int len, int *x, int *y) {
    for (int i = start; i < len; i++) {
        if (this->tailX[0] == x[i] && this->tailY[0] == y[i]) {
            return true;
        }
    }
    return false;
}

/// <returns>returns this snake's name</returns>
char *Snake::getName() {
    return this->name;
}

/// <returns>returns this snake's direction</returns>
direction Snake::getDirection() {
    return this->dir;
}

/// <returns>returns this snake's y coordinate</returns>
int *Snake::getX() {
    return this->tailX;
}

/// <returns>returns this snake's x coordinate</returns>
int *Snake::getY() {
    return this->tailY;
}

/// <returns>returns this snake's length</returns>
int Snake::getLength() {
    return this->len;
}

/// <summary>sets this snake's color</summary>
void Snake::setColor(float r, float g, float b) {
    this->color->r = r;
    this->color->g = g;
    this->color->b = b;
}

/// <returns>this snake's color</returns>
color_t *Snake::getColor() {
    return this->color;
}

/// <summary>sets this snake's player index</summary>
void Snake::setIndex(char index) {
    this->index = index;
}

/// <returns>this snake's player index</returns>
char Snake::getIndex() {
    return this->index;
}

/// <summary>makes this snake 'fainted'</summary>
void Snake::faint() {
    this->fainted = true;
}

/// <returns>true iff this snake is fainted</returns>
bool Snake::isFainted() {
    return this->fainted;
}
