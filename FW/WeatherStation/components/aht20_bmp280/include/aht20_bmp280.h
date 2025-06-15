typedef struct aht20_measure {
    int16_t aht20_temperature_i16;
    int16_t aht20_humidity_i16;
    float aht20_temperature;
    float aht20_humidity;
} aht20_measure;

typedef struct bmp280_measure {
    float bmp280_temperature;
    float bmp280_pressure;
    float bmp280_humidity;
} bmp280_measure;

void aht20_init(QueueHandle_t queue_aht20);
void bmp280_init(QueueHandle_t queue_bmp280);