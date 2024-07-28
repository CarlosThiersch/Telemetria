#include <BMP280.h>

BMP_calibration calibration = {
    0,0,0,0,0,0,0,0,0,0,0,0,0
};
int32_t fine_temp;

void BMP_i2c_setup(){
    rcc_periph_clock_enable(GPIOB);

    //Configuração dos pinos PB6 e PB7 para o i2c
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
    gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO6 | GPIO7);
    gpio_set_af(GPIOB, GPIO_AF4, GPIO6 | GPIO7);

    rcc_periph_clock_enable(RCC_I2C1);
    
    //Configura i2c para fast mode plus
    i2c_peripheral_disable(I2C1);
    i2c_set_speed(I2C1, i2c_speed_fmp_1m, rcc_apb1_frequency / 1e6);
    i2c_peripheral_enable(I2C1);
}

bool BMP_checkID(){
    uint8_t addr = BMP_CHIP_ID_REGISTER;
    uint8_t id;
    i2c_transfer7(I2C1, BMP_DEVICE_ADDR, &addr, sizeof(addr), &id, sizeof(id));
    return id == BMP_CHIP_ID;
}

BMP_calibration BMP_read_calibration(){
    uint8_t addr = BMP_CALIBRATION_REGISTERS_PREFIX;
    uint8_t data[24];
    BMP_calibration calib;

    i2c_transfer7(I2C1, BMP_DEVICE_ADDR, &addr, 1, data, 24);

    calib.T1 = ((data[1] << 8) | data[0]);
    calib.T2 = ((data[3] << 8) | data[2]);
    calib.T3 = ((data[5] << 8) | data[4]);
    calib.P1 = (data[7] << 8) | data[6];
    calib.P2 = (data[9] << 8) | data[8];
    calib.P3 = (data[11] << 8) | data[10];
    calib.P4 = (data[13] << 8) | data[12];
    calib.P5 = (data[15] << 8) | data[14];
    calib.P6 = (data[17] << 8) | data[16];
    calib.P7 = (data[19] << 8) | data[18];
    calib.P8 = (data[21] << 8) | data[20];
    calib.P9 = (data[23] << 8) | data[22];

    return calib;
}

bool BMP_init(){
    BMP_i2c_setup();

    bool check_id = BMP_checkID();
    calibration = BMP_read_calibration();

    uint8_t config_data[2] = {BMP_CONFIG_REGISTER, ((0b000 << 5) | (0b000 << 2) | 0b00)};
    i2c_transfer7(I2C1, BMP_DEVICE_ADDR, config_data, 2, NULL, 0);

    uint8_t control_data[2] = {BMP_CONTROL_REGISTER, ((0b001 << 5) | (0b001 << 2) | 0b11)};
    uint8_t config;
    i2c_transfer7(I2C1, BMP_DEVICE_ADDR, control_data, 2, &config, 1);
    bool check_config = config == ((0b001 << 5) | (0b001 << 2) | 0b11);

    return check_id && check_config;
}

int32_t BMP_get_raw_pressure(){
    int32_t pressure = 0;
    uint8_t p_read[3];
    uint8_t addr = BMP_PRESSURE_REGISTER;
    i2c_transfer7(I2C1, BMP_DEVICE_ADDR, &addr, 1, p_read, 3);
    
    pressure = (p_read[0] << 16 | p_read[1] << 8 | p_read[2]) >> 4;
    return pressure;
}

float BMP_get_pressure(){
    //Função descrita no datasheet que usa os
    //paramêtros de calibração para corrigir a pressão
    float pressure = 0.0;
    int32_t raw_pres = BMP_get_raw_pressure();
    int64_t pres;
    int64_t var1, var2;

    var1 = ((int64_t)fine_temp) - 128000;
    var2 = var1 * var1 * (int64_t)calibration.P6;
    var2 = var2 + ((var1 * (int64_t)calibration.P5) << 17);
    var2 = var2 + (((int64_t)calibration.P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calibration.P3) >> 8) + ((var1 * (int64_t)calibration.P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calibration.P1) >> 33;

    if(var1 == 0) return 0;

    pres = 1048576 - raw_pres;
    pres = (((pres << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calibration.P9) * (pres >> 13)) >> 25;
    var2 = (((int64_t)calibration.P8) * pres) >> 19;
    pres = ((pres + var1 + var2) >> 8) + (((int64_t)calibration.P7) << 4);

    pressure = (float)pres/256;
    return pressure;
}

int32_t BMP_get_raw_temperature(){
    int32_t temperature = 0;
    uint8_t t_read[3];
    uint8_t addr = BMP_TEMPERATURE_REGISTER;
    i2c_transfer7(I2C1, BMP_DEVICE_ADDR, &addr, 1, t_read, 3);

    temperature = (t_read[0] << 16 | t_read[1] << 8 | t_read[2]) >> 4;
    return temperature;
}

float BMP_get_temperature(){
    //Função descrita no datasheet que usa 
    //os parâmetros de calibração para corrigir a temperatura
    float temperature = 0.0;
    int32_t raw_temp = BMP_get_raw_temperature();
    int32_t var1, var2;

    var1 = ((((raw_temp >> 3) - ((int32_t)calibration.T1 << 1))) * ((int32_t)calibration.T2)) >> 11;
    var2 = (((((raw_temp >> 4) - ((int32_t)calibration.T1)) * ((raw_temp >> 4) - ((int32_t)calibration.T1))) >> 12) *((int32_t)calibration.T3)) >> 14;
    fine_temp = var1 + var2;

    temperature = (((fine_temp*5) + 128) >> 8);
    temperature /= 100;
    return temperature;
}