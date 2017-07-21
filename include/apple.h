#ifndef APPLE_H
#define APPLE_H

#include <window.h>
#include <game.h>

using namespace game;

class Apple
{
    public:
        Apple() {}
        Apple(int x, int y);
        bool canEat(int x, int y);
        void movePosition(int x, int y);
        void draw(int camX, int camY);
        void toSparkle(int snake, int id);
        int getX();
        int getY();
        int getSparkleSnake();
        int getSparkleIndex();

    private:
        int x;
        int y;
        bool sparkle;
        int sparkleSnakeId;
        int sparkleIndex;
};

#endif // APPLE_H
