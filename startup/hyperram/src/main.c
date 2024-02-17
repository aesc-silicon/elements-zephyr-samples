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

#define REGISTER_ADDR	0xf0023000
#define MEMORY_ADDR	0x90100000

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

int main(void)
{
	volatile unsigned char *hyperramB = (volatile unsigned char *)MEMORY_ADDR;
	volatile unsigned short *hyperramS = (volatile unsigned short *)MEMORY_ADDR;
	volatile unsigned int *hyperramW = (volatile unsigned int *)MEMORY_ADDR;
	int i;

	for(i = 0; i < 8; i++) {
		hyperramB[i] = i;
	}
	for(i = 0x20; i < 0x28; i++) {
		hyperramS[i] = i;
	}
	for(i = 0x50; i < 0x58; i++) {
		hyperramW[i] = i;
	}

	for(i = 0; i < 8; i++) {
		printk("Read: 0x%x\n", hyperramB[i]);
	}
	for(i = 0x20; i < 0x28; i++) {
		printk("Read: 0x%x\n", hyperramS[i]);
	}
	for(i = 0x50; i < 0x58; i++) {
		printk("Read: 0x%x\n", hyperramW[i]);
	}

	dump_register(0x0, "Identification Register 0");
	dump_register(0x1, "Identification Register 1");
	dump_register(0x800, "Configuration Register 0");
	dump_register(0x801, "Configuration Register 1");

	printk("Write 0xcafebabe\n");
	*hyperramW = 0xcafebabe;
	printk("Read: 0x%x\n", *hyperramW);

	printk("Write 0xbeef\n");
	*hyperramS = 0xbeef;
	printk("Read: 0x%x\n", *hyperramS);
	printk("Read: 0x%x\n", *hyperramW);

	printk("Write 0xa5\n");
	*hyperramB = 0xa5;
	printk("Read: 0x%x\n", *hyperramB);
	printk("Read: 0x%x\n", *hyperramW);

	printk("Read: 0x%x\n", *hyperramW);
	printk("Write 0xcafebabe\n");
	*hyperramW = 0xcafebabe;
	printk("Read: 0x%x\n", *hyperramW);

	while (1) {
		k_sleep(K_SECONDS(1));
	}

	return 0;
}
