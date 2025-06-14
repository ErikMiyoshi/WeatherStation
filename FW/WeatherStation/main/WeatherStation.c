#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"

#include "wifi_st.h"
#include "i2c_cfg.h"
#include "aht20_bmp280.h"

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_sta();
    i2c_init();
    aht20_init();
    bmp280_init();

    int i = 0;
    while(1) {
        i++;
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
