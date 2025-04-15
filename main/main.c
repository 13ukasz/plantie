#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

#include "moisture_sensor.h"

static const char *TAG = "ADC_EXAMPLE";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting ADC Task...");

    xTaskCreate(moisture_sensor_fun, "moisture_sensor_fun", 2048, NULL, 1, NULL);
}
