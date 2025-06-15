#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "i2c_cfg.h"

#include "aht20.h"
#include "bmx280.h"
#include "aht20_bmp280.h"

static const char *TAG = "aht20_bmp280";
static const char *TAG20 = "aht20";
static const char *TAG280 = "bmp280";

#define AHT20_INTERVAL 10000 //ms
#define BMP280_INTERVAL 10000 //ms

static aht20_dev_handle_t aht20_handle = NULL;
static bmx280_t* bmx280 = NULL;

aht20_measure aht20;
bmp280_measure bmp280;

QueueHandle_t queue_aht;

static void aht20_task(void *args) {
    while(1) {
        ESP_ERROR_CHECK(aht20_read_float(aht20_handle, &aht20.aht20_temperature, &aht20.aht20_humidity));
        ESP_LOGI(TAG20, "%-20s: %2.2fdegC", "temperature is", aht20.aht20_temperature);
        ESP_LOGI(TAG20, "%-20s: %2.2f%%", "humidity is", aht20.aht20_humidity);

        ESP_ERROR_CHECK(aht20_read_i16(aht20_handle, &aht20.aht20_temperature_i16, &aht20.aht20_humidity_i16));
        ESP_LOGI(TAG20, "%-20s: %d", "temperature is", aht20.aht20_temperature_i16);
        ESP_LOGI(TAG20, "%-20s: %d", "humidity is", aht20.aht20_humidity_i16);

        if( pdPASS == xQueueSend(queue_aht, &aht20,portMAX_DELAY)) {
            ESP_LOGI(TAG,"Send data to queue AHT20");
        } else {
            ESP_LOGI(TAG,"Send data to queue AHT20 failed");
        }

        vTaskDelay(AHT20_INTERVAL / portTICK_PERIOD_MS);
    }
}

void aht20_init(QueueHandle_t queue_aht20) {
    queue_aht = queue_aht20;

    i2c_master_bus_handle_t i2c_master_handle;
    ESP_ERROR_CHECK(i2c_master_get_bus_handle(I2C_MASTER_NUM, &i2c_master_handle));

    i2c_aht20_config_t aht20_i2c_config = {
        .i2c_config.device_address = AHT20_ADDRESS_0,
        .i2c_config.scl_speed_hz = I2C_MASTER_FREQ_HZ,
        .i2c_timeout = 100,
    };
    aht20_new_sensor(i2c_master_handle, &aht20_i2c_config, &aht20_handle);

    xTaskCreate(aht20_task, "aht20_task", 2048, queue_aht20, 10, NULL);
}

static void bmp280_task(void *args) {
    ESP_ERROR_CHECK(bmx280_setMode(bmx280, BMX280_MODE_CYCLE));

    while(1) {
        for(int i = 0; i < 3; i++) {
        do {
            vTaskDelay(pdMS_TO_TICKS(1));
        } while(bmx280_isSampling(bmx280));

        ESP_ERROR_CHECK(bmx280_readoutFloat(bmx280, &bmp280.bmp280_temperature, &bmp280.bmp280_pressure, &bmp280.bmp280_humidity));
        ESP_LOGI(TAG280, "Read Values: temp = %f, pres = %f, hum = %f", bmp280.bmp280_temperature, bmp280.bmp280_pressure, bmp280.bmp280_humidity);
        vTaskDelay(BMP280_INTERVAL / portTICK_PERIOD_MS);
        }
    }
}

void bmp280_init(QueueHandle_t queue_bmp280) {
    i2c_master_bus_handle_t i2c_master_handle;
    ESP_ERROR_CHECK(i2c_master_get_bus_handle(I2C_MASTER_NUM, &i2c_master_handle));

    bmx280 = bmx280_create_master(i2c_master_handle);

    if (!bmx280) { 
        ESP_LOGE(TAG, "Could not create bmx280 driver.");
    }
    ESP_ERROR_CHECK(bmx280_init(bmx280));

    bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
    ESP_ERROR_CHECK(bmx280_configure(bmx280, &bmx_cfg));

    xTaskCreate(bmp280_task, "bmp280_task", 2048, NULL, 10, NULL);
}