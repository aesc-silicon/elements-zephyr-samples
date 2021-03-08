#ifndef THREADS_H
#define THREADS_H

/* size of stack area used by each thread */
#define HEARTBEAT_STACKSIZE 2048
#define FRAMESYNC_STACKSIZE 4096
#define PIXELMOVER_STACKSIZE 2048
#define CONTROLLER_STACKSIZE 2048

/* scheduling priority used by each thread */
#define HEARTBEAT_PRIORITY 7
#define FRAMESYNC_PRIORITY 4
#define PIXELMOVER_PRIORITY 5
#define CONTROLLER_PRIORITY 6

#endif /* THREADS_H */
