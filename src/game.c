#include "game.h"

#include <stdlib.h>

#define INITIAL_TICK_MS 250
#define EARLY_SPEED_STEP_MS 10
#define EARLY_SPEED_LEVELS 10
#define FOOD_PER_SPEED_LEVEL 5

static bool points_equal(Point first, Point second)
{
    return first.x == second.x && first.y == second.y;
}

static bool snake_contains(const Game *game, Point point, int segment_count)
{
    int index;

    for (index = 0; index < segment_count; ++index) {
        if (points_equal(game->snake[index], point)) {
            return true;
        }
    }

    return false;
}

static bool directions_are_opposite(Direction first, Direction second)
{
    return (first == DIRECTION_UP && second == DIRECTION_DOWN) ||
           (first == DIRECTION_DOWN && second == DIRECTION_UP) ||
           (first == DIRECTION_LEFT && second == DIRECTION_RIGHT) ||
           (first == DIRECTION_RIGHT && second == DIRECTION_LEFT);
}

static Point next_head_position(const Game *game)
{
    Point next = game->snake[0];

    switch (game->direction) {
        case DIRECTION_UP:
            ++next.y;
            break;
        case DIRECTION_DOWN:
            --next.y;
            break;
        case DIRECTION_LEFT:
            --next.x;
            break;
        case DIRECTION_RIGHT:
            ++next.x;
            break;
    }

    return next;
}

static bool is_outside_board(Point point)
{
    return point.x < 0 || point.x >= GRID_WIDTH ||
           point.y < 0 || point.y >= GRID_HEIGHT;
}

void game_init(Game *game)
{
    int index;
    const int center_x = GRID_WIDTH / 2;
    const int center_y = GRID_HEIGHT / 2;

    game->length = INITIAL_SNAKE_LENGTH;
    for (index = 0; index < game->length; ++index) {
        game->snake[index].x = center_x - index;
        game->snake[index].y = center_y;
    }

    game->direction = DIRECTION_RIGHT;
    game->score = 0;
    game->status = GAME_RUNNING;
    game->turn_locked = false;
    game_spawn_food(game);
}

bool game_spawn_food(Game *game)
{
    int free_cell_count = GRID_WIDTH * GRID_HEIGHT - game->length;
    int selected_free_cell;
    int x;
    int y;

    if (free_cell_count <= 0) {
        return false;
    }

    selected_free_cell = rand() % free_cell_count;
    for (y = 0; y < GRID_HEIGHT; ++y) {
        for (x = 0; x < GRID_WIDTH; ++x) {
            Point candidate = {x, y};

            if (snake_contains(game, candidate, game->length)) {
                continue;
            }

            if (selected_free_cell == 0) {
                game->food = candidate;
                return true;
            }
            --selected_free_cell;
        }
    }

    return false;
}

bool game_set_direction(Game *game, Direction direction)
{
    if (game->status != GAME_RUNNING || game->turn_locked) {
        return false;
    }

    if (direction == game->direction) {
        return true;
    }

    if (directions_are_opposite(game->direction, direction)) {
        return false;
    }

    game->direction = direction;
    game->turn_locked = true;
    return true;
}

void game_toggle_pause(Game *game)
{
    if (game->status == GAME_RUNNING) {
        game->status = GAME_PAUSED;
    } else if (game->status == GAME_PAUSED) {
        game->status = GAME_RUNNING;
    }
}

void game_update(Game *game)
{
    Point next_head;
    bool ate_food;
    int collision_segment_count;
    int new_length;
    int index;

    if (game->status != GAME_RUNNING) {
        return;
    }

    next_head = next_head_position(game);
    ate_food = points_equal(next_head, game->food);

    if (is_outside_board(next_head)) {
        game->status = GAME_OVER;
        return;
    }

    /* The current tail cell is free on this tick unless food is eaten. */
    collision_segment_count = ate_food ? game->length : game->length - 1;
    if (snake_contains(game, next_head, collision_segment_count)) {
        game->status = GAME_OVER;
        return;
    }

    new_length = game->length + (ate_food ? 1 : 0);
    for (index = new_length - 1; index > 0; --index) {
        game->snake[index] = game->snake[index - 1];
    }
    game->snake[0] = next_head;
    game->length = new_length;
    game->turn_locked = false;

    if (ate_food) {
        ++game->score;
        if (!game_spawn_food(game)) {
            game->status = GAME_WON;
        }
    }
}

int game_tick_ms(const Game *game)
{
    int speed_level = game->score / FOOD_PER_SPEED_LEVEL;
    const int maximum_speed_level =
        (MAX_SNAKE_LENGTH - INITIAL_SNAKE_LENGTH) / FOOD_PER_SPEED_LEVEL;

    if (speed_level > maximum_speed_level) {
        speed_level = maximum_speed_level;
    }

    if (speed_level <= EARLY_SPEED_LEVELS) {
        return INITIAL_TICK_MS - speed_level * EARLY_SPEED_STEP_MS;
    }

    /* Keep increasing at every level while preserving a positive interval. */
    return INITIAL_TICK_MS - EARLY_SPEED_LEVELS * EARLY_SPEED_STEP_MS -
           (speed_level - EARLY_SPEED_LEVELS);
}
