#include "sensor.h"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

static const struct device* g_dev = NULL;  

void sensor_startup(void)
{
    g_dev = DEVICE_DT_GET_ANY(bosch_bme280);

    if (g_dev == NULL || !device_is_ready(g_dev)) {
        printk("Error: BME280 device not ready\n");
        return;
    }

    printk("BME280 ready.\n");
}

void get_sensor_values(struct sensor_value* temp,
    struct sensor_value* hum,
    struct sensor_value* pres)
{
    if (g_dev == NULL) {
        printk("Error: Sensor device not initialized\n");
        return;
    }

    struct sensor_value t, h, p;

    if (sensor_sample_fetch(g_dev) == 0 &&
        sensor_channel_get(g_dev, SENSOR_CHAN_AMBIENT_TEMP, &t) == 0 &&
        sensor_channel_get(g_dev, SENSOR_CHAN_HUMIDITY, &h) == 0 &&
        sensor_channel_get(g_dev, SENSOR_CHAN_PRESS, &p) == 0) {

        if (temp) *temp = t;
        if (hum)  *hum = h;
        if (pres) *pres = p;

    }
    else {
        printk("Sensor read failed\n");
    }
}
