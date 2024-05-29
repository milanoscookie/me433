#include <stdio.h>
#include <string.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

#define MOTORPIN 16
#define ENPIN 17

#define UART_TX_PIN 0
#define UART_RX_PIN 1

volatile int i = 0;
volatile char msg[128];

void set_motor_speed(int i) {
    if (i > 25) {
        pwm_set_gpio_level(MOTORPIN,  0);
    } else if (i < 0) {
        pwm_set_gpio_level(MOTORPIN,  62500);
    } else {
        pwm_set_gpio_level(MOTORPIN,  62500* (25 - i) / 25);
    }
}

void on_uart_rx() {
    while (uart_is_readable(uart0)) {
        uint8_t ch = uart_getc(uart0);
        if (ch == '\r') {
            uart_puts(uart0, msg);
            msg[i] = '\0';
            int speed = 0;
            sscanf(msg, "%d", &speed);
            set_motor_speed(speed);
            i = 0;
            msg[0] = 0;
        } else {
            msg[i] = ch;
            i++;
        }
    }
}

void init_pwm() {
    gpio_set_function(MOTORPIN, GPIO_FUNC_PWM); // Set the LED Pin to be PWM

    uint slice_num = pwm_gpio_to_slice_num(MOTORPIN); // Get PWM slice number

    float div = 40; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider

    uint16_t wrap = 62500; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);

    pwm_set_enabled(slice_num, true); // turn on the PWM
    pwm_set_gpio_level(MOTORPIN,  62500);
}

int main() {

    stdio_init_all();

    uart_init(uart0, 115200);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_set_hw_flow(uart0, false, false);
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(uart0, false);

    int UART_IRQ = uart0 == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(uart0, true, false);

    gpio_init(ENPIN);
    gpio_set_dir(ENPIN, GPIO_OUT);

    init_pwm();

    uart_puts(uart0, "\ninterrupt\n");

    gpio_put(ENPIN, 1);

    while (1) {
        // just loop
    }
}

