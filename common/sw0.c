/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/__assert.h>
#include "sw0.h"


static const struct gpio_dt_spec key = GPIO_DT_SPEC_GET(SOFTWARE0_KEY_NODE,
							gpios);
static struct gpio_callback key_cb_data;

void software0_handler(const struct device *gpio, struct gpio_callback *cb,
		 uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

void software0_key(void)
{
	int ret;

	if (!device_is_ready(key.port)) {
		return;
	}

	ret = gpio_pin_configure_dt(&key, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: Failed to configure sw0 key\n", ret);
		return;
	}

	ret = gpio_pin_interrupt_configure_dt(&key, GPIO_INT_EDGE_RISING);
	if (ret != 0) {
		printk("Error %d: Failed to configure interrupt on pin 3\n",
		       ret);
		return;
	}

	gpio_init_callback(&key_cb_data, software0_handler, BIT(key.pin));
	gpio_add_callback(key.port, &key_cb_data);

	while (1) {
		k_sleep(K_SECONDS(1));
	}
}
