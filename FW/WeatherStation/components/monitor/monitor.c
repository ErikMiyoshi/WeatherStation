#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "aht20_bmp280.h"
#include "oled_ui.h"
#include "app_button.h"
#include "menu.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "mqtt_app.h"

#define DEVICE_NUM 2

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
    uint8_t batteryOperated = (int)arg;
    uint64_t time;
    time = esp_timer_get_time();
    int deviceNum = DEVICE_NUM;

    uint8_t messageSent = 0; //bit 0 -> aht20
                             //bit 1 -> bmp280
                             //bit 2 -> adc battery
    while(1) {
        //ESP_LOGI(TAG,"Monitor Task");
        lv_turn_off();
        if(esp_timer_get_time() - time > 10000000) {
            time = esp_timer_get_time();
            ESP_LOGI(TAG,"Turn display off");
            lv_turn_off();
        }
        if(xQueueReceive(queues.queue_aht20, &sensors_data.aht20_data, 0)) {
            ESP_LOGI(TAG,"Received data from queue AHT20");
            
            char payload[100];
            snprintf(payload, sizeof(payload), "{\"device\":%d, \"sensor\": \"aht20\", \"t\":%.2f, \"h\":%.2f}", deviceNum, sensors_data.aht20_data.aht20_temperature, sensors_data.aht20_data.aht20_humidity);
            if(mqtt_send_message("measure/", payload) > 0)
                messageSent |= 1;
            lv_set_sensors_value(sensors_data.aht20_data.aht20_temperature,sensors_data.aht20_data.aht20_humidity);
        }
        if(xQueueReceive(queues.queue_bmp280, &sensors_data.bmp280_data, 0)) {
            ESP_LOGI(TAG,"Received data from queue BMP280");
            char payload[100];
            snprintf(payload, sizeof(payload), "{\"device\":%d, \"sensor\": \"bmp280\", \"t\":%.2f, \"p\":%.2f}", deviceNum, sensors_data.bmp280_data.bmp280_temperature, sensors_data.bmp280_data.bmp280_pressure);
            if(mqtt_send_message("measure/", payload) > 0)
                messageSent |= (1 << 1);
        }
        lv_set_battery_value(battery_get_voltage());
        if(is_button_center_pressed()) {
            ESP_LOGI(TAG,"Button center pressed");
            menu_handle_input(0,0,1);
            time = esp_timer_get_time();
        }
        if(is_button_left_pressed()) {
            ESP_LOGI(TAG,"Button left pressed");
            menu_handle_input(1,0,0);
            time = esp_timer_get_time();
        }
        if(is_button_right_pressed()) {
            ESP_LOGI(TAG,"Button right pressed");
            menu_handle_input(0,1,0);
            time = esp_timer_get_time();
        }
        if(batteryOperated) {
            int bVoltage = adc_get_single();
            ESP_LOGI(TAG,"ADC reading: %dV", adc_get_single());
            char payload[100];
            snprintf(payload, sizeof(payload), "{\"device\":%d, \"sensor\": \"battery\", \"v\":%d}", deviceNum, 2*bVoltage);
            mqtt_send_message("measure/", payload);
            messageSent |= 1 << 2;
        }
        printf("messageSent: %d", messageSent);
        if(messageSent == 0b111 || (!batteryOperated && messageSent == 0b011)) {
            ESP_LOGI(TAG,"All messages were sent\n");
            const int wakeup_time_sec = 10*60;
            printf("Enabling timer wakeup, %ds\n", wakeup_time_sec);
            ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000));
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            esp_deep_sleep_start();
            //Todo, enter in sleep mode for 10minutes
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
        //menu_handle_input(0,0,0);
    }
}

void monitor_init(QueueHandle_t queue_aht20, QueueHandle_t queue_bmp280, int batOp) {
    queues.queue_aht20 = queue_aht20;
    queues.queue_bmp280 = queue_bmp280;

    xTaskCreate(monitor_task, "monitor_task", 4096, (void *)batOp, 1, NULL);
}