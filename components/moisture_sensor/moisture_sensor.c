#include <stdio.h>

#include "driver/adc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "moisture_sensor.h"

#define RELAY_GPIO        4
#define MAX_ADC_VALUE     4095
#define REFERENCE_VOLTAGE 3.3   // V

static const char *TAG = "MoistureSensor";

float moisture = 0.0;

static float min_voltage = 0.8;
static float max_voltage = 1.8;

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

void calculate_moisture(float adc_voltage)
{
    if (adc_voltage > max_voltage) {
        max_voltage = adc_voltage;
    } else if (adc_voltage < min_voltage) {
        min_voltage = adc_voltage;
    }

    moisture = (1.0f - (adc_voltage - min_voltage) / (max_voltage - min_voltage)) * 100.0f;
}

void moisture_sensor_task(void *pvParameters)
{   
    bool alert_triggered = false;

    int ret = configure_adc();
    if (0 != ret) {
        ESP_LOGE(TAG, "Failed to configure ADC");
        return;
    }

    while (1) {
        float adc_voltage = read_adc_voltage();

        /* Sometimes at boot-up 0.0 voltage is being read, causing the pump to be triggered*/
        if (adc_voltage == 0.0) {
            continue; 
        }

        calculate_moisture(adc_voltage);

        /* Trigger water pump */
        if (moisture < 30.0f) {
            if (!alert_triggered) {
                ESP_LOGI(TAG, "Moisture too low, triggering water pump");
                alert_triggered = true;
            }
            gpio_set_level(RELAY_GPIO, 1);
        } else if (moisture >= 50.0f) {
            gpio_set_level(RELAY_GPIO, 0);
        }
        
        ESP_LOGI(TAG, "ADC Voltage: %.3f", adc_voltage);
        ESP_LOGI(TAG, "Moisture percent: %.2f", moisture);

        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}
