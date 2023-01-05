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

#define PLYR1_ARROW_RIGHT_NODE	GPIO_DT_SPEC_GET(DT_PATH(keys, player1_arrow_right), gpios)
#define PLYR1_ARROW_UP_NODE	GPIO_DT_SPEC_GET(DT_PATH(keys, player1_arrow_up), gpios)
#define PLYR1_ARROW_LEFT_NODE	GPIO_DT_SPEC_GET(DT_PATH(keys, player1_arrow_left), gpios)
#define PLYR1_ARROW_DOWN_NODE	GPIO_DT_SPEC_GET(DT_PATH(keys, player1_arrow_down), gpios)
#define PLYR1_SELECT_NODE	GPIO_DT_SPEC_GET(DT_PATH(keys, player1_select), gpios)

#endif /* CONTROLLER_H */
