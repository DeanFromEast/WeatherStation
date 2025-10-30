/**
 * @file uart_comm.c
 * @brief UART communication and WiFi interface using ESP8266.
 *
 * This module implements UART communication with the ESP8266 WiFi module,
 * including sending AT commands, establishing WiFi connections, and
 * transmitting sensor data via HTTP requests.
 */
#include "uart_comm.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <string.h>

// UART1 device (PA9/PA10 - verbonden met ESP8266)
static const struct device *uart_esp;

// Buffer voor ontvangen data
#define RX_BUFFER_SIZE 512
static char rx_buffer[RX_BUFFER_SIZE];
static volatile size_t rx_index = 0;
static K_SEM_DEFINE(rx_sem, 0, 1);

/**
 * @brief UART interrupt service routine for ESP8266 communication.
 *
 * Handles incoming UART data, stores it in the receive buffer,
 * and releases a semaphore when a newline character is received.
 *
 * @param dev Pointer to UART device.
 * @param user_data Unused.
 */
static void uart_isr(const struct device *dev, void *user_data)
{
    uint8_t c;
    
    if (!uart_irq_update(dev)) {
        return;
    }

    // Lees alle beschikbare bytes in één keer
    while (uart_irq_rx_ready(dev)) {
        if (uart_fifo_read(dev, &c, 1) == 1) {
            // Sla op in buffer
            if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = c;
                
                // Print direct naar console (geen buffering)
                printk("%c", c);
                
                // Als we een newline ontvangen, geef semafoor vrij
                if (c == '\n') {
                    k_sem_give(&rx_sem);
                }
            }
        }
    }
}

/**
 * @brief Initialize UART communication with the ESP8266 module.
 *
 * @return 1 on success, 0 if the UART device is not ready.
 */

int uart_comm_init(void)
{
    // Haal UART1 device op
    uart_esp = DEVICE_DT_GET(DT_NODELABEL(usart1));
    
    if (!device_is_ready(uart_esp)) {
        printk("Error: UART1 device not ready\n");
        return 0;
    }
    // Configureer interrupt callback
    uart_irq_callback_user_data_set(uart_esp, uart_isr, NULL);
    uart_irq_rx_enable(uart_esp);
    
    return 1;
}

/**
 * @brief Send an AT command over UART to the ESP8266.
 *
 * @param cmd Null-terminated AT command string (without \r\n).
 * @return 0 on success.
 */
int uart_send_at_command(const char *cmd)
{

    // Reset receive buffer
    rx_index = 0;
    memset(rx_buffer, 0, RX_BUFFER_SIZE);

    // Stuur het commando
    for (size_t i = 0; i < strlen(cmd); i++) {
        uart_poll_out(uart_esp, cmd[i]);
    }
    
    // Stuur CR+LF
    uart_poll_out(uart_esp, '\r');
    uart_poll_out(uart_esp, '\n');

    return 0;
}
/**
 * @brief Attempt to connect to a WiFi network.
 *
 * @param ssid WiFi SSID.
 * @param password WiFi password.
 * @return 1 on successful connection, 0 on failure or timeout.
 */
int wifi_connect(const char *ssid, const char *password)
{
    char cmd[128];
    char response[256];
    int ret;
    
    // Maak connectie commando
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, password); 

    uart_send_at_command(cmd);
    
    // Wacht en check meerdere keren
    for (int i = 0; i < 15; i++) {
        k_sleep(K_MSEC(1000));
        
        // Check of we "OK" of "WIFI CONNECTED" hebben ontvangen
        if (strstr(rx_buffer, "OK") != NULL || 
            strstr(rx_buffer, "WIFI CONNECTED") != NULL) {
            k_sleep(K_MSEC(2000));
            return 1;
        }
        
        // Check voor error
        if (strstr(rx_buffer, "FAIL") != NULL || 
            strstr(rx_buffer, "ERROR") != NULL) {
            return 0;
        }
    }   
    printk("\nConnection timeout\n");
    return 0;
}
/**
 * @brief Check if the ESP8266 is connected to a WiFi network.
 *
 * @return 1 if connected, 0 if not connected, -1 if unknown/error.
 */
int wifi_is_connected(void)
{
    // Check WiFi status via CWJAP
    rx_index = 0;
    memset(rx_buffer, 0, RX_BUFFER_SIZE);
    
    uart_send_at_command("AT+CWJAP?");
    k_sleep(K_MSEC(500));
     
    if (strstr(rx_buffer, "No AP") != NULL) {
        return 0;
    }
    if (strstr(rx_buffer, "+CWJAP:") != NULL) {
        return 1;
    }
    
    return -1; // Onbekend/Error
}
/**
 * @brief Send sensor data to a server via HTTP GET.
 *
 * @param host Server IP or hostname.
 * @param port Server port number.
 * @param temp Temperature value.
 * @param hum Humidity value.
 * @param pres Pressure value.
 * @return 0 on success, negative value on failure.
 */
int http_send_sensor_data(const char *host, 
                          int port,
                          float temp, 
                          float hum, 
                          float pres)
{
    char cmd[128];
    char http_request[256];
    int request_len;
    
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", host, port);
    
    rx_index = 0;
    memset(rx_buffer, 0, RX_BUFFER_SIZE);
    
    uart_send_at_command(cmd);
    
    // Wacht max 3 seconden op connectie
    for (int i = 0; i < 6; i++) {
        k_sleep(K_MSEC(500));
        
        if (strstr(rx_buffer, "CONNECT") != NULL || 
            strstr(rx_buffer, "ALREADY CONNECTED") != NULL) {
            break;
        }
        
        if (strstr(rx_buffer, "ERROR") != NULL) {
            return -1;
        }
        
        if (strstr(rx_buffer, "CLOSED") != NULL) {
            return -1;
        }
    }
    
    k_sleep(K_MSEC(500)); 
    
    // Stap 2: Bouw HTTP GET request
    snprintf(http_request, sizeof(http_request),
             "GET /data?temp=%.2f&hum=%.2f&pres=%.2f HTTP/1.0\r\nHost: %s\r\n\r\n",
             temp, hum, pres, host);
    
    request_len = strlen(http_request);
    
    // Stap 3: Stuur CIPSEND commando
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", request_len);
    
    rx_index = 0;
    memset(rx_buffer, 0, RX_BUFFER_SIZE);
    
    uart_send_at_command(cmd);
    k_sleep(K_MSEC(1000));
    
    // Wacht op '>' prompt
    if (strchr(rx_buffer, '>') == NULL) {
        printk("ESP8266 not ready to receive data\n");
        uart_send_at_command("AT+CIPCLOSE");
        k_sleep(K_MSEC(1000));
        return -2;
    }

    // Stap 4: Stuur de HTTP request data 
    uart_send_at_command(http_request);

    // Stap 5: Wacht op server respons
    k_sleep(K_MSEC(2000));
      
    // Stap 6: Sluit connectie
    uart_send_at_command("AT+CIPCLOSE");
    k_sleep(K_MSEC(500));
    
    return 0;
}