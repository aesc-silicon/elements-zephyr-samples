/*
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
#define HEARTBEAT_STACKSIZE 2048
#define PIO_STACKSIZE 2048

/* scheduling priority used by each thread */
#define HEARTBEAT_PRIORITY 5
#define PIO_PRIORITY 5

#define LED_HEARTBEAT		DT_PATH(leds, heartbeat)
#define LED_HEARTBEAT_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(LED_HEARTBEAT, gpios, 0), label)
#define LED_HEARTBEAT_PIN	DT_PHA_BY_IDX(LED_HEARTBEAT, gpios, 0, pin)
#define LED_HEARTBEAT_FLAGS	DT_PHA_BY_IDX(LED_HEARTBEAT, gpios, 0, flags)

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
void heartbeat(void)
{
	const struct device *led_dev;

	led_dev = device_get_binding(LED_HEARTBEAT_CTRL);
	__ASSERT_NO_MSG(led_dev != NULL);

	gpio_pin_configure(led_dev, LED_HEARTBEAT_PIN,
		LED_HEARTBEAT_FLAGS | GPIO_OUTPUT);

	while (1) {
		gpio_pin_set(led_dev, LED_HEARTBEAT_PIN, 1);
		k_sleep(K_MSEC(150));
		gpio_pin_set(led_dev, LED_HEARTBEAT_PIN, 0);
		k_sleep(K_MSEC(50));
		gpio_pin_set(led_dev, LED_HEARTBEAT_PIN, 1);
		k_sleep(K_MSEC(150));
		gpio_pin_set(led_dev, LED_HEARTBEAT_PIN, 0);
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(heartbeat_tid, HEARTBEAT_STACKSIZE, heartbeat, NULL, NULL, NULL,
		HEARTBEAT_PRIORITY, 0, 0);

K_THREAD_DEFINE(pio_tid, PIO_STACKSIZE, pio, NULL, NULL, NULL,
		PIO_PRIORITY, 0, 0);
