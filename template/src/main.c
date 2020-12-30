/*
 * Copyright (c) 2020 Phytec Messtechnik GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <drivers/i2c.h>
#include <drivers/spi.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <string.h>

/* size of stack area used by each thread */
#define TEMPLATE_STACKSIZE 2048

/* scheduling priority used by each thread */
#define TEMPLATE_PRIORITY 5

void template(void)
{
	const struct device *dev;
	gpio_pin_t pin = 0;
	unsigned int value = 0;

	dev = device_get_binding("gpio0");
	__ASSERT_NO_MSG(dev != NULL);

	while (1) {
		value = gpio_pin_get(dev, pin);
		printk("GPIO Value: %x!\n", value);
		k_sleep(K_SECONDS(5));
	}
}

K_THREAD_DEFINE(template_tid, TEMPLATE_STACKSIZE, template, NULL, NULL, NULL,
		TEMPLATE_PRIORITY, 0, 0);
