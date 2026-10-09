#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define GRID_WIDTH 30
#define GRID_HEIGHT 20
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT)
#define INITIAL_SNAKE_LENGTH 3

typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} Direction;

typedef enum {
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_WON
} GameStatus;

typedef struct {
    Point snake[MAX_SNAKE_LENGTH];
    int length;
    Point food;
    Direction direction;
    int score;
    GameStatus status;
    bool turn_locked;
} Game;

void game_init(Game *game);
void game_update(Game *game);
bool game_set_direction(Game *game, Direction direction);
void game_toggle_pause(Game *game);
bool game_spawn_food(Game *game);
int game_tick_ms(const Game *game);

#endif
