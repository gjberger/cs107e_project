#include "uart.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "i2c.h"
#include "mpu6050.h"

typedef enum {
    BLOCK = 1,
    BEE = 2,
    FLY = 3,
} barrier_t;

typedef enum {
    STICK = 1,
    STEVE = 2,
    MARIO = 3,
    LUIGI = 4,
} skin_t;
#define LUIGI_GREEN 0x1fa94a
#define TRAIN_SLATS 0xc048c0 
#define TRAIN_RAIL 0xc0d5d4
#define RED_GHOST 0xfd0201
#define GHOST_EYE 0x0431f0

void gl_draw_circle(int xCenter, int yCenter, int radius, color_t c);

void draw_background(void);
void draw_acknowledgements(void);
void draw_loading_screen(void);
void draw_startscreen(void);
void draw_startscreen_2(void);
void blinking_start_screen(void);
void game_countdown(void);
void draw_endscreen(void);
void draw_character(int x);
void draw_character_2(int x);
void draw_steve(int x);
void draw_steve_2(int x);
void draw_mario(int x);
void draw_mario_2(int x);
void draw_luigi(int x);
void draw_luigi_2(int x);
void character_animation(int secs, position_t pos);
void mid_to_left(int secs);
void mid_to_right(int secs);
void left_to_mid(int secs);
void right_to_mid(int secs);
int get_secs(void);
void draw_barrier(int x, int y, barrier_t type);
void draw_barrier_bee(int x, int y);
void draw_barrier_fly(int x, int y);
void barrier_animation(position_t pos);
void left_barrier_animate(void);
void draw_score(int secs);
void draw_num(int num, int x, int y);
void character_pose_1(position_t pos, skin_t skin);
void character_pose_2(position_t pos, skin_t skin);
void draw_menu(int cur_select);
void draw_train_slats(void);
void draw_train_slats_2(void);
