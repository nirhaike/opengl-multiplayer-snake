#ifndef SNAKE_H
#define SNAKE_H

#include <window.h>
#include <game.h>

using namespace game;

class Snake
{
    public:
        Snake(char *name, int capacity, int x, int y, int len);
        virtual ~Snake();
        void update();
        void draw(int camX, int camY);
        void setDirection(direction dir);
        void enlarge();
        bool collides(int start, int len, int *x, int *y);
        char *getName();
        int *getX();
        int *getY();
        int getLength();
        void setColor(float r, float g, float b);
        color_t *getColor();

    private:
        char *name;
        int capacity;
        int len;
        int *tailX;
        int *tailY;
        direction dir;
        direction nextDir;
        color_t *color;

        void moveAll(int dx, int dy);
};

#endif // SNAKE_H
