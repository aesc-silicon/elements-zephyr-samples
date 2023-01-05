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
#define HEARTBEAT_STACKSIZE 2048

/* scheduling priority used by each thread */
#define HEARTBEAT_PRIORITY 5

#define HEARTBEAT_NODE		DT_PATH(leds, heartbeat)

#if !DT_NODE_HAS_STATUS(HEARTBEAT_NODE, okay)
#error "Unsupported board: heartbeat devicetree alias is not defined"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(HEARTBEAT_NODE, gpios);

void heartbeat(void) {
	int ret;

	if (!device_is_ready(led.port)) {
		return;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	while (1) {
		ret = gpio_pin_set_dt(&led, 1);
		if (ret < 0) {
			return;
		}
		k_sleep(K_MSEC(150));

		ret = gpio_pin_set_dt(&led, 0);
		if (ret < 0) {
			return;
		}
		k_sleep(K_MSEC(50));

		ret = gpio_pin_set_dt(&led, 1);
		if (ret < 0) {
			return;
		}
		k_sleep(K_MSEC(150));

		ret = gpio_pin_set_dt(&led, 0);
		if (ret < 0) {
			return;
		}
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(heartbeat_tid, HEARTBEAT_STACKSIZE, heartbeat, NULL, NULL, NULL,
		HEARTBEAT_PRIORITY, 0, 0);
