#include "uart_comm.h"
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define UART_NODE_ESP DT_NODELABEL(usart2)

static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE_ESP);
static bool wifi_connected = false;

/* --- Basis AT-command functie --- */
static void send_at_command(const char *cmd)
{
    if (!device_is_ready(uart_dev)) {
        printk("ESP UART not ready\n");
        return;
    }

    for (size_t i = 0; i < strlen(cmd); i++) {
        uart_poll_out(uart_dev, cmd[i]);
    }
    uart_poll_out(uart_dev, '\r');
    uart_poll_out(uart_dev, '\n');
    k_msleep(300);
}

/* --- Debug print via UART --- */
void debug_print(const char *msg)
{
    if (!device_is_ready(uart_dev)) {
        printk("UART not ready\n");
        return;
    }

    for (size_t i = 0; i < strlen(msg); i++) {
        uart_poll_out(uart_dev, msg[i]);
    }
    uart_poll_out(uart_dev, '\n');
}

/* --- Lees volledige ESP-respons --- */
bool uart_read_response_for_ok(char *buf, size_t buf_size, int timeout_ms)
{
    int idx = 0;
    uint8_t c;
    int waited = 0;

    while (waited < timeout_ms && idx < (int)(buf_size - 1)) {
        if (uart_poll_in(uart_dev, &c) == 0) {
            buf[idx++] = c;
            waited = 0; // reset timeout bij ontvangen byte
        } else {
            k_msleep(10);
            waited += 10;
        }
    }

    buf[idx] = '\0';

    if (idx == 0) {
        debug_print("ESP antwoord: (geen data ontvangen!)");
        return false;
    } else {
        debug_print("ESP antwoord:");
        debug_print(buf);

        // check of antwoord "OK" bevat
        if (strstr(buf, "OK") != NULL) {
            return true;
        }
        return false;
    }
}

/* --- Verbind ESP met WiFi met retry --- */
void esp_connect_wifi(void)
{
    if (wifi_connected) {
        debug_print("ESP al verbonden met WiFi.");
        return;
    }

    debug_print("Reset ESP en verbinden met WiFi...");

    send_at_command("AT+RST");
    k_msleep(5000); // wacht op reset
    send_at_command("AT");
    k_msleep(500);

    char join_cmd[128];
    snprintf(join_cmd, sizeof(join_cmd),
             "AT+CWJAP=\"WIFISPIJKERBOER\",\"Spijker2392017\"");

    bool connected = false;
    int attempts = 0;
    const int max_attempts = 3;
    char response[256];

    while (!connected && attempts < max_attempts) {
        attempts++;
        send_at_command(join_cmd);

        // lees maximaal 10 seconden op antwoord
        connected = uart_read_response_for_ok(response, sizeof(response), 10000);
        if (!connected) {
            debug_print("WiFi verbinding mislukt, opnieuw proberen...");
            k_msleep(3000);
        }
    }

    if (connected) {
        wifi_connected = true;
        debug_print("ESP succesvol verbonden met WiFi.");
    } else {
        debug_print("Kon geen WiFi verbinding maken.");
    }
}

/* --- Controleer WiFi status --- */
void check_wifi_status(void)
{
    debug_print("ESP8266 status check verzonden.");
    send_at_command("AT+CWJAP?");
    char buf[256];
    uart_read_response_for_ok(buf, sizeof(buf), 3000);
}

/* --- Verzend sensorwaarden naar server --- */
void uart_send_values(const struct sensor_value *temp,
                      const struct sensor_value *hum,
                      const struct sensor_value *pres)
{
    if (!wifi_connected) {
        debug_print("WiFi niet verbonden. Eerst verbinden!");
        esp_connect_wifi();
        if (!wifi_connected) {
            debug_print("Kan sensorwaarden niet verzenden zonder WiFi.");
            return;
        }
    }

    char http_payload[200];
    int payload_len = snprintf(http_payload, sizeof(http_payload),
        "GET /data?temp=%d.%06d&hum=%d.%06d&pres=%d.%06d HTTP/1.1\r\n"
        "Host: 145.49.98.153:3000\r\n"
        "Connection: close\r\n\r\n",
        temp->val1, temp->val2,
        hum->val1, hum->val2,
        pres->val1, pres->val2);

    if (payload_len <= 0 || payload_len >= (int)sizeof(http_payload)) {
        debug_print("Payload snprintf failed");
        return;
    }

    send_at_command("AT+CIPSTART=\"TCP\",\"145.49.98.153\",3000");
    k_msleep(500);

    char cipsend_cmd[64];
    snprintf(cipsend_cmd, sizeof(cipsend_cmd), "AT+CIPSEND=%d", payload_len);
    send_at_command(cipsend_cmd);

    for (int i = 0; i < payload_len; i++) {
        uart_poll_out(uart_dev, http_payload[i]);
    }

    k_msleep(500);
    send_at_command("AT+CIPCLOSE");
}
