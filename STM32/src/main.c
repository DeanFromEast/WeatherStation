#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "sensor.h"  

#define STACKSIZE 1024
#define PRIORITY 5



K_THREAD_DEFINE(sensor_id, STACKSIZE, sensor_thread, NULL, NULL, NULL, PRIORITY, 0, 0);

void main(void)
{
    printk("Zephyr sensor app gestart\n");
}
