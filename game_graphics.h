#include "uart.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"

void gl_draw_circle(int xCenter, int yCenter, int radius, color_t c);

void draw_background(int secs);
void draw_character(int x);
void draw_character_2(int x);
void mid_to_left(int secs);
void mid_to_right(int secs);
void left_to_mid(int secs);
void right_to_mid(int secs);
int get_secs(void);
