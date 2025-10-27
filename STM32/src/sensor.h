#pragma once

#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

void sensor_startup(void);

/**
 * @brief Haal de laatste sensorwaarden op.
 *
 * @param temp pointer waar temperatuur wordt opgeslagen
 * @param hum  pointer waar luchtvochtigheid wordt opgeslagen
 * @param pres pointer waar druk wordt opgeslagen
 */
void get_sensor_values(struct sensor_value *temp,
                       struct sensor_value *hum,
                       struct sensor_value *pres);

