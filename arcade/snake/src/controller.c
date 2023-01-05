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
#include "controller.h"
#include "threads.h"

K_FIFO_DEFINE(cursor_fifo);

static struct gpio_callback button_cb_data[5];

void button_pressed(const struct device *gpio, struct gpio_callback *cb,
		    uint32_t pins)
{
	const struct gpio_dt_spec plyr1_arrow_right = PLYR1_ARROW_RIGHT_NODE;
	const struct gpio_dt_spec plyr1_arrow_up = PLYR1_ARROW_UP_NODE;
	const struct gpio_dt_spec plyr1_arrow_left = PLYR1_ARROW_LEFT_NODE;
	const struct gpio_dt_spec plyr1_arrow_down = PLYR1_ARROW_DOWN_NODE;
	const struct gpio_dt_spec plyr1_select = PLYR1_SELECT_NODE;
	struct cursor_data cursor_tx;
	int ret;

	if (pins == BIT(plyr1_arrow_up.pin)) {
		cursor_tx.command = PLYR1_ARROW_UP;
		printk("Cursor up pressed\n");
	}
	if (pins == BIT(plyr1_arrow_right.pin)) {
		cursor_tx.command = PLYR1_ARROW_RIGHT;
		printk("Cursor right pressed\n");
	}
	if (pins == BIT(plyr1_arrow_down.pin)) {
		cursor_tx.command = PLYR1_ARROW_DOWN;
		printk("Cursor down pressed\n");
	}
	if (pins == BIT(plyr1_arrow_left.pin)) {
		cursor_tx.command = PLYR1_ARROW_LEFT;
		printk("Cursor left pressed\n");
	}
	if (pins == BIT(plyr1_select.pin)) {
		cursor_tx.command = PLYR1_SELECT;
		printk("Cursor select pressed\n");
	}

	ret = k_fifo_alloc_put(&cursor_fifo, &cursor_tx);
	if (ret == -ENOMEM)
		printk("Unable to add cursor command to FIFO: -ENOMEM\n");
}

int initialize_button(const struct gpio_dt_spec *button)
{
	int ret;
	int pin;

	if (button == NULL) {
		printk("Error: didn't find button\n");
		return -1;
	}
	pin = button->pin;

	ret = gpio_pin_configure_dt(button, button->dt_flags);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %s\n",
		       ret, button->port->name);
		return ret;
	}

	ret = gpio_pin_interrupt_configure_dt(button, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on pin %s\n",
			ret, button->port->name);
		return ret;
	}

	gpio_init_callback(&button_cb_data[pin], button_pressed, BIT(pin));
	ret = gpio_add_callback(button->port, &button_cb_data[pin]);
	if (ret != 0) {
		printk("Error %d: failed to add callback on pin %s\n",
			ret, button->port->name);
		return ret;
	}
	printk("Set up button at %s pin %d\n", button->port->name, pin);

	return 0;
}

void controller(void)
{
	const struct gpio_dt_spec plyr1_arrow_right = PLYR1_ARROW_RIGHT_NODE;
	const struct gpio_dt_spec plyr1_arrow_up = PLYR1_ARROW_UP_NODE;
	const struct gpio_dt_spec plyr1_arrow_left = PLYR1_ARROW_LEFT_NODE;
	const struct gpio_dt_spec plyr1_arrow_down = PLYR1_ARROW_DOWN_NODE;
	const struct gpio_dt_spec plyr1_select = PLYR1_SELECT_NODE;
	int ret;

	ret = initialize_button(&plyr1_arrow_right);
	if (ret < 0)
		return;

	ret = initialize_button(&plyr1_arrow_up);
	if (ret < 0)
		return;

	ret = initialize_button(&plyr1_arrow_left);
	if (ret < 0)
		return;

	ret = initialize_button(&plyr1_arrow_down);
	if (ret < 0)
		return;

	ret = initialize_button(&plyr1_select);
	if (ret < 0)
		return;

	while(1) {
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(controller_tid, CONTROLLER_STACKSIZE, controller, NULL, NULL,
		NULL, CONTROLLER_PRIORITY, 0, 0);
