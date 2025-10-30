#ifndef SENSOR_H
#define SENSOR_H

#include <zephyr/drivers/sensor.h>

void sensor_thread(struct sensor_value *temp,
                   struct sensor_value *hum,
                   struct sensor_value *pres);

#endif
