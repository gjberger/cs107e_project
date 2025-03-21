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
#include "malloc.h"
#include "gpio.h"
#include "gpio_extra.h"

#define WIDTH 400
#define HEIGHT 600
#define LANE1 (WIDTH / 6)
#define LANE2 (WIDTH / 2)
#define LANE3 (5 * WIDTH / 6)
#define SELECTOR GPIO_PB4
#define CONFIRM GPIO_PB3

static void start_game();


static int cur_menu_item = 0;
static unsigned long button_debounce_confirm = 0;
static unsigned long button_debounce_selector = 0;
static int last_confirm_state = 0;
static int last_selector_state = 0;

static struct {
	int *list;
	int num_filled;
	int min_score_index;
} top_scores;

static struct {
    position_t pos;
    int x;
    int y;
    int head;
    bool seen_zero;
    bool alive;
    skin_t skin;
    int top_y;
    int bottom_y;
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

    int random_barrier_l = rand(4) + 1;
    int random_barrier_m = rand(4) + 1;
    int random_barrier_r = rand(4) + 1;

    left_block.barrier = random_barrier_l;
    if (left_block.barrier == BLOCK) {
        left_block.y = 0;
        left_block.x = LANE1 + 15;
    } else if (left_block.barrier == BEE) {
        left_block.y = 0;
        left_block.x = LANE1 + 35;
    } else if (left_block.barrier == FLY) {
        left_block.x = LANE1 + 35;
        left_block.y = 0;
    } else if (left_block.barrier == RED_GHOST) {
        left_block.x = LANE1 + 35;
        left_block.y = 0;
    }


    middle_block.barrier = random_barrier_m;
    if (middle_block.barrier == BLOCK) {
        middle_block.x = LANE2 - 20;
        middle_block.y = 0;
    } else if (middle_block.barrier == BEE) {
        middle_block.x = LANE2;
        middle_block.y = 0;
    } else if (middle_block.barrier == FLY) {
        middle_block.x = LANE2 - 2;
        middle_block.y = 0;
    } else if (middle_block.barrier == RED_GHOST) {
        middle_block.x = LANE2 - 2;
        middle_block.y = 0;
    }
    right_block.barrier = random_barrier_r;
    
    if (right_block.barrier == BLOCK) {
        right_block.x = LANE3 - 50;
        right_block.y = 0;
    } else if (right_block.barrier == BEE) {
        right_block.x = LANE3 - 45;
        right_block.y = 0;
    } else if (right_block.barrier == FLY) {
        right_block.x = LANE3 - 45;
        right_block.y = 0;
    } else if (right_block.barrier == RED_GHOST) {
        right_block.x = LANE3 - 45;
        right_block.y = 0;
    }
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
        // draw_train_slats();
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
        character_pose_1(surfer.pos, surfer.skin);
        gl_swap_buffer();
      
        draw_background();
        // draw_train_slats_2();
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
        character_pose_2(surfer.pos, surfer.skin);
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

static void init_once(void) {
	top_scores.list = (int *)malloc(10 * sizeof(int));
	for (int i = 0; i < 10; i++) {
		top_scores.list[i] = 5;
	}
	top_scores.num_filled = 10;
	top_scores.min_score_index = 0;

	gpio_set_input(CONFIRM);
	gpio_set_input(SELECTOR);
	gpio_set_pullup(CONFIRM);
	gpio_set_pullup(SELECTOR);
}

void init_game_data(void) {
    surfer.pos = CENTER;
    surfer.alive = true;
	surfer.seen_zero = true;
    left_block.on = false;
    middle_block.on = false;
    right_block.on = false;

}

void character_select(void) {



}

void top_scores_screen(void) {
	draw_top_scores(top_scores.list);
	while(1) {
		if ((timer_get_ticks() - button_debounce_confirm) > 500000) {
			button_debounce_confirm = timer_get_ticks();
			int state = gpio_read(CONFIRM);
			if (state == 0 && last_confirm_state == 1) {
				return;
			}
			last_confirm_state = state;
		}	
	}
}

void main_menu(void) {

	draw_menu(cur_menu_item);
	while(1) {
		if ((timer_get_ticks() - button_debounce_confirm) > 500000) {
			button_debounce_confirm = timer_get_ticks();
			int state1 = gpio_read(CONFIRM);
			if (state1 == 0 && last_confirm_state == 1 && (cur_menu_item == 0)) {
				// play the game
				break;
			} else if ((gpio_read(CONFIRM) == 0) && last_confirm_state == 1 && (cur_menu_item == 1)) {
				// character selec screen	

			} else if ((gpio_read(CONFIRM) == 0) && last_confirm_state == 1 && (cur_menu_item == 2)) {
				// Top Scores Screen
				top_scores_screen();
				last_confirm_state = 0;
			}
			last_confirm_state = state1;
		}

		if ((timer_get_ticks() - button_debounce_selector) > 500000) {
			int state2 = gpio_read(SELECTOR);
			button_debounce_selector = timer_get_ticks();
			if (state2 == 0 && last_selector_state == 1) {
				cur_menu_item = (cur_menu_item + 1) % 3;
				draw_menu(cur_menu_item);
			}

			last_selector_state = state2;
		}
	}
}

static void add_to_top_scores(int time_init) {
	if (top_scores.num_filled <= 9) {
		if (time_init > top_scores.list[top_scores.min_score_index]) {
			top_scores.list[top_scores.num_filled] = time_init;
			top_scores.num_filled++;
		} else {
			top_scores.min_score_index = top_scores.num_filled;
			top_scores.list[top_scores.num_filled] = time_init;	
			top_scores.num_filled++;
		}
	} else {
		if (time_init > top_scores.list[top_scores.min_score_index]) {
			top_scores.list[top_scores.min_score_index] = time_init;
		}
	}
}

static void dead_condition_reset(void) {
	hstimer_disable(HSTIMER0);
	hstimer_disable(HSTIMER0);
	left_block.on = false;
	middle_block.on = false;
	right_block.on = false;

	while(1) {
		if ((timer_get_ticks() - button_debounce_confirm) > 500000) {
			int state = gpio_read(CONFIRM);
			if (state == 0 && last_confirm_state == 1) {
					button_debounce_confirm = timer_get_ticks();
					last_confirm_state = 0;
					start_game();
			}
			last_confirm_state = state;
		}
	}
}

static void start_game(void) {
	init_game_data();
    surfer.skin = STEVE;
	main_menu();
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
	add_to_top_scores(time_init);
	dead_condition_reset();

}

void set_surfer_bounds(void) {
    if (surfer.skin == STICK) {
        surfer.top_y = 0.81 * HEIGHT;
        surfer.bottom_y = 0.98 * HEIGHT;
    } else if (surfer.skin == STEVE) {
        surfer.top_y = 0.837 * HEIGHT;
        surfer.bottom_y = 0.974 * HEIGHT;
    } else if (surfer.skin == MARIO) {
        surfer.top_y = 0.845 * HEIGHT;
        surfer.bottom_y = 0.935 * HEIGHT;
    } else if (surfer.skin == LUIGI) {
        surfer.top_y = 0.845 * HEIGHT;
        surfer.bottom_y = 0.935 * HEIGHT;
    }
}

void main(void) {
    uart_init();
	interrupts_init();
	timer_init();

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

	set_up_timer_interrupts();
	set_up_timer2_interrupts();
	interrupts_global_enable();

    /*
    */
    //init_game_data();
    //surfer.skin = LUIGI;

    //draw_acknowledgements();
    //gl_swap_buffer();
    //draw_loading_screen();
    //gl_swap_buffer();

	init_once();
	start_game();
}
