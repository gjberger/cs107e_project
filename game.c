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
#define SELECTOR GPIO_PB1
#define CONFIRM GPIO_PB2

static int cur_menu_item = 0;
static unsigned long button_debounce_confirm = 0;
static unsigned long button_debounce_selector = 0;

static struct {
    position_t pos;
    int x;
    int y;
    int head;
    bool seen_zero;
    bool alive;
    skin_t skin;
} surfer;

static struct {
    bool on;
    int x;
    int y;
    barrier_t barrier;
} left_block;

static struct {
    bool on;
    int x;
    int y;
    barrier_t barrier;
} middle_block;

static struct {
    bool on;
    int x;
    int y;
    barrier_t barrier;
} right_block;

void handle_board(void *dev) {
	hstimer_interrupt_clear(HSTIMER0);

	i2c_device_t *dev1 = (i2c_device_t *) dev;
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

    int random_barrier_l = rand(3) + 1;
    int random_barrier_m = rand(3) + 1;
    int random_barrier_r = rand(3) + 1;

    left_block.barrier = random_barrier_l;
    if (left_block.barrier == BLOCK) {
        left_block.x = LANE1 + 15;
    } else if (left_block.barrier == BEE) {
        left_block.x = LANE1 + 35;
    } else if (left_block.barrier == FLY) {
        left_block.x = LANE1 + 35;
    }
    left_block.y = 0;
    middle_block.barrier = random_barrier_m;
    if (middle_block.barrier == BLOCK) {
        middle_block.x = LANE2 - 20;
    } else if (middle_block.barrier == BEE) {
        middle_block.x = LANE2;
    } else if (middle_block.barrier == FLY) {
        middle_block.x = LANE2 - 2;
    }
    middle_block.y = 0;
    right_block.barrier = random_barrier_r;
    
    if (right_block.barrier == BLOCK) {
        right_block.x = LANE3 - 50;
    } else if (right_block.barrier == BEE) {
        right_block.x = LANE3 - 45;
    } else if (right_block.barrier == FLY) {
        right_block.x = LANE3 - 45;
    }
    right_block.y = 0;
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
            draw_barrier(left_block.x, left_block.y, left_block.barrier);
        }
        if (middle_block.on) {
            draw_barrier(middle_block.x, middle_block.y, middle_block.barrier);
        }
        if (right_block.on) {
            draw_barrier(right_block.x, right_block.y, right_block.barrier);
        }
        character_pose_1(surfer.pos, STEVE);
        gl_swap_buffer();
       
        draw_background();
        draw_score(time_init);
        if (left_block.on) {
            draw_barrier(left_block.x, left_block.y, left_block.barrier);
        }
        if (middle_block.on) {
            draw_barrier(middle_block.x, middle_block.y, middle_block.barrier);
        }
        if (right_block.on) {
            draw_barrier(right_block.x, right_block.y, right_block.barrier);
        }
        character_pose_2(surfer.pos, STEVE);
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
            if ((0.68 * HEIGHT + left_block.y >= (0.79 * HEIGHT)) && (0.68 * HEIGHT + left_block.y <= (0.95 * HEIGHT))) {
                surfer.alive = false;
            }
        }
    } else if (surfer.pos == CENTER) {
        if (middle_block.on) {
            if ((0.68 * HEIGHT + middle_block.y >= (0.79 * HEIGHT)) && (0.68 * HEIGHT + middle_block.y <= (0.95 * HEIGHT))) {
                surfer.alive = false;
            }
        }
    }
    else if (surfer.pos == RIGHT) {
        if (right_block.on) {
            if ((0.68 * HEIGHT + right_block.y >= (0.79 * HEIGHT)) && (0.68 * HEIGHT + right_block.y <= (0.95 * HEIGHT))) {
                surfer.alive = false;
            }
        }
    }
}

void init_game_data(void) {
    surfer.pos = CENTER;
    surfer.alive = true;
	surfer.seen_zero = true;
    left_block.on = false;
    middle_block.on = false;
    right_block.on = false;
}

void draw_mario(void) {
    // hat
    gl_draw_rect(LANE2 - 21, 0.87 * HEIGHT - 5, 22, 5, 0xc81a27);

    // head and hair
    gl_draw_rect(LANE2 - 20, 0.87 * HEIGHT, 20, 20, 0xe2ab7d);
    gl_draw_rect(LANE2 - 20, 0.87 * HEIGHT, 20, 16, 0x7e3f14);
    // ears
    gl_draw_rect(LANE2 - 22, 0.87 * HEIGHT + 5, 2, 6, 0xe2ab7d);
    gl_draw_rect(LANE2, 0.87 * HEIGHT + 5, 2, 6, 0xe2ab7d);

    // body
    gl_draw_rect(LANE2 - 18, 0.87 * HEIGHT + 20, 16, 25, 0x1f22b4);
    gl_draw_rect(LANE2 - 13, 0.87 * HEIGHT + 20, 6, 6, 0xc81a27);
    
    gl_draw_rect(LANE2 - 18, 0.87 * HEIGHT + 20, 3, 3, 0xc81a27);
    gl_draw_rect(LANE2 - 5, 0.87 * HEIGHT + 20, 3, 3, 0xc81a27);

    // shoes
    gl_draw_rect(LANE2 - 18, 0.87 * HEIGHT + 45, 6, 4, GL_BLACK);
    gl_draw_rect(LANE2 - 8, 0.87 * HEIGHT + 45, 6, 4, GL_BLACK);


    // arms
    // left
    gl_draw_rect(LANE2 - 21, 0.87 * HEIGHT + 22, 4, 4, 0xc81a27);
    gl_draw_rect(LANE2 - 22, 0.87 * HEIGHT + 25, 4, 4, 0xc81a27);
    gl_draw_rect(LANE2 - 23, 0.87 * HEIGHT + 28, 4, 4, 0xc81a27);

    // right
    gl_draw_rect(LANE2 - 3, 0.87 * HEIGHT + 22, 4, 4, 0xc81a27);
    gl_draw_rect(LANE2 - 2, 0.87 * HEIGHT + 25, 4, 4, 0xc81a27);
    gl_draw_rect(LANE2 - 1, 0.87 * HEIGHT + 28, 4, 4, 0xc81a27);
    
    // hands
    gl_draw_rect(LANE2 - 23, 0.87 * HEIGHT + 31, 4, 4, 0xe2ab7d);
    gl_draw_rect(LANE2 - 1, 0.87 * HEIGHT + 31, 4, 4, 0xe2ab7d);

}

void main(void) {
    uart_init();
	interrupts_init();
	timer_init();

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

	set_up_timer_interrupts();
	set_up_timer2_interrupts();
	interrupts_global_enable();

    gl_clear(GL_WHITE);
    draw_mario();
    gl_swap_buffer();
    while (1) {}


    init_game_data();
/*
	draw_menu(cur_menu_item);
	while(1) {
		// not going to use interrupts for this, cause I think will need different functionality in different
		// places
		// also will prob put this in function later
		if ((timer_get_ticks() - button_debounce_confirm) > 2000) {
			button_debounce_confirm = timer_get_ticks();
			if ((gpio_read(CONFIRM) == 0) && (cur_menu_item == 0)) {
					// play the game
					break;
			} else if ((gpio_read(CONFIRM) == 0) && (cur_menu_item == 1)) {
					// character selec screen	

			} else if ((gpio_read(CONFIRM) == 0) && (cur_menu_item == 1)) {
					// Top Scores Screen
			}
		}

		if ((timer_get_ticks() - button_debounce_selector) > 2000) {
			button_debounce_selector = timer_get_ticks();
			if (gpio_read(SELECTOR) == 0) {
				cur_menu_item++;
				if (cur_menu_item > 2) {
					cur_menu_item = 0;
				}
				draw_menu(cur_menu_item);
			}
		}
	}
	*/
	
    /*
    draw_acknowledgements();
    gl_swap_buffer();
    draw_loading_screen();
    gl_swap_buffer();
    blinking_start_screen();
    game_countdown();
	*/
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
