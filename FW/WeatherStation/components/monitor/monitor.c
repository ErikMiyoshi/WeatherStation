#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "aht20_bmp280.h"
#include "oled_ui.h"
#include "app_button.h"
#include "menu.h"

#include "driver/gpio.h"

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
        //ESP_LOGI(TAG,"Monitor Task");
        if(xQueueReceive(queues.queue_aht20, &sensors_data.aht20_data, 0)) {
            lv_set_aht20_value(sensors_data.aht20_data.aht20_temperature,sensors_data.aht20_data.aht20_humidity);
        }
        if(xQueueReceive(queues.queue_bmp280, &sensors_data.bmp280_data, 0)) {
            lv_set_bmp280_value(sensors_data.bmp280_data.bmp280_temperature,sensors_data.bmp280_data.bmp280_pressure);
        }
        lv_set_vbat(get_bat_value()*2);

        if(is_button1_pressed()) {
            menu_handle_input(1,0,0);
        }
        if(is_button2_pressed()) {
            menu_handle_input(0,0,1);
        }
        if(is_button3_pressed()) {
            menu_handle_input(0,1,0);
        }
        menu_handle_input(0,0,0);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void monitor_init(QueueHandle_t queue_aht20, QueueHandle_t queue_bmp280) {
    queues.queue_aht20 = queue_aht20;
    queues.queue_bmp280 = queue_bmp280;

    xTaskCreate(monitor_task, "monitor_task", 4096, NULL, 1, NULL);
}