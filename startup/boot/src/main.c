/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <sys/printk.h>
#include <sys/__assert.h>
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
