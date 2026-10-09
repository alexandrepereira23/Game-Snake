#include "game.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static bool point_is_on_snake(const Game *game, Point point)
{
    int index;

    for (index = 0; index < game->length; ++index) {
        if (game->snake[index].x == point.x &&
            game->snake[index].y == point.y) {
            return true;
        }
    }
    return false;
}

static void test_initial_state_and_automatic_movement(void)
{
    Game game;
    int initial_head_x;

    game_init(&game);
    assert(game.length == 3);
    assert(game.score == 0);
    assert(game.direction == DIRECTION_RIGHT);
    assert(game.status == GAME_RUNNING);
    initial_head_x = game.snake[0].x;

    game_update(&game);
    assert(game.snake[0].x == initial_head_x + 1);
    assert(game.snake[0].y == GRID_HEIGHT / 2);
}

static void test_direction_rules(void)
{
    Game game;

    game_init(&game);
    assert(!game_set_direction(&game, DIRECTION_LEFT));
    assert(game.direction == DIRECTION_RIGHT);
    assert(game_set_direction(&game, DIRECTION_UP));
    assert(!game_set_direction(&game, DIRECTION_LEFT));

    game.food = (Point){0, 0};
    game_update(&game);
    assert(game.direction == DIRECTION_UP);
    assert(game_set_direction(&game, DIRECTION_LEFT));
}

static void test_growth_score_and_speed(void)
{
    Game game;
    Point old_tail;

    game_init(&game);
    old_tail = game.snake[game.length - 1];
    game.food = (Point){game.snake[0].x + 1, game.snake[0].y};
    game_update(&game);

    assert(game.length == 4);
    assert(game.score == 1);
    assert(game.snake[game.length - 1].x == old_tail.x);
    assert(game.snake[game.length - 1].y == old_tail.y);

    game.score = 4;
    assert(game_tick_ms(&game) == 250);
    game.score = 5;
    assert(game_tick_ms(&game) == 240);
    game.score = 10;
    assert(game_tick_ms(&game) == 230);
    game.score = 50;
    assert(game_tick_ms(&game) == 150);
    game.score = 55;
    assert(game_tick_ms(&game) == 149);
    game.score = MAX_SNAKE_LENGTH - INITIAL_SNAKE_LENGTH;
    assert(game_tick_ms(&game) == 41);
}

static void test_wall_collision(void)
{
    Game game;

    game_init(&game);
    game.snake[0] = (Point){GRID_WIDTH - 1, 5};
    game.snake[1] = (Point){GRID_WIDTH - 2, 5};
    game.snake[2] = (Point){GRID_WIDTH - 3, 5};
    game.food = (Point){0, 0};
    game_update(&game);

    assert(game.status == GAME_OVER);
}

static void test_self_collision(void)
{
    Game game;

    game_init(&game);
    game.length = 5;
    game.snake[0] = (Point){2, 2};
    game.snake[1] = (Point){1, 2};
    game.snake[2] = (Point){1, 1};
    game.snake[3] = (Point){2, 1};
    game.snake[4] = (Point){3, 1};
    game.direction = DIRECTION_DOWN;
    game.food = (Point){0, 0};
    game_update(&game);

    assert(game.status == GAME_OVER);
}

static void test_tail_cell_is_released(void)
{
    Game game;

    game_init(&game);
    game.length = 4;
    game.snake[0] = (Point){1, 1};
    game.snake[1] = (Point){1, 2};
    game.snake[2] = (Point){2, 2};
    game.snake[3] = (Point){2, 1};
    game.direction = DIRECTION_RIGHT;
    game.food = (Point){0, 0};
    game_update(&game);

    assert(game.status == GAME_RUNNING);
    assert(game.snake[0].x == 2 && game.snake[0].y == 1);
}

static void test_food_never_spawns_on_snake(void)
{
    Game game;
    int attempt;

    game_init(&game);
    srand(1234);
    for (attempt = 0; attempt < 1000; ++attempt) {
        assert(game_spawn_food(&game));
        assert(!point_is_on_snake(&game, game.food));
    }
}

static void test_pause_stops_updates(void)
{
    Game game;
    Point head;

    game_init(&game);
    head = game.snake[0];
    game_toggle_pause(&game);
    assert(game.status == GAME_PAUSED);
    game_update(&game);
    assert(game.snake[0].x == head.x && game.snake[0].y == head.y);
    game_toggle_pause(&game);
    assert(game.status == GAME_RUNNING);
}

static void test_filling_board_wins_without_overflow(void)
{
    Game game;
    int x;
    int y;
    int index = 1;

    game_init(&game);
    game.length = MAX_SNAKE_LENGTH - 1;
    game.snake[0] = (Point){0, 0};
    for (y = 0; y < GRID_HEIGHT; ++y) {
        for (x = 0; x < GRID_WIDTH; ++x) {
            if ((x == 0 && y == 0) || (x == 1 && y == 0)) {
                continue;
            }
            game.snake[index++] = (Point){x, y};
        }
    }

    assert(index == game.length);
    game.direction = DIRECTION_RIGHT;
    game.food = (Point){1, 0};
    game.score = MAX_SNAKE_LENGTH - INITIAL_SNAKE_LENGTH - 1;
    game_update(&game);

    assert(game.length == MAX_SNAKE_LENGTH);
    assert(game.score == MAX_SNAKE_LENGTH - INITIAL_SNAKE_LENGTH);
    assert(game.status == GAME_WON);

    game_init(&game);
    assert(game.length == INITIAL_SNAKE_LENGTH);
    assert(game.score == 0);
    assert(game.status == GAME_RUNNING);
}

int main(void)
{
    test_initial_state_and_automatic_movement();
    test_direction_rules();
    test_growth_score_and_speed();
    test_wall_collision();
    test_self_collision();
    test_tail_cell_is_released();
    test_food_never_spawns_on_snake();
    test_pause_stops_updates();
    test_filling_board_wins_without_overflow();

    puts("Todos os testes da logica passaram.");
    return EXIT_SUCCESS;
}
