#include "esp_lvgl_port.h"
#include "lvgl.h"

void oled_ui_start(lv_disp_t *disp);
void lv_set_sensors_value(float temp, float hum);

void disp_show_time(void);
void disp_show_sensors_value(void);
void menu_view_draw_main(int selected);