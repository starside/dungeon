#define TITLE_LEN 100
#define DESC_LEN 300
#define NUM_DIRECTIONS 4 
#define MAX_PLAYERS_IN_ROOM 4
#define MAX_NPCS 5
#define MAX_ITEMS 4*MAX_PLAYERS

typedef player_id int;
typedef dungeon_dir int;

struct Room {
	char title[TITLE_LEN]; // room title
	char desc[DESC_LEN]; // Room description length
	int directions[NUM_DIRECTIONS];
	int players[MAX_PLAYERS_IN_ROOM];
	int npcs[MAX_NPCS];
	int items[MAX_ITEMS];
};

#define ROOM(title, desc, n, s, e, w) {title, desc, {n,s,e,w}, {-1}, {-1}, {-1} }

struct Room dungeon_Rooms[2] = {
	ROOM("Entrance", "The entry way", 1, -1, -1, -1),				// 0
	ROOM("Treasure Room", "A big fat ass treasure room",-1,0,-1,-1)			// 1
};

/*
 * Get an available player_id.
 *
 * Returns -1 if could not posess a player
 */
player_id possesPlayer();

/*
 * Look at a room.  Takes a player ID, and writes
 * a description to output
 */
void lookRoom(char *out_buffer, player_id player);

/*
 * Moves a player a direction dir
 * Returns the direction if succeeded, and -1 if failed
 */
int movePlayer(player_id player, dungeon_dir dir);

/*
 * Writes a list of visible players to list.  Takes maximum length of output list.
 * Returns the number of visible players.  It is possible there are more players visible
 * than the list size.  Toggle the most significant bit if this happens.  In other words
 * returns -number of players.
 */
int getVisiblePlayers(player_id *list, size_t list_len, player_id player);


