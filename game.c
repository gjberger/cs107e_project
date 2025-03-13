#include "uart.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"

const int WIDTH = 400;
const int HEIGHT = 600;

void draw_background(void) {
    gl_clear(GL_WHITE);

    // overarch line
    gl_draw_line(WIDTH / 8, 0.25 * HEIGHT, (7 * WIDTH) / 8, 0.25 * HEIGHT, GL_BLACK);
    

    // left
    gl_draw_line(0, HEIGHT, 0.125 * WIDTH, 0.75 * HEIGHT, GL_BLACK);
    // Straight up
    gl_draw_line(0.125 * WIDTH, 0.75 * HEIGHT, 0.125 * WIDTH, 0.25 * HEIGHT, GL_BLACK);
    
    //right
    gl_draw_line(WIDTH, HEIGHT, (0.875 * WIDTH), 0.75 * HEIGHT, GL_BLACK);
    // Straight up
    gl_draw_line((0.875 * WIDTH), 0.75 * HEIGHT, 0.875 * WIDTH, 0.25 * HEIGHT, GL_BLACK);
   
    //banner
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);
    

    // Left Lane
    gl_draw_line(0.3 * WIDTH, HEIGHT, 0.375 * WIDTH, 0.75 * HEIGHT, GL_BLACK);
    // Right Lane
    gl_draw_line(0.70 * WIDTH, HEIGHT, (0.625 * WIDTH), 0.75 * HEIGHT, GL_BLACK);


    gl_draw_line(0, 0, 0.125 * WIDTH, 0.25 * HEIGHT, GL_BLACK);
    gl_draw_line(WIDTH, 0, 0.875 * WIDTH, 0.25 * HEIGHT, GL_BLACK);
}

// need to be able to give x, y coordinates, draw character depending on that
void draw_character(void) {
    gl_draw_circle(WIDTH / 2, 0.87 * HEIGHT, 15, GL_BLACK);
    // Body
    gl_draw_line(WIDTH / 2, 0.89 * HEIGHT, WIDTH / 2, 0.95 * HEIGHT, GL_BLACK);
    // Arms
    gl_draw_line((WIDTH / 2) - 5, 0.92 * HEIGHT, (WIDTH / 2) + 7, 0.92 * HEIGHT, GL_BLACK);
    // Legs
    gl_draw_line(WIDTH / 2, 0.95 * HEIGHT, (WIDTH / 2) - 7, 0.98 * HEIGHT, GL_BLACK);
    gl_draw_line(WIDTH / 2, 0.95 * HEIGHT, (WIDTH / 2) + 7, (0.98 * HEIGHT) - 7, GL_BLACK);
    
}

void draw_character_2(void) {
    gl_draw_circle(WIDTH / 2, 0.87 * HEIGHT, 15, GL_BLACK);
    gl_draw_line(WIDTH / 2, 0.89 * HEIGHT, WIDTH / 2, 0.95 * HEIGHT, GL_BLACK);
    gl_draw_line((WIDTH / 2) - 5, 0.92 * HEIGHT, (WIDTH / 2) + 7, 0.92 * HEIGHT, GL_BLACK);
    gl_draw_line(WIDTH / 2, 0.95 * HEIGHT, (WIDTH / 2) - 7, (0.98 * HEIGHT) - 7, GL_BLACK);
    gl_draw_line(WIDTH / 2, 0.95 * HEIGHT, (WIDTH / 2) + 7, 0.98 * HEIGHT, GL_BLACK);
    
}
void main(void) {
    uart_init();
    printf("Test");


    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    draw_background();
    draw_character();
    gl_swap_buffer();
    draw_background();
    draw_character_2();
    
    while (1) {

        gl_swap_buffer();
        timer_delay_ms(250);
    }
}
