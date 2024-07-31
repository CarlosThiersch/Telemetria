#include <UART/uart.h>
#include <UART/ring_buffer.h>

#define BUFFER_SIZE 128

static ring_buffer_t rb = {0U};
static uint8_t buffer[BUFFER_SIZE] = {0U};

void E32_uart_setup(){
    ring_buffer_setup(&rb, buffer, BUFFER_SIZE);
    rcc_periph_clock_enable(RCC_UART5);

    usart_set_mode(UART5, USART_MODE_TX_RX);
    usart_set_flow_control(UART5, USART_FLOWCONTROL_NONE);
    usart_set_databits(UART5, 8);
    usart_set_baudrate(UART5, 9600);
    usart_set_parity(UART5, 0);
    usart_set_stopbits(UART5, 1);

    usart_enable_rx_interrupt(UART5);
    nvic_enable_irq(NVIC_UART5_IRQ);

    usart_enable(UART5);
}

void uart5_isr(){
    const bool overrun_occurred = usart_get_flag(UART5, USART_FLAG_ORE) == 1;
    const bool received_data = usart_get_flag(UART5, USART_FLAG_RXNE) == 1;

    if(received_data || overrun_occurred){
        ring_buffer_write(&rb, (uint8_t)usart_recv(UART5));
    }
}

void E32_uart_write(uint8_t *data, const uint32_t length){
    for (uint32_t i = 0; i < length; i++){
        E32_uart_write_byte(data[i]);
    }
}

void E32_uart_write_byte(uint8_t data){
    usart_send_blocking(UART5, (uint16_t)data);
}

uint32_t E32_uart_read(uint8_t *r_data, const uint32_t length){
    if(length == 0){
        return 0;
    }

    for(uint32_t i = 0; i < length; i++){
        if (!ring_buffer_read(&rb, &r_data[i])){
            return i; //Retorna o número de bytes lidos com sucesso
        }
    }

    return length;
}

uint8_t E32_uart_read_byte(){
    uint8_t byte = 0;

    E32_uart_read(&byte, 1);
    return byte;
}

bool E32_uart_data_available(){
    return !ring_buffer_empty(&rb);
}