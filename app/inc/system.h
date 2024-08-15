#ifndef SYSTEM_H
#define SYSTEM_H
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/exti.h>

#define CPU_FREQ 180000000
#define SYSTICK_FREQ 1000
#define LED_PORT GPIOB
#define LED_PIN GPIO15

uint32_t get_ticks();

void system_delay(uint32_t mills);

void rcc_setup();

void gpio_setup();

void systick_setup();

#endif