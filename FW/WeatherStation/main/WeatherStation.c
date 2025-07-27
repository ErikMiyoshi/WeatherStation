#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"

#include "wifi_st.h"
#include "i2c_cfg.h"
#include "aht20_bmp280.h"
#include "oled.h"
#include "monitor.h"
#include "menu.h"
#include "app_button.h"
#include "mqtt_app.h"

#define BATTERTY_OP 1

static const char *TAG = "Main";

void app_main(void)
{
    //Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle...");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }

    uint8_t batOp;
    err = nvs_get_u8(my_handle, "batteryOp", &batOp);

    switch (err) {
        case ESP_OK:
            if(batOp == BATTERTY_OP) {
                ESP_LOGI(TAG, "Read batteryOp = %"PRIu8", variable is stored correctly", batOp);
            } else {
                ESP_LOGI(TAG, "Read batteryOp = %"PRIu8", variable is stored incorrectly", batOp);
                ESP_LOGI(TAG, "Updating variable batOp...");
                err = nvs_set_i32(my_handle, "batteryOp", batOp);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to write batteryOp!");
                }
                ESP_LOGI(TAG, "Committing updates in NVS...");
                err = nvs_commit(my_handle);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to commit NVS changes!");
                }
            }
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAG, "The value is not initialized yet!");
            ESP_LOGI(TAG, "Writing variable to nvs!");
            err = nvs_set_u8(my_handle, "batteryOp", BATTERTY_OP);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to write batteryOp!");
            }
            ESP_LOGI(TAG, "Committing updates in NVS...");
            err = nvs_commit(my_handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to commit NVS changes!");
            }
            break;
        default:
            ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
    }

    // Example of nvs_get_stats() to get overview of actual statistics of data entries :
    nvs_stats_t nvs_stats;
    nvs_get_stats(NULL, &nvs_stats);
    printf("Count: UsedEntries = (%d), FreeEntries = (%d), AvailableEntries = (%d), AllEntries = (%d)\n",
       nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.available_entries, nvs_stats.total_entries);

    QueueHandle_t queue_aht20 = xQueueCreate(10,sizeof(aht20_measure));
    QueueHandle_t queue_bmp280 = xQueueCreate(10,sizeof(bmp280_measure));

    wifi_init_sta();
    mqtt_init();
    app_sntp_init();
    i2c_init();
    aht20_init(queue_aht20);
    bmp280_init(queue_bmp280);
    app_button_init();
    //display_init();
    monitor_init(queue_aht20,queue_bmp280, batOp);
    //menu_init();

    int i = 0;

    while(1) {
        //menu_handle_input(0, 1, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        //menu_handle_input(0, 0, 1);
        while(1){
            i++;
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}
