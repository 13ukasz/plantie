#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ds18b20.h"
#include "onewire_bus.h"

#define ONEWIRE_BUS_GPIO 1

static float temperature = 0.0;
static ds18b20_device_handle_t ds18b20 = NULL;
static onewire_bus_handle_t bus = NULL;

static const char *TAG = "DS18B20";

void sensor_init(void)
{
    onewire_bus_config_t bus_config = {
        .bus_gpio_num = ONEWIRE_BUS_GPIO,
    };
    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 10,
    };
    ESP_ERROR_CHECK(onewire_new_bus_rmt(&bus_config, &rmt_config, &bus));

    onewire_device_t             device;
    onewire_device_iter_handle_t iter;

    ESP_ERROR_CHECK(onewire_new_device_iter(bus, &iter));
    if (onewire_device_iter_get_next(iter, &device) == ESP_OK) {
        ds18b20_config_t ds_cfg = {};
        ESP_ERROR_CHECK(ds18b20_new_device(&device, &ds_cfg, &ds18b20));
        ESP_LOGI(TAG, "DS18B20 sensor initialized, address: %016llX", device.address);
    } else {
        ESP_LOGE(TAG, "No DS18B20 device found!");
    }
    ESP_ERROR_CHECK(onewire_del_device_iter(iter));
}

void sensor_read(void)
{
    ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion(ds18b20));
    ESP_ERROR_CHECK(ds18b20_get_temperature(ds18b20, &temperature));
    ESP_LOGI(TAG, "Temperature: %.2f°C", temperature);
}

void sensor_read_task(void *pvParameters)
{
    sensor_init();
    
    while (1) {
        sensor_read();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
