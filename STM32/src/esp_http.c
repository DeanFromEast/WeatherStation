#include "esp_http.h"
#include <string.h>

void esp_connect_wifi(const char* ssid, const char* password) {
    // Zet ESP in station mode
    printf("AT+CWMODE=1\r\n");
    // Wacht op OK...

    // Verbind met WiFi
    printf("AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    // Wacht op "WIFI CONNECTED" en "WIFI GOT IP"
}

void esp_send_get(const char* ip, int port, const char* path) {
    char cmd[256];
    int len;

    // Start TCP verbinding
    printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ip, port);
    // Wacht op CONNECT

    // Bereken lengte van GET-request
    len = snprintf(cmd, sizeof(cmd),
                   "GET %s HTTP/1.1\r\n"
                   "Host: %s:%d\r\n"
                   "Connection: close\r\n\r\n",
                   path, ip, port);

    // Stuur lengte naar ESP
    printf("AT+CIPSEND=%d\r\n", len);
    // Wacht op '>'

    // Stuur GET-request
    printf("%s", cmd);
}
