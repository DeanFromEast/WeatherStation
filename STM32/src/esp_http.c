/**
 * @file esp_http.c
 * @brief Implementation of basic HTTP communication via ESP8266 AT commands.
 */

#include "esp_http.h"
#include <string.h>
 

void esp_connect_wifi(const char* ssid, const char* password) {
    // Set ESP in station mode
    printf("AT+CWMODE=1\r\n");
    // Wait for OK...

    // Connect to WiFi
    printf("AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    // Wait for "WIFI CONNECTED" and "WIFI GOT IP"
}


void esp_send_get(const char* ip, int port, const char* path) {
    char cmd[256];
    int len;

    // Start TCP connection
    printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ip, port);
    // Wait for CONNECT

    // Build GET request
    len = snprintf(cmd, sizeof(cmd),
                   "GET %s HTTP/1.1\r\n"
                   "Host: %s:%d\r\n"
                   "Connection: close\r\n\r\n",
                   path, ip, port);

    // Send length
    printf("AT+CIPSEND=%d\r\n", len);
    // Wait for '>'

    // Send GET-request
    printf("%s", cmd);
}
