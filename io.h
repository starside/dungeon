#ifndef __GAME_IO__
#define __GAME_IO__

#include <asm/semaphore.h>

#define MUTEX_INIT(sem) sema_init(sem, 1)

#define PLAYER_IO_BUF_LEN 512

struct Player_ringbuffer {
	int head;
	int tail;
	char buf[PLAYER_IO_BUF_LEN]; // must be power of 2
	struct semaphore lock;
};

int init_ringbuffer(struct Player_ringbuffer **player_buf);
int free_ringbuffer(struct Player_ringbuffer *player_buf);

int write_buffer(struct Player_ringbuffer *buf, const char *message, size_t len);
int read_buffer_to_user(struct Player_ringbuffer *buf, char *dest, size_t len);

#endif
