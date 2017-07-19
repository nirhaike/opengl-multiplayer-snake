#ifndef SNAKE_H
#define SNAKE_H

#include <window.h>

enum direction {
    UP, DOWN, LEFT, RIGHT
};

class Snake
{
    public:
        Snake(char *name, int capacity, int x, int y);
        virtual ~Snake();
        void update();
        void draw(int camX, int camY);
        void setDirection(direction dir);
        void enlarge();
        bool collides(int start, int len, int *x, int *y);
        int *getX();
        int *getY();
        int getLength();

    private:
        char *name;
        int capacity;
        int len;
        int *tailX;
        int *tailY;
        direction dir;

        void moveAll(int dx, int dy);
};

#endif // SNAKE_H
