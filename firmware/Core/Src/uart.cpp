#include "uart.h"

#include "stm32f1xx_hal_uart.h"

namespace uart {
    void init() {
        // Configure the TXD pin as output
        // PORTF.DIRSET = 1 << 4;
        // PORTF.DIRCLR = 1 << 5;

        // Assign USART2 to the correct pins
        // PORTMUX.USARTROUTEA = 0x1 << 4;

        // Set the baud rate to 57600
        // (assumes CLK_PER is 3.333MHz, see page 288)
        // USART2.BAUD = 93 * 6 + 135;

        // Set the frame format:
        // - Asynchronous communication
        // - No parity
        // - 1 stop bit
        // - 8 bit character size
        // USART2.CTRLC = 0x3;

        /*while (1) {
            PORTF.OUTTGL = 1 << 4;
            volatile int i = 0;
            for (i = 0; i < 1000; ++i);
        }*/

        // Enable the transmitter and receiver
        // USART2.CTRLB |= 0b11 << 6;
    }

    void put(uint8_t data) {
        HAL_UART_Transmit(&huart1, &data, 1, HAL_MAX_DELAY);
        // Wait for tx data register empty
        // while ((USART2.STATUS & (1 << 5)) == 0);

        // USART2.TXDATAL = data;
    }

    void put_bytes(const uint8_t data[], size_t len) {
        HAL_UART_Transmit(&huart1, data, len, HAL_MAX_DELAY);
    }

    uint8_t get() {
        uint8_t buf;
        while (HAL_UART_Receive(&huart1, &buf, 1, HAL_MAX_DELAY) != HAL_OK);
        return buf;
    }

    void get_bytes(uint8_t data[], size_t len) {
        while (HAL_UART_Receive(&huart1, data, len, HAL_MAX_DELAY) != HAL_OK);
    }

    int get_nonblocking() {
        uint8_t buf;
        if (HAL_UART_Receive(&huart1, &buf, 1, 0) == HAL_OK) {
            return buf;
        } else {
            return -1;
        }
    }
}
