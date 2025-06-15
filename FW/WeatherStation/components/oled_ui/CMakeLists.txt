idf_component_register(SRCS "oled_ui.c" "../../assets/temp.c"
                    INCLUDE_DIRS "include" "../../assets"
                    REQUIRES esp_lvgl_port driver)
