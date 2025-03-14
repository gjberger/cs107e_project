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
    bool alive;
};

void main(void) {
    uart_init();
    printf("Test");
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    bool left_barrier = false;
    bool middle_barrier = false;
    bool right_barrier = false;

    struct character surfer;
    surfer.pos = RIGHT;
    surfer.alive = true;
   
    if (surfer.pos == 0) {
        printf("Yes");
    }
    draw_background(0);
    gl_swap_buffer();
    draw_background(0);
    int time_init = get_secs();
    while (1) {
        // character 2 now showing
        
        if (surfer.pos == LEFT) {
            gl_swap_buffer();
            draw_background(0);
            draw_character(LANE1);
            timer_delay_ms(20);
            gl_swap_buffer();
            draw_background(0);
            draw_character_2(LANE1);
        }
        if (surfer.pos == CENTER) {
            gl_swap_buffer();
            draw_background(0);
            draw_character(LANE2);
            timer_delay_ms(20);
            gl_swap_buffer();
            draw_background(0);
            draw_character_2(LANE2);
        }
        if (surfer.pos == RIGHT) {
            gl_swap_buffer();
            draw_background(0);
            draw_character(LANE3);
            timer_delay_ms(20);
            gl_swap_buffer();
            draw_background(0);
            draw_character_2(LANE3);
        }

        /*
        mid_to_left(time_init);
        left_to_mid(time_init);
        mid_to_right(time_init);
        right_to_mid(time_init);
        */
    }
}
