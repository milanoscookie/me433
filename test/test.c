#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


#define ADDR 0x68

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75


void imu_init() {
  uint8_t buf[2];

  //turn chip on
  buf[0] = PWR_MGMT_1;
  buf[1] = 0x00; 

  i2c_write_blocking(i2c_default, ADDR, buf, 2, false);

  // set sens to 2g for accel
  buf[0] = ACCEL_CONFIG;
  buf[1] = 0x00;

  i2c_write_blocking(i2c_default, ADDR, buf, 2, false);

  // set sens to 2000 dps for gyro
  buf[0] = GYRO_CONFIG;
  buf[1] = 0x18;

  i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
}

void read_imu(int16_t buf[2], uint8_t reg) {

  uint8_t buffer[4];

  i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);
  i2c_read_blocking(i2c_default, ADDR, buffer, 4, false);

  for (int i = 0; i < 2; i++) {
    buf[i] = ((buffer[i * 2] << 8) | buffer[(i * 2) + 1]);
  }
}


void i2c_setup() {
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

    // initialize the heartbeat LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN,GPIO_OUT);
}
int main()
{
    stdio_init_all();
    sleep_ms(10);

    
    i2c_setup();
    sleep_ms(100);

    imu_init();
    sleep_ms(100);
    while (true) {
        int16_t acceleration[2];
        read_imu(acceleration, ACCEL_XOUT_H);

        float a[2];
        for (int i = 0; i < 2; i++)
        {
            a[i] = acceleration[i] * 0.000061;
        }

        int8_t x_delta = a[0] * -10;
        int8_t y_delta = a[1] * 10;

        printf("%d, %d\n", x_delta, y_delta);
    }
}

