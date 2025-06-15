idf_component_register(SRCS "oled.c"
                    INCLUDE_DIRS "include"
                    REQUIRES i2c_cfg esp_lvgl_port driver oled_ui)
