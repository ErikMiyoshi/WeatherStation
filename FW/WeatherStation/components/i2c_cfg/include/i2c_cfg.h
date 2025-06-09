#include <stdio.h>
#include <string.h>
#include "driver/i2c_master.h"

#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_NUM I2C_NUM_0
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ    (400 * 1000)

void i2c_init(void);