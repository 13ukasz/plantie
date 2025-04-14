#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

static const char *TAG = "ADC_EXAMPLE";

void app_main(void)
{
    // Configure ADC1 channel 0 (GPIO0)
    adc1_config_width(ADC_WIDTH_BIT_12);  // 12-bit resolution
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // wider voltage range ~0-3.3V

    while (1) {
        // Read raw ADC value
        int raw = adc1_get_raw(ADC1_CHANNEL_0);

        // Print value
        ESP_LOGI(TAG, "ADC Raw: %d", raw);

        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second delay
    }
}
