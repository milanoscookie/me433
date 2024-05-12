#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/pwm.h"

const int pin = 28;
const int wrap = 20000;
const int div = 125;
const float raw_max = .128;
const float raw_min = .022;

void setup() {
    gpio_set_function(pin, GPIO_FUNC_PWM); // Set the pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Get PWM slice number

    pwm_set_clkdiv(slice_num, div); // divider
    pwm_set_wrap(slice_num, wrap);

    pwm_set_enabled(slice_num, true); // turn on the PWM

    pwm_set_gpio_level(pin, wrap / 2); // set the duty cycle to 50%
}


void set_angle(float theta) {
    float multiplier = (raw_max-raw_min) / (180);
    float scaler = theta * multiplier + raw_min;
    float newPWM = scaler * wrap;
    pwm_set_gpio_level(pin, newPWM); // set the duty cycle
}

int main() {
    setup();
    set_angle(0);
    while(1) {
        for (int i = 0; i <= 180; i++) {
            set_angle(i);
            sleep_ms(10);
        }
        for (int j = 180; j >= 0; j--) {
            set_angle(j);
            sleep_ms(10);
        }
        set_angle(0);
    }
}