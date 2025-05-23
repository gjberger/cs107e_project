/*
 * File: test_mpu.c
 *
 * File for testing the mpu-6050 module and some of the buttons
 *
 *
 * */

#include "i2c.h"
#include "mpu6050.h"
#include "timer.h"
#include "printf.h"
#include <stdint.h>
#include "uart.h"
#include "gpio.h"
#include "gpio_extra.h"

#define CONFIRM GPIO_PB3
#define SELECTOR GPIO_PB4
static struct {
    position_t pos;
    int x;
    int y;
    int head;
    bool seen_zero;
    bool alive;
} surfer;

// function to test the controller buttons
static void test_buttons(void) {
	gpio_set_input(CONFIRM);
	gpio_set_input(SELECTOR);
	gpio_set_pullup(CONFIRM);
	gpio_set_pullup(SELECTOR);
	printf("Confirm Button: %d\n", gpio_read(CONFIRM));
	printf("Select Button: %d\n", gpio_read(SELECTOR));
	timer_delay_ms(20);
}

// tests the handler function that is implemeted in game.c / ensures the logic is correct
static void test_handle_board(i2c_device_t *dev) {
	position_t pos = get_cur_position(dev);

	if (pos == CENTER) {
		surfer.seen_zero = true;
		printf("%d\n", surfer.pos);
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
	printf("%d\n", surfer.pos);
}

static void print_accel_data(i2c_device_t *dev) {
	int16_t *data = read_accel_data(dev);	
	printf("x accel = %6d | y accel = %6d | z accel = %6d\n", data[0], data[1], data[2]);
//	timer_delay(1);
}

static void print_gyro_data(i2c_device_t *dev) {
	int16_t *data = read_gyro_data(dev);	
	printf(" | x gyro = %6d | y gyro = %6d | z gyro = %6d\n", data[0], data[1], data[2]);
	//timer_delay(1);
}

static void test_position(i2c_device_t *dev) {
	position_t current = get_cur_position(dev);
	printf("%d\n", current);
}

static void test_angle_measure(i2c_device_t *dev) {
	float angle = get_tilt_angle(dev);
	printf("%d\n", (int)angle);
}

void main(void) {
	uart_init();
	gpio_init();
	i2c_device_t *dev = mpu_init();
	config_mpu(dev);
	surfer.seen_zero = true;
	surfer.pos = CENTER;
	while(1) {
		//print_accel_data(dev);	
		//print_gyro_data(dev);
		test_position(dev);
		//test_handle_board(dev);
		//1test_angle_measure(dev);
		timer_delay_ms(SAMPLE_RATE * 1000);
		//test_buttons();
	}

}
