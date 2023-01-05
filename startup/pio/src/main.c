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

#include "../../../common/heartbeat.h"

/* size of stack area used by each thread */
#define PIO_STACKSIZE 2048

/* scheduling priority used by each thread */
#define PIO_PRIORITY 5

#define DATA(data)	(data << 8)
#define PIN(no)		((no & 0xF) << 4)
#define COMMAND_HIGH	0x0
#define COMMAND_LOW	0x1
#define COMMAND_WAIT	0x2
#define COMMAND_READ	0x3

inline void send_bit(volatile unsigned int *reg, unsigned int level)
{
	*reg = DATA(0) | PIN(0) | level;
	*reg = DATA(1) | PIN(0) | COMMAND_WAIT;
}

void pio(void)
{
	volatile unsigned int *readWrite = (unsigned int *)0xF0012000;
	volatile unsigned int *status = readWrite + 1;
	unsigned int value;

	*readWrite = DATA(0) | PIN(0) | COMMAND_HIGH;
	*(readWrite + 2) = 868;
	*readWrite = DATA(2) | PIN(0) | COMMAND_WAIT;

	send_bit(readWrite, COMMAND_LOW);
	send_bit(readWrite, COMMAND_HIGH);
	send_bit(readWrite, COMMAND_HIGH);
	send_bit(readWrite, COMMAND_HIGH);
	send_bit(readWrite, COMMAND_LOW);
	send_bit(readWrite, COMMAND_LOW);
	send_bit(readWrite, COMMAND_LOW);
	send_bit(readWrite, COMMAND_HIGH);
	send_bit(readWrite, COMMAND_LOW);
	send_bit(readWrite, COMMAND_HIGH);

	*readWrite = DATA(0) | PIN(1) | COMMAND_READ;

	while (!(*status >> 24)) {}
	value = *readWrite;
	printk("Read: %x\n", value);

	while (1) {
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(heartbeat_tid, HEARTBEAT_STACKSIZE, heartbeat, NULL, NULL, NULL,
		HEARTBEAT_PRIORITY, 0, 0);

K_THREAD_DEFINE(pio_tid, PIO_STACKSIZE, pio, NULL, NULL, NULL,
		PIO_PRIORITY, 0, 0);
