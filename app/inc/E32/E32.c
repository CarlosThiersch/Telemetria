#include <E32.h>


//Debug, iterate and find a better value to ramp up speed:
#define PROCESS_TIME 500e3

static void clock_setup(void)
{
    /* Enable GPIOC and GPIOD clocks for UART5. */
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOD);
    rcc_periph_clock_enable(RCC_GPIOB);    
    /* Enable clock for UART5. */
    rcc_periph_clock_enable(RCC_UART5);    
}

static void gpio_setup(void)
{
    /* Setup GPIO pin C12 for UART5 TX. */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
    gpio_set_af(GPIOC, GPIO_AF8, GPIO12);

    /* Setup GPIO pin D2 for UART5 RX. */
    gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
    gpio_set_af(GPIOD, GPIO_AF8, GPIO2);

    gpio_mode_setup(GPIOB,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO3 | GPIO4);
    gpio_mode_setup(GPIOC,GPIO_MODE_INPUT,GPIO_PUPD_NONE,GPIO11);

}  

static void usart_setup(void)
{
    /* Setup UART5 parameters. */
    usart_set_baudrate(UART5, 9600);
    usart_set_databits(UART5, 8);
    usart_set_stopbits(UART5, USART_STOPBITS_1);
    usart_set_mode(UART5, USART_MODE_TX_RX);
    usart_set_parity(UART5, USART_PARITY_NONE);
    usart_set_flow_control(UART5, USART_FLOWCONTROL_NONE);

    /* Enable UART5. */
    usart_enable(UART5);
}

void usart_send_data(uint32_t usart, const uint8_t *data, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++) {
        /* Wait until the transmit data register is empty */
        //usart_wait_send_ready(usart);        
        /* Send data */
        usart_send_blocking(usart, data[i]);
    }    
}

void usart_recv_data(uint32_t usart, uint8_t *data, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++) {
        __asm__ volatile ("mov r9, %0" : : "r" (i));   
        //usart_wait_recv_ready(usart);
        data[i] = usart_recv_blocking(usart);
    }
}


void E32_init(){
    clock_setup();
    gpio_setup();
    gpio_set(GPIOB,GPIO3 | GPIO4);
    usart_setup();
    gpio_clear(GPIOB,GPIO3 | GPIO4);
    uint32_t help_var = 0x1;
    __asm__ volatile ("mov r9, %0" : : "r" (help_var));
}
void setRadio(){
    //uint8_t error;
    //OperatingModes previousMode = currentMode;
    //setMode(SLEEP);
    // _configure(TX_20dBm,T250ms,862e6,DATARATE_2400BPS,BAUD_9600,PARITY_8N1); //Default for now
    Config defaultConf;
    defaultConf.head = 0xc0;
    defaultConf.channel = freq2Hex(915e6);
    Speed defaultSpeed;
    
    defaultSpeed.baudrate = BAUD_9600;
    defaultSpeed.datarate = DATARATE_2400BPS;
    defaultSpeed.parity = PARITY_8N1;
    
    defaultConf.speed = defaultSpeed;
    Option defaultOpt;
    
    defaultOpt.power = TX_20dBm;
    defaultOpt.driveMode = PULL_UP;
    defaultOpt.fec = FEC_OFF;
    defaultOpt.wakeupTime = T250ms;
    defaultOpt.modbus = TRANSPARENT;

    defaultConf.opt = defaultOpt;
    uint32_t help_var = 0x2;
    __asm__ volatile ("mov r9, %0" : : "r" (help_var));
    configure(defaultConf);
    //setMode(previousMode);

}
uint8_t freq2Hex(int freq){
    uint8_t result = 0x00;
    if(freq > 1e3)
        freq /= 1e6;
    if(freq <= 931 && freq >= 862){
        freq = (freq - 862);
        result = (uint8_t)freq;
    }
    return result;
}

void sendOperatingParams(uint8_t cfgByte){
    uint8_t message[3] = {cfgByte,cfgByte,cfgByte};
    usart_send_data(UART5,message, sizeof(message));
}

void resetModule(){
    uint8_t message[3] = {C4,C4,C4};
    usart_send_data(UART5,(uint8_t*) message,sizeof(message)); //Test it out
}

void configure(Config config){
    gpio_set(GPIOB,GPIO3 | GPIO4);
    for(volatile int i =0;i<1000000;i++);
    uint32_t help_var = 0x3;
    __asm__ volatile ("mov r9, %0" : : "r" (help_var));
    usart_send_data(UART5,(uint8_t *)&config, sizeof(Config));
    for(volatile int i =0;i<1000000;i++);
    //usart_disable(USART5);
    help_var = 0x4;
    __asm__ volatile ("mov r9, %0" : : "r" (help_var));    
    uint8_t ch[6];
    uint8_t configByte = 0xC1;

    sendOperatingParams(configByte);
    for(volatile int i =0;i<1000000;i++);
    help_var = 0x5;
    __asm__ volatile ("mov r9, %0" : : "r" (help_var));    
    // usart_recv_data(UART5, ch, 6);
    // help_var = 0x6;
    // __asm__ volatile ("mov r9, %0" : : "r" (help_var));    
    // uint32_t temp1 = ((ch[2]<<16) | (ch[1]<<8) | ch[0]);
    // uint32_t temp2 = ((ch[5]<<16) | (ch[4]<<8) | ch[3]);
    // __asm__ volatile ("mov r9, %0" : : "r" (temp1));
    // __asm__ volatile ("mov r10, %0" : : "r" (temp2));
    gpio_clear(GPIOB,GPIO3 | GPIO4);
    //usart_set_baudrate(UART5, 9600);
    // usart_enable(UART5);
}
