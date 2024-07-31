#include <common_defines.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define BMP_DEVICE_ADDR 0b1110111 
#define BMP_CHIP_ID 0b1011000 
#define BMP_CONTROL_REGISTER 0b11110100 
#define BMP_CONFIG_REGISTER 0b11110101 
#define BMP_CHIP_ID_REGISTER 0b11010000 
#define BMP_PRESSURE_REGISTER 0b11110111 
#define BMP_TEMPERATURE_REGISTER 0b11111010
#define BMP_CALIBRATION_REGISTERS_PREFIX 0b10001000

//Parâmetros calibrados durante a fabricação do sersor,
//para serem usados na formula de compensação
typedef struct {
    uint16_t  T1;
    int16_t   T2;
    int16_t   T3;
    uint16_t  P1;
    int16_t   P2;
    int16_t   P3;
    int16_t   P4;
    int16_t   P5;
    int16_t   P6;
    int16_t   P7;
    int16_t   P8;
    int16_t   P9;
} BMP_calibration;

void BMP_i2c_setup();
bool BMP_checkID();
BMP_calibration BMP_read_calibration();
bool BMP_init();

float BMP_get_pressure();
float BMP_get_temperature();
