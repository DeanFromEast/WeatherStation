// CODE for the STM 32 that recieves sensor input and send AT commands to ESP8266

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define UART_DEVICE_NODE DT_NODELABEL(usart2)
static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

static void send_uart(const char *str)
{
  for (const char *p = str; *p; p++)
  {
    uart_poll_out(uart_dev, *p);
  }
}

void main(void)
{
  if (!device_is_ready(uart_dev))
  {
    printk("UART not ready!\n");
    return;
  }

  printk("Connecting to WiFi...\n");

  // 1. Set WiFi mode
  send_uart("AT+CWMODE=1\r\n");
  k_sleep(K_SECONDS(1));

  // 2. Connect to WiFi (replace SSID and PASSWORD)
  send_uart("AT+CWJAP=\"Nummer 5\",\"Molenplein_5+0183505497\"\r\n");
  k_sleep(K_SECONDS(5));

  // 3. Start TCP connection to Node server (replace IP and PORT)
  send_uart("AT+CIPSTART=\"TCP\",\"192.168.1.134\",3000\r\n");
  k_sleep(K_SECONDS(2));

  // 4. Prepare POST request
  const char *post_data = "temperature=25&humidity=50";
  char buf[256];
  int len = snprintf(buf, sizeof(buf),
                     "POST /data HTTP/1.1\r\n"
                     "Host: 192.168.1.134:3000\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n"
                     "Content-Length: %d\r\n\r\n"
                     "%s",
                     strlen(post_data), post_data);

  // 5. Tell ESP8266 how many bytes to send
  char send_cmd[32];
  snprintf(send_cmd, sizeof(send_cmd), "AT+CIPSEND=%d\r\n", len);
  send_uart(send_cmd);
  k_sleep(K_SECONDS(1));

  // 6. Send actual POST request
  send_uart(buf);

  printk("POST request sent.\n");
}
