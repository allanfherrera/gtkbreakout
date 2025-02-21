#include <gtk/gtk.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Game constants
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

// Game state
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
    int difficulty; // 0: Easy, 1: Medium, 2: Hard
    double paddle_speed;
    double ball_speed;
} GameState;

static GameState game = {
    .score = 0,
    .lives = 3,
    .game_over = false,
    .left_pressed = false,
    .right_pressed = false,
    .window_width = BASE_WINDOW_WIDTH,
    .window_height = BASE_WINDOW_HEIGHT,
    .difficulty = 1 // Default to Medium
};

// Colors for bricks (RGB)
static const GdkRGBA BRICK_COLORS[BRICK_ROWS] = {
    {1.0, 0.25, 0.21, 1.0}, // Red
    {1.0, 0.52, 0.11, 1.0}, // Orange
    {1.0, 0.86, 0.0, 1.0},  // Yellow
    {0.18, 0.80, 0.25, 1.0},// Green
    {0.0, 0.45, 0.85, 1.0}  // Blue
};

static void set_difficulty() {
    switch (game.difficulty) {
        case 0: // Easy
            game.paddle_speed = 8.0;
            game.ball_speed = 4.0;
            game.lives = 5;
            break;
        case 1: // Medium
            game.paddle_speed = 6.0;
            game.ball_speed = 5.0;
            game.lives = 3;
            break;
        case 2: // Hard
            game.paddle_speed = 5.0;
            game.ball_speed = 6.0;
            game.lives = 2;
            break;
    }
    game.ball_dx = game.ball_speed;
    game.ball_dy = -game.ball_speed;
}

static void init_game() {
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

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    switch (event->keyval) {
        case GDK_KEY_Left:
            game.left_pressed = true;
            return TRUE;
        case GDK_KEY_Right:
            game.right_pressed = true;
            return TRUE;
        case GDK_KEY_f: // Toggle fullscreen
            if (gtk_window_is_maximized(GTK_WINDOW(widget))) {
                gtk_window_unfullscreen(GTK_WINDOW(widget));
            } else {
                gtk_window_fullscreen(GTK_WINDOW(widget));
            }
            return TRUE;
        case GDK_KEY_1: // Easy
            game.difficulty = 0;
            init_game();
            return TRUE;
        case GDK_KEY_2: // Medium
            game.difficulty = 1;
            init_game();
            return TRUE;
        case GDK_KEY_3: // Hard
            game.difficulty = 2;
            init_game();
            return TRUE;
    }
    return FALSE;
}

static gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    switch (event->keyval) {
        case GDK_KEY_Left:
            game.left_pressed = false;
            return TRUE;
        case GDK_KEY_Right:
            game.right_pressed = false;
            return TRUE;
    }
    return FALSE;
}

static gboolean update_game(gpointer data) {
    if (game.game_over) return FALSE;

    // Move paddle
    if (game.left_pressed && game.paddle_x > 0) {
        game.paddle_x -= game.paddle_speed;
    }
    if (game.right_pressed && game.paddle_x < game.window_width - PADDLE_WIDTH) {
        game.paddle_x += game.paddle_speed;
    }

    // Update ball
    game.ball_x += game.ball_dx;
    game.ball_y += game.ball_dy;

    // Ball collisions
    if (game.ball_x + BALL_RADIUS > game.window_width || game.ball_x - BALL_RADIUS < 0)
        game.ball_dx = -game.ball_dx;
    if (game.ball_y - BALL_RADIUS < 0)
        game.ball_dy = -game.ball_dy;
    if (game.ball_y + BALL_RADIUS > game.window_height) {
        game.lives--;
        game.ball_x = game.window_width / 2;
        game.ball_y = game.window_height - 50;
        game.ball_dx = game.ball_speed;
        game.ball_dy = -game.ball_speed;
        game.paddle_x = game.window_width / 2 - PADDLE_WIDTH / 2;
    }

    // Paddle collision
    if (game.ball_y + BALL_RADIUS > game.window_height - PADDLE_HEIGHT &&
        game.ball_x > game.paddle_x && 
        game.ball_x < game.paddle_x + PADDLE_WIDTH) {
        game.ball_dy = -game.ball_dy * 1.1;
        game.ball_dx *= 1.1;
        double hit_pos = (game.ball_x - game.paddle_x) / PADDLE_WIDTH;
        double base_dx = 8 * (hit_pos - 0.5);
        double random_tweak = (rand() / (double)RAND_MAX - 0.5) * 2;
        game.ball_dx = base_dx + random_tweak;
        if (fabs(game.ball_dx) < 1) game.ball_dx = (game.ball_dx >= 0 ? 1 : -1);
    }

    // Brick collision
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            Brick *brick = &game.bricks[row][col];
            if (brick->active && 
                game.ball_x > brick->x && game.ball_x < brick->x + BRICK_WIDTH &&
                game.ball_y > brick->y && game.ball_y < brick->y + BRICK_HEIGHT) {
                brick->active = false;
                game.ball_dy = -game.ball_dy;
                game.score += 10;
            }
        }
    }

    // Check win/lose conditions
    if (game.score == BRICK_ROWS * BRICK_COLS * 10 || game.lives <= 0)
        game.game_over = true;

    gtk_widget_queue_draw(GTK_WIDGET(data));
    return TRUE;
}

static gboolean draw_game(GtkWidget *widget, cairo_t *cr, gpointer data) {
    // Clear canvas
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    if (game.game_over) {
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 40);
        const char *text = game.lives <= 0 ? "Game Over" : "You Win!";
        cairo_move_to(cr, game.window_width / 2 - 100, game.window_height / 2);
        cairo_show_text(cr, text);
        char score_text[20];
        snprintf(score_text, sizeof(score_text), "Score: %d", game.score);
        cairo_move_to(cr, game.window_width / 2 - 70, game.window_height / 2 + 50);
        cairo_show_text(cr, score_text);
        return FALSE;
    }

    // Draw paddle
    cairo_set_source_rgb(cr, 0, 1, 1); // Cyan
    cairo_rectangle(cr, game.paddle_x, game.window_height - PADDLE_HEIGHT, PADDLE_WIDTH, PADDLE_HEIGHT);
    cairo_fill(cr);

    // Draw ball
    cairo_set_source_rgb(cr, 1, 1, 1); // White
    cairo_arc(cr, game.ball_x, game.ball_y, BALL_RADIUS, 0, 2 * M_PI);
    cairo_fill(cr);

    // Draw bricks
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            Brick *brick = &game.bricks[row][col];
            if (brick->active) {
                gdk_cairo_set_source_rgba(cr, &BRICK_COLORS[row]);
                cairo_rectangle(cr, brick->x, brick->y, BRICK_WIDTH, BRICK_HEIGHT);
                cairo_fill(cr);
            }
        }
    }

    // Draw score and lives
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_set_font_size(cr, 20);
    char score_text[20];
    snprintf(score_text, sizeof(score_text), "Score: %d", game.score);
    cairo_move_to(cr, 10, 30);
    cairo_show_text(cr, score_text);
    char lives_text[20];
    snprintf(lives_text, sizeof(lives_text), "Lives: %d", game.lives);
    cairo_move_to(cr, game.window_width - 100, 30);
    cairo_show_text(cr, lives_text);

    return FALSE;
}

static void on_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
    game.window_width = event->width;
    game.window_height = event->height;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    gtk_init(&argc, &argv);

    // Parse command-line arguments for screen size
    int width = BASE_WINDOW_WIDTH;
    int height = BASE_WINDOW_HEIGHT;
    if (argc >= 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        if (width < BASE_WINDOW_WIDTH || height < BASE_WINDOW_HEIGHT) {
            width = BASE_WINDOW_WIDTH;
            height = BASE_WINDOW_HEIGHT;
        }
    }
    game.window_width = width;
    game.window_height = height;

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Breakout Clone");
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "configure-event", G_CALLBACK(on_configure_event), NULL);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_game), NULL);

    // Add keyboard event handlers
    gtk_widget_set_events(window, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);
    g_signal_connect(window, "key-release-event", G_CALLBACK(on_key_release), NULL);

    init_game();
    g_timeout_add(16, update_game, drawing_area); // ~60 FPS

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
