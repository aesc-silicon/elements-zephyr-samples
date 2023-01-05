/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>

/* size of stack area used by each thread */
#define STACKSIZE 2048

/* scheduling priority used by each thread */
#define PRE_PRIORITY 7
#define COP_PRIORITY -7

#define ERASE 0

void startup_spi(void) {
	unsigned int i;
	struct spi_config spi_cfg;
	/* SPI request */
	unsigned char tx_cmd[1];
	unsigned char tx_adr[12];
	unsigned char tx_buf[48];
	struct spi_buf tx_bufs[3];
	struct spi_buf_set tx_req;
	/* SPI response */
	unsigned char rx_buf[48];
	struct spi_buf rx_bufs[1];
	struct spi_buf_set rx_res;

	const struct device *spi_dev = device_get_binding("spi0");
	__ASSERT_NO_MSG(spi_dev != NULL);

	spi_cfg.frequency = 1000000;
	spi_cfg.operation = SPI_WORD_SET(8) | SPI_OP_MODE_MASTER;
	spi_cfg.slave = 0;
	spi_cfg.cs = NULL;

	// SPI request
	tx_cmd[0] = 0x9E; //9Eh --> READ ID
	tx_bufs[0].len = 1;
	tx_bufs[0].buf = &tx_cmd;
	tx_req.count = 1;
	tx_req.buffers = tx_bufs;

	// SPI response
	rx_bufs[0].len = 20;
	rx_bufs[0].buf = &rx_buf;
	rx_res.count = 1;
	rx_res.buffers = rx_bufs;

	spi_transceive(spi_dev, &spi_cfg, &tx_req, &rx_res);

	printk("Manufacturer: ");
	switch(rx_buf[0]) {
	case 0x20:
		printk("Micron\n");
		break;
	default:
		printk("Unknown\n");
		break;
	}
	printk("Memory Type: ");
	switch(rx_buf[1]) {
	case 0xBA:
		printk("SPI-NOR\n");
		break;
	default:
		printk("Unknown\n");
		break;
	}
	printk("Memory Capacity: ");
	switch(rx_buf[2]) {

	case 0x17:
		printk("64Mb\n");
		break;
	case 0x18:
		printk("128Mb\n");
		break;
	case 0x19:
		printk("256Mb\n");
		break;
	case 0x20:
		printk("512Mb\n");
		break;
	case 0x21:
		printk("1Gb\n");
		break;
	case 0x22:
		printk("2Gb\n");
		break;
	default:
		printk("Unknown\n");
		break;
	}
	printk("CFD: ");
	for (i = 0; i <= 15; i++)
		printk("%x:", rx_buf[4 + i]);
	
	printk("\n");


	// Write enable
	// SPI request
	tx_cmd[0] = 0x06;
	tx_bufs[0].len = 1;
	tx_bufs[0].buf = &tx_cmd;
	tx_req.count = 1;
	tx_req.buffers = tx_bufs;

	// SPI response
	rx_res.count = 0;

	spi_transceive(spi_dev, &spi_cfg, &tx_req, &rx_res);

#if ERASE

	// Erase page
	// SPI request
	tx_cmd[0] = 0x20;
	tx_bufs[0].len = 1;
	tx_bufs[0].buf = &tx_cmd;
	tx_adr[0] = 0x0;
	tx_adr[1] = 0x0;
	tx_adr[2] = 0x0;
	tx_bufs[1].len = 3;
	tx_bufs[1].buf = &tx_adr;
	tx_req.count = 2;
	tx_req.buffers = tx_bufs;

	// SPI response
	rx_res.count = 0;

	spi_transceive(spi_dev, &spi_cfg, &tx_req, &rx_res);

#else

	// Program data
	// SPI request
	tx_cmd[0] = 0x02;
	tx_bufs[0].len = 1;
	tx_bufs[0].buf = &tx_cmd;
	tx_adr[0] = 0x0;
	tx_adr[1] = 0x0;
	tx_adr[2] = 0x0;
	tx_bufs[1].len = 3;
	tx_bufs[1].buf = &tx_adr;
	tx_buf[0] = 0xC0;
	tx_buf[1] = 0x01;
	tx_buf[2] = 0xCA;
	tx_buf[3] = 0xFE;
	tx_bufs[2].len = 4;
	tx_bufs[2].buf = &tx_buf;
	tx_req.count = 3;
	tx_req.buffers = tx_bufs;

	// SPI response */
	rx_res.count = 0;

	spi_transceive(spi_dev, &spi_cfg, &tx_req, &rx_res);

#endif

	// Write disable
	// SPI request
	tx_cmd[0] = 0x04;
	tx_bufs[0].len = 1;
	tx_bufs[0].buf = &tx_cmd;
	tx_req.count = 1;
	tx_req.buffers = tx_bufs;

	// SPI response
	rx_res.count = 0;

	spi_transceive(spi_dev, &spi_cfg, &tx_req, &rx_res);

	// Read content
	// SPI request
	tx_cmd[0] = 0x03;
	tx_bufs[0].len = 1;
	tx_bufs[0].buf = &tx_cmd;
	tx_adr[0] = 0x0;
	tx_adr[1] = 0x0;
	tx_adr[2] = 0x0;
	tx_bufs[1].len = 3;
	tx_bufs[1].buf = &tx_adr;
	tx_req.count = 2;
	tx_req.buffers = tx_bufs;

	// SPI response
	rx_bufs[0].len = 48;
	rx_bufs[0].buf = &rx_buf;
	rx_res.count = 1;
	rx_res.buffers = rx_bufs;

	spi_transceive(spi_dev, &spi_cfg, &tx_req, &rx_res);

	
	for (i = 0; i < 48; i++) {
		if (i % 16 == 0) {
			printk("\n0x%02x%02x%02x:", tx_adr[2], tx_adr[1], tx_adr[0]);
			tx_adr[0] += 0x10;
		}
		if (i % 4 == 0)
			printk(" ");
		printk("%02x", rx_buf[i]);
	}
	printk("\n");

	while (1) {
		k_msleep(1000);
	}
}

K_THREAD_DEFINE(startup_spi_id, STACKSIZE, startup_spi, NULL, NULL, NULL,
		PRE_PRIORITY, 0, 0);
