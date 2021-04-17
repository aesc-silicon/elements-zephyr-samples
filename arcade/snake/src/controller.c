/*
 * Copyright (c) 2021 Phytec Messtechnik GmbH
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
#include "controller.h"
#include "threads.h"

K_FIFO_DEFINE(cursor_fifo);

static struct gpio_callback button_cb_data[5];

void button_pressed(const struct device *gpio, struct gpio_callback *cb,
		 uint32_t pins)
{
	struct cursor_data cursor_tx;
	int ret;

	printk("IRQ triggered for pin %i at %d\n", pins, k_cycle_get_32());
	if (pins == BIT(PLYR1_ARROW_UP_PIN)) {
		cursor_tx.command = PLYR1_ARROW_UP;
		printk("Cursor up pressed\n");
	}
	if (pins == BIT(PLYR1_ARROW_RIGHT_PIN)) {
		cursor_tx.command = PLYR1_ARROW_RIGHT;
		printk("Cursor right pressed\n");
	}
	if (pins == BIT(PLYR1_ARROW_DOWN_PIN)) {
		cursor_tx.command = PLYR1_ARROW_DOWN;
		printk("Cursor down pressed\n");
	}
	if (pins == BIT(PLYR1_ARROW_LEFT_PIN)) {
		cursor_tx.command = PLYR1_ARROW_LEFT;
		printk("Cursor left pressed\n");
	}
	if (pins == BIT(PLYR1_SELECT_PIN)) {
		cursor_tx.command = PLYR1_SELECT;
		printk("Cursor enter pressed\n");
	}

	printk("Command send: %i\n", cursor_tx.command);
	ret = k_fifo_alloc_put(&cursor_fifo, &cursor_tx);
	if (ret == -ENOMEM)
		printk("Unable to add cursor command to FIFO: -ENOMEM\n");
}

int initialize_button(char *label, int pin, int flags)
{
	const struct device *button;
	int ret;

	button = device_get_binding(label);
	if (button == NULL) {
		printk("Error: didn't find %s device\n", label);
		return -1;
	}

	ret = gpio_pin_configure(button, pin, flags);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, label, pin);
		return ret;
	}

	ret = gpio_pin_interrupt_configure(button, pin,
					   GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, label, pin);
		return ret;
	}

	gpio_init_callback(&button_cb_data[pin], button_pressed, BIT(pin));
	ret = gpio_add_callback(button, &button_cb_data[pin]);
	if (ret != 0) {
		printk("Error %d: failed to add callback on %s pin %d\n",
			ret, label, pin);
		return ret;
	}
	printk("Set up button at %s pin %d\n", label, pin);

	return 0;
}

void controller(void)
{

	int ret;

	ret = initialize_button(PLYR1_ARROW_UP_CTRL,
				PLYR1_ARROW_UP_PIN,
				PLYR1_ARROW_UP_FLAGS);
	if (ret < 0)
		return;

	ret = initialize_button(PLYR1_ARROW_RIGHT_CTRL,
				PLYR1_ARROW_RIGHT_PIN,
				PLYR1_ARROW_RIGHT_FLAGS);
	if (ret < 0)
		return;

	ret = initialize_button(PLYR1_ARROW_DOWN_CTRL,
				PLYR1_ARROW_DOWN_PIN,
				PLYR1_ARROW_DOWN_FLAGS);
	if (ret < 0)
		return;

	ret = initialize_button(PLYR1_ARROW_LEFT_CTRL,
				PLYR1_ARROW_LEFT_PIN,
				PLYR1_ARROW_LEFT_FLAGS);
	if (ret < 0)
		return;

	ret = initialize_button(PLYR1_SELECT_CTRL,
				PLYR1_SELECT_PIN,
				PLYR1_SELECT_FLAGS);
	if (ret < 0)
		return;

	while(1) {
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(controller_tid, CONTROLLER_STACKSIZE, controller, NULL, NULL,
		NULL, CONTROLLER_PRIORITY, 0, 0);
