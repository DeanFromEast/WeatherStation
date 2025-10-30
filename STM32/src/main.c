/**
 * @file main.c
 * @brief Main application file for the Zephyr-based weather station.
 *
 * This file initializes the UART communication with the ESP8266 WiFi module,
 * connects to a WiFi network, reads environmental sensor data from a BME280 sensor,
 * and periodically sends the measurements to a remote server over HTTP.
 *
 * @details
 * The main loop performs the following steps:
 * 1. Ensures the WiFi module is connected.
 * 2. Reads temperature, humidity, and pressure data.
 * 3. Converts the sensor readings to floating-point values.
 * 4. Sends the data to the server via an HTTP GET request.
 *
 * The loop repeats every `SLEEP_TIME_MS` milliseconds.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include "sensor.h"
#include "uart_comm.h"

/** @brief WiFi network SSID. */
#define WIFI_SSID "iPhone van Daan"
/** @brief WiFi network password. */
#define WIFI_PASSWORD "yeahyeahyeah"

/** @brief IP address of the remote server. */
#define SERVER_IP "172.20.10.2"
/** @brief Port of the remote server. */
#define SERVER_PORT 3000

/** @brief Delay between sensor data transmissions in milliseconds. */
#define SLEEP_TIME_MS 10000 

/**
 * @brief Main entry point for the weather station application.
 *
 * Initializes UART, connects to WiFi, continuously reads sensor data,
 * and sends it to the configured server.
 *
 * @return This function never returns.
 */

void main(void)
{
    struct sensor_value temp, hum, pres;
    float temp_float, hum_float, pres_float;
    int wifi_status;
    int ret;

    printk("\n=== Weather Station Starting ===\n");

    // Initialiseer UART communicatie met ESP8266
    while (!uart_comm_init()) {
        printk("Failed to initialize UART\n");
    }
    k_sleep(K_MSEC(2000));

    // WiFi Connectie
    uart_send_at_command("AT+CWMODE=1");
    k_sleep(K_MSEC(1000));

    wifi_connect(WIFI_SSID, WIFI_PASSWORD);

    printk("\n=== Starting main loop ===\n");

    while (1) {
         
        // Check WiFi status
        if (!wifi_is_connected()) {
            printk("\nWifi not connected, attempting reconnect\n");
            wifi_connect(WIFI_SSID, WIFI_PASSWORD);
            continue;
        }

        // Lees sensor data
        sensor_thread(&temp, &hum, &pres);

        // Converteer sensor_value naar float
        temp_float = temp.val1 + (temp.val2 / 1000000.0f);
        hum_float = hum.val1 + (hum.val2 / 1000000.0f);
        pres_float = pres.val1 + (pres.val2 / 1000000.0f);

        // Stuur data naar server via HTTP GET
        ret = http_send_sensor_data(SERVER_IP, SERVER_PORT, 
                                     temp_float, hum_float, pres_float);
        
        if (ret != 0) {
            printk("Failed to send data to server\n");
        } 

        k_sleep(K_MSEC(SLEEP_TIME_MS));
    }
}