#ifndef _MPU_6050_H
#define _MPU_6050_H

typedef enum {
	LEFT = -1,
	CENTER = 0,
	RIGHT = 1,
} position_t;

//void say_hello(const char *name);

i2c_device_t *mpu_init(void);
void config_mpu(i2c_device_t *dev);

int16_t read_accel_x(i2c_device_t *dev);
int16_t read_accel_y(i2c_device_t *dev);
int16_t read_accel_z(i2c_device_t *dev);
int16_t *read_accel_data(i2c_device_t *dev);

int16_t read_gryo_x(i2c_device_t *dev);
int16_t read_gryo_y(i2c_device_t *dev);
int16_t read_gryo_z(i2c_device_t *dev);
int16_t *read_gyro_data(i2c_device_t *dev);

float accel_angle(i2c_device_t *dev);
float gyro_angle(i2c_device_t *dev);
float get_tilt_angle(i2c_device_t *dev);
position_t get_cur_position(i2c_device_t *dev);

#endif
