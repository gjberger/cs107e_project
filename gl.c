/* File: gl.c
 * ----------
 *  This file contains functions which implement basic graphics
 *  library primitives. This module builds on the framebuffer
 *  module, which allocates memory for 1 or 2 framebuffers,
 *  depending on client preference. The included functions
 *  can draw pixels, read pixels, draw rectangles, resert
 *  the screen to a different color, etc.
 */
#include "gl.h"
#include "font.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"

void gl_draw_circle(int xCenter, int yCenter, int radius, color_t c);
float sqrt(int number);

// This function initializes the graphics library using the
// underlying framebuffer module. The library can be set
// to either single or double framebuffer mode.
void gl_init(int width, int height, gl_mode_t mode) {
    // 32 bits used to read mstatus CSRR, or it with 1 in the 13th
    // bit, which controls floating point, then write it back to
    // mstatus.
    uint32_t rax = 0;
    // read register val
    // The following are inline assembly instructions to turn on
    // floating point within RISC-V hardware.
    //asm volatile ("csrr %0, mstatus" : "=r" (rax));
    rax |= (1 << 13);
    //asm volatile ("csrw mstatus, %0" : : "r" (rax));
    fb_init(width, height, mode);
}

// Returns framebuffer width.
int gl_get_width(void) {
    return fb_get_width();
}

// Returns framebuffer height.
int gl_get_height(void) {
    return fb_get_height();
}

// This function creates a color composed of a red, green, and blue
// color component.
color_t gl_color(uint8_t r, uint8_t g, uint8_t b) {
    // Use shifting and OR logic to put RGBA in proper place.
    color_t color = 0xff000000;
    color |= (r << 16);
    color |= (g << 8);
    color |= b;
    return color;
}

// This function, if in double buffer mode, swaps the active buffer.
// Otherwise, nothing happens.
void gl_swap_buffer(void) {
    fb_swap_buffer();
}

// This function clears all pixels in framebuffer and sets them
// to the specified color.
void gl_clear(color_t c) {
    // which buffer does it draw on
    int height = fb_get_height();
    int width = fb_get_width();
    unsigned int (*im)[width] = fb_get_draw_buffer();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            im[y][x] = c;
        }
    }
}

// This function draws a pixel of color `c` at destination
// (x, y). If this destination is outside the bounds of the
// frame buffer, nothing happens.
void gl_draw_pixel(int x, int y, color_t c) {
    // Bounds check.
    unsigned int (*im)[fb_get_width()] = fb_get_draw_buffer();
    if (x >= 0 && y >= 0 && x < fb_get_width() && y < fb_get_height()) {
        im[y][x] = c;
    }
}

// This function returns the color of a pixel at destination
// (x, y). If this destination is outside the bounds of the
// framebuffer, it returns 0.
color_t gl_read_pixel(int x, int y) {
    unsigned int (*im)[fb_get_width()] = fb_get_draw_buffer();
    
    if (x >= 0 && y >= 0 && x < fb_get_width() && y < fb_get_height()) {
        return im[y][x];
    } else {
        return 0;
    }
}

// This function draws a rectangle of height `h` and width `w`
// starting at the specified (x, y) coordinates. If it goes
// beyond the bounds of the framebuffer, `gl_draw_pixel` doesn't
// draw the pixels.
void gl_draw_rect(int x, int y, int w, int h, color_t c) {
    // doing the simple, slow way first
    if (w > 0 && h > 0) {
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                gl_draw_pixel(x + i, y + j, c);
            }
        }
    }
}

// This fucntion draws a char to the screen, starting at location
// (x, y). The font module is used to get the char's glyph, height,
// and width. Since `gl_draw_pixel` is used, the function will not
// draw beyond the framebuffer.
void gl_draw_char(int x, int y, char ch, color_t c) {
    int font_width = font_get_glyph_width();
    int font_height = font_get_glyph_height();
    uint8_t buf[font_get_glyph_size()];
    bool exists = font_get_glyph(ch, buf, sizeof(buf));
    if (exists) {
        uint8_t (*img) [font_width] = (void *) buf;
        // Use loop to draw out pixels.
        // Outer loop is width.
        for (int i = 0; i < font_width; i++) {
            // Inner loop is height.
            for (int j = 0; j < font_height; j++) {
                if (img[j][i] == 0xff) {
                    gl_draw_pixel(x + i, y + j, c);
                }
            }
        }
    }
}

// This function draws a string to the screen using repeated calls to
// `gl_draw_char`. The string is drawn in a single line. The function
// will not draw beyond the framebuffer because `gl_draw_pixel` is used.
void gl_draw_string(int x, int y, const char* str, color_t c) {
    static int num_counted = 0;
    int width = font_get_glyph_width();
    // Loop through chars in string.
    int length = strlen(str);
    for (int i = 0; i < length; i++) {
        // Each new character starts at an x value offset from
        // the original, depending on what other characters have
        // been output.
        gl_draw_char(x + (i * width), y, str[i], c);
        num_counted++;
    }
}

// This function gives the absolute value.
int abs(int a) {
    if (a < 0) {
        return -a;
    } else {
        return a;
    }
}

// This function draws an anti-aliased line using Xiaolin Wu's algorithm.
void gl_draw_line(int x1, int y1, int x2, int y2, color_t c) {
    int tempx = 0;
    int tempy = 0;
    // Checks which difference is bigger, delta y or delta x.
    if (abs(y2 - y1) < abs(x2 - x1)) {
        // Swaps vals.
        if (x2 < x1) {
            tempy = y2;
            tempx = x2;
            y2 = y1;
            x2 = x1;
            y1 = tempy;
            x1 = tempx;
        }
        // Calculate slope.
        int dx = x2 - x1;
        int dy = y2 - y1;
        float m = (float) dy / dx;
        // Draw pixels with varying brightness dependent on distance from line.
        for (int x = x1; x < x2; x++) {
            float y = y1 + m * (x - x1);
            int y_int = (int) y;
            float dist = y - y_int;
            float top_divisor = (1) / (1 - dist);
            float bottom_divisor = 1 / dist;
            uint8_t red = (c >> 16) & 0xff;
            uint8_t green = (c >> 8) & 0xff;
            uint8_t blue = c & 0xff;
            if (top_divisor != 0) {
                red /= top_divisor;
                green /= top_divisor;
                blue /= top_divisor;
            }
            uint8_t red1 = (c >> 16) & 0xff;
            uint8_t green1 = (c >> 8) & 0xff;
            uint8_t blue1 = c & 0xff;
            if (bottom_divisor != 0) {
                red1 /= bottom_divisor;
                green1 /= bottom_divisor;
                blue1 /= bottom_divisor;
            }
            gl_draw_pixel(x, y_int, gl_color(red, green, blue));
            gl_draw_pixel(x, y_int + 1, gl_color(red1, green1, blue1));
        }
    } else {
        if (y2 < y1) {
            tempy = y2;
            tempx = x2;
            y2 = y1;
            x2 = x1;
            y1 = tempy;
            x1 = tempx;
        }

        int dx = x2 - x1;
        int dy = y2 - y1;
        float m = (float) dx / dy; // speicla case?
        for (int y = y1; y < y2; y++) {
            float x = x1 + m * (y - y1);
            int x_int = (int) x;
            float dist = x - x_int;
            float top_divisor = (1) / (1 - dist);
            float bottom_divisor = 1 / dist;
            uint8_t red = (c >> 16) & 0xff;
            uint8_t green = (c >> 8) & 0xff;
            uint8_t blue = c & 0xff;
            if (top_divisor != 0) {
                red /= top_divisor;
                green /= top_divisor;
                blue /= top_divisor;
            }
            uint8_t red1 = (c >> 16) & 0xff;
            uint8_t green1 = (c >> 8) & 0xff;
            uint8_t blue1 = c & 0xff;
            if (bottom_divisor != 0) {
                red1 /= bottom_divisor;
                green1 /= bottom_divisor;
                blue1 /= bottom_divisor;
            }
            gl_draw_pixel(x_int, y, gl_color(red, green, blue));
            gl_draw_pixel(x_int + 1, y, gl_color(red1, green1, blue1));
        }
    }
}

float sqrt(int number) {
    int start = 0, end = number;
    int mid;

    // To store the answer
    float ans;

    // To find integral part of square
    // root of number
    while (start <= end) {

        // Find mid
        mid = (start + end) / 2;

        // If number is perfect square
        // then break
        if (mid * mid == number) {
            ans = mid;
            break;
        }

        // Increment start if integral
        // part lies on right side
        // of the mid
        if (mid * mid < number) {
          //first start value should be added to answer
            ans=start;
          //then start should be changed
            start = mid + 1;
        }

        // Decrement end if integral part
        // lies on the left side of the mid
        else {
            end = mid - 1;
        }
    }

    // To find the fractional part
    // of square root upto 5 decimal
    float increment = 0.1;
    for (int i = 0; i < 5; i++) {
        while (ans * ans <= number) {
            ans += increment;
        }

        // Loop terminates,
        // when ans * ans > number
        ans = ans - increment;
        increment = increment / 10;
    }
    return ans;

}

// MIT CSAIL
void gl_draw_circle(int xCenter, int yCenter, int radius, color_t c) {
    int y;
    int r2 = radius * radius;

    for (int x = -radius; x <= radius; x++) {
        y = (int) sqrt(r2 - (x * x)) + 0.5;
        gl_draw_pixel(xCenter + x, yCenter + y, c);
        gl_draw_pixel(xCenter + x, yCenter - y, c);

    }
}


// This function interpolates an x value given 2 points, and a y value in between.
float interpolate_x(int x1, int x2, int y1, int y2, int y) {
    float n = (((float)(x2 - x1) / (y2 - y1)) * (y - y1)) + x1;
    return n;
}

// This function draws a triangle using a series of horizontal lines.
void gl_draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t c) {

    int tempy = 0;
    int tempx = 0;
    if (y1 < y0) {
        tempy = y0;
        tempx = x0;
        y0 = y1;
        x0 = x1;
        y1 = tempy;
        x1 = tempx;
    }
    if (y2 < y0) {
        tempy = y0;
        tempx = x0;
        y0 = y2;
        x0 = x2;
        y2 = tempy;
        x2 = tempx;
    }
    if (y2 < y1) {
        tempy = y1;
        tempx = x1;
        y1 = y2;
        x1 = x2;
        y2 = tempy;
        x2 = tempx;
    }
    
    float p01[y1 - y0];
    float p12[y2 - y1];
    float p02[y2 - y0];

    for (int i = y0; i < y1; i++) {
        p01[i - y0] = interpolate_x(x0, x1, y0, y1, i);
    }
    
    for (int i = y1; i < y2; i++) {
        p12[i - y1] = interpolate_x(x1, x2, y1, y2, i);
    }
    
    for (int i = y0; i < y2; i++) {
        p02[i - y0] = interpolate_x(x0, x2, y0, y2, i);
    }

    // add 0 to 1 then 1 to 2 together, minus 1 val
    int p012[y2 - y0];
    int size = y1 - y0;
    for (int i = 0; i < size; i++) {
        p012[i] = p01[i];
    }

    int size2 = y2 - y1;
    for (int i = 0; i < size2; i++) {
        p012[i + size] = p12[i];
    }
    
    for (int i = y0; i < y2; i++) {
        gl_draw_line(p012[i - y0], i, p02[i - y0], i, c);
    }
}

// Return font glyph height.
int gl_get_char_height(void) {
    return font_get_glyph_height();
}

// Return font char height.
int gl_get_char_width(void) {
    return font_get_glyph_width();
}
