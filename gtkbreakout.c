#include "game.h"
#include <stdlib.h>
#include <time.h>

GameState game = {
    .score = 0,
    .lives = 3,
    .game_over = false,
    .left_pressed = false,
    .right_pressed = false,
    .window_width = BASE_WINDOW_WIDTH,
    .window_height = BASE_WINDOW_HEIGHT,
    .difficulty = 1
};

void set_difficulty() {
    switch (game.difficulty) {
        case 0:
            game.paddle_speed = 8.0;
            game.ball_speed = 4.0;
            game.lives = 5;
            break;
        case 1:
            game.paddle_speed = 6.0;
            game.ball_speed = 5.0;
            game.lives = 3;
            break;
        case 2:
            game.paddle_speed = 5.0;
            game.ball_speed = 6.0;
            game.lives = 2;
            break;
    }
    game.ball_dx = game.ball_speed;
    game.ball_dy = -game.ball_speed;
}

void init_game() {
    game.paddle_x = game.window_width / 2 - PADDLE_WIDTH / 2;
    game.ball_x = game.window_width / 2;
    game.ball_y = game.window_height - 50;
    set_difficulty();
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            game.bricks[row][col].x = col * (BRICK_WIDTH + BRICK_GAP) + BRICK_GAP;
            game.bricks[row][col].y = row * (BRICK_HEIGHT + BRICK_GAP) + 50;
            game.bricks[row][col].active = true;
        }
    }
}

void update_game_logic() {
    if (game.game_over) return;
    
    if (game.left_pressed && game.paddle_x > 0) {
        game.paddle_x -= game.paddle_speed;
    }
    if (game.right_pressed && game.paddle_x < game.window_width - PADDLE_WIDTH) {
        game.paddle_x += game.paddle_speed;
    }
}
