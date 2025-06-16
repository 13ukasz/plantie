#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

/**
 * @brief Get the current temperature reading.
 *
 * This function returns the most recently read temperature value
 * from the sensor.
 *
 * @return Current temperature in degrees Celsius.
 */
float get_temperature(void);

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
