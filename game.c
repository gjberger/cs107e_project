#include "uart.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "i2c.h"
#include "mpu6050.h"
#include "game_graphics.h"
#include "interrupts.h"
#include "hstimer.h"
// Theres code to generate a random number for a specific range in random.c
#include "random.h"

#define WIDTH 400
#define HEIGHT 600
#define LANE1 (WIDTH / 6)
#define LANE2 (WIDTH / 2)
#define LANE3 (5 * WIDTH / 6)

static struct {
    position_t pos;
    int x;
    int y;
    int head;
    bool seen_zero;
    bool alive;
} surfer;

struct block {
    bool on;
    int x;
    int y;
};


void handle_board(void *dev) {
	hstimer_interrupt_clear(HSTIMER0);

	i2c_device_t *dev1 = (i2c_device_t *)dev;
	position_t pos = get_cur_position(dev1);

	if (pos == CENTER) {
		surfer.seen_zero = true;
		return;
	}
	if (!surfer.seen_zero) {
		return;
	}	
	int new_pos = pos + surfer.pos;
	if ((new_pos > 1) || (new_pos < -1)) {
		return;
	} else {
		surfer.pos = new_pos;
	}
	surfer.seen_zero = false;
}

void handle_barriers(void) {

}

void set_up_timer_interrupts(void) {
	i2c_device_t *dev = mpu_init();
	config_mpu(dev);
	// sampling at 200 Hz, or every 5ms
	// SAMPLE_RATE defined in mpu6050.h
	hstimer_init(HSTIMER0, SAMPLE_RATE * 1000000);
	interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER0, handle_board, dev);
	interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER0);
}

void set_up_timer2_interrupts(void) {
    hstimer_init(HSTIMER1, 3000000);
    interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER1, handle_board, NULL);
    interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER1);
}

void main(void) {
    uart_init();
    printf("Test");
    for (int i = 0; i < 10; i++) {
        printf("\nRandom number: %d", rand(7));
    }
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

	interrupts_init();
	set_up_timer_interrupts();
	set_up_timer2_interrupts();
	interrupts_global_enable();

    bool left_barrier = false;
    bool middle_barrier = false;
    bool right_barrier = false;

    surfer.pos = RIGHT;
    surfer.alive = true;
	surfer.seen_zero = true;

    struct block block1;
    struct block block2;
    struct block block3;

   
    if (surfer.pos == 0) {
        printf("Yes");
    }
    draw_background(0);
    gl_swap_buffer();
    draw_background(0);
    int time_init = get_secs();
    while (1) {
        // character 2 now showing
        character_animation(time_init, 0);
     
        // will plan to implement model view controller
        // will update player position, 3 block position, etc
        // then will redraw all to screen in order
        // background
        // person
        // blocks
        // score?
    }
}
