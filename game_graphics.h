#include "uart.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"

void gl_draw_circle(int xCenter, int yCenter, int radius, color_t c);

void draw_background(void);
void draw_character(int x);
void draw_character_2(int x);
void mid_to_left(void);
void mid_to_right(void);
void left_to_mid(void);
void right_to_mid(void);
