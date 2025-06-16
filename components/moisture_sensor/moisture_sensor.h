#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H

/**
 * @brief Get the current moisture reading.
 *
 * This function returns the most recently read ADC voltage value
 *
 * @return Current temperature in degrees Celsius.
 */
int get_moisture(void);

/**
 * @brief Task to read moisture sensor data.
 *
 * This function runs as a FreeRTOS task. It reads raw ADC values from the moisture
 * sensor, processes the data, and logs the results in an infinite loop.
 *
 * @param pvParameters Unused task parameters.
 */
void moisture_sensor_task(void *pvParameters);

#endif // MOISTURE_SENSOR_H
