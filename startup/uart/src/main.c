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

	uart_irq_update(uart_dev);

	if (uart_irq_rx_ready(uart_dev)) {
		printk("R");
		val = uart_fifo_read(uart_dev, rx_data, 16);
		for (i = 0; i < val; i++)
			printk("%c", rx_data[i]);
		printk("\n");
	} else if (uart_irq_tx_ready(uart_dev)) {
		printk("T\n");
		printk("irq_handler: tx branch\n");
	} else {
		printk("U\n");
		printk("irq_handler: unknow reason!\n");
	}
}

void startup_uart(void)
{
	const struct device *uart_dev;
	int ret;
	uint8_t character;

	uart_dev = device_get_binding("uartStdCtrl");
	__ASSERT_NO_MSG(uart_dev != NULL);

	uart_irq_rx_disable(uart_dev);
	uart_irq_callback_set(uart_dev, irq_handler);
	uart_irq_update(uart_dev);

	uart_poll_out(uart_dev, 'X');

	do {
		ret = uart_poll_in(uart_dev, &character);
	} while (ret != 0);

	uart_poll_out(uart_dev, character);

	uart_irq_rx_enable(uart_dev);

	while (1) {
		k_msleep(1);
	}
}

K_THREAD_DEFINE(startup_uart_id, STACKSIZE, startup_uart, NULL, NULL, NULL,
		PRE_PRIORITY, 0, 0);
