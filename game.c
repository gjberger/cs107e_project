#include "uart.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "i2c.h"
#include "mpu6050.h"
#include "game_graphics.h"

#define WIDTH 400
#define HEIGHT 600
#define LANE1 (WIDTH / 6)
#define LANE2 (WIDTH / 2)
#define LANE3 (5 * WIDTH / 6)

struct character {
    position_t pos;
    int x;
    int y;
    int head;
    bool zero_registed;
};

void main(void) {
    uart_init();
    printf("Test");
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    struct character surfer;
    surfer.pos = 0;
   
    if (surfer.pos == 0) {
        printf("Yes");
    }
    draw_background();
    draw_character(WIDTH / 6);
    gl_swap_buffer();
    draw_background();
    draw_character_2(WIDTH / 6);
    while (1) {
        // character 2 now showing
        for (int i = LANE1; i < LANE3; i += 30) {
            gl_swap_buffer();
            draw_background();
            draw_character(i);
            timer_delay_ms(500);
            gl_swap_buffer();
            draw_background();
            draw_character_2(i);
            timer_delay_ms(500);
        }
    }
}
