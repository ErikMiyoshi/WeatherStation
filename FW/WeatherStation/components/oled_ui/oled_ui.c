#include "oled_ui.h"
#include "esp_log.h"

#include "temp.h"   // ou .h se tiver

static const char *TAG = "display_ui";

lv_obj_t * label;
lv_obj_t * label2;
lv_obj_t * bar1;
lv_obj_t *img;

float temperature;
float humidity;

void lv_set_sensors_value(float temp, float hum) {
    temperature = temp;
    humidity = hum;
}

void lv_show_sensors_value(int counter1, int counter2) {
    char buf[30];

    lv_img_set_src(img, &temp);
    lv_obj_align(img,LV_ALIGN_TOP_LEFT,0,0);

    snprintf(buf,sizeof(buf),"Temp: %.2f°C", temperature);
    lv_label_set_text(label, buf);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 12, 0);

    snprintf(buf,sizeof(buf),"Humidity: %.2f%%", humidity);
    lv_label_set_text(label2, buf);
    lv_obj_align(label2, LV_ALIGN_LEFT_MID, 0, 0);
}

void update_display_task(void *arg) {
    lv_obj_t *scr = lv_disp_get_scr_act((lv_disp_t *)(arg));
    label = lv_label_create(lv_screen_active());
    label2 = lv_label_create(lv_screen_active());
    img = lv_img_create(lv_scr_act());        // Cria objeto de imagem

    int counter = 0;

    while (1) {
        //ESP_LOGI(TAG,"Running display task...%d", counter);

        if (lvgl_port_lock(0)) {
            lv_show_sensors_value(counter,counter);
            lvgl_port_unlock();
        }
        counter++;
        if(counter > 10000) {
            counter=0;
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}


void oled_ui_start(lv_disp_t *disp) {
    lv_obj_t *scr = lv_disp_get_scr_act(disp);

    xTaskCreate(update_display_task, "ui_update", 4096, disp, 1, NULL);
}