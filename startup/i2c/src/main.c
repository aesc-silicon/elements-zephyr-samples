/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <string.h>
#include <kernel.h>
#include <drivers/i2c.h>

/* size of stack area used by each thread */
#define STACKSIZE 2048

/* scheduling priority used by each thread */
#define PRE_PRIORITY 7
#define COP_PRIORITY -7


void startup_i2c(void) {

	const struct device *i2c_dev = device_get_binding("i2c0");
	__ASSERT_NO_MSG(i2c_dev != NULL);

	struct i2c_msg msg[2];
	uint8_t addr = 0xAE;
	uint8_t reg = 0xFA;
	uint8_t data[6];

	msg[0].buf = &reg;
	msg[0].len = 1;
	msg[0].flags = I2C_MSG_WRITE;

	msg[1].buf = data;
	msg[1].len = 6;
	msg[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

	i2c_transfer(i2c_dev, msg, 2, addr);

	printk("Manufacturer Code: %x\n", data[0]);
	printk("Device Code: %x\n", data[1]);
	printk("Serial Number: 0x%02x%02x%02x%02x\n", data[2], data[3],
						      data[4], data[5]);

	while (1) {
		k_msleep(1000);
	}
}

K_THREAD_DEFINE(startup_i2c_id, STACKSIZE, startup_i2c, NULL, NULL, NULL,
		PRE_PRIORITY, 0, 0);
