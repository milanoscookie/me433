/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>  
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

int32_t t_fine;

uint16_t dig_T1;
int16_t dig_T2, dig_T3;
uint16_t dig_P1;
int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
uint8_t dig_H1, dig_H3;
int8_t dig_H6;
int16_t dig_H2, dig_H4, dig_H5;

#define N_SAMPLES 200
#define N_SINE_CYCLES 8
#define N_TRIANGLE_CYCLES 4

void printBinary(unsigned long num) {
    if (num > 1)
        printBinary(num / 2);
    printf("%lu", num % 2);
}

static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    asm volatile("nop \n nop \n nop");
}

static void write_register(uint8_t AorB, uint16_t data) {
    uint8_t buf[2];
    uint8_t given = 0b011;
    //[A/B BUF GA SHDN D9 D8 D7 D6 D5 D4 D3 D2 D1 D0 x x]
    //[input 0 1 1 input ...]
    uint16_t encoded = ((uint16_t) AorB << 15) | ((uint16_t) given << 12) | ((uint16_t) data << 2); 
    // Assign the results of bitwise AND operations to variables
    buf[0] = (encoded & 0xFF00 ) >> 8; 
    buf[1]  = encoded & 0x00FF;

    cs_select(); // make this pin go low
    // here is a pointer (buf) to the array of data i would like to send it has two componenets
    spi_write_blocking(spi_default, buf, 2); 
    cs_deselect(); // make this pin go high
    sleep_ms(10);
}

void gen_sin(uint16_t *sin_wave, uint16_t n_samples, uint16_t n_cycles) {
    for (int i = 0; i < n_samples; i++) {
        double angle = (2.0 * M_PI * n_cycles * i) / n_samples;
        sin_wave[i] = (uint16_t)(512.0 * sin(angle) + 512.0);
    }
}

void gen_tri(uint16_t *tri_wave, uint16_t n_samples, uint16_t n_cycles) {
    for (int i = 0; i < n_samples; i++) {
        double frac_cycle = (double) i / n_samples * n_cycles;
        double triangle_value = fmod(frac_cycle, 1.0);
        tri_wave[i] = (uint16_t)(1023.0 * (2.0 * fabs(triangle_value - 0.5)));
    }
}

int main() {
    stdio_init_all();

    // LED to make sure its running
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // This example will use SPI0 at 0.5MHz.
    spi_init(spi_default, 12000);
    // gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
  
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN); // initialize
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT); // set as output
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1); // set it high

    uint16_t sine_wave[N_SAMPLES];
    uint16_t triangle_wave[N_SAMPLES];

    gen_sin(sine_wave, N_SAMPLES, N_SINE_CYCLES);
    gen_tri(triangle_wave, N_SAMPLES, N_TRIANGLE_CYCLES);


    while (1) {
        for (int i = 0; i < 200; i++) {
            write_register(0, sine_wave[i]);
            write_register(1, triangle_wave[i]);
        }
    }
}

