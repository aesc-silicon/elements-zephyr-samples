#ifndef CONTROLLER_H
#define CONTROLLER_H

struct cursor_data {
	void *fifo_reserved;
	unsigned int command;
};

#define PLYR1_ARROW_RIGHT	0x1 <<  0
#define PLYR1_ARROW_UP		0x1 <<  1
#define PLYR1_ARROW_LEFT	0x1 <<  2
#define PLYR1_ARROW_DOWN	0x1 <<  3
#define PLYR1_BACK_LEFT		0x1 <<  4
#define PLYR1_BACK_RIGHT	0x1 <<  5
#define PLYR1_A			0x1 <<  6
#define PLYR1_B			0x1 <<  7
#define PLYR1_X			0x1 <<  8
#define PLYR1_Y			0x1 <<  9
#define PLYR1_SELECT		0x1 << 10
#define PLYR1_START		0x1 << 11

#define PLYR1_ARROW_RIGHT_KEY	DT_PATH(keys, player1_arrow_right)
#define PLYR1_ARROW_RIGHT_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(PLYR1_ARROW_RIGHT_KEY, gpios, 0), label)
#define PLYR1_ARROW_RIGHT_PIN	DT_PHA_BY_IDX(PLYR1_ARROW_RIGHT_KEY, gpios, 0, pin)
#define PLYR1_ARROW_RIGHT_FLAGS	DT_PHA_BY_IDX(PLYR1_ARROW_RIGHT_KEY, gpios, 0, flags)

#define PLYR1_ARROW_UP_KEY	DT_PATH(keys, player1_arrow_up)
#define PLYR1_ARROW_UP_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(PLYR1_ARROW_UP_KEY, gpios, 0), label)
#define PLYR1_ARROW_UP_PIN	DT_PHA_BY_IDX(PLYR1_ARROW_UP_KEY, gpios, 0, pin)
#define PLYR1_ARROW_UP_FLAGS	DT_PHA_BY_IDX(PLYR1_ARROW_UP_KEY, gpios, 0, flags)

#define PLYR1_ARROW_LEFT_KEY	DT_PATH(keys, player1_arrow_left)
#define PLYR1_ARROW_LEFT_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(PLYR1_ARROW_LEFT_KEY, gpios, 0), label)
#define PLYR1_ARROW_LEFT_PIN	DT_PHA_BY_IDX(PLYR1_ARROW_LEFT_KEY, gpios, 0, pin)
#define PLYR1_ARROW_LEFT_FLAGS	DT_PHA_BY_IDX(PLYR1_ARROW_LEFT_KEY, gpios, 0, flags)

#define PLYR1_ARROW_DOWN_KEY	DT_PATH(keys, player1_arrow_down)
#define PLYR1_ARROW_DOWN_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(PLYR1_ARROW_DOWN_KEY, gpios, 0), label)
#define PLYR1_ARROW_DOWN_PIN	DT_PHA_BY_IDX(PLYR1_ARROW_DOWN_KEY, gpios, 0, pin)
#define PLYR1_ARROW_DOWN_FLAGS	DT_PHA_BY_IDX(PLYR1_ARROW_DOWN_KEY, gpios, 0, flags)

#define PLYR1_SELECT_KEY	DT_PATH(keys, player1_select)
#define PLYR1_SELECT_CTRL	DT_PROP(DT_PHANDLE_BY_IDX(PLYR1_SELECT_KEY, gpios, 0), label)
#define PLYR1_SELECT_PIN	DT_PHA_BY_IDX(PLYR1_SELECT_KEY, gpios, 0, pin)
#define PLYR1_SELECT_FLAGS	DT_PHA_BY_IDX(PLYR1_SELECT_KEY, gpios, 0, flags)

#endif /* CONTROLLER_H */
