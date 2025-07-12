#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"

#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "driver/gpio.h"

#include "app_button.h"

#define EXAMPLE_READ_LEN 4*128

static const char *TAG = "ADC";

static TaskHandle_t s_task_handle;

#define ADC_UNIT        ADC_UNIT_1
#define ADC_BITWIDTH    ADC_BITWIDTH_12
#define ADC_ATTEN_DB    ADC_ATTEN_DB_12

adc_continuous_handle_t adc_handle = NULL;
adc_cali_handle_t adc_cali_handle = NULL;

int v_joystick;
int vbattery;
int buttonLeft = 1;
int buttonCenter = 1;
int buttonRight = 1;

bool is_button_left_pressed(void) {
    return !buttonLeft;
}

bool is_button_center_pressed(void) {
    return !buttonCenter;
}

bool is_button_right_pressed(void) {
    return !buttonRight;
}

int battery_get_voltage(void) {
    return vbattery;
}

bool is_joystick_pressed_right(void) {
    if(v_joystick > 3000) {
        return true;
    }
    return false;
}

bool is_joystick_pressed_left(void) {
    if(v_joystick < 200) {
        return true;
    }
    return false;
}

static void gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    if(gpio_num == GPIO_NUM_19) {
        buttonLeft = gpio_get_level(gpio_num);
    }
    if(gpio_num == GPIO_NUM_16) {
        buttonCenter = gpio_get_level(gpio_num);
    }
    if(gpio_num == GPIO_NUM_4) {
        buttonRight = gpio_get_level(gpio_num);
    }
}

static void button_gpio_init(void) {
    gpio_config_t cfg = {};
    cfg.intr_type = GPIO_INTR_ANYEDGE;
    cfg.mode = GPIO_MODE_INPUT;
    cfg.pull_up_en = GPIO_PULLUP_ENABLE;
    cfg.pin_bit_mask = (1ULL << GPIO_NUM_19) | (1ULL << GPIO_NUM_16) | (1ULL << GPIO_NUM_4) ;
    ESP_ERROR_CHECK(gpio_config(&cfg));
    
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_19, gpio_isr_handler, (void *) GPIO_NUM_19);
    gpio_isr_handler_add(GPIO_NUM_16, gpio_isr_handler, (void *) GPIO_NUM_16);
    gpio_isr_handler_add(GPIO_NUM_4, gpio_isr_handler, (void *) GPIO_NUM_4);
}


static bool adc_conv_done(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data) {
    BaseType_t mustYield = pdFALSE;
    vTaskNotifyGiveFromISR(s_task_handle, &mustYield);
    return (mustYield == pdTRUE);
}

static void adc_task(void *args) {
    esp_err_t ret;
    uint32_t ret_num = 0;
    uint8_t result[EXAMPLE_READ_LEN] = {0};
    
    while(1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ret = adc_continuous_read(adc_handle, result, EXAMPLE_READ_LEN, &ret_num, 0);
        if (ret == ESP_OK) {
            for (int i = 0; i < 5/*ret_num*/; i += SOC_ADC_DIGI_RESULT_BYTES) {
                adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
                uint32_t chan_num = p->type1.channel;
                uint32_t data = p->type1.data;
                /* Check the channel number validation, the data is invalid if the channel num exceed the maximum channel */
                if (chan_num < SOC_ADC_CHANNEL_NUM(ADC_UNIT)) {
                    adc_cali_raw_to_voltage(adc_cali_handle,data, &vbattery);
                    // if(chan_num == 0) {
                    // ESP_LOGI(TAG, "C: %"PRIu32", V: %"PRIx32", Cali: %d", chan_num, data, v_joystick);
                    // }
                    //ESP_LOGI(TAG, "Unit: %d, Channel: %"PRIu32", Value: %"PRIx32, ADC_UNIT, chan_num, data);
                } else {
                    ESP_LOGW(TAG, "Invalid data");
                }
            }
            vTaskDelay(50 / portTICK_PERIOD_MS);
        } else if (ret == ESP_ERR_TIMEOUT) {
            //We try to read `EXAMPLE_READ_LEN` until API returns timeout, which means there's no available data
            break;
        }
    }
}

static void calibrate_adc(adc_cali_handle_t *out_handle) {
    adc_cali_scheme_ver_t cali_scheme;
    adc_cali_handle_t cali_handle = NULL;
    bool calibrated = false;
    esp_err_t ret;

    ESP_ERROR_CHECK(adc_cali_check_scheme(&cali_scheme));

    if(cali_scheme & 1) {
        ESP_LOGI(TAG, "Suport ADC_CALI_SCHEME_VER_LINE_FITTING");
    }
    if(cali_scheme & 2) {
        ESP_LOGI(TAG, "Suport ADC_CALI_SCHEME_VER_CURVE_FITTING");
    }
    
    if(!calibrated) {
        ESP_LOGI(TAG, "Calibrating ADC...");
        adc_cali_line_fitting_config_t cali_cfg = {
            .unit_id = ADC_UNIT,
            .atten = ADC_ATTEN_DB,
            .bitwidth = ADC_BITWIDTH,
        };

        ret = adc_cali_create_scheme_line_fitting(&cali_cfg, &cali_handle);
    }
    
    *out_handle = cali_handle;

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }
}

static void adc_init(void) {
    adc_cali_handle = NULL;
    calibrate_adc(&adc_cali_handle);

    adc_handle = NULL;

    adc_continuous_handle_cfg_t adc_h_cfg = {
        .max_store_buf_size = 1024,
        .conv_frame_size = EXAMPLE_READ_LEN,
    };

    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_h_cfg, &adc_handle));
    
    adc_continuous_config_t adc_c_cfg = {
        .sample_freq_hz = 20*1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
    };
    
    adc_digi_pattern_config_t adc_pattern[1] = {0};
    adc_pattern[0].atten = ADC_ATTEN_DB,
    adc_pattern[0].channel = 0,
    adc_pattern[0].unit = ADC_UNIT,
    adc_pattern[0].bit_width = ADC_BITWIDTH,

    // adc_pattern[1].atten = ADC_ATTEN_DB,
    // adc_pattern[1].channel = 7,
    // adc_pattern[1].unit = ADC_UNIT,
    // adc_pattern[1].bit_width = ADC_BITWIDTH,

    adc_c_cfg.pattern_num = 1;
    adc_c_cfg.adc_pattern = adc_pattern;

    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &adc_c_cfg));

    adc_continuous_evt_cbs_t adc_cbs = {
        .on_conv_done = adc_conv_done,
    };

    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(adc_handle, &adc_cbs, NULL));

    s_task_handle = xTaskGetCurrentTaskHandle();

    esp_err_t ret;
    uint32_t ret_num = 0;
    uint8_t result[EXAMPLE_READ_LEN] = {0};

    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));
    ESP_LOGI(TAG,"ADC start");
}

void app_button_init(void) {
    adc_init(); //Using joystick encoder for now
    xTaskCreate(adc_task, "adc_task", 4096, NULL, 1, &s_task_handle);
    button_gpio_init();
}