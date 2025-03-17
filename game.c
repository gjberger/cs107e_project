#include "uart.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "i2c.h"
#include "mpu6050.h"
#include "game_graphics.h"
#include "interrupts.h"
#include "hstimer.h"
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

static struct {
    bool on;
    int x;
    int y;
} left_block;

static struct {
    bool on;
    int x;
    int y;
} middle_block;

static struct {
    bool on;
    int x;
    int y;
} right_block;

void handle_board(void *dev) {
    uart_putstring("board\n");
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

void handle_barriers(void *dev) {
	hstimer_interrupt_clear(HSTIMER1);
    uart_putstring("barrier\n");
    int random = rand(6) + 1;
    if (random == 1) {
        left_block.on = false;
        middle_block.on = false;
        right_block.on = true;
    } else if (random == 2) {
        left_block.on = false;
        middle_block.on = true;
        right_block.on = false;
    } else if (random == 3) {
        left_block.on = false;
        middle_block.on = true;
        right_block.on = true;
    } else if (random == 4) {
        left_block.on = true;
        middle_block.on = false;
        right_block.on = false;
    } else if (random == 5) {
        left_block.on = true;
        middle_block.on = false;
        right_block.on = true;
    } else if (random == 6) {
        left_block.on = true;
        middle_block.on = true;
        right_block.on = false;
    }
}

void set_up_timer_interrupts(void) {
    printf("\nset up timer 1 interrupts");
	i2c_device_t *dev = mpu_init();
	config_mpu(dev);
	// sampling at 200 Hz, or every 5ms
	// SAMPLE_RATE defined in mpu6050.h
	hstimer_init(HSTIMER0, SAMPLE_RATE * 1000000);
	interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER0, handle_board, dev);
	interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER0);
}

void set_up_timer2_interrupts(void) {
    printf("\nset up timer 2 interrupts");
    hstimer_init(HSTIMER1, 3000000);
    interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER1, handle_barriers, NULL);
    interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER1);
}

void main(void) {
    uart_init();
	interrupts_init();
	timer_init();
	gpio_init();

    printf("\nTest");
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

	set_up_timer_interrupts();
	set_up_timer2_interrupts();
	interrupts_global_enable();

	hstimer_enable(HSTIMER0);
	hstimer_enable(HSTIMER1);

    bool left_barrier = false;
    bool middle_barrier = false;
    bool right_barrier = false;

    surfer.pos = RIGHT;
    surfer.alive = true;
	surfer.seen_zero = true;

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
    
        /*
        printf("block 1 on: %d\n", (int)left_block.on);
        printf("block 2 on: %d\n", (int)middle_block.on);
        printf("block 3 on: %d\n", (int)right_block.on);
        */
        // will plan to implement model view controller
        // will update player position, 3 block position, etc
        // then will redraw all to screen in order
        // background
        // person
        // blocks
        // score?
    }
}
