#ifndef SOFTWARE0_KEY_H
#define SOFTWARE0_KEY_H

/* size of stack area used by each thread */
#define SOFTWARE0_KEY_STACKSIZE		2048

/* scheduling priority used by each thread */
#define SOFTWARE0_KEY_PRIORITY		5

#define SOFTWARE0_KEY_NODE		DT_PATH(keys, sw0)

#if !DT_NODE_HAS_STATUS(SOFTWARE0_KEY_NODE, okay)
#error "Unsupported board: sw0 key devicetree alias is not defined"
#endif

void software0_key(void);

#endif /* SOFTWARE0_KEY_H */
