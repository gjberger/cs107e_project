#include "uart.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "game_graphics.h"
#include "strings.h"
#include "mpu6050.h"

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

void character_pose_1(position_t pos) {
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

void character_pose_2(position_t pos) {
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

void draw_barrier(int x, int y) {
    gl_draw_rect(x, y, 40, 40, GL_BLACK);
}

