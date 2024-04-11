#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <string.h>
#include <stdlib.h>

const uint LEDPIN = PICO_DEFAULT_LED_PIN;
#define BUTTONPIN 16
#define ADC0 26

float readVoltage() {
    uint16_t raw = adc_read();
    return raw/4096. * 3.3;
}
int main() {

    gpio_init(LEDPIN);
    gpio_set_dir(LEDPIN, GPIO_OUT);

    gpio_init(BUTTONPIN);
    gpio_set_dir(BUTTONPIN, GPIO_IN);

    adc_init(); // init the adc module
    adc_gpio_init(ADC0); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0


    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
 

    while (1) {
        gpio_put(LEDPIN, 1);
        while(gpio_get(BUTTONPIN) == 0) sleep_ms(1);
        gpio_put(LEDPIN, 0);

        printf("Enter a number from 1-100:\t");
        char s[5];
        scanf("%s", &s);
        uint8_t n = atoi(s);
        printf("\n");

        for(uint8_t i = 0; i < n; i++) {
            printf("%f\n", readVoltage());
            sleep_ms(10);
        }
        sleep_ms(50);
    }

}