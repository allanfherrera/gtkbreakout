#ifndef GAME_H
#define GAME_H

#include <gtk/gtk.h>
#include <stdbool.h>

#define BASE_WINDOW_WIDTH 800
#define BASE_WINDOW_HEIGHT 600
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 10
#define BALL_RADIUS 8
#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_WIDTH 75
#define BRICK_HEIGHT 20
#define BRICK_GAP 5

typedef struct {
    double x, y;
    bool active;
} Brick;

typedef struct {
    double paddle_x;
    double ball_x, ball_y;
    double ball_dx, ball_dy;
    Brick bricks[BRICK_ROWS][BRICK_COLS];
    int score;
    int lives;
    bool game_over;
    bool left_pressed;
    bool right_pressed;
    int window_width;
    int window_height;
    int difficulty;
    double paddle_speed;
    double ball_speed;
} GameState;

extern GameState game;
void init_game();
void set_difficulty();
void update_game_logic();

#endif
