#include <MPU6050/MPU6050.h>
#include <BMP280/BMP280.h>
#include <E32/E32.h>
#include <system.h>

union {
        float temp;
        uint8_t b_temp[4];        
    } u_temp;

    union {
        float pres;
        uint8_t b_pres[4];        
    } u_pres;

    union {
        float a_x;
        uint8_t b_a_x[4];
    } u_a_x;

    union {
        float a_y;
        uint8_t b_a_y[4];
    } u_a_y;

    union {
        float a_z;
        uint8_t b_a_z[4];
    } u_a_z;

    union {
        float w_x;
        uint8_t b_w_x[4];
    } u_w_x;

    union {
        float w_y;
        uint8_t b_w_y[4];
    } u_w_y;

    union {
        float w_z;
        uint8_t b_w_z[4];
    } u_w_z;

    mpu_data imu_data;

int main(){

    rcc_setup();
    gpio_setup();
    systick_setup();

    if(BMP_init()){
        gpio_set(LED_PORT, LED_PIN);
    } else {
        gpio_clear(LED_PORT, LED_PIN);     
    }

    if(MPU_init()){
        gpio_set(LED_PORT, LED_PIN);
    } else {
        gpio_clear(LED_PORT, LED_PIN);
    }

    system_delay(1000);

    E32_init();
    system_delay(300);

    while(!E32_configure());
    system_delay(300);

    E32_enable_irq();
    system_delay(100);

    u_temp.temp = BMP_get_temperature();
    u_pres.pres = BMP_get_pressure();
    imu_data = MPU_get_data();
    u_a_x.a_x = imu_data.a_x;
    u_a_y.a_y = imu_data.a_y;
    u_a_z.a_z = imu_data.a_z;
    u_w_x.w_x = imu_data.w_x;
    u_w_y.w_y = imu_data.w_y;
    u_w_z.w_z = imu_data.w_z;
    E32_uart_write(u_temp.b_temp, 4);
    E32_uart_write(u_pres.b_pres, 4);
    E32_uart_write(u_a_x.b_a_x, 4);
    E32_uart_write(u_a_y.b_a_y, 4);
    E32_uart_write(u_a_z.b_a_z, 4);
    E32_uart_write(u_w_x.b_w_x, 4);
    E32_uart_write(u_w_y.b_w_y, 4);
    E32_uart_write(u_w_z.b_w_z, 4);

    for(int i = 0; i < 100000000; i++){
        __asm__ volatile("nop");
    }

    while (1){
            u_temp.temp = BMP_get_temperature();
            u_pres.pres = BMP_get_pressure();
            imu_data = MPU_get_data();
            u_a_x.a_x = imu_data.a_x;
            u_a_y.a_y = imu_data.a_y;
            u_a_z.a_z = imu_data.a_z;
            u_w_x.w_x = imu_data.w_x;
            u_w_y.w_y = imu_data.w_y;
            u_w_z.w_z = imu_data.w_z;
            system_delay(10);
    }

    return 0;
}

//Interrupção do pino AUX do transmissor
void exti15_10_isr(){
    if(exti_get_flag_status(EXTI11)){
        exti_reset_request(EXTI11);

        E32_uart_write(u_temp.b_temp, 4);
        E32_uart_write(u_pres.b_pres, 4);
        E32_uart_write(u_a_x.b_a_x, 4);
        E32_uart_write(u_a_y.b_a_y, 4);
        E32_uart_write(u_a_z.b_a_z, 4);
        E32_uart_write(u_w_x.b_w_x, 4);
        E32_uart_write(u_w_y.b_w_y, 4);
        E32_uart_write(u_w_z.b_w_z, 4);
        gpio_toggle(LED_PORT, LED_PIN);
    }
}