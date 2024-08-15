#include <common_defines.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

void E32_uart_setup();

void E32_uart_change_baud(uint32_t baud);

void E32_uart_write(uint8_t *data, uint32_t length);

void E32_uart_write_byte(uint8_t data);

uint32_t E32_uart_read(uint8_t *r_data, const uint32_t length);

uint8_t E32_uart_read_byte();

bool E32_uart_data_available();