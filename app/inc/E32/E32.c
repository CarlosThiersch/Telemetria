#include <E32.h>


void E32_init(){
    E32_uart_setup();
    E32_set_mode(OPEN);
}

bool E32_configure(){
    E32_set_mode(SLEEP);
    system_delay(100);

    Config configuration = {
        .head = 0xC0,
        .addr_h = 0x00,
        .addr_l = 0x00,
        .speed = {
            .datarate = 0b101,
            .baudrate = 0b011,
            .parity = 0b00
        },
        .channel = 0x35,
        .opt = {
            .power = 0b00,
            .fec = 0b0,
            .wakeupTime = 0b000,
            .driveMode = 0b1,
            .modbus = 0b0
        }
    };

    E32_uart_write ((uint8_t *)&configuration, sizeof(Config));
    system_delay(300);

    uint8_t read_command[3] = {0xC1, 0xC1, 0xC1};

    E32_uart_write(read_command, 3);
    while(!E32_uart_data_available()){}
    
    uint8_t config[6];
    E32_uart_read(config, sizeof(config));

    E32_set_mode(OPEN);
    system_delay(100);
    E32_uart_change_baud(9600);

    return compare_configuration(config, configuration);
}

bool compare_configuration(uint8_t *c1, Config c2){
    return (
        c1[0] == (uint8_t)c2.head &&
        c1[1] == (uint8_t)c2.addr_h && 
        c1[2] == (uint8_t)c2.addr_l &&
        c1[3] == (uint8_t)((uint8_t)c2.speed.parity << 6 | (uint8_t)c2.speed.baudrate << 3 | (uint8_t)c2.speed.datarate) &&
        c1[4] == (uint8_t)c2.channel &&
        c1[5] == (uint8_t)((uint8_t)c2.opt.modbus << 7 | (uint8_t)c2.opt.driveMode << 6 | (uint8_t)c2.opt.wakeupTime << 3 | (uint8_t)c2.opt.fec << 2 | (uint8_t)c2.opt.power) 
    );
}

void E32_set_mode(uint8_t mode){
    if (mode == OPEN) gpio_clear(MODE_PORT, M0_PIN | M1_PIN);

    if (mode == SLEEP) gpio_set(MODE_PORT, M0_PIN | M1_PIN);
}

void E32_enable_irq(){
    rcc_periph_clock_enable(RCC_SYSCFG);
    nvic_enable_irq(NVIC_EXTI15_10_IRQ);
    exti_select_source(EXTI11, GPIOC);
    exti_set_trigger(EXTI11, EXTI_TRIGGER_RISING);
    exti_enable_request(EXTI11);
}
