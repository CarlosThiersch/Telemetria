#include <MPU6050/MPU6050.h>

void MPU_i2c_setup(){
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);

    //Configuração dos pinos PC9 e PA8 para o i2c
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
    gpio_set_output_options(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO9);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO8);
    gpio_set_af(GPIOC, GPIO_AF4, GPIO9);
    gpio_set_af(GPIOA, GPIO_AF4, GPIO8);

    rcc_periph_clock_enable(RCC_I2C3);

    //configura i2c para fast mode
    i2c_peripheral_disable(I2C3);
    i2c_set_speed(I2C3, i2c_speed_fm_400k, rcc_apb1_frequency / 1e6);
    i2c_peripheral_enable(I2C3);
}

bool MPU_check_addr(){
    uint8_t reg = MPU_WHO_AM_I_REGISTER;
    uint8_t addr;
    i2c_transfer7(I2C3, MPU_DEVICE_ADDR, &reg, 1, &addr, 1);

    return ((uint8_t)MPU_DEVICE_ADDR) == ((uint8_t)addr);
}

bool MPU_init(){
    MPU_i2c_setup();

    //Sai do modo sleep
    uint8_t pwr_mgmt_1[2] = {MPU_PWR_MGMT_1_REGISTER, 0b00001000};
    i2c_transfer7(I2C3, MPU_DEVICE_ADDR, &pwr_mgmt_1, 2, NULL, 0);

    system_delay(100);

    //Seleciona clock de giroscópio
    pwr_mgmt_1[1] = 0b00001001;
    i2c_transfer7(I2C3, MPU_DEVICE_ADDR, &pwr_mgmt_1, 2, NULL, 0);

    uint8_t config[2] = {MPU_CONFIG_REGISTER, 0b00000010};
    i2c_transfer7(I2C3, MPU_DEVICE_ADDR, config, 2, NULL, 0);

    bool check_addr = MPU_check_addr();
    return check_addr;
}

mpu_raw_data MPU_get_raw_data(){

    uint8_t base_reg = MPU_ACCEL_X_REGISTER;
    uint8_t buffer[14];
    i2c_transfer7(I2C3, MPU_DEVICE_ADDR, &base_reg, 1, buffer, 14);

    mpu_raw_data data = {
        .a_x = (buffer[0]  << 8) | buffer[1],
        .a_y = (buffer[2]  << 8) | buffer[3],
        .a_z = (buffer[4]  << 8) | buffer[5],
        .w_x = (buffer[8]  << 8) | buffer[9],
        .w_y = (buffer[10] << 8) | buffer[11],
        .w_z = (buffer[12] << 8) | buffer[13],
    };

    return data;
}

mpu_data MPU_get_data(){
    mpu_raw_data raw_data = MPU_get_raw_data();

    mpu_data data = {
        .a_x = (float)raw_data.a_x / (2.0 / 32768.0),
        .a_y = (float)raw_data.a_y / (2.0 / 32768.0),
        .a_z = (float)raw_data.a_z / (2.0 / 32768.0),
        .w_x = (float)raw_data.w_x / (250.0 / 32768.0),
        .w_y = (float)raw_data.w_y / (250.0 / 32768.0),
        .w_z = (float)raw_data.w_z / (250.0 / 32768.0)
    };
    return data;
}