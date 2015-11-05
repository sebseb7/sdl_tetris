
#include <time.h>
#include <stdlib.h>


#include "tetris.h"
static Grid grids[PLAYERS];

#include "sdl_util.h"


#include "SDL.h"

void gameover(int nr,int lines)
{
	if(nr)
		return;

	char filename[256];
	int highscore=0;

	snprintf(filename, sizeof filename, "%s%s", SDL_GetPrefPath("net.exse.seb","tetris"), "highscore");
	SDL_RWops *file = SDL_RWFromFile(filename, "rb");
	if(file)
	{
		highscore=SDL_ReadBE32(file);
		SDL_RWclose(file);
	}

	if(lines > highscore)
	{
		file = SDL_RWFromFile(filename, "wb");
		SDL_WriteBE32(file,lines);
		SDL_RWclose(file);
	}
}


int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) 
{
	srand(time(NULL));
	

	
	int zoom = 18;

	unsigned int* pixelbuffer = sdl_init(PLAYERS*12*zoom, 27*zoom,"Tetris",60);

	for(int i = 0; i < PLAYERS; i++) init_grid(&grids[i], i);

	set_gameover_callback(gameover);

	int limiter=0;

	while(sdl_handle_events(pixelbuffer)) //limits loop to 60fps
	{
		while(sdl_limit_fps(&limiter,30))
		{
			for(int i = 0; i < PLAYERS; i++) {
				update_grid(&grids[i],getkey);
			}
			release_upped_keys();
		}

		for(int i = 0; i < PLAYERS; i++) {
			draw_grid(&grids[i],i,pixelbuffer,zoom);
		}

	}
	sdl_deinit();
	return 0;
}

