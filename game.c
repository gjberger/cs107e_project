#include "uart.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "i2c.h"
#include "mpu6050.h"
#include "game_graphics.h"

#define WIDTH 400
#define HEIGHT 600

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
    character.position = 0;
    
    draw_background();
    draw_character();
    gl_swap_buffer();
    draw_background();
    draw_character_2();
    while (1) {
        // character 2 now showing
        gl_swap_buffer();
        draw_character();
        timer_delay_ms(500);
        gl_swap_buffer();
        draw_character_2();
        timer_delay_ms(500);
    }
}
