#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>
#include <BMP280/BMP280.h>
#include <E32/E32.h>
#define CPU_FREQ 180000000
#define SYSTICK_FREQ 1000
#define LED_PORT GPIOB
#define LED_PIN GPIO15

volatile uint32_t ticks = 0;
void sys_tick_handler(){
    ticks++;
}

static uint32_t get_ticks(){
    return ticks;
}

static void rcc_setup(){
    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[3]);
}

static void gpio_setup(){
    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

static void systick_setup(){
    systick_set_frequency(SYSTICK_FREQ, CPU_FREQ);
    systick_counter_enable();
    systick_interrupt_enable();
}

int main(){

    rcc_setup();
    gpio_setup();
    systick_setup();
   
    uint32_t start_time = get_ticks();

    if(BMP_init()){
        gpio_set(LED_PORT, LED_PIN);
    } else {
        gpio_clear(LED_PORT, LED_PIN);
    }

    union {
        float temperature;
        uint32_t b_temperature;
    } u_temperature;

    union {
        float pressure;
        uint32_t b_pressure;
    } u_pressure;
    

    //E32_init();
    //setRadio();

    while (1){

            u_temperature.temperature = BMP_get_temperature();
            u_pressure.pressure = BMP_get_pressure();
            __asm__ volatile ("mov r9, %0" : : "r" (u_temperature.b_temperature));
            __asm__ volatile ("mov r10, %0" : : "r" (u_pressure.b_pressure));
            //usart_send_data(UART5,u_temperature.x_temperature,4);
            
            if(u_temperature.temperature > 10.0 && u_temperature.temperature < 40.0
                && u_pressure.pressure > 90000.0 && u_pressure.pressure < 100000.0){
                 gpio_clear(LED_PORT, LED_PIN);
             } else {
                 gpio_set(LED_PORT, LED_PIN);
            }
            
    }

    return 0;
}