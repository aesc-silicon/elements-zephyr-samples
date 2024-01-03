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
#include "../../../common/sw0.h"

#define SEVEN_SEGMENT_STACKSIZE		256
#define SEVEN_SEGMENT_PRIORITY		5


void seven_segment() {
	volatile uint32_t *seven_segment = (volatile uint32_t *)0xf0014010;
	unsigned int counter = 0;

	while (1) {
		*seven_segment = counter;
		k_sleep(K_SECONDS(1));
		counter++;
	}
}


K_THREAD_DEFINE(seven_segment_tid, SEVEN_SEGMENT_STACKSIZE, seven_segment,
		NULL, NULL, NULL, SEVEN_SEGMENT_PRIORITY, 0, 0);

K_THREAD_DEFINE(heartbeat_tid, HEARTBEAT_STACKSIZE, heartbeat, NULL, NULL, NULL,
		HEARTBEAT_PRIORITY, 0, 0);

K_THREAD_DEFINE(software0_key_tid, SOFTWARE0_KEY_STACKSIZE, software0_key,
		NULL, NULL, NULL, SOFTWARE0_KEY_PRIORITY, 0, 0);
