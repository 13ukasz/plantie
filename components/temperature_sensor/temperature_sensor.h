#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

/** 
 * @brief Current temperature in degrees Celsius.
 * 
 * This global variable holds the latest measured or computed temperature 
 * value. It is used throughout the system for temperature monitoring, 
 * control logic, and logging.
 * 
 * @note The variable should be updated periodically by a temperature 
 *       sensor reading function.
 */
extern float temperature;

/**
 * @brief FreeRTOS task to periodically read temperature sensor.
 *
 * This task is intended to be run in a FreeRTOS environment. It reads
 * the temperature sensor at regular intervals and updates the
 * internally stored temperature value used by get_temperature().
 *
 * @param pvParameters Pointer to task parameters (unused).
 */
void sensor_read_task(void *pvParameters);

#endif // TEMPERATURE_SENSOR_H
