#include "game.h"

#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 690
#define WORLD_LEFT -1.0
#define WORLD_RIGHT (GRID_WIDTH + 1.0)
#define WORLD_BOTTOM -1.0
#define WORLD_TOP (GRID_HEIGHT + 2.0)

static Game game;
static int current_viewport_width = WINDOW_WIDTH;

static void draw_rectangle(float left, float bottom, float right, float top)
{
    glBegin(GL_QUADS);
    glVertex2f(left, bottom);
    glVertex2f(right, bottom);
    glVertex2f(right, top);
    glVertex2f(left, top);
    glEnd();
}

static void draw_cell(Point point, float inset)
{
    draw_rectangle((float)point.x + inset,
                   (float)point.y + inset,
                   (float)point.x + 1.0f - inset,
                   (float)point.y + 1.0f - inset);
}

static void draw_text(float x, float y, void *font, const char *text)
{
    const unsigned char *character = (const unsigned char *)text;

    glRasterPos2f(x, y);
    while (*character != '\0') {
        glutBitmapCharacter(font, *character);
        ++character;
    }
}

static float centered_text_x(const char *text, void *font)
{
    int pixel_width = glutBitmapLength(font, (const unsigned char *)text);
    float world_width = (float)(WORLD_RIGHT - WORLD_LEFT);

    return (GRID_WIDTH -
            (pixel_width * world_width / current_viewport_width)) / 2.0f;
}

static void draw_board(void)
{
    const float thickness = 0.35f;

    glColor3f(0.08f, 0.62f, 0.78f);
    draw_rectangle(-thickness, -thickness, GRID_WIDTH + thickness, 0.0f);
    draw_rectangle(-thickness, GRID_HEIGHT, GRID_WIDTH + thickness,
                   GRID_HEIGHT + thickness);
    draw_rectangle(-thickness, 0.0f, 0.0f, GRID_HEIGHT);
    draw_rectangle(GRID_WIDTH, 0.0f, GRID_WIDTH + thickness, GRID_HEIGHT);
}

static void draw_snake(void)
{
    int index;

    for (index = game.length - 1; index >= 0; --index) {
        if (index == 0) {
            glColor3f(0.55f, 1.0f, 0.30f);
        } else {
            glColor3f(0.20f, 0.78f, 0.22f);
        }
        draw_cell(game.snake[index], 0.08f);
    }
}

static void draw_hud(void)
{
    char score_text[64];

    snprintf(score_text, sizeof(score_text), "Pontuacao: %d", game.score);
    glColor3f(0.93f, 0.95f, 1.0f);
    draw_text(0.0f, GRID_HEIGHT + 0.8f, GLUT_BITMAP_HELVETICA_18, score_text);

    if (game.status == GAME_PAUSED) {
        draw_text(GRID_WIDTH - 4.1f, GRID_HEIGHT + 0.8f,
                  GLUT_BITMAP_HELVETICA_18, "PAUSADO");
    }
}

static void draw_end_screen(void)
{
    char title[32];
    char score_text[64];
    const char *restart_text = "Pressione R para reiniciar";

    snprintf(title, sizeof(title), "%s",
             game.status == GAME_WON ? "VOCE VENCEU" : "GAME OVER");
    snprintf(score_text, sizeof(score_text), "Pontuacao final: %d", game.score);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.02f, 0.03f, 0.08f, 0.82f);
    draw_rectangle(0.0f, 0.0f, GRID_WIDTH, GRID_HEIGHT);
    glDisable(GL_BLEND);

    glColor3f(1.0f, 0.35f, 0.22f);
    draw_text(centered_text_x(title, GLUT_BITMAP_TIMES_ROMAN_24),
              11.0f, GLUT_BITMAP_TIMES_ROMAN_24, title);
    glColor3f(0.95f, 0.95f, 1.0f);
    draw_text(centered_text_x(score_text, GLUT_BITMAP_HELVETICA_18),
              9.5f, GLUT_BITMAP_HELVETICA_18, score_text);
    draw_text(centered_text_x(restart_text, GLUT_BITMAP_HELVETICA_18),
              8.3f, GLUT_BITMAP_HELVETICA_18, restart_text);
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    draw_board();

    glColor3f(1.0f, 0.35f, 0.12f);
    draw_cell(game.food, 0.12f);
    draw_snake();
    draw_hud();

    if (game.status == GAME_OVER || game.status == GAME_WON) {
        draw_end_screen();
    }

    glutSwapBuffers();
}

static void reshape(int width, int height)
{
    const float target_aspect =
        (float)(WORLD_RIGHT - WORLD_LEFT) / (float)(WORLD_TOP - WORLD_BOTTOM);
    float window_aspect;
    int viewport_width;
    int viewport_height;
    int viewport_x = 0;
    int viewport_y = 0;

    if (width < 1) {
        width = 1;
    }
    if (height < 1) {
        height = 1;
    }

    window_aspect = (float)width / (float)height;
    viewport_width = width;
    viewport_height = height;

    if (window_aspect > target_aspect) {
        viewport_width = (int)(height * target_aspect);
        viewport_x = (width - viewport_width) / 2;
    } else {
        viewport_height = (int)(width / target_aspect);
        viewport_y = (height - viewport_height) / 2;
    }

    glViewport(viewport_x, viewport_y, viewport_width, viewport_height);
    current_viewport_width = viewport_width;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void timer_callback(int value)
{
    (void)value;
    game_update(&game);
    glutPostRedisplay();
    glutTimerFunc((unsigned int)game_tick_ms(&game), timer_callback, 0);
}

static void change_direction(Direction direction)
{
    game_set_direction(&game, direction);
}

static void special_key(int key, int x, int y)
{
    (void)x;
    (void)y;

    switch (key) {
        case GLUT_KEY_UP:
            change_direction(DIRECTION_UP);
            break;
        case GLUT_KEY_DOWN:
            change_direction(DIRECTION_DOWN);
            break;
        case GLUT_KEY_LEFT:
            change_direction(DIRECTION_LEFT);
            break;
        case GLUT_KEY_RIGHT:
            change_direction(DIRECTION_RIGHT);
            break;
    }
}

static void keyboard(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    switch (key) {
        case 'w':
        case 'W':
            change_direction(DIRECTION_UP);
            break;
        case 's':
        case 'S':
            change_direction(DIRECTION_DOWN);
            break;
        case 'a':
        case 'A':
            change_direction(DIRECTION_LEFT);
            break;
        case 'd':
        case 'D':
            change_direction(DIRECTION_RIGHT);
            break;
        case 'p':
        case 'P':
            game_toggle_pause(&game);
            break;
        case 'r':
        case 'R':
            if (game.status == GAME_OVER || game.status == GAME_WON) {
                game_init(&game);
            }
            break;
        case 27:
            glutLeaveMainLoop();
            return;
    }

    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    srand((unsigned int)time(NULL));
    game_init(&game);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Snake - OpenGL");
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glClearColor(0.025f, 0.045f, 0.09f, 1.0f);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special_key);
    glutTimerFunc((unsigned int)game_tick_ms(&game), timer_callback, 0);
    glutMainLoop();

    return EXIT_SUCCESS;
}
