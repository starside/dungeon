#ifndef _ENTITY_H
#define _ENTITY_H

#include "io.h"

#define NAME_LEN 100

struct Player {
	char name[NAME_LEN];
	int location;  //Room player is in
	struct Player_ringbuffer *input;
	struct Player_ringbuffer *output;
	struct semaphore lock;
};

int init_player(struct Player *player, const char *name, int location);
int deinit_player(struct Player *player);

int lock_player(struct Player *player);
void unlock_player(struct Player *player);
#endif
