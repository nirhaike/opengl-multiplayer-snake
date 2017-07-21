#include "game.h"

namespace game {
    game_info_t *init_game_info(char players, char wait_time, char snake_size, char amount_apples) {
        game_info_t *info = new game_info_t;
        info->max_players = (int)players;
        info->wait_time = (unsigned short)wait_time;
        info->snake_initial_size = (int)snake_size;
        info->amount_apples = (int)amount_apples;
        return info;
    }

    char *copyPointer(char *arr, int len) {
        char *c = new char[len+1];
        for (int i = 0; i < len; i++) {
            c[i] = arr[i];
        }
        return c;
    }

}
