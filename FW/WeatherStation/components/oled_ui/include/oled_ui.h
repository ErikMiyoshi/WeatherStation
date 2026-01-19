#include "esp_lvgl_port.h"
#include "lvgl.h"

void oled_ui_start(lv_disp_t *disp);

void lv_set_vbat(int vbat);
void lv_set_aht20_value(float temp, float hum);
void lv_set_bmp280_value(float temp, float pressure);

void disp_show_bat_status(void);
void disp_show_time(void);
void disp_show_sensors_value(void);
void menu_view_draw_main(int selected);