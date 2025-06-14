#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "aht20_bmp280.h"
#include "oled_ui.h"

static const char *TAG = "monitor";

typedef struct sensors_queue {
    QueueHandle_t queue_aht20;
    QueueHandle_t queue_bmp280;
} sensors_queue;

typedef struct sensors_data_t {
    aht20_measure aht20_data;
    bmp280_measure bmp280_data;
} sensors_data_t;

struct sensors_queue queues;
sensors_data_t sensors_data;

void monitor_task(void *arg) {
    while(1) {
        if(xQueueReceive(queues.queue_aht20, &sensors_data.aht20_data, portMAX_DELAY)) {
            ESP_LOGI(TAG,"Received data from queue AHT20");

            lv_set_sensors_value(sensors_data.aht20_data.aht20_temperature,sensors_data.aht20_data.aht20_humidity);
        }
    }
}

void monitor_init(QueueHandle_t queue_aht20, QueueHandle_t queue_bmp280) {
    queues.queue_aht20 = queue_aht20;
    queues.queue_bmp280 = queue_bmp280;

    xTaskCreate(monitor_task, "monitor_task", 4096, NULL, 1, NULL);
}