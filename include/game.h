#ifndef GAME_H
#define GAME_H

typedef struct {
    char max_players;
    char wait_time;
    char snake_initial_size;
} game_info_t;

game_info_t *init_game_info(char players, char wait_time, char snake_size) {
    game_info_t *info = new game_info_t;
    info->max_players = players;
    info->wait_time = wait_time;
    info->snake_initial_size = snake_size;
    return info;
}

#endif // GAME_H
