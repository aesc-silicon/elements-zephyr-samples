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

K_THREAD_DEFINE(heartbeat_tid, HEARTBEAT_STACKSIZE, heartbeat, NULL, NULL, NULL,
		HEARTBEAT_PRIORITY, 0, 0);

K_THREAD_DEFINE(software0_key_tid, SOFTWARE0_KEY_STACKSIZE, software0_key,
		NULL, NULL, NULL, SOFTWARE0_KEY_PRIORITY, 0, 0);
