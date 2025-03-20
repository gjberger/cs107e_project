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

    left_block.x = LANE1 + 15;
    left_block.y = 0.68 * HEIGHT;
    middle_block.x = LANE2 - 20;
    middle_block.y = 0.68 * HEIGHT;
    right_block.x = LANE3 - 50;
    right_block.y = 0.68 * HEIGHT;
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
    hstimer_init(HSTIMER1, 15000000);
    interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER1, handle_barriers, NULL);
    interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER1);
}


void update_screen(int time_init) {
        draw_background();
        draw_score(time_init);
        if (left_block.on) {
            draw_barrier(left_block.x, left_block.y);
        }
        if (middle_block.on) {
            draw_barrier(middle_block.x, middle_block.y);
        }
        if (right_block.on) {
            draw_barrier(right_block.x, right_block.y);
        }
        character_pose_1(surfer.pos);
        gl_swap_buffer();
        
        draw_background();
        draw_score(time_init);
        if (left_block.on) {
            draw_barrier(left_block.x, left_block.y);
        }
        if (middle_block.on) {
            draw_barrier(middle_block.x, middle_block.y);
        }
        if (right_block.on) {
            draw_barrier(right_block.x, right_block.y);
        }
        character_pose_2(surfer.pos);
        gl_swap_buffer();

        left_block.x = left_block.x - 2;
        left_block.y = left_block.y + 6;
        middle_block.y = middle_block.y + 6;
        right_block.x = right_block.x + 2;
        right_block.y = right_block.y + 6;
}

void check_if_dead(void) {
    if (surfer.pos == LEFT) {
        if (left_block.on) {
            if ((left_block.y >= (0.79 * HEIGHT)) && (left_block.y <= (0.95 * HEIGHT))) {
                surfer.alive = false;
            }
        }
    } else if (surfer.pos == CENTER) {
        if (middle_block.on) {
            if ((middle_block.y >= (0.79 * HEIGHT)) && (middle_block.y <= (0.95 * HEIGHT))) {
                surfer.alive = false;
            }
        }
    }
    else if (surfer.pos == RIGHT) {
        if (right_block.on) {
            if ((right_block.y >= (0.79 * HEIGHT)) && (right_block.y <= (0.95 * HEIGHT))) {
                surfer.alive = false;
            }
        }
    }
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

void init_game_data(void) {
    surfer.pos = CENTER;
    surfer.alive = true;
	surfer.seen_zero = true;
    left_block.on = false;
    middle_block.on = false;
    right_block.on = false;
}

void main(void) {
    uart_init();
	interrupts_init();
	timer_init();

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

	set_up_timer_interrupts();
	set_up_timer2_interrupts();
	interrupts_global_enable();


    init_game_data();
    draw_loading_screen();
    gl_swap_buffer();
    blinking_start_screen();
    game_countdown();
	
    hstimer_enable(HSTIMER0);
	hstimer_enable(HSTIMER1);
    
    int time_init = get_secs();
    while (surfer.alive) {
        update_screen(time_init);
        check_if_dead();
    }
    
    draw_endscreen();
    gl_swap_buffer();
    while(1) {}
}
