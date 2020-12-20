/*
 * Copyright (c) 2020 Phytec Messtechnik GmbH
 *
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
#define STACKSIZE 2048

/* scheduling priority used by each thread */
#define PRE_PRIORITY 7
#define COP_PRIORITY -7

void irq_handler(const struct device *gpio, struct gpio_callback *cb,
		 uint32_t pins)
{
	printk("IRQ triggered for pin %i at %d\n", pins, k_cycle_get_32());
}

static struct gpio_callback gpio_cb[1];

void startup_irq(void)
{
	const struct device *gpio_dev;
	int ret;

	gpio_dev = device_get_binding("gpio1");
	__ASSERT_NO_MSG(gpio_dev != NULL);

	ret = gpio_pin_configure(gpio_dev, 2, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: Failed to configure pin 2\n", ret);
		return;
	}
	ret = gpio_pin_configure(gpio_dev, 3, GPIO_OUTPUT_LOW);
	if (ret != 0) {
		printk("Error %d: Failed to configure pin 2\n", ret);
		return;
	}
	ret = gpio_pin_interrupt_configure(gpio_dev, 2, GPIO_INT_EDGE_FALLING);
	if (ret != 0) {
		printk("Error %d: Failed to configure interrupt on pin 2\n",
		       ret);
		return;
	}

	gpio_init_callback(&gpio_cb[0], irq_handler, BIT(2));
	gpio_add_callback(gpio_dev, &gpio_cb[0]);

	while (1) {
		gpio_pin_set(gpio_dev, 3, 1);
		gpio_pin_set(gpio_dev, 3, 0);
		k_msleep(2000);
	}
}

void startup_gpio(void)
{
	const struct device *gpio_dev;
	unsigned int read = 1;
	unsigned int write = 1;

	gpio_dev = device_get_binding("gpio1");
	__ASSERT_NO_MSG(gpio_dev != NULL);

	gpio_pin_configure(gpio_dev, 0, GPIO_INPUT);
	gpio_pin_configure(gpio_dev, 1, GPIO_OUTPUT_LOW);

	while (1) {
		gpio_pin_set(gpio_dev, 1, write);
		k_msleep(2000);
		read = gpio_pin_get(gpio_dev, 0);
		if (read != write) {
			printk("Read (%i) != write (%i) at %d\n", read, write,
			       k_cycle_get_32());
		}
		write = !write;
	}
}

K_THREAD_DEFINE(startup_gpio_id, STACKSIZE, startup_gpio, NULL, NULL, NULL,
		PRE_PRIORITY, 0, 0);

K_THREAD_DEFINE(startup_irq_id, STACKSIZE, startup_irq, NULL, NULL, NULL,
		PRE_PRIORITY, 0, 0);
