#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

#include "moisture_sensor.h"
#include "temperature_sensor.h"
#include "web_server.h"
#include "zigbee_relay.h"

#define RELAY_GPIO 4

#define TAG "main"

static esp_err_t deferred_driver_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RELAY_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 0,
        .pull_up_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);
    gpio_set_level(RELAY_GPIO, 0);

    return ESP_OK;
}

void app_main(void)
{
    int ret = deferred_driver_init();
    if (0 != ret) {
        ESP_LOGE(TAG, "GPIO driver initialization failed");
        return;
    }

    xTaskCreate(moisture_sensor_task, "moisture_sensor_fun", 2048, NULL, 5, NULL);
    xTaskCreate(sensor_read_task, "sensor_read_task", 4096, NULL, 5, NULL);
    // xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 4, NULL);
    xTaskCreate(web_server_task, "web_server_task", 4096, NULL, 3, NULL);
}
