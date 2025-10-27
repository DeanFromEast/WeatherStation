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

// Verbindt ESP8266 met WiFi
// Parameters:
//   ssid: WiFi naam
//   password: WiFi wachtwoord
void esp_connect_wifi(const char* ssid, const char* password);

// Stuurt een HTTP GET request via ESP8266 AT
// Parameters:
//   ip: server IP (bijv. "192.168.2.55")
//   port: server poort (bijv. 3000)
//   path: URL path inclusief query (bijv. "/data?temp=20&hum=70&pres=999")
void esp_send_get(const char* ip, int port, const char* path);

#endif // ESP_HTTP_H
