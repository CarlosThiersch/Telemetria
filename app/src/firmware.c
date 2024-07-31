#include <libopencm3/stm32/gpio.h>
#include <BMP280/BMP280.h>
#include <E32/E32.h>
#include <system.h>

int main(){

    rcc_setup();
    gpio_setup();
    systick_setup();

    if(BMP_init()){
        gpio_set(LED_PORT, LED_PIN);
    } else {
        gpio_clear(LED_PORT, LED_PIN);     
    }

    system_delay(1000);

    E32_init();
    system_delay(300);

    while(!E32_configure());
    system_delay(300);

    union {
        float temp;
        uint8_t b_temp[4];        
    } u_temp;

    union {
        float pres;
        uint8_t b_pres[4];        
    } u_pres;

    while (1){

            u_temp.temp = BMP_get_temperature();
            u_pres.pres = BMP_get_pressure();
            E32_uart_write(u_temp.b_temp, 4);
            E32_uart_write(u_pres.b_pres, 4);
            gpio_toggle(LED_PORT, LED_PIN);
            system_delay(1000);
    }

    return 0;
}