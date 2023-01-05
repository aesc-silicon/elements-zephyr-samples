/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>

#include "../../../common/heartbeat.h"

/* size of stack area used by each thread */
#define FREQUENCY_STACKSIZE 2048

/* scheduling priority used by each thread */
#define FREQUENCY_PRIORITY 5

void frequency(void)
{
	volatile unsigned int *device = (unsigned int *)0xF0060000;
	unsigned int divider = *(device + 1);
	unsigned int count;
	unsigned int duration;
	unsigned int external_clock;
	unsigned int frequency;
	unsigned int value;

	k_sleep(K_MSEC(3));
	while (1) {
		count = *(device + 2);
		duration = count * 10;
		external_clock = duration * 1000 / divider;
		frequency = (1 * 1000 * 1000 * 1000) / external_clock;
		value = frequency * 1000;
		printk("%i Hz (%i counts)\n", value, count);
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(heartbeat_tid, HEARTBEAT_STACKSIZE, heartbeat, NULL, NULL, NULL,
		HEARTBEAT_PRIORITY, 0, 0);

K_THREAD_DEFINE(frequency_tid, FREQUENCY_STACKSIZE, frequency, NULL, NULL, NULL,
		FREQUENCY_PRIORITY, 0, 0);
