#include <stdio.h>
#include "driver/i2c.h"
#include "ccs811.h"

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

void app_main(void) {
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(I2C_MASTER_NUM, &i2c_config);
    i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);

    ccs811_t sensor;
    ccs811_init(&sensor, I2C_MASTER_NUM, CCS811_ADDR_LOW);

    printf("Initializing CCS811\n");
    if (ccs811_start(&sensor) != CCS811_OK) {
        printf("Failed to start CCS811 sensor\n");
        return;
    }

    while (1) {
        uint16_t eco2, tvoc;
        if (ccs811_read_data(&sensor, &eco2, &tvoc) == CCS811_OK) {
            printf("eCO2: %d ppm, TVOC: %d ppb\n", eco2, tvoc);
        } else {
            printf("Error reading data from CCS811\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

  

  

