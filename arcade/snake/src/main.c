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

#define RED		(0xF << 0)
#define GREEN		(0xF << 4)
#define BLUE		(0xF << 8)
#define WHITE		(RED | GREEN | BLUE)
#define BLACK		0x0000
#define GREY		0x0AAA

#define ROWS		100
#define COLUMNS		75

#define PIXEL(x, y)	(((x) + ROWS * (y)) & 0xFFFF)
#define GET_X(pixel)	((pixel) % ROWS)
#define GET_Y(pixel)	((pixel) / ROWS)

#include "background.h"

extern struct k_fifo cursor_fifo;

void inline write_pixel(volatile unsigned int *framebuffer, int address, int pixel)
{
	*framebuffer = ((pixel & 0xFFFF) << 16) | (address & 0xFFFF);
}

struct snake_node {
	unsigned short pixel;
	unsigned short color;
};
struct snake_node snake_nodes[100];

unsigned int start = false;
unsigned int direction = MOVE_RIGHT;
unsigned int color;

void draw_cover(volatile unsigned int *framebuffer, int clear)
{
	int i;

	for (i = 0; i < RED_PIXELS; i++) {
		write_pixel(framebuffer, red_pixel[i], clear ? BLACK : RED);
	}
	for (i = 0; i < GREEN_PIXELS; i++) {
		write_pixel(framebuffer, green_pixel[i], clear ? BLACK : GREEN);
	}
	for (i = 0; i < BLUE_PIXELS; i++) {
		write_pixel(framebuffer, blue_pixel[i], clear ? BLACK : BLUE);
	}
	for (i = 0; i < GREY_PIXELS; i++) {
		write_pixel(framebuffer, grey_pixel[i], clear ? BLACK : GREY);
	}

}

void draw_frame(volatile unsigned int *framebuffer, unsigned short color)
{
	int i;

	for (i = 0; i < ROWS; i++) {
		write_pixel(framebuffer, PIXEL(i, 0), color);
		write_pixel(framebuffer, PIXEL(i, COLUMNS - 1), color);
	}

	for (i = 0; i < COLUMNS; i++) {
		write_pixel(framebuffer, PIXEL(0, i), color);
		write_pixel(framebuffer, PIXEL(ROWS - 1, i), color);
	}
}

int random_pixel()
{
	unsigned int i = k_cycle_get_32();
	unsigned int x = (i % (ROWS - 2)) + 1;
	unsigned int y = (i % (COLUMNS - 2)) + 1;

	printk("random loot on PIXEL(%i, %i)\n", x, y);
	return PIXEL(x, y);
}

int random_color()
{
	int i;
	i = k_cycle_get_32() % 3;

	if (i == 0)
		return RED;
	if (i == 1)
		return GREEN;
	return BLUE;
}

void exit_game(volatile unsigned int *framebuffer)
{
	draw_frame(framebuffer, RED);
	while (1)
		k_sleep(K_SECONDS(1));
}

void framesync(void)
{
	int i;
	volatile unsigned int *framebuffer = (unsigned int *)0xF0050000;
	unsigned short loot;
	unsigned int snake_length = 5;
	unsigned int pixel;
	unsigned int snake_direction;

	for (i = 0; i < snake_length; i++) {
		snake_nodes[i].pixel = PIXEL(48 + 1, 10);
		snake_nodes[i].color = random_color();
	}

	loot = random_pixel();
	color = random_color();

	/* Draw game border */
	draw_frame(framebuffer, WHITE);

	/* Draw cover characters */
	draw_cover(framebuffer, false);

	/* Wait until to start the game */
	while(!start)
		k_sleep(K_MSEC(500));

	draw_cover(framebuffer, true);

	for (i = 0; i < snake_length; i++) {
		write_pixel(framebuffer, snake_nodes[i].pixel,
					 snake_nodes[i].color);
	}

	while (1) {
		snake_direction = direction;
		pixel = snake_nodes[snake_length - 1].pixel;
		switch (snake_direction) {
		case MOVE_UP:
			pixel = PIXEL(GET_X(pixel), GET_Y(pixel) - 1);
			break;
		case MOVE_RIGHT:
			pixel = PIXEL(GET_X(pixel) + 1, GET_Y(pixel));
			break;
		case MOVE_DOWN:
			pixel = PIXEL(GET_X(pixel), GET_Y(pixel) + 1);
			break;
		case MOVE_LEFT:
			pixel = PIXEL(GET_X(pixel) - 1, GET_Y(pixel));
			break;
		}
		if (pixel == loot) {
			snake_nodes[snake_length].pixel = pixel;
			snake_nodes[snake_length].color = color;
			snake_length++;
			loot = random_pixel();
		} else {
			write_pixel(framebuffer, snake_nodes[0].pixel, BLACK);
			for (i = 0; i < snake_length - 1; i++) {
				snake_nodes[i].pixel = snake_nodes[i + 1].pixel;
			}

			snake_nodes[snake_length - 1].pixel = pixel;
		}

		/* Draw loot */
		write_pixel(framebuffer, loot, color);

		/* Detect collisions */
		if (GET_X(pixel) == 0 || GET_X(pixel) == ROWS - 1 ||
		    GET_Y(pixel) == 0 || GET_Y(pixel) == COLUMNS - 1) {
			exit_game(framebuffer);
		}

		for (i = 0; i < snake_length - 2; i++) {
			if (snake_nodes[i].pixel == pixel) {
				exit_game(framebuffer);
			}
		}

		/* Draw snake */
		for (i = 0; i < snake_length; i++) {
			write_pixel(framebuffer, snake_nodes[i].pixel,
						 snake_nodes[i].color);
		}
		k_sleep(K_MSEC(500));
	}
}

void pixelmover(void)
{
	struct cursor_data *cursor;

	while (1) {
		cursor = k_fifo_get(&cursor_fifo, K_FOREVER);

		printk("Command received: %i\n", cursor->command);
		switch (cursor->command) {
		case MOVE_UP:
			if (direction != MOVE_DOWN)
				direction = cursor->command;
			break;
		case MOVE_RIGHT:
			if (direction != MOVE_LEFT)
				direction = cursor->command;
			break;
		case MOVE_DOWN:
			if (direction != MOVE_UP)
				direction = cursor->command;
			break;
		case MOVE_LEFT:
			if (direction != MOVE_RIGHT)
				direction = cursor->command;
			break;
		case BUTTON_A:
			/* Wait for first button A input to start */
			if (!start) {
				start = true;
				break;
			}
			if (color == RED)
				color = GREEN;
			else if (color == GREEN)
				color = BLUE;
			else if (color == BLUE)
				color = RED;
			break;
		}
	}
}

K_THREAD_DEFINE(framesync_tid, FRAMESYNC_STACKSIZE, framesync, NULL, NULL, NULL,
		FRAMESYNC_PRIORITY, 0, 0);
K_THREAD_DEFINE(pixelmover_tid, PIXELMOVER_STACKSIZE, pixelmover, NULL, NULL, NULL,
		PIXELMOVER_PRIORITY, 0, 0);
