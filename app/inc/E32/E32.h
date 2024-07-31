/* Basic Comms Pins
    M0:     pull-up - Working Mode bit 0
    M1:     pull-up - Working Mode bit 1
    RXD:    UART RXD, connects to TXD in microcontroller
    TXD:    UART TXD, connects to RXD in microcontroller
    AUX:    Indicates working status of the module
*/

/* Operating modes
    M0 | M1
    0    0  : UART channels open, transmission is ON
    0    1  : UART closed, wireless is in air-awaken status, if module receives data, UART is opened and data is sent
    1    0  : UART and wireless are open, increase wake up code automatically before transmitting
    1    1  : Sleep mode, programmable settings

    While on Mode (M0|M1) == 11:
        Parameter setting with 9600 baud and 8N1 bit parity, parameter format below:
    | Format 
    | C0 + 5  bytes of hex woking parameters | Sets working conditions
    | C1 + C1 + C1 | Save parameters at power-down, returns values configured (C0 + 5 bytes)
    | C2 + 3 bytes of hex working parameters | Sets working conditions (Do not save at power-down)
    | C3 + C3 + C3 | Returns the version information
    | C4 + C4 + C4 | In sleep mode (M0|M1) == 11 reset command

    Default values:
        | Frequency | Address | Channel | Datarate | Baudrate | Parity | TX Power |
        |   868MHz  |   0x0   |   0x6   | 2400bps  |   9600   |  8N1   |   20dBm  |

        c00000001a35c40
*/
#include <common_defines.h>
#include <libopencm3/stm32/gpio.h>
#include <UART/uart.h>

#define MODE_PORT GPIOB
#define M0_PIN GPIO4
#define M1_PIN GPIO3

typedef enum {
    C0 = 0xC0,  // Saves data on power-down
    C1 = 0xC1,
    C2 = 0xC2,  // Does NOT save data on power-down
    C3 = 0xC3,
    C4 = 0xC4
} CONFIG_BYTE;

enum UART_PARITY{
    PARITY_8N1=0b00,
    PARITY_8O1=0b01,
    PARITY_8E1=0b10
};

enum UART_BAUDRATE{
    BAUD_1200=0b000,
    BAUD_2400=0b001,
    BAUD_4800=0b010,
    BAUD_9600=0b011,
    BAUD_19200=0b100,
    BAUD_38400=0b101,
    BAUD_57600=0b110,
    BAUD_115200=0b111
};

enum DATARATE{
    DATARATE_300BPS=    0b000,
    DATARATE_1200BPS=   0b001,
    DATARATE_2400BPS=   0b010,
    DATARATE_4800BPS=   0b011,
    DATARATE_9600BPS=   0b100,
    DATARATE_19200BPS=  0b101
};

/* GENERAL SPECIFICATIONS*/

enum TRANSMISSION_MODE{
    TRANSPARENT = 0b0,
    FIXED = 0b1
};

enum IO_DRIVE_MODES{
    PULL_UP = 0b1,
    OPEN_COLLECTOR = 0b0
};

enum WAKE_UP_TIME{
    T250ms = 0b000,
    T500ms = 0b001,
    T750ms = 0b010,
    T1000ms = 0b011,
    T1250ms = 0b100,
    T1500ms = 0b101,
    T1750ms = 0b110,
    T2000ms = 0b111
};

enum FEC_SWITCH{
    FEC_OFF = 0b0,
    FEC_ON = 0b1
};

enum TX_POWER{
    TX_20dBm = 0b00,
    TX_17dBm = 0b01,
    TX_24dBm = 0b10,
    TX_10dBm = 0b11
};


/* OP MODES */
enum OperatingModes{
    OPEN= 0b00,
    AIR_WAKE= 0b01,
    ALMOST_OPEN = 0b10,
    SLEEP = 0b11
};

typedef struct{
    // Bit-fields, ref: https://en.cppreference.com/w/cpp/language/bit_field
    uint8_t datarate : 3;
    uint8_t baudrate : 3;
    uint8_t parity : 2;
} Speed;

typedef struct{
    uint8_t power : 2;
    uint8_t fec : 1;
    uint8_t wakeupTime : 3;
    uint8_t driveMode : 1;
    uint8_t modbus : 1;
} Option;

typedef struct {
    uint8_t head;
    uint8_t addr_h;
    uint8_t addr_l;
    Speed speed;
    uint8_t channel;
    Option opt;
} Config;

void E32_init();

bool E32_configure();

bool compare_configuration(uint8_t *c1, Config c2);

void E32_set_mode(uint8_t mode);