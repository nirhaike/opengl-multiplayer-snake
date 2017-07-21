#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <string.h>
#include <sstream>

using namespace std;

namespace game {
    typedef struct {
        int  max_players;
        unsigned short wait_time;
        int snake_initial_size;
        int amount_apples;
    } game_info_t;

    typedef struct {
        float r;
        float g;
        float b;
    } color_t;

    enum direction {
        UP, LEFT, DOWN, RIGHT
    };

    template <typename T>
    static string toString(T val) {
        stringstream stream;
        stream << val;
        return stream.str();
    }

    char *copyPointer(char *arr, int len);

    game_info_t *init_game_info(char players, char wait_time, char snake_size, char amount_apples);
}

#endif // GAME_H
