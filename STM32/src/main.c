/**
 * @file main.c
 * @brief Main application entry for the weather station.
 *
 * Initializes sensors, connects to WiFi via ESP8266, and
 * periodically sends measurements to an HTTP server.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "sensor.h"
#include "uart_comm.h"
#include "esp_http.h"

#define STACKSIZE 1024
#define PRIORITY 5

K_THREAD_DEFINE(sensor_id, STACKSIZE, sensor_thread, NULL, NULL, NULL, PRIORITY, 0, 0);

void main(void)
{
    struct sensor_value t, h, p;

	sensor_startup();

    while (1) {
        get_sensor_values(&t, &h, &p);

        printk("Main(): T:%d.%06d H:%d.%06d P:%d.%06d\n",
               t.val1, t.val2, h.val1, h.val2, p.val1, p.val2);

        // Connect to iPhone hotspot
    esp_connect_wifi("Iphone van Rik", "12345678");

    // Send GET-request
    esp_send_get("192.168.2.55", 3000, "/data?temp=20&hum=70&pres=999");
    }
}
