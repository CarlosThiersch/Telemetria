#include <common_defines.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define MPU_DEVICE_ADDR         0b1101000
#define MPU_WHO_AM_I_REGISTER   0b1110101
#define MPU_CONFIG_REGISTER     0b0011010
#define MPU_PWR_MGMT_1_REGISTER 0b1101011
#define MPU_ACCEL_X_REGISTER    0b0111011

typedef struct {
    uint16_t a_x;
    uint16_t a_y;
    uint16_t a_z;
    uint16_t w_x;
    uint16_t w_y;
    uint16_t w_z;
} mpu_raw_data;

typedef struct {
    float a_x;
    float a_y;
    float a_z;
    float w_x;
    float w_y;
    float w_z;
} mpu_data;


void MPU_i2c_setup(); 

bool MPU_check_addr();

bool MPU_init();

mpu_raw_data MPU_get_raw_data();

mpu_data MPU_get_data();