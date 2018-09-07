#ifndef _ENTITY_H
#define _ENTITY_H

#define NAME_LEN 100


struct Player {
	char name[NAME_LEN];
	int location;  //Room player is in
};

#endif
