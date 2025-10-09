#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include "sensor.h"

#define SENSOR_INTERVAL_MS 2000

void sensor_thread(void)
{
    const struct device *dev = DEVICE_DT_GET_ANY(bosch_bme280);

    if (dev == NULL) {
        printk("Error: No BME280 device found in DeviceTree\n");
        return;
    }

    if (!device_is_ready(dev)) {
        printk("Error: BME280 device '%s' not ready\n", dev->name);
        return;
    }

    printk("BME280 device '%s' ready. Starting sensor loop...\n", dev->name);

    struct sensor_value temp, hum, pres;

    while (1) {
        if (sensor_sample_fetch(dev) < 0 ||
            sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0 ||
            sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &hum) < 0 ||
            sensor_channel_get(dev, SENSOR_CHAN_PRESS, &pres) < 0) {

            printk("Failed to read sensor\n");
        } else {
            printk("Temp: %d.%06d C, Hum: %d.%06d %%, Press: %d.%06d Pa\n",
                   temp.val1, temp.val2,
                   hum.val1, hum.val2,
                   pres.val1, pres.val2);
        }
        k_msleep(SENSOR_INTERVAL_MS);
    }
}
