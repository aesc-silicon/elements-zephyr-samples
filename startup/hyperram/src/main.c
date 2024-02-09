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
#define NOTHING_STACKSIZE 2048

/* scheduling priority used by each thread */
#define NOTHING_PRIORITY 5

#define REGISTER_ADDR	0xf0023000

void dump_register(unsigned int address, char *name)
{
	volatile unsigned int *reg = (volatile unsigned int *)REGISTER_ADDR;
	unsigned int result;

	*(reg + 12) = 0x00008000 | (address & 0x7FFF);
	while ((*(reg + 13) & 0xFFFF) == 0) {}
	do {
		result = *(reg + 12);
	} while (result >> 31);
	printk("%s: 0x%04x\n", name, (result & 0xFFFF));
}

unsigned short read_register(unsigned int address)
{
	volatile unsigned int *reg = (volatile unsigned int *)REGISTER_ADDR;
	unsigned int result;

	*(reg + 12) = 0x00008000 | (address & 0x7FFF);
	while ((*(reg + 13) & 0xFFFF) == 0) {}
	do {
		result = *(reg + 12);
	} while (result >> 31);

	return result & 0xFFFF;
}

void write_register(unsigned int address, unsigned int value)
{
	volatile unsigned int *reg = (volatile unsigned int *)REGISTER_ADDR;
	unsigned int result;

	*(reg + 12) = 0x0 | (address & 0x7FFF) | (value << 16);
	while ((*(reg + 13) & 0xFFFF) == 0) {}
	do {
		result = *(reg + 12);
	} while (result >> 31);
}

void read_clock_latency()
{
	volatile unsigned int *reg = (volatile unsigned int *)REGISTER_ADDR;
	unsigned int tmp;

	*(reg + 8) = 6;
	tmp = read_register(0x800);
	switch ((tmp >> 4) & 0xf) {
		case 0xe:
			printk("Set 3 clock latency\n");
			*(reg + 8) = 3;
			break;
		case 0xf:
			printk("Set 4 clock latency\n");
			*(reg + 8) = 4;
			break;
		case 0:
			printk("Set 5 clock latency\n");
			*(reg + 8) = 5;
			break;
		case 1:
			printk("Set 6 clock latency\n");
			*(reg + 8) = 6;
			break;
		case 2:
			printk("Set 7 clock latency\n");
			*(reg + 8) = 7;
			break;
		default:
			printk("Unknown number of clock latency\n");
	}
}

void nothing(void)
{
	volatile unsigned int *hyperram = (volatile unsigned int *)0x90000000;

	dump_register(0x0, "Identification Register 0");
	dump_register(0x1, "Identification Register 1");
	dump_register(0x800, "Configuration Register 0");
	dump_register(0x801, "Configuration Register 1");

	printk("Read: 0x%x\n", *hyperram);

	printk("Write 0xcafebabe\n");
	*hyperram = 0xcafebabe;
	printk("Read: 0x%x\n", *hyperram);

	printk("Write 0xbeef\n");
	*(unsigned short *)hyperram = 0xbeef;
	printk("Read: 0x%x\n", *(unsigned short *)hyperram);
	printk("Read: 0x%x\n", *hyperram);

	printk("Write 0xa5\n");
	*(unsigned char *)hyperram = 0xa5;
	printk("Read: 0x%x\n", *(unsigned char *)hyperram);
	printk("Read: 0x%x\n", *hyperram);

	dump_register(0x800, "Configuration Register 0");
	write_register(0x800, 0x8fef);
	read_clock_latency();
	dump_register(0x800, "Configuration Register 0");

	printk("Read: 0x%x\n", *hyperram);
	printk("Write 0xcafebabe\n");
	*hyperram = 0xcafebabe;
	printk("Read: 0x%x\n", *hyperram);







	while (1) {
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(nothing_tid, NOTHING_STACKSIZE, nothing, NULL, NULL, NULL,
		NOTHING_PRIORITY, 0, 0);
