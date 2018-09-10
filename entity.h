#ifndef _ENTITY_H
#define _ENTITY_H

#include "io.h"

#define NAME_LEN 100

struct Player {
	char name[NAME_LEN];
	int location;  //Room player is in
	struct Player_IOBuffer *io;
};

#endif
