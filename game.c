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
      
        for (int i = -1; i <= 1; i++) {
            for (int j = 0; j < 5; j++) {
                character_animation(time_init, i);
            }
            if (i == -1) {
                left_to_mid(time_init);
            } else if (i == 0) {
                mid_to_right(time_init);
            }
        }
        for (int i = 1; i >= -1; i--) {
            for (int j = 0; j < 5; j++) {
                character_animation(time_init, i);
            }
            if (i == 1) {
                right_to_mid(time_init);
            } else if (i == 0) {
                mid_to_left(time_init);
            }
        }
    }
}
