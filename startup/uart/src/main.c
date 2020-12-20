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

void irq_handler(const struct device *uart_dev, void *user_data)
{
	uint8_t rx_data[16];
	int val = 0;
	int i = 0;

	printk("IRQ triggered at %d\n", k_cycle_get_32());

	uart_irq_update(uart_dev);

	if (uart_irq_rx_ready(uart_dev)) {
		val = uart_fifo_read(uart_dev, rx_data, 16);
		printk("irq_handler: received %i frames - 0x", val);
		for (i = 0; i < val; i++)
			printk("%x", rx_data[i]);
		printk("\n");
	} else if (uart_irq_tx_ready(uart_dev)) {
		printk("irq_handler: tx branch\n");
	} else {
		printk("irq_handler: unknow reason!\n");
	}
}

void startup_irq(void)
{
	const struct device *uart_dev;

	uart_dev = device_get_binding("uart0");
	__ASSERT_NO_MSG(uart_dev != NULL);

	uart_irq_rx_disable(uart_dev);
	uart_irq_callback_set(uart_dev, irq_handler);
	uart_irq_update(uart_dev);
	uart_irq_rx_enable(uart_dev);

	while (1) {
		k_msleep(10000);
	}
}

void startup_uart(void)
{

	const struct device *uart_dev;

	uart_dev = device_get_binding("uart0");
	__ASSERT_NO_MSG(uart_dev != NULL);

	while (1) {
		uart_poll_out(uart_dev, 'X');
		k_msleep(1000);
	}
}

K_THREAD_DEFINE(startup_uart_id, STACKSIZE, startup_uart, NULL, NULL, NULL,
		PRE_PRIORITY, 0, 0);

K_THREAD_DEFINE(startup_irq_id, STACKSIZE, startup_irq, NULL, NULL, NULL,
		PRE_PRIORITY, 0, 0);
