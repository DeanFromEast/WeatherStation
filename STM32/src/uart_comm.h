/**
 * @file uart_comm.h
 * @brief UART communication interface for ESP8266 AT firmware.
 *
 * Provides functions for WiFi management and sending sensor data to a server
 * through the ESP8266 using AT commands.
 */

#pragma once

#include <zephyr/drivers/sensor.h>

 /**
  * @brief Send the latest sensor values via UART to the ESP8266.
  *
  * Builds and sends an HTTP GET request using the ESP8266 AT command set.
  *
  * @param temp Pointer to temperature value
  * @param hum Pointer to humidity value
  * @param pres Pointer to pressure value
  */
void uart_send_values(const struct sensor_value *temp,
                      const struct sensor_value *hum,
                      const struct sensor_value *pres);

/**
 * @brief Check the current WiFi connection status.
 */
void check_wifi_status(void);

                      