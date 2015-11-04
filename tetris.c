#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grid.h"
#include "tetris.h"

enum {
	MAX_PLAYERS = 3
};

static Grid grids[MAX_PLAYERS];


void tetris_load() {
	int i;
	for(i = 0; i < MAX_PLAYERS; i++) init_grid(&grids[i], i);
}

void tetris_suspend() {
	int i;
	for(i = 0; i < MAX_PLAYERS; i++) suspend_grid(&grids[i]);
}

void tetris_update(int getkey(int)) {
	int i;
	for(i = 0; i < MAX_PLAYERS; i++) {
		update_grid(&grids[i],getkey);
	}
}

void tetris_render(unsigned int* pixelbuffer,int zoom)
{
	int i;
	for(i = 0; i < MAX_PLAYERS; i++) {
		draw_grid(&grids[i],i,pixelbuffer,zoom);
	}
}
void reset_player(int nr) {
	init_grid(&grids[nr], nr);
}

