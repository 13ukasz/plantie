#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

#include "moisture_sensor.h"
#include "temperature_sensor.h"
#include "web_server.h"
#include "zigbee_relay.h"

void app_main(void)
{
    xTaskCreate(moisture_sensor_task, "moisture_sensor_fun", 2048, NULL, 2, NULL);
    // xTaskCreate(sensor_read_task, "sensor_read_task", 4096, NULL, 5, NULL);
    xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 5, NULL);
    xTaskCreate(web_server_task, "web_server_task", 4096, NULL, 1, NULL);
}
