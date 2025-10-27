/**
 * @file esp_http.h
 * @brief Simple ESP8266 HTTP client using AT commands.
 *
 * This module provides basic WiFi connection and HTTP GET functionality
 * for an ESP8266 module via UART AT commands.
 *
 * @date 2025-10-27
 * @author Rik
 */

#ifndef WIFI_COMM_H
#define WIFI_COMM_H

#include <zephyr/kernel.h>
#include <stdbool.h>

/**
 * @brief Initialiseer de UART en verbind met WiFi.
 */
void wifi_connect(void);

#endif // WIFI_COMM_H
#ifndef ESP_HTTP_H
#define ESP_HTTP_H

#include <stdio.h>

/**
 * @brief Connect to a WiFi network using the ESP8266.
 *
 * Sends AT commands to configure the ESP8266 and connect to the specified network.
 *
 * @param ssid WiFi SSID (network name)
 * @param password WiFi password
 */
void esp_connect_wifi(const char* ssid, const char* password);

/**
 * @brief Send an HTTP GET request through the ESP8266.
 *
 * Opens a TCP connection and transmits an HTTP GET request to the given IP and port.
 *
 * @param ip Server IP address
 * @param port Server TCP port (e.g., 3000)
 * @param path URL path including query parameters (e.g., "/data?temp=20&hum=70&pres=999")
 */
void esp_send_get(const char* ip, int port, const char* path);

#endif // ESP_HTTP_H
