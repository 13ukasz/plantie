#include <stdio.h>

#include "driver/adc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "moisture_sensor.h"

#define MAX_ADC_VALUE     4095
#define REFERENCE_VOLTAGE 3.3   // V

static const char *TAG = "MoistureSensor";

static int configure_adc(void)
{
    int ret = adc1_config_width(ADC_WIDTH_BIT_12);
    if (0 != ret) {
        ESP_LOGE(TAG, "Failed to config ADC width");
        goto EXIT;
    }

    /* ADC range: 0 V - ~2.5 V*/
    ret = adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12); 
    if (0 != ret) {
        ESP_LOGE(TAG, "Failed to set ADC range");
        goto EXIT;
    }

EXIT:
    return ret;
}

static float read_adc_voltage()
{
    int raw_value = adc1_get_raw(ADC1_CHANNEL_0);
    if (0 > raw_value) {
        ESP_LOGE(TAG, "ADC read failed");
        return ESP_FAIL;
    }

    return ((float)raw_value / MAX_ADC_VALUE) * REFERENCE_VOLTAGE;
}

void moisture_sensor_task(void *pvParameters)
{   
    int ret = configure_adc();
    if (0 != ret) {
        ESP_LOGE(TAG, "Failed to configure ADC");
        return;
    }

    while (1) {
        float adc_voltage = read_adc_voltage();
        
        ESP_LOGI(TAG, "ADC Voltage: %.3f", adc_voltage);

        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}
