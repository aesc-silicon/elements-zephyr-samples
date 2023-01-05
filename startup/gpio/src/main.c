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
#define STACKSIZE 2048

/* scheduling priority used by each thread */
#define PRE_PRIORITY 7

void irq_handler(const struct device *gpio, struct gpio_callback *cb,
		 uint32_t pins)
{
	printk("IRQ: %i\n", pins);
}

static struct gpio_callback gpio_cb[1];

void startup_irq(void)
{
	const struct device *gpio_dev;
	int ret;

	gpio_dev = device_get_binding("gpioACtrl");
	__ASSERT_NO_MSG(gpio_dev != NULL);

	ret = gpio_pin_configure(gpio_dev, 3, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: Failed to configure pin 3\n", ret);
		return;
	}
	ret = gpio_pin_configure(gpio_dev, 4, GPIO_OUTPUT_LOW);
	if (ret != 0) {
		printk("Error %d: Failed to configure pin 4\n", ret);
		return;
	}

	ret = gpio_pin_interrupt_configure(gpio_dev, 3, GPIO_INT_EDGE_RISING);
	if (ret != 0) {
		printk("Error %d: Failed to configure interrupt on pin 3\n",
		       ret);
		return;
	}

	gpio_init_callback(&gpio_cb[0], irq_handler, BIT(3));
	gpio_add_callback(gpio_dev, &gpio_cb[0]);

	while (1) {
		gpio_pin_set(gpio_dev, 4, 1);
		gpio_pin_set(gpio_dev, 4, 0);
		k_msleep(1);
	}
}

void startup_gpio(void)
{
	const struct device *gpio_dev;
	unsigned int write = 1;
	unsigned int read;

	gpio_dev = device_get_binding("gpioACtrl");
	__ASSERT_NO_MSG(gpio_dev != NULL);

	gpio_pin_configure(gpio_dev, 0, GPIO_OUTPUT_LOW);
	gpio_pin_configure(gpio_dev, 1, GPIO_INPUT);
	gpio_pin_configure(gpio_dev, 2, GPIO_OUTPUT_LOW);

	while (1) {
		gpio_pin_set(gpio_dev, 0, write);
		read = gpio_pin_get(gpio_dev, 1);
		gpio_pin_set(gpio_dev, 2, read);
		k_msleep(1);
	}
}

K_THREAD_DEFINE(startup_gpio_id, STACKSIZE, startup_gpio, NULL, NULL, NULL,
		PRE_PRIORITY, 0, 0);

K_THREAD_DEFINE(startup_irq_id, STACKSIZE, startup_irq, NULL, NULL, NULL,
		PRE_PRIORITY, 0, 0);
