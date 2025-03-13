#include "assert.h"
#include "i2c.h"
#include "mpu6050.h"
#include "malloc.h"
#include "math_float.h"

#define MPU_RATE 0x07
#define GYRO_SENSIT 0x00
#define ACCEL_SENSIT 0x00
#define DLPF 0x00
#define WAKE_UP 0x00

#define SAMPLE_RATE 0.001
#define ALPHA 0.02
#define GYRO_LSB 131.0
#define THRESHOLD_ANGLE 9

i2c_device_t *mpu_init(void) {
	i2c_init();
	return i2c_new(0x68);
}

static void config_pwr(i2c_device_t *dev) {
	i2c_write_reg(dev, 0x6b, WAKE_UP);	
}

static void config_rate_divide(i2c_device_t *dev) {
	i2c_write_reg(dev, 0x19, MPU_RATE);
}

static void config_dlpf(i2c_device_t *dev) {
	i2c_write_reg(dev, 0x1a, DLPF);	
}

static void config_gyro_sensit(i2c_device_t *dev) {
	i2c_write_reg(dev, 0x1b, GYRO_SENSIT);	
}

static void config_accel_sensit(i2c_device_t *dev) {
	i2c_write_reg(dev, 0x1c, ACCEL_SENSIT);
}

void config_mpu(i2c_device_t *dev) {
	assert(dev);
	config_pwr(dev);
	config_rate_divide(dev);
	config_dlpf(dev);
	config_gyro_sensit(dev);
	config_accel_sensit(dev);
}

int16_t read_accel_x(i2c_device_t *dev) {
	int16_t data_x = (i2c_read_reg(dev, 0x3b) << 8) | i2c_read_reg(dev, 0x3c);
	return data_x;
}

int16_t read_accel_y(i2c_device_t *dev) {
	int16_t data_y = (i2c_read_reg(dev, 0x3d) << 8) | i2c_read_reg(dev, 0x3e);
	return data_y;
}

int16_t read_accel_z(i2c_device_t *dev) {
	int16_t data_z = (i2c_read_reg(dev, 0x3f) << 8) | i2c_read_reg(dev, 0x40);
	return data_z;
}

int16_t *read_accel_data(i2c_device_t *dev) {
	int16_t *ptr = malloc(3 * sizeof(int16_t));
	ptr[0] = read_accel_x(dev);
	ptr[1] = read_accel_y(dev);
	ptr[2] = read_accel_z(dev);
	return ptr;
}

int16_t read_gyro_x(i2c_device_t *dev) {
	int16_t data_x = (i2c_read_reg(dev, 0x43) << 8) | i2c_read_reg(dev, 0x44);
	return data_x;
}

int16_t read_gyro_y(i2c_device_t *dev) {
	int16_t data_y = (i2c_read_reg(dev, 0x45) << 8) | i2c_read_reg(dev, 0x46);
	return data_y;
}

int16_t read_gyro_z(i2c_device_t *dev) {
	int16_t data_z = (i2c_read_reg(dev, 0x47) << 8) | i2c_read_reg(dev, 0x48);
	return data_z;
}

int16_t *read_gyro_data(i2c_device_t *dev) {
	int16_t *ptr = malloc(3 * sizeof(int16_t));
	ptr[0] = read_gyro_x(dev);
	ptr[1] = read_gyro_y(dev);
	ptr[2] = read_gyro_z(dev);
	return ptr;
}

// maybe need some kind of calibration function for when it starts up


// get the angle of the chip as calc by the accelerometer
float accel_angle(i2c_device_t *dev) {
	int16_t ax = read_accel_x(dev);
	int16_t az = read_accel_z(dev);

	float accel_angle = atan2(ax, az) * 57.2957795;
	return accel_angle;

}

//get the change in the angle of the chip as calc by the gyroscope
float gyro_angle(i2c_device_t *dev) {
	int16_t raw_angle = read_gyro_y(dev);
	float real_angle = raw_angle / GYRO_LSB;
	float gyro_angle = real_angle * SAMPLE_RATE;
	return gyro_angle;
}

// use alpha to combine the measurements, see top for ALPHA, will need to test
float get_tilt_angle(i2c_device_t *dev) {
	static float cur_angle = 0;

	float accel_ang = accel_angle(dev);
	float gyro_ang = gyro_angle(dev);

	cur_angle = (ALPHA * accel_ang) + ((1 - ALPHA) * (cur_angle + gyro_ang));
	return cur_angle;

}

// when upside down, do THRESHOLD_ANGLE + 180
position_t get_cur_position(i2c_device_t *dev) {
	static position_t cur_pos = CENTER;

	float cur_angle = get_tilt_angle(dev);

	if ((cur_angle > (THRESHOLD_ANGLE)) && (cur_pos == CENTER)) {
		cur_pos = LEFT;
	} else if ((cur_angle < (-THRESHOLD_ANGLE)) && (cur_pos == CENTER)) {
		cur_pos = RIGHT;
	} else if ((cur_angle > (-THRESHOLD_ANGLE)) && (cur_angle < (THRESHOLD_ANGLE)) && (cur_pos != CENTER)) {
		cur_pos = CENTER;
	}

	return cur_pos;

}	

