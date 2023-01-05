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

/* size of stack area used by each thread */
#define NOTHING_STACKSIZE 2048

/* scheduling priority used by each thread */
#define NOTHING_PRIORITY 5

void nothing(void)
{
	while (1) {}
}

K_THREAD_DEFINE(nothing_tid, NOTHING_STACKSIZE, nothing, NULL, NULL, NULL,
		NOTHING_PRIORITY, 0, 0);
