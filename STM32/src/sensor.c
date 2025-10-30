/**
 * @file sensor.c
 * @brief Sensor interface implementation for the BME280 environmental sensor.
 *
 * This module handles sensor reading and data retrieval for temperature,
 * humidity, and pressure using the Zephyr sensor API.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include "sensor.h"

/**
 * @brief Read temperature, humidity, and pressure values from the BME280 sensor.
 *
 * @param[out] temp Pointer to store the temperature reading.
 * @param[out] hum  Pointer to store the humidity reading.
 * @param[out] pres Pointer to store the pressure reading.
 *
 * @note This function blocks until all sensor channels have been fetched.
 * It assumes a BME280 sensor is present and ready.
 */

void sensor_thread(struct sensor_value *temp,
                   struct sensor_value *hum,
                   struct sensor_value *pres)
{
    const struct device *dev = DEVICE_DT_GET_ANY(bosch_bme280);

    if (!dev || !device_is_ready(dev)) {
        printk("Error: BME280 device not ready\n");
        return;
    }

    if (sensor_sample_fetch(dev) < 0 ||
        sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, temp) < 0 ||
        sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, hum) < 0 ||
        sensor_channel_get(dev, SENSOR_CHAN_PRESS, pres) < 0) {
        printk("Failed to read sensor\n");
        return;
    }
}
