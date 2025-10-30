/**
 * @file uart_comm.h
 * @brief UART communication and WiFi control interface for ESP8266.
 */
#ifndef UART_COMM_H
#define UART_COMM_H

#include <zephyr/kernel.h>

/**
 * @brief Initialiseer UART communicatie met ESP8266
 * @return 1 bij succes, 0 bij fout
 */
int uart_comm_init(void);

/**
 * @brief Stuur een AT commando naar ESP8266
 * @param cmd Het AT commando (zonder \r\n, wordt automatisch toegevoegd)
 * @return 0 bij succes, negatieve waarde bij fout
 */
int uart_send_at_command(const char *cmd);

/**
 * @brief Verbind met WiFi netwerk
 * @param ssid WiFi netwerk naam
 * @param password WiFi wachtwoord
 * @return 1 bij succes, 0 bij fout
 */
int wifi_connect(const char *ssid, const char *password);

/**
 * @brief Check of ESP8266 verbonden is met WiFi
 * @return 1 als verbonden, 0 als niet verbonden, negatief bij fout
 */
int wifi_is_connected(void);

/**
 * @brief Stuur HTTP GET request met sensor data
 * @param host Server IP adres 
 * @param port Server poort
 * @param temp Temperatuur waarde
 * @param hum Vochtigheid waarde  
 * @param pres Druk waarde
 * @return 0 bij succes, negatieve waarde bij fout
 */
int http_send_sensor_data(const char *host, 
                          int port,
                          float temp, 
                          float hum, 
                          float pres);

#endif // UART_COMM_H