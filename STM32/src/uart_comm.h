#pragma once

#include <zephyr/drivers/sensor.h>

/* verstuurt de laatste sensorwaarden via UART (naar ESP met AT-firmware) */
void uart_send_values(const struct sensor_value *temp,
                      const struct sensor_value *hum,
                      const struct sensor_value *pres);

void check_wifi_status(void);

                      