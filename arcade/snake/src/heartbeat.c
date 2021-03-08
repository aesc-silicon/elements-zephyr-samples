/*
 * Copyright (c) 2021 Phytec Messtechnik GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/__assert.h>
#include "heartbeat.h"
#include "threads.h"

void heartbeat(void)
{
	const struct device *led_dev;

	led_dev = device_get_binding(LED_HEARTBEAT_CTRL);
	__ASSERT_NO_MSG(led_dev != NULL);

	gpio_pin_configure(led_dev, LED_HEARTBEAT_PIN, LED_HEARTBEAT_FLAGS);

	while (1) {
		gpio_pin_set(led_dev, LED_HEARTBEAT_PIN, 1);
		k_sleep(K_MSEC(150));
		gpio_pin_set(led_dev, LED_HEARTBEAT_PIN, 0);
		k_sleep(K_MSEC(50));
		gpio_pin_set(led_dev, LED_HEARTBEAT_PIN, 1);
		k_sleep(K_MSEC(150));
		gpio_pin_set(led_dev, LED_HEARTBEAT_PIN, 0);
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(heartbeat_tid, HEARTBEAT_STACKSIZE, heartbeat, NULL, NULL, NULL,
		HEARTBEAT_PRIORITY, 0, 0);
