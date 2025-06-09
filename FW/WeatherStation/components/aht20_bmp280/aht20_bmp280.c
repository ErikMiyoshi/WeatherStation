#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "i2c_cfg.h"

#include "aht20.h"
#include "aht20_bmp280.h"

static const char *TAG = "aht20_bmp280";

#define AHT20_INTERVAL 5000 //ms

static aht20_dev_handle_t aht20_handle = NULL;

int16_t temperature_i16;
int16_t humidity_i16;
float temperature;
float humidity;

static void aht20_task(void *args) {
    while(1) {
        ESP_ERROR_CHECK(aht20_read_float(aht20_handle, &temperature, &humidity));
        ESP_LOGI(TAG, "%-20s: %2.2fdegC", "temperature is", temperature);
        ESP_LOGI(TAG, "%-20s: %2.2f%%", "humidity is", humidity);

        ESP_ERROR_CHECK(aht20_read_i16(aht20_handle, &temperature_i16, &humidity_i16));
        ESP_LOGI(TAG, "%-20s: %d", "temperature is", temperature_i16);
        ESP_LOGI(TAG, "%-20s: %d", "humidity is", humidity_i16);

        vTaskDelay(AHT20_INTERVAL / portTICK_PERIOD_MS);
    }
}

void aht20_init(void) {
    i2c_master_bus_handle_t i2c_master_handle;
    ESP_ERROR_CHECK(i2c_master_get_bus_handle(I2C_MASTER_NUM, &i2c_master_handle));

    i2c_aht20_config_t aht20_i2c_config = {
        .i2c_config.device_address = AHT20_ADDRESS_0,
        .i2c_config.scl_speed_hz = I2C_MASTER_FREQ_HZ,
        .i2c_timeout = 100,
    };
    aht20_new_sensor(i2c_master_handle, &aht20_i2c_config, &aht20_handle);

    xTaskCreate(aht20_task, "aht20_task", 2048, NULL, 10, NULL);

}