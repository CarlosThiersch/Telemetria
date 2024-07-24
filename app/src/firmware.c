#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>

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
    
    while (1){
        if (get_ticks() - start_time >= 100){
            start_time = get_ticks();
            gpio_toggle(LED_PORT, LED_PIN);
        }
    }

    return 0;
}