#include "oled_ui.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <time.h>

#include "temp.h"   // ou .h se tiver

static const char *TAG = "display_ui";

lv_obj_t * label;
lv_obj_t * label2;
lv_obj_t * bar1;
lv_obj_t * img;

float temperature;
float humidity;

void menu_view_draw_main(int selected) {
    if (lvgl_port_lock(0)) {
        lv_obj_clean(lv_scr_act());

        const char *items[] = { "Sensors", "Time" };

        for (int i = 0; i < 2; i++) {
            lv_obj_t *label = lv_label_create(lv_scr_act());
            lv_label_set_text_fmt(label, "%s %s", (i == selected ? "-" : " "), items[i]);
            lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 30 * i);
        }
        lvgl_port_unlock();
    }
}

void get_current_time(struct tm *out_timeinfo) {
    time_t now;
    time(&now);
    localtime_r(&now, out_timeinfo);
}

void lv_set_sensors_value(float temp, float hum) {
    temperature = temp;
    humidity = hum;
}

void disp_show_time(void) {
    struct tm my_time;
    get_current_time(&my_time);
    if (lvgl_port_lock(0)) {
        lv_obj_clean(lv_scr_act());
        label = lv_label_create(lv_scr_act());

        char buf[30];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", my_time.tm_hour,my_time.tm_min,my_time.tm_sec);
        lv_label_set_text(label, buf);
        lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

        lvgl_port_unlock();
    }
}

void disp_show_sensors_value(void) {
    if (lvgl_port_lock(0)) {
        lv_obj_clean(lv_scr_act());
        label = lv_label_create(lv_scr_act());
        label2 = lv_label_create(lv_screen_active());
        img = lv_img_create(lv_scr_act());
        char buf[30];

        lv_img_set_src(img, &temp);
        lv_obj_align(img,LV_ALIGN_TOP_LEFT,0,0);
        lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);
        

        snprintf(buf,sizeof(buf),"Temp: %.2f°C", temperature);
        lv_label_set_text(label, buf);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 12, 0);

        snprintf(buf,sizeof(buf),"Humidity: %.2f%%", humidity);
        lv_label_set_text(label2, buf);
        lv_obj_align(label2, LV_ALIGN_LEFT_MID, 0, 0);
        lv_obj_clear_flag(label2, LV_OBJ_FLAG_HIDDEN);
        lvgl_port_unlock();
    }
}

void update_display_task(void *arg) {
    label = lv_label_create(lv_screen_active());
    label2 = lv_label_create(lv_screen_active());
    img = lv_img_create(lv_scr_act());

    int counter = 0;

    uint64_t time = esp_timer_get_time();
    bool screen = 0;

    struct tm my_time;

    int select = 0;

    while (1) {
        // get_current_time(&my_time);
        // //ESP_LOGI(TAG,"Running display task...%d", counter);
        // if(esp_timer_get_time() - time > 5000000) {
        //     screen = !screen;
        //     time = esp_timer_get_time();
        // }
        // if (lvgl_port_lock(0)) {
        //     menu_view_draw_main(select);
        //     lvgl_port_unlock();
        // }
        // counter++;
        // if(counter > 10000) {
        //     counter=0;
        // }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void oled_ui_start(lv_disp_t *disp) {
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    //xTaskCreate(update_display_task, "ui_update", 4096, disp, 1, NULL);
}