#include "i2c.h"
#include "mpu6050.h"
#include "timer.h"
#include "printf.h"
#include <stdint.h>

static void print_accel_data(i2c_device_t *dev) {
	int16_t *data = read_accel_data(dev);	
	printf("x accel = %6d | y accel = %6d | z accel = %6d\n", data[0], data[1], data[2]);
	timer_delay(1);
}

static void print_gyro_data(i2c_device_t *dev) {
	int16_t *data = read_gyro_data(dev);	
	printf("x gyro = %6d | y gyro = %6d | z gyro = %6d\n", data[0], data[1], data[2]);
	timer_delay(1);
}

void main(void) {
	i2c_device_t *dev = mpu_init();
	config_mpu(dev);
	print_accel_data(dev);	
	//print_gryo_data(dev);

}
