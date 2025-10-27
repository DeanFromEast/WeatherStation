/**
 * @file sensor.h
 * @brief Interface for the BME280 environmental sensor driver.
 *
 * Provides initialization and reading functions for temperature, humidity, and pressure.
 */

#pragma once

#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

 /**
  * @brief Initialize the BME280 sensor.
  *
  * Detects the BME280 device and verifies readiness.
  */
void sensor_startup(void);

/**
 * @brief Retrieve the latest sensor values from the BME280.
 *
 * Reads temperature, humidity, and pressure from the BME280.
 *
 * @param temp Pointer to store temperature
 * @param hum Pointer to store humidity
 * @param pres Pointer to store pressure
 */
void get_sensor_values(struct sensor_value *temp,
                       struct sensor_value *hum,
                       struct sensor_value *pres);

