#include "uart.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "game_graphics.h"
#include "strings.h"
#include "mpu6050.h"
#include "malloc.h"

#define WIDTH 400
#define HEIGHT 600
#define RECT_OFFSET 19
#define LEG_OFFSET 7
#define ARM_OFFSET 5
#define LANE1 (WIDTH / 6)
#define LANE2 (WIDTH / 2)
#define LANE3 (5 * WIDTH / 6)
// This helper function allows you convert from a number to
// it's char value on the ASCII table.

int get_secs(void) {
    return (timer_get_ticks() / (24 * 1000000));
}

static char num_to_char(int num) {
    char c = (char)num;
    if (num < 10) {
        num += 48;
        c = (char)num;
    } else {
        num += 87;
        c = (char)num;
    }
    return c;
}

// This function takes a number and a base, and converts it into a string.
static void num_to_string(unsigned long num, int base, char *outstr) {

    // Temporary buffer to store digits as we convert into a string
    // in a specific base.
    char intbuffer[32]; // Must be adequate size to store unsigned long in dec/hex form. 32 works.
    
    // Mod by 10 to extract last digit, divide by 10
    // to shift over.
    
    // Special case, if number is 0, just add '0'.
    if (num == 0) {
        outstr[0] = '0';
        outstr[1] = '\0';
    }
    // If any other number, continually extract the least significant digit
    // until num = 0. Store these to intbuffer, will be in reverse order
    // since we are extracting least significant digit.
    else {
        int i = 0;
        while (num != 0) {
            // num % base gives value in least significant digit, in the base of interest.
            intbuffer[i] = num_to_char((num % base));
            // Shifts number.
            num /= base;
            i++;
        }

        // Null-terminate.
        intbuffer[i] = '\0';
       
        i--;
        // Copy the converted string to `outstr`, but reverse the order to
        // get back to the original order of the number.
        // To do this, increment j and decrement i, and proceed until i = 0.
        int j = 0;
        while (i >= 0) {
            outstr[j] = intbuffer[i];
            i--;
            j++;
        }
        // Null terminate final string.
        outstr[j] = '\0';
    }
}

void draw_score(int secs) {
    char buf[10];
    num_to_string(get_secs() - secs, 10, buf);
    gl_draw_string(0.59 * WIDTH, 0.05 * WIDTH, "Score: ", GL_BLACK);
    gl_draw_string(0.8 * WIDTH, 0.05 * WIDTH, buf, GL_BLACK);
}

void draw_num(int num, int x, int y) {
    char buf[10];
    num_to_string(num, 10, buf);
    gl_draw_string(x, y, buf, GL_WHITE);
}

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

void draw_acknowledgements(void) {
    gl_clear(GL_MAGENTA);
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);
    gl_swap_buffer();
    gl_clear(GL_MAGENTA);
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);

    timer_delay(3);
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "P", GL_WHITE);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Po", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Pow", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powe", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Power", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powere", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powered", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powered b", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powered by", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powered by R", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powered by RI", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powered by RIS", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powered by RISC", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powered by RISC-", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    gl_draw_string(0.2 * WIDTH, (0.7 * HEIGHT) + 2, "Powered by RISC-V", GL_WHITE);
    timer_delay_ms(200);
    gl_swap_buffer();
    timer_delay(2);
}

void draw_loading_screen(void) {
    gl_clear(GL_MAGENTA);
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);
    gl_draw_string(0.35 * WIDTH, (0.7 * HEIGHT) + 2, "Loading...", GL_WHITE);
    gl_swap_buffer();
    gl_clear(GL_MAGENTA);
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);
    gl_draw_string(0.35 * WIDTH, (0.7 * HEIGHT) + 2, "Loading...", GL_WHITE);
    for (int i = 0; i < 250; i++) {
        gl_draw_rect(0.18 * WIDTH, 0.75 * HEIGHT, i, 50, GL_WHITE);
        timer_delay_ms(20);
        gl_swap_buffer();
    }
}

void draw_startscreen(void) {
    gl_clear(GL_MAGENTA);
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);
    gl_draw_string(0.4 * WIDTH, (0.7 * HEIGHT) + 2, "Start", GL_WHITE);
}

void draw_startscreen_2(void) {
    gl_clear(GL_MAGENTA);
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);
}

void blinking_start_screen(void) {
    long num = 0;
    while (num < 5) {
        draw_startscreen();
        timer_delay_ms(500);
        gl_swap_buffer();
        draw_startscreen_2();
        timer_delay_ms(500);
        gl_swap_buffer();
        num++;
    }
}

void game_countdown(void) {
    draw_startscreen_2();
    draw_num(3, 0.45 * WIDTH, 0.7 * HEIGHT);
    gl_swap_buffer();
    timer_delay(1);
    draw_startscreen_2();
    draw_num(2, 0.45 * WIDTH, 0.7 * HEIGHT);
    gl_swap_buffer();
    timer_delay(1);
    draw_startscreen_2();
    draw_num(1, 0.45 * WIDTH, 0.7 * HEIGHT);
    gl_swap_buffer();
    timer_delay(1);
}

void draw_endscreen(void) {
    gl_clear(GL_MAGENTA);
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);
    gl_draw_string(0.43 * WIDTH, (0.7 * HEIGHT) + 2, "RIP", GL_WHITE);

	gl_draw_string(0.43 * WIDTH - (4 * gl_get_char_width()), (0.9 * HEIGHT), "Click for Menu", GL_YELLOW);

	gl_draw_line((.05 * WIDTH), (.9 * HEIGHT) +  (0.5 * gl_get_char_height()), (.15 * WIDTH) + 20, (.9 * HEIGHT) + (0.5 * gl_get_char_height()), GL_YELLOW);
	gl_draw_line((.2 * WIDTH) + -10, (.9 * HEIGHT) + (0.5 * gl_get_char_height()) - 10, (.15 * WIDTH) + 20, (.9 * HEIGHT) + (0.5 * gl_get_char_height()), GL_YELLOW);
	gl_draw_line((.2 * WIDTH) -10, (.9 * HEIGHT) + (0.5 * gl_get_char_height()) + 10, (.15 * WIDTH) + 20, (.9 * HEIGHT) + (0.5 * gl_get_char_height()), GL_YELLOW);
}

// need to be able to give x, y coordinates, draw character depending on that
void draw_character(int x) {
    //gl_draw_rect(((x * WIDTH) / WIDTH) - RECT_OFFSET, 0.8 * HEIGHT, 40, 200, GL_WHITE);
    gl_draw_circle(((x * WIDTH) / WIDTH), 0.87 * HEIGHT, 15, GL_BLACK);
    // Body
    gl_draw_line(((x * WIDTH) / WIDTH), 0.89 * HEIGHT, ((x * WIDTH) / WIDTH), 0.95 * HEIGHT, GL_BLACK);
    // Arms
    gl_draw_line(((x * WIDTH) / WIDTH) - ARM_OFFSET, 0.92 * HEIGHT, ((x * WIDTH) / WIDTH) + 7, 0.92 * HEIGHT, GL_BLACK);
    // Legs
    gl_draw_line(((x * WIDTH) / WIDTH), 0.95 * HEIGHT, ((x * WIDTH) / WIDTH) - LEG_OFFSET, 0.98 * HEIGHT, GL_BLACK);
    gl_draw_line(((x * WIDTH) / WIDTH), 0.95 * HEIGHT, ((x * WIDTH) / WIDTH) + LEG_OFFSET, (0.98 * HEIGHT) - LEG_OFFSET, GL_BLACK);
}

void draw_character_2(int x) {
    //gl_draw_rect(0.45 * WIDTH, 0.8 * HEIGHT, 40, 200, GL_WHITE);
    gl_draw_circle(((x * WIDTH) / WIDTH), 0.87 * HEIGHT, 15, GL_BLACK);
    gl_draw_line(((x * WIDTH) / WIDTH), 0.89 * HEIGHT, ((x * WIDTH) / WIDTH), 0.95 * HEIGHT, GL_BLACK);
    gl_draw_line(((x * WIDTH) / WIDTH) - ARM_OFFSET, 0.92 * HEIGHT, ((x * WIDTH) / WIDTH) + 7, 0.92 * HEIGHT, GL_BLACK);
    gl_draw_line(((x * WIDTH) / WIDTH), 0.95 * HEIGHT, ((x * WIDTH) / WIDTH) - LEG_OFFSET, (0.98 * HEIGHT) - LEG_OFFSET, GL_BLACK);
    gl_draw_line(((x * WIDTH) / WIDTH), 0.95 * HEIGHT, ((x * WIDTH) / WIDTH) + LEG_OFFSET, 0.98 * HEIGHT, GL_BLACK);
}

void draw_steve(int x) {
    // head
    gl_draw_rect(x - 10, 0.87 * HEIGHT - 20, 20, 20, 0x492816);
    // torso
    gl_draw_rect(x - 10, 0.87 * HEIGHT, 20, 40, GL_CYAN);

    //arms
    gl_draw_rect(x - 20, 0.87 * HEIGHT, 10, 10, GL_CYAN);
    gl_draw_rect(x - 20, 0.87 * HEIGHT + 10, 10, 7, 0xa88165);
    gl_draw_rect(x + 10, 0.87 * HEIGHT, 10, 15, GL_CYAN);
    gl_draw_rect(x + 10, 0.87 * HEIGHT + 15, 10, 10, 0xa88165);

    // legs
    // left
    gl_draw_rect(x - 10, 0.85 * HEIGHT + 50, 10, 20, GL_INDIGO + 100);
    gl_draw_rect(x - 10, 0.85 * HEIGHT + 70, 10, 4, GL_SILVER);
    // right
    gl_draw_rect(x, 0.85 * HEIGHT + 50, 10, 15, GL_INDIGO + 100);
    gl_draw_rect(x, 0.85 * HEIGHT + 65, 10, 4, GL_SILVER);
    // leg divider
    gl_draw_line(x - 1, 0.85 * HEIGHT + 50, x - 1, 0.85 * HEIGHT + 65, GL_BLACK);
}

void draw_steve_2(int x) {
    // head
    gl_draw_rect(x - 10, 0.87 * HEIGHT - 20, 20, 20, 0x492816);
    // torso
    gl_draw_rect(x - 10, 0.87 * HEIGHT, 20, 40, GL_CYAN);

    //arms
    gl_draw_rect(x - 20, 0.87 * HEIGHT, 10, 15, GL_CYAN);
    gl_draw_rect(x - 20, 0.87 * HEIGHT + 15, 10, 10, 0xa88165);
    gl_draw_rect(x + 10, 0.87 * HEIGHT, 10, 10, GL_CYAN);
    gl_draw_rect(x + 10, 0.87 * HEIGHT + 10, 10, 7, 0xa88165);

    // legs
    // left
    gl_draw_rect(x - 10, 0.85 * HEIGHT + 50, 10, 15, GL_INDIGO + 100);
    gl_draw_rect(x - 10, 0.85 * HEIGHT + 65, 10, 4, GL_SILVER);
    // right
    gl_draw_rect(x, 0.85 * HEIGHT + 50, 10, 20, GL_INDIGO + 100);
    gl_draw_rect(x, 0.85 * HEIGHT + 70, 10, 4, GL_SILVER);
    // leg divider
    gl_draw_line(x - 1, 0.85 * HEIGHT + 50, x - 1, 0.85 * HEIGHT + 65, GL_BLACK);
}

void draw_mario(int x) {
    // hat
    gl_draw_rect(x - 11, 0.87 * HEIGHT - 15, 22, 5, 0xc81a27);

    // head and hair
    gl_draw_rect(x - 10, 0.87 * HEIGHT - 10, 20, 20, 0xe2ab7d);
    gl_draw_rect(x - 10, 0.87 * HEIGHT - 10, 20, 16, 0x7e3f14);
    // ears
    gl_draw_rect(x - 12, 0.87 * HEIGHT - 5, 2, 6, 0xe2ab7d);
    gl_draw_rect(x + 10, 0.87 * HEIGHT - 5, 2, 6, 0xe2ab7d);

    // body
    gl_draw_rect(x - 8, 0.87 * HEIGHT + 10, 16, 25, 0x1f22b4);
    gl_draw_rect(x - 3, 0.87 * HEIGHT + 10, 6, 6, 0xc81a27);

    gl_draw_rect(x - 8, 0.87 * HEIGHT + 10, 3, 3, 0xc81a27);
    gl_draw_rect(x + 5, 0.87 * HEIGHT + 10, 3, 3, 0xc81a27);

    // shoes
    gl_draw_rect(x - 8, 0.87 * HEIGHT + 35, 6, 4, GL_BLACK);
    gl_draw_rect(x + 2, 0.87 * HEIGHT + 35, 6, 2, GL_BLACK);

    // arms
    // left
    gl_draw_rect(x - 11, 0.87 * HEIGHT + 12, 4, 4, 0xc81a27);
    gl_draw_rect(x - 12, 0.87 * HEIGHT + 13, 4, 4, 0xc81a27);
    gl_draw_rect(x - 13, 0.87 * HEIGHT + 14, 4, 4, 0xc81a27);
    gl_draw_rect(x - 13, 0.87 * HEIGHT + 18, 4, 2, 0xe2ab7d);

    // right
    gl_draw_rect(x + 7, 0.87 * HEIGHT + 12, 4, 4, 0xc81a27);
    gl_draw_rect(x + 8, 0.87 * HEIGHT + 15, 4, 4, 0xc81a27);
    gl_draw_rect(x + 9, 0.87 * HEIGHT + 18, 4, 4, 0xc81a27);
    gl_draw_rect(x + 9, 0.87 * HEIGHT + 21, 4, 4, 0xe2ab7d);

}

void draw_mario_2(int x) {
    // hat
    gl_draw_rect(x - 11, 0.87 * HEIGHT - 15, 22, 5, 0xc81a27);

    // head and hair
    gl_draw_rect(x - 10, 0.87 * HEIGHT - 10, 20, 20, 0xe2ab7d);
    gl_draw_rect(x - 10, 0.87 * HEIGHT - 10, 20, 16, 0x7e3f14);
    // ears
    gl_draw_rect(x - 12, 0.87 * HEIGHT - 5, 2, 6, 0xe2ab7d);
    gl_draw_rect(x + 10, 0.87 * HEIGHT - 5, 2, 6, 0xe2ab7d);

    // body
    gl_draw_rect(x - 8, 0.87 * HEIGHT + 10, 16, 25, 0x1f22b4);
    gl_draw_rect(x - 3, 0.87 * HEIGHT + 10, 6, 6, 0xc81a27);

    gl_draw_rect(x - 8, 0.87 * HEIGHT + 10, 3, 3, 0xc81a27);
    gl_draw_rect(x + 5, 0.87 * HEIGHT + 10, 3, 3, 0xc81a27);

    // shoes
    gl_draw_rect(x - 8, 0.87 * HEIGHT + 35, 6, 2, GL_BLACK);
    gl_draw_rect(x + 2, 0.87 * HEIGHT + 35, 6, 4, GL_BLACK);

    // arms
    // left
    gl_draw_rect(x - 11, 0.87 * HEIGHT + 12, 4, 4, 0xc81a27);
    gl_draw_rect(x - 12, 0.87 * HEIGHT + 15, 4, 4, 0xc81a27);
    gl_draw_rect(x - 13, 0.87 * HEIGHT + 18, 4, 4, 0xc81a27);
    gl_draw_rect(x - 13, 0.87 * HEIGHT + 21, 4, 4, 0xe2ab7d);

    // right
    gl_draw_rect(x + 7, 0.87 * HEIGHT + 12, 4, 4, 0xc81a27);
    gl_draw_rect(x + 8, 0.87 * HEIGHT + 13, 4, 4, 0xc81a27);
    gl_draw_rect(x + 9, 0.87 * HEIGHT + 14, 4, 4, 0xc81a27);
    gl_draw_rect(x + 9, 0.87 * HEIGHT + 18, 4, 2, 0xe2ab7d);
}

void draw_luigi(int x) {
    // hat
    gl_draw_rect(x - 11, 0.87 * HEIGHT - 15, 22, 5, LUIGI_GREEN);

    // head and hair
    gl_draw_rect(x - 10, 0.87 * HEIGHT - 10, 20, 20, 0xe2ab7d);
    gl_draw_rect(x - 10, 0.87 * HEIGHT - 10, 20, 16, 0x7e3f14);
    // ears
    gl_draw_rect(x - 12, 0.87 * HEIGHT - 5, 2, 6, 0xe2ab7d);
    gl_draw_rect(x + 10, 0.87 * HEIGHT - 5, 2, 6, 0xe2ab7d);

    // body
    gl_draw_rect(x - 8, 0.87 * HEIGHT + 10, 16, 25, 0x1f22b4);
    gl_draw_rect(x - 3, 0.87 * HEIGHT + 10, 6, 6, LUIGI_GREEN);

    gl_draw_rect(x - 8, 0.87 * HEIGHT + 10, 3, 3, LUIGI_GREEN);
    gl_draw_rect(x + 5, 0.87 * HEIGHT + 10, 3, 3, LUIGI_GREEN);

    // shoes
    gl_draw_rect(x - 8, 0.87 * HEIGHT + 35, 6, 4, GL_BLACK);
    gl_draw_rect(x + 2, 0.87 * HEIGHT + 35, 6, 2, GL_BLACK);

    // arms
    // left
    gl_draw_rect(x - 11, 0.87 * HEIGHT + 12, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x - 12, 0.87 * HEIGHT + 13, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x - 13, 0.87 * HEIGHT + 14, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x - 13, 0.87 * HEIGHT + 18, 4, 2, 0xe2ab7d);

    // right
    gl_draw_rect(x + 7, 0.87 * HEIGHT + 12, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x + 8, 0.87 * HEIGHT + 15, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x + 9, 0.87 * HEIGHT + 18, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x + 9, 0.87 * HEIGHT + 21, 4, 4, 0xe2ab7d);

}

void draw_luigi_2(int x) {
    // hat
    gl_draw_rect(x - 11, 0.87 * HEIGHT - 15, 22, 5, LUIGI_GREEN);

    // head and hair
    gl_draw_rect(x - 10, 0.87 * HEIGHT - 10, 20, 20, 0xe2ab7d);
    gl_draw_rect(x - 10, 0.87 * HEIGHT - 10, 20, 16, 0x7e3f14);
    // ears
    gl_draw_rect(x - 12, 0.87 * HEIGHT - 5, 2, 6, 0xe2ab7d);
    gl_draw_rect(x + 10, 0.87 * HEIGHT - 5, 2, 6, 0xe2ab7d);

    // body
    gl_draw_rect(x - 8, 0.87 * HEIGHT + 10, 16, 25, 0x1f22b4);
    gl_draw_rect(x - 3, 0.87 * HEIGHT + 10, 6, 6, LUIGI_GREEN);

    gl_draw_rect(x - 8, 0.87 * HEIGHT + 10, 3, 3, LUIGI_GREEN);
    gl_draw_rect(x + 5, 0.87 * HEIGHT + 10, 3, 3, LUIGI_GREEN);

    // shoes
    gl_draw_rect(x - 8, 0.87 * HEIGHT + 35, 6, 2, GL_BLACK);
    gl_draw_rect(x + 2, 0.87 * HEIGHT + 35, 6, 4, GL_BLACK);

    // arms
    // left
    gl_draw_rect(x - 11, 0.87 * HEIGHT + 12, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x - 12, 0.87 * HEIGHT + 15, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x - 13, 0.87 * HEIGHT + 18, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x - 13, 0.87 * HEIGHT + 21, 4, 4, 0xe2ab7d);

    // right
    gl_draw_rect(x + 7, 0.87 * HEIGHT + 12, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x + 8, 0.87 * HEIGHT + 13, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x + 9, 0.87 * HEIGHT + 14, 4, 4, LUIGI_GREEN);
    gl_draw_rect(x + 9, 0.87 * HEIGHT + 18, 4, 2, 0xe2ab7d);
}


void character_pose_1(position_t pos, skin_t skin) {
    if (skin == STICK) {
        if (pos == LEFT) {
            draw_character(LANE1);
        }
        if (pos == CENTER) {
            draw_character(LANE2);
        }
        if (pos == RIGHT) {
            draw_character(LANE3);
        }
    }
    else if (skin == STEVE) {
        if (pos == LEFT) {
            draw_steve(LANE1);
        }
        if (pos == CENTER) {
            draw_steve(LANE2);
        }
        if (pos == RIGHT) {
            draw_steve(LANE3);
        }
    } else if (skin == MARIO) {
        if (pos == LEFT) {
            draw_mario(LANE1);
        }
        if (pos == CENTER) {
            draw_mario(LANE2);
        }
        if (pos == RIGHT) {
            draw_mario(LANE3);
        }
    } else if (skin == LUIGI) {
        if (pos == LEFT) {
            draw_luigi(LANE1);
        }
        if (pos == CENTER) {
            draw_luigi(LANE2);
        }
        if (pos == RIGHT) {
            draw_luigi(LANE3);
        }
    }
}

void character_pose_2(position_t pos, skin_t skin) {
    if (skin == STICK) {
        if (pos == LEFT) {
            draw_character_2(LANE1);
        }
        if (pos == CENTER) {
            draw_character_2(LANE2);
        }
        if (pos == RIGHT) {
            draw_character_2(LANE3);
        }
    }
    else if (skin == STEVE) {
        if (pos == LEFT) {
            draw_steve_2(LANE1);
        }
        if (pos == CENTER) {
            draw_steve_2(LANE2);
        }
        if (pos == RIGHT) {
            draw_steve_2(LANE3);
        }
    }
    else if (skin == MARIO) {
        if (pos == LEFT) {
            draw_mario_2(LANE1);
        }
        if (pos == CENTER) {
            draw_mario_2(LANE2);
        }
        if (pos == RIGHT) {
            draw_mario_2(LANE3);
        }
    } else if (skin == LUIGI) {
        if (pos == LEFT) {
            draw_luigi_2(LANE1);
        }
        if (pos == CENTER) {
            draw_luigi_2(LANE2);
        }
        if (pos == RIGHT) {
            draw_luigi_2(LANE3);
        }
    }
}

void draw_barrier(int x, int y, barrier_t type) {
    if (type == BLOCK) {
        gl_draw_rect(x, 0.68 * HEIGHT + y, 40, 40, GL_BLACK);
    } else if (type == BEE) {
        draw_barrier_bee(x, y);
    } else if (type == FLY) {
        draw_barrier_fly(x, y);
    } else if (type == RED_GHOST) {
        draw_barrier_ghost_red(x, y);
    }
}

void draw_barrier_bee(int x, int y) {
    gl_draw_rect(x - 1, 0.68 * HEIGHT - 7 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 6, 0.68 * HEIGHT - 7 + y, 3, 3, GL_BLACK);

    gl_draw_rect(x - 3, 0.68 * HEIGHT - 9 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 8, 0.68 * HEIGHT - 9 + y, 3, 3, GL_BLACK);

    gl_draw_rect(x - 5, 0.68 * HEIGHT - 11 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 10, 0.68 * HEIGHT - 11 + y, 3, 3, GL_BLACK);
 
    gl_draw_rect(x + 1, 0.68 * HEIGHT - 5 + y, 6, 5, GL_BLACK);
    gl_draw_rect(x - 3, 0.68 * HEIGHT + y, 14, 5, GL_BLACK);
    gl_draw_rect(x - 3, 0.68 * HEIGHT + 5 + y, 14, 5, 0xffc40c);
    gl_draw_rect(x - 6, 0.68 * HEIGHT + 10 + y, 20, 5, GL_BLACK);
    gl_draw_rect(x - 6, 0.68 * HEIGHT + 15 + y, 20, 5, 0xffc40c);
    gl_draw_rect(x - 6, 0.68 * HEIGHT + 20 + y, 20, 5, GL_BLACK);
    gl_draw_rect(x - 6, 0.68 * HEIGHT + 25 + y, 20, 5, 0xffc40c);
    gl_draw_rect(x - 3, 0.68 * HEIGHT + 30 + y, 14, 5, GL_BLACK);
    gl_draw_rect(x + 1, 0.68 * HEIGHT + 35 + y, 6, 5, GL_BLACK);

    gl_draw_rect(x - 16, 0.68 * HEIGHT + 6 + y, 10, 10, 0xbbeffd);
    gl_draw_rect(x - 16, 0.68 * HEIGHT + 22 + y, 10, 10, 0xbbeffd);

    gl_draw_rect(x + 14, 0.68 * HEIGHT + 6 + y, 10, 10, 0xbbeffd);
    gl_draw_rect(x + 14, 0.68 * HEIGHT + 22 + y, 10, 10, 0xbbeffd);
}


void draw_menu(int cur_select) {
    gl_clear(GL_MAGENTA);
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);

	if (cur_select == 0) {
		gl_draw_line((.05 * WIDTH), (.14 * HEIGHT) + 350 + (0.5 * gl_get_char_height()), (.1 * WIDTH) + 20, (.14 * HEIGHT) + 350 + (0.5 * gl_get_char_height()), GL_YELLOW);
		gl_draw_line((.1 * WIDTH) + 10, (.14 * HEIGHT) + 350 + (0.5 * gl_get_char_height()) - 10, (.1 * WIDTH) + 20, (.14 * HEIGHT) + 350 + (0.5 * gl_get_char_height()), GL_YELLOW);
		gl_draw_line((.1 * WIDTH) + 10, (.14 * HEIGHT) + 350 + (0.5 * gl_get_char_height()) + 10, (.1 * WIDTH) + 20, (.14 * HEIGHT) + 350 + (0.5 * gl_get_char_height()), GL_YELLOW);


		gl_draw_string((.22 * WIDTH) + (3 * gl_get_char_width()) , (.14 * HEIGHT) + 350, "Play Game", GL_YELLOW);
		gl_draw_string(.22 * WIDTH, (.14 * HEIGHT) + 350 + (2 * gl_get_char_height()), "Character Select", GL_WHITE);
		gl_draw_string((.22 * WIDTH) + (3 * gl_get_char_width()), (.14 * HEIGHT) + 350 + (4 * gl_get_char_height()), "Top Scores", GL_WHITE);
	} else if (cur_select == 1) {

		gl_draw_line((.05 * WIDTH), (.14 * HEIGHT) + 350 + (2.5 * gl_get_char_height()), (.1 * WIDTH) + 20, (.14 * HEIGHT) + 350 + (2.5 * gl_get_char_height()), GL_YELLOW);
		gl_draw_line((.1 * WIDTH) + 10, (.14 * HEIGHT) + 350 + (2.5 * gl_get_char_height()) - 10, (.1 * WIDTH) + 20, (.14 * HEIGHT) + 350 + (2.5 * gl_get_char_height()), GL_YELLOW);
		gl_draw_line((.1 * WIDTH) + 10, (.14 * HEIGHT) + 350 + (2.5 * gl_get_char_height()) + 10, (.1 * WIDTH) + 20, (.14 * HEIGHT) + 350 + (2.5 * gl_get_char_height()), GL_YELLOW);


		gl_draw_string((.22 * WIDTH) + (3 * gl_get_char_width()) , (.14 * HEIGHT) + 350, "Play Game", GL_WHITE);
		gl_draw_string(.22 * WIDTH, (.14 * HEIGHT) + 350 + (2 * gl_get_char_height()), "Character Select", GL_YELLOW);
		gl_draw_string((.22 * WIDTH) + (3 * gl_get_char_width()), (.14 * HEIGHT) + 350 + (4 * gl_get_char_height()), "Top Scores", GL_WHITE);
	} else if (cur_select == 2) {
			
		gl_draw_line((.05 * WIDTH), (.14 * HEIGHT) + 350 + (4.5 * gl_get_char_height()), (.1 * WIDTH) + 20, (.14 * HEIGHT) + 350 + (4.5 * gl_get_char_height()), GL_YELLOW);
		gl_draw_line((.1 * WIDTH) + 10, (.14 * HEIGHT) + 350 + (4.5 * gl_get_char_height()) - 10, (.1 * WIDTH) + 20, (.14 * HEIGHT) + 350 + (4.5 * gl_get_char_height()), GL_YELLOW);
		gl_draw_line((.1 * WIDTH) + 10, (.14 * HEIGHT) + 350 + (4.5 * gl_get_char_height()) + 10, (.1 * WIDTH) + 20, (.14 * HEIGHT) + 350 + (4.5 * gl_get_char_height()), GL_YELLOW);

		gl_draw_string((.22 * WIDTH) + (3 * gl_get_char_width()) , (.14 * HEIGHT) + 350, "Play Game", GL_WHITE);
		gl_draw_string(.22 * WIDTH, (.14 * HEIGHT) + 350 + (2 * gl_get_char_height()), "Character Select", GL_WHITE);
		gl_draw_string((.22 * WIDTH) + (3 * gl_get_char_width()), (.14 * HEIGHT) + 350 + (4 * gl_get_char_height()), "Top Scores", GL_YELLOW);

	}
	
    gl_swap_buffer();
}

static void sort_top_scores(int *scores) {
	for (int i = 0; i < 9; i++) {
		int min_index = i;
	
		for (int j = i + 1; j < 10; j++) {
			if (scores[j] < scores[min_index]) {
				min_index = j;
			}
		}
		int temp = scores[i];
		scores[i] = scores[min_index];
		scores[min_index] = temp;
	}
}

void draw_character_select(int cur_select) {
	int test_height = 100;
	gl_clear(GL_MAGENTA);
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);

    gl_draw_string(0.2 * WIDTH - (0.5 * gl_get_char_width()) + 2, (0.14 * HEIGHT) + 40, "Character Select", GL_WHITE);
	gl_draw_line(0.2 * WIDTH - 5 - 0.5*gl_get_char_width() + 2, ((0.14 * HEIGHT) + 40) + gl_get_char_height() + 2, (0.2 * WIDTH) + (gl_get_char_width() * 16) + 5 - 2 - 0.5*gl_get_char_width(), ((0.14 * HEIGHT) + 40) + gl_get_char_height() + 2, GL_WHITE);
	if (cur_select == 0) {
		gl_draw_line((.05 * WIDTH) - 20, (.14 * HEIGHT) + (0.5 * gl_get_char_height()) + test_height, (.1 * WIDTH)+ 20 - 20, (.14 * HEIGHT) + (0.5 * gl_get_char_height()) + test_height, GL_YELLOW);
		gl_draw_line((.1 * WIDTH) + 10 - 20, (.14 * HEIGHT) + (0.5 * gl_get_char_height()) - 10 + test_height, (.1 * WIDTH) + 20 - 20, (.14 * HEIGHT) + (0.5 * gl_get_char_height()) + test_height, GL_YELLOW);
		gl_draw_line((.1 * WIDTH) + 10 - 20, (.14 * HEIGHT) + (0.5 * gl_get_char_height()) + 10 + test_height, (.1 * WIDTH) + 20 - 20, (.14 * HEIGHT) + (0.5 * gl_get_char_height()) + test_height, GL_YELLOW);


		gl_draw_string((.22 * WIDTH) , (.14 * HEIGHT)  + test_height, "Next Character", GL_YELLOW);
		gl_draw_string(.22 * WIDTH - 3*gl_get_char_width(), (.14 * HEIGHT)  + (2 * gl_get_char_height()) + test_height, "Select This Character", GL_WHITE);
	} else if (cur_select == 1) {

		gl_draw_line((.05 * WIDTH) - 20, (.14 * HEIGHT) + (2.5 * gl_get_char_height()) + test_height, (.1 * WIDTH) + 20 - 20, (.14 * HEIGHT) + (2.5 * gl_get_char_height()) + test_height, GL_YELLOW);
		gl_draw_line((.1 * WIDTH) + 10 - 20, (.14 * HEIGHT) + (2.5 * gl_get_char_height()) - 10 + test_height, (.1 * WIDTH) + 20 - 20, (.14 * HEIGHT) + (2.5 * gl_get_char_height()) + test_height, GL_YELLOW);
		gl_draw_line((.1 * WIDTH) + 10 - 20, (.14 * HEIGHT)  + (2.5 * gl_get_char_height()) + 10 + test_height, (.1 * WIDTH) + 20 - 20, (.14 * HEIGHT) + (2.5 * gl_get_char_height()) + test_height, GL_YELLOW);


		gl_draw_string((.22 * WIDTH) , (.14 * HEIGHT) + test_height, "Next Character", GL_WHITE);
		gl_draw_string(.22 * WIDTH - 3*gl_get_char_width(), (.14 * HEIGHT)  + (2 * gl_get_char_height()) + test_height, "Select This Character", GL_YELLOW);
	}

	//gl_swap_buffer();

}

void draw_top_scores(int *scores) {
	gl_clear(GL_MAGENTA);
    gl_draw_rect(0.18 * WIDTH, 0.11 * HEIGHT, 250, 50, GL_MAGENTA);
    gl_draw_rect(0.72 * WIDTH, 0.14 * HEIGHT, 20, 20, GL_YELLOW);
    gl_draw_rect(0.76 * WIDTH, 0.133 * HEIGHT, 6, 6, GL_GREEN);
    gl_draw_string(0.2 * WIDTH, (0.14 * HEIGHT) + 2, "CS107E Surfers", GL_BLACK);

	int *scores_copy = malloc(10 * sizeof(int));
	memcpy(scores_copy, scores, 10 * sizeof(int));
	sort_top_scores(scores_copy);

	//header
    gl_draw_string(0.2 * WIDTH + 3*gl_get_char_width(), (0.14 * HEIGHT) + 40, "TOP SCORES", GL_WHITE);
	gl_draw_line(0.2 * WIDTH - 5 + 3*gl_get_char_width(), ((0.14 * HEIGHT) + 40) + gl_get_char_height() + 2, (0.2 * WIDTH) + (gl_get_char_width() * 10) + 5 + 3*gl_get_char_width(), ((0.14 * HEIGHT) + 40) + gl_get_char_height() + 2, GL_WHITE);
	
	for (int i = 9; i >= 0; i--) {
		if (scores_copy[i] == -1) {
			continue;
		}		

		char buf[40];
		snprintf(buf, 40, "%d) %d", (10 - i), scores_copy[i]);
		gl_draw_string(0.2 * WIDTH, (0.25 * HEIGHT) + (((10 - i) * 2 * gl_get_char_height()) + 2), buf, GL_WHITE);
	}
    gl_draw_string(0.2 * WIDTH - 3*gl_get_char_width(), (0.9 * HEIGHT) + 20, "Press Confirm for Menu", GL_YELLOW);
	gl_swap_buffer();
}

void draw_barrier_fly(int x, int y) {
    // body
    gl_draw_rect(x, 0.70 * HEIGHT + y, 12, 12, 0x349a4c);
    gl_draw_rect(x, 0.70 * HEIGHT + 12 + y, 12, 12, 0x1b714d);

    // eyes

    gl_draw_rect(x, 0.70 * HEIGHT + y, 4, 4, 0xff4f01);
    gl_draw_rect(x + 8, 0.70 * HEIGHT + y, 4, 4, 0xff4f01);

    // front antennae
    gl_draw_rect(x + 2, 0.70 * HEIGHT - 3 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 2, 0.70 * HEIGHT - 6 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 7, 0.70 * HEIGHT - 3 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 7, 0.70 * HEIGHT - 6 + y, 3, 3, GL_BLACK);
    
    gl_draw_rect(x - 1, 0.70 * HEIGHT - 6 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x - 1, 0.70 * HEIGHT - 9 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 10, 0.70 * HEIGHT - 6 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 10, 0.70 * HEIGHT - 9 + y, 3, 3, GL_BLACK);

    // front legs
    // left
    gl_draw_rect(x - 3, 0.70 * HEIGHT + 7 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x - 5, 0.70 * HEIGHT + 5 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x - 7, 0.70 * HEIGHT + 3 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x - 9, 0.70 * HEIGHT + 1 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x - 11, 0.70 * HEIGHT - 1 + y, 3, 3, GL_BLACK);
   
    // right
    gl_draw_rect(x + 12, 0.70 * HEIGHT + 7 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 14, 0.70 * HEIGHT + 5 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 16, 0.70 * HEIGHT + 3 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 18, 0.70 * HEIGHT + 1 + y, 3, 3, GL_BLACK);
    gl_draw_rect(x + 20, 0.70 * HEIGHT - 1 + y, 3, 3, GL_BLACK);


    // wings
    gl_draw_rect(x - 6, 0.70 * HEIGHT + 10 + y, 8, 8, 0xc8cfd9);
    gl_draw_rect(x - 11, 0.70 * HEIGHT + 15 + y, 8, 8, 0xc8cfd9);
    gl_draw_rect(x + 10, 0.70 * HEIGHT + 10 + y, 8, 8, 0xc8cfd9);
    gl_draw_rect(x + 15, 0.70 * HEIGHT + 15 + y, 8, 8, 0xc8cfd9);
}

void draw_barrier_ghost_red(int x, int y) {
    printf("GHOST");
    // body/head
    gl_draw_rect(x - 1, 0.68 * HEIGHT - 6 + y, 2, 2, RED_GHOST);
    gl_draw_rect(x - 4, 0.68 * HEIGHT - 4 + y, 8, 2, RED_GHOST);
    gl_draw_rect(x - 7, 0.68 * HEIGHT - 2 + y, 14, 2, RED_GHOST);
    gl_draw_rect(x - 10, 0.68 * HEIGHT + y, 20, 14, RED_GHOST);

    // eyes
    gl_draw_rect(x - 6, 0.68 * HEIGHT + 2 + y, 4, 4, GL_WHITE);
    gl_draw_rect(x - 4, 0.68 * HEIGHT + 3 + y, 2, 3, GHOST_EYE);
    gl_draw_rect(x + 2, 0.68 * HEIGHT + 2 + y, 4, 4, GL_WHITE);
    gl_draw_rect(x + 4, 0.68 * HEIGHT + 3 + y, 2, 3, GHOST_EYE);

    // bottom half
    gl_draw_rect(x - 10, 0.68 * HEIGHT + 14 + y, 2, 6, RED_GHOST);
    gl_draw_rect(x - 8, 0.68 * HEIGHT + 14 + y, 2, 4, RED_GHOST);
    gl_draw_rect(x - 6, 0.68 * HEIGHT + 14 + y, 2, 2, RED_GHOST);
    
    gl_draw_rect(x - 2, 0.68 * HEIGHT + 14 + y, 2, 2, RED_GHOST);
    gl_draw_rect(x - 1, 0.68 * HEIGHT + 14 + y, 2, 4, RED_GHOST);
    gl_draw_rect(x, 0.68 * HEIGHT + 14 + y, 2, 2, RED_GHOST);
    
    gl_draw_rect(x + 8, 0.68 * HEIGHT + 14 + y, 2, 6, RED_GHOST);
    gl_draw_rect(x + 6, 0.68 * HEIGHT + 14 + y, 2, 4, RED_GHOST);
    gl_draw_rect(x + 4, 0.68 * HEIGHT + 14 + y, 2, 2, RED_GHOST);
}

void draw_train_slats(void) {
    // left
    gl_draw_line(LANE1 - 15, 0.75 * HEIGHT + 1, LANE1 + 83, 0.75 * HEIGHT + 1, TRAIN_SLATS);
    gl_draw_line(LANE1 - 23, 0.75 * HEIGHT + 20, LANE1 + 79, 0.75 * HEIGHT + 20, TRAIN_SLATS);
    gl_draw_line(LANE1 - 29, 0.75 * HEIGHT + 40, LANE1 + 77, 0.75 * HEIGHT + 40, TRAIN_SLATS);
    gl_draw_line(LANE1 - 35, 0.75 * HEIGHT + 60, LANE1 + 72, 0.75 * HEIGHT + 60, TRAIN_SLATS);
    gl_draw_line(LANE1 - 41, 0.75 * HEIGHT + 80, LANE1 + 68, 0.75 * HEIGHT + 80, TRAIN_SLATS);
    gl_draw_line(LANE1 - 49, 0.75 * HEIGHT + 100, LANE1 + 65, 0.75 * HEIGHT + 100, TRAIN_SLATS);
    gl_draw_line(LANE1 - 55, 0.75 * HEIGHT + 120, LANE1 + 63, 0.75 * HEIGHT + 120, TRAIN_SLATS);
    gl_draw_line(LANE1 - 61, 0.75 * HEIGHT + 140, LANE1 + 56, 0.75 * HEIGHT + 140, TRAIN_SLATS);
    

    // middle
    gl_draw_line(LANE2 - 50, 0.75 * HEIGHT + 1, LANE2 + 50, 0.75 * HEIGHT + 1, TRAIN_SLATS);
    gl_draw_line(LANE2 - 55, 0.75 * HEIGHT + 20, LANE2 + 55, 0.75 * HEIGHT + 20, TRAIN_SLATS);
    gl_draw_line(LANE2 - 58, 0.75 * HEIGHT + 40, LANE2 + 58, 0.75 * HEIGHT + 40, TRAIN_SLATS);
    gl_draw_line(LANE2 - 62, 0.75 * HEIGHT + 60, LANE2 + 62, 0.75 * HEIGHT + 60, TRAIN_SLATS);
    gl_draw_line(LANE2 - 66, 0.75 * HEIGHT + 80, LANE2 + 66, 0.75 * HEIGHT + 80, TRAIN_SLATS);
    gl_draw_line(LANE2 - 70, 0.75 * HEIGHT + 100, LANE2 + 70, 0.75 * HEIGHT + 100, TRAIN_SLATS);
    gl_draw_line(LANE2 - 74, 0.75 * HEIGHT + 120, LANE2 + 74, 0.75 * HEIGHT + 120, TRAIN_SLATS);
    gl_draw_line(LANE2 - 78, 0.75 * HEIGHT + 140, LANE2 + 78, 0.75 * HEIGHT + 140, TRAIN_SLATS);

    // right
    gl_draw_line(LANE3 - 84, 0.75 * HEIGHT + 1, LANE3 + 18, 0.75 * HEIGHT + 1, TRAIN_SLATS);
    gl_draw_line(LANE3 - 80, 0.75 * HEIGHT + 20, LANE3 + 24, 0.75 * HEIGHT + 20, TRAIN_SLATS);
    gl_draw_line(LANE3 - 76, 0.75 * HEIGHT + 40, LANE3 + 31, 0.75 * HEIGHT + 40, TRAIN_SLATS);
    gl_draw_line(LANE3 - 72, 0.75 * HEIGHT + 60, LANE3 + 37, 0.75 * HEIGHT + 60, TRAIN_SLATS);
    gl_draw_line(LANE3 - 68, 0.75 * HEIGHT + 80, LANE3 + 44, 0.75 * HEIGHT + 80, TRAIN_SLATS);
    gl_draw_line(LANE3 - 64, 0.75 * HEIGHT + 100, LANE3 + 51, 0.75 * HEIGHT + 100, TRAIN_SLATS);
    gl_draw_line(LANE3 - 60, 0.75 * HEIGHT + 120, LANE3 + 58, 0.75 * HEIGHT + 120, TRAIN_SLATS);
    gl_draw_line(LANE3 - 56, 0.75 * HEIGHT + 140, LANE3 + 66, 0.75 * HEIGHT + 140, TRAIN_SLATS);
}

void draw_train_slats_2(void) {
    // left
    gl_draw_line(LANE1 - 18, 0.75 * HEIGHT + 10, LANE1 + 82, 0.75 * HEIGHT + 10, TRAIN_SLATS);
    gl_draw_line(LANE1 - 26, 0.75 * HEIGHT + 30, LANE1 + 78, 0.75 * HEIGHT + 30, TRAIN_SLATS);
    gl_draw_line(LANE1 - 32, 0.75 * HEIGHT + 50, LANE1 + 76, 0.75 * HEIGHT + 50, TRAIN_SLATS);
    gl_draw_line(LANE1 - 38, 0.75 * HEIGHT + 70, LANE1 + 71, 0.75 * HEIGHT + 70, TRAIN_SLATS);
    gl_draw_line(LANE1 - 44, 0.75 * HEIGHT + 90, LANE1 + 67, 0.75 * HEIGHT + 90, TRAIN_SLATS);
    gl_draw_line(LANE1 - 53, 0.75 * HEIGHT + 110, LANE1 + 62, 0.75 * HEIGHT + 110, TRAIN_SLATS);
    gl_draw_line(LANE1 - 58, 0.75 * HEIGHT + 130, LANE1 + 60, 0.75 * HEIGHT + 130, TRAIN_SLATS);
    gl_draw_line(LANE1 - 64, 0.75 * HEIGHT + 150, LANE1 + 53, 0.75 * HEIGHT + 150, TRAIN_SLATS);

    // middle
    gl_draw_line(LANE2 - 52, 0.75 * HEIGHT + 10, LANE2 + 52, 0.75 * HEIGHT + 10, TRAIN_SLATS);
    gl_draw_line(LANE2 - 57, 0.75 * HEIGHT + 30, LANE2 + 57, 0.75 * HEIGHT + 30, TRAIN_SLATS);
    gl_draw_line(LANE2 - 60, 0.75 * HEIGHT + 50, LANE2 + 60, 0.75 * HEIGHT + 50, TRAIN_SLATS);
    gl_draw_line(LANE2 - 64, 0.75 * HEIGHT + 70, LANE2 + 64, 0.75 * HEIGHT + 70, TRAIN_SLATS);
    gl_draw_line(LANE2 - 68, 0.75 * HEIGHT + 90, LANE2 + 68, 0.75 * HEIGHT + 90, TRAIN_SLATS);
    gl_draw_line(LANE2 - 72, 0.75 * HEIGHT + 110, LANE2 + 72, 0.75 * HEIGHT + 110, TRAIN_SLATS);
    gl_draw_line(LANE2 - 76, 0.75 * HEIGHT + 130, LANE2 + 76, 0.75 * HEIGHT + 130, TRAIN_SLATS);
    gl_draw_line(LANE2 - 80, 0.75 * HEIGHT + 150, LANE2 + 80, 0.75 * HEIGHT + 150, TRAIN_SLATS);
    
    // right
    gl_draw_line(LANE3 - 80, 0.75 * HEIGHT + 10, LANE3 + 22, 0.75 * HEIGHT + 10, TRAIN_SLATS);
    gl_draw_line(LANE3 - 76, 0.75 * HEIGHT + 30, LANE3 + 28, 0.75 * HEIGHT + 30, TRAIN_SLATS);
    gl_draw_line(LANE3 - 72, 0.75 * HEIGHT + 50, LANE3 + 35, 0.75 * HEIGHT + 50, TRAIN_SLATS);
    gl_draw_line(LANE3 - 68, 0.75 * HEIGHT + 70, LANE3 + 41, 0.75 * HEIGHT + 70, TRAIN_SLATS);
    gl_draw_line(LANE3 - 64, 0.75 * HEIGHT + 90, LANE3 + 48, 0.75 * HEIGHT + 90, TRAIN_SLATS);
    gl_draw_line(LANE3 - 60, 0.75 * HEIGHT + 110, LANE3 + 55, 0.75 * HEIGHT + 110, TRAIN_SLATS);
    gl_draw_line(LANE3 - 56, 0.75 * HEIGHT + 130, LANE3 + 62, 0.75 * HEIGHT + 130, TRAIN_SLATS);
    gl_draw_line(LANE3 - 52, 0.75 * HEIGHT + 150, LANE3 + 70, 0.75 * HEIGHT + 150, TRAIN_SLATS);
}
