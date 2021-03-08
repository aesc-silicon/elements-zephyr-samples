#ifndef CONTROLLER_H
#define CONTROLLER_H

struct cursor_data {
	void *fifo_reserved;
	unsigned int command;
};

#define MOVE_UP		0x1 << 0
#define MOVE_RIGHT	0x1 << 1
#define MOVE_DOWN	0x1 << 2
#define MOVE_LEFT	0x1 << 3
#define BUTTON_A	0x1 << 4

#define KEY_CURSOR_UP		DT_PATH(keys, cursor_up)
#define KEY_CURSOR_UP_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(KEY_CURSOR_UP, gpios, 0), label)
#define KEY_CURSOR_UP_PIN	DT_PHA_BY_IDX(KEY_CURSOR_UP, gpios, 0, pin)
#define KEY_CURSOR_UP_FLAGS	DT_PHA_BY_IDX(KEY_CURSOR_UP, gpios, 0, flags)

#define KEY_CURSOR_RIGHT	DT_PATH(keys, cursor_right)
#define KEY_CURSOR_RIGHT_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(KEY_CURSOR_RIGHT, gpios, 0), label)
#define KEY_CURSOR_RIGHT_PIN	DT_PHA_BY_IDX(KEY_CURSOR_RIGHT, gpios, 0, pin)
#define KEY_CURSOR_RIGHT_FLAGS	DT_PHA_BY_IDX(KEY_CURSOR_RIGHT, gpios, 0, flags)

#define KEY_CURSOR_DOWN		DT_PATH(keys, cursor_down)
#define KEY_CURSOR_DOWN_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(KEY_CURSOR_DOWN, gpios, 0), label)
#define KEY_CURSOR_DOWN_PIN	DT_PHA_BY_IDX(KEY_CURSOR_DOWN, gpios, 0, pin)
#define KEY_CURSOR_DOWN_FLAGS	DT_PHA_BY_IDX(KEY_CURSOR_DOWN, gpios, 0, flags)

#define KEY_CURSOR_LEFT		DT_PATH(keys, cursor_left)
#define KEY_CURSOR_LEFT_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(KEY_CURSOR_LEFT, gpios, 0), label)
#define KEY_CURSOR_LEFT_PIN	DT_PHA_BY_IDX(KEY_CURSOR_LEFT, gpios, 0, pin)
#define KEY_CURSOR_LEFT_FLAGS	DT_PHA_BY_IDX(KEY_CURSOR_LEFT, gpios, 0, flags)

#define KEY_BUTTON_A	DT_PATH(keys, cursor_enter)
#define KEY_BUTTON_A_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(KEY_BUTTON_A, gpios, 0), label)
#define KEY_BUTTON_A_PIN	DT_PHA_BY_IDX(KEY_BUTTON_A, gpios, 0, pin)
#define KEY_BUTTON_A_FLAGS	DT_PHA_BY_IDX(KEY_BUTTON_A, gpios, 0, flags)

#endif /* CONTROLLER_H */
