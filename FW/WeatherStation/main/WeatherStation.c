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

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    QueueHandle_t queue_aht20 = xQueueCreate(10,sizeof(aht20_measure));
    QueueHandle_t queue_bmp280 = xQueueCreate(10,sizeof(bmp280_measure));

    wifi_init_sta();
    app_sntp_init();
    i2c_init();
    aht20_init(queue_aht20);
    bmp280_init(queue_bmp280);
    app_button_init();
    display_init();
    mqtt_init();
    monitor_init(queue_aht20,queue_bmp280);
    menu_init();

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
