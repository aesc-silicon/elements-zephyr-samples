/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>

#include "../../../common/heartbeat.h"

/* size of stack area used by each thread */
#define FREQUENCY_STACKSIZE 2048

/* scheduling priority used by each thread */
#define FREQUENCY_PRIORITY 5

void reset(void)
{
	printk("Trigger cold reboot.\n");
	k_sleep(K_MSEC(2));
	sys_reboot(SYS_REBOOT_COLD);

	k_sleep(K_MSEC(1));
	printk("Reset failed!\n");

	while (1) {
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(heartbeat_tid, HEARTBEAT_STACKSIZE, heartbeat, NULL, NULL, NULL,
		HEARTBEAT_PRIORITY, 0, 0);

K_THREAD_DEFINE(reset_tid, FREQUENCY_STACKSIZE, reset, NULL, NULL, NULL,
		FREQUENCY_PRIORITY, 0, 0);
