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
	int *list;
	int num_filled;
	int min_score;
	int min_index;
} top_scores;

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

void update_screen(int time_init) {
        draw_background();
        draw_train_slats();
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
      
        timer_delay_ms(100);
        draw_background();
        draw_train_slats_2();
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

        timer_delay_ms(100);
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

	top_scores.list = (int *)malloc(10 * sizeof(int));
	for (int i = 0; i < 10; i++) {
		top_scores.list[i] = -1;
	}
	top_scores.min_score = 0;
	top_scores.num_filled = 0;
	top_scores.min_index = 0;
}

void character_select(void) {



}

void top_scores_screen(void) {




}

void main_menu(void) {
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
    surfer.skin = LUIGI;

    /*
    draw_acknowledgements();
    gl_swap_buffer();
    draw_loading_screen();
    gl_swap_buffer();
	main_menu();
    blinking_start_screen();
    game_countdown();
    */



    //hstimer_enable(HSTIMER0);
	//hstimer_enable(HSTIMER1);
    
    int time_init = get_secs();
    while (surfer.alive) {
        update_screen(time_init);
        check_if_dead();
    }
    
    draw_endscreen();
    gl_swap_buffer();
	if (top_scores.list[top_scores.num_filled] == -1) {
		if (time_init > top_scores.min_score) {
			top_scores.list[top_scores.num_filled] = time_init;
			top_scores.num_filled++;
		}		
	} else {
				


	}
    while(1) {}
}
