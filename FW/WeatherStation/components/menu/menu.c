#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "oled_ui.h"

static const char *TAG = "menu";

typedef enum {
    MENU_STATE_MAIN,
    MENU_STATE_MONITOR,
    MENU_STATE_TIME,
    MENU_STATE_BATTERY,
} menu_state_t;

static menu_state_t current_state = MENU_STATE_MAIN;
static int selected_index = 0;

void menu_handle_input(bool up, bool down, bool ok) {
    ESP_LOGI(TAG,"Enter handle input %d %d %d %d %d", current_state, selected_index, up, down, ok);
    switch (current_state) {
        case MENU_STATE_MAIN:
            menu_view_draw_main(selected_index);
            if (up || down) {
                selected_index = (selected_index + (down ? 1 : -1) + 3) % 3;
                menu_view_draw_main(selected_index);
            }
            if (ok) {
                if (selected_index == 0) {
                    ESP_LOGI(TAG,"Enter sensor page");
                    current_state = MENU_STATE_MONITOR;
                } else if(selected_index == 1) {
                    current_state = MENU_STATE_TIME;
                } else if(selected_index == 2) {
                    current_state = MENU_STATE_BATTERY;
                }
            }
            break;

        case MENU_STATE_MONITOR:
            disp_show_sensors_value();
            if (ok) {
                current_state = MENU_STATE_MAIN;
            }
            break;
        case MENU_STATE_TIME:
            disp_show_time();
            if (ok) {
                current_state = MENU_STATE_MAIN;
                menu_view_draw_main(selected_index);
            }
            break;
        case MENU_STATE_BATTERY:
            disp_show_battery();
            if (ok) {
                current_state = MENU_STATE_MAIN;
                menu_view_draw_main(selected_index);
            }
            break;
        default:
            break;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void menu_task(void *args) {
    while(1) {
        //menu_handle_input(0,0,0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void menu_init(void) {
    current_state = MENU_STATE_MAIN;
    selected_index = 0;
    //menu_view_draw_main(selected_index);
    //xTaskCreate(menu_task, "menu_task", 4096, NULL, 1, NULL);
}