#include "uart.h"
#include "printf.h"
#include "gl.c"
#include "assert.h"

const int WIDTH = 400;
const int HEIGHT = 600;

void draw_background(void) {
    gl_clear(GL_WHITE);

    gl_draw_line(0, HEIGHT, WIDTH / 8, 0.75 * HEIGHT, GL_BLACK);
    gl_draw_line(WIDTH / 8, 0.75 * HEIGHT, WIDTH / 8, 0.25 * HEIGHT, GL_BLACK);
    
    gl_draw_line(WIDTH, HEIGHT, (7 * WIDTH) / 8, 0.75 * HEIGHT, GL_BLACK);
    gl_draw_line((7 * WIDTH) / 8, 0.75 * HEIGHT, (7 * WIDTH) / 8, 0.25 * HEIGHT, GL_BLACK);
    
    gl_draw_string(WIDTH / 4, 0.15 * HEIGHT, "CS107E Surfers", GL_BLACK);

   
    gl_draw_circle(WIDTH / 2, 0.75 * HEIGHT, 10, GL_BLACK);

    gl_swap_buffer();

}

void main(void) {
    uart_init();
    printf("Test");


    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    draw_background();
    while (1) {

    }
}
