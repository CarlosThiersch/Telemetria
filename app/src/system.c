#include <system.h>

volatile uint32_t ticks = 0;
void sys_tick_handler(){
    ticks++;
}

uint32_t get_ticks(){
    return ticks;
}

void system_delay(uint32_t mills){
    uint32_t end_time = get_ticks() + mills;
    while(get_ticks() < end_time){}
}

void rcc_setup(){
    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[3]);
}

void gpio_setup(){
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOD);

    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
    gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO11);
    
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
    gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
    gpio_set_af(GPIOC, GPIO_AF8, GPIO12);
    gpio_set_af(GPIOD, GPIO_AF8, GPIO2);
}

void systick_setup(){
    systick_set_frequency(SYSTICK_FREQ, CPU_FREQ);
    systick_counter_enable();
    systick_interrupt_enable();
}