
#include <time.h>
#include <stdlib.h>


#include "tetris.h"
static Grid grids[PLAYERS];

#include "sdl_util.h"


void gameover(int nr,int lines)
{
	//save highscore
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

// couple events + game-progcess -> 8fps limited
//        render + draw          -> 60ps limited

// any fps limit for both sould work.
//
//
// sdl_util_run("Boudlerdash",h,w,game_logic,8,game_render,60)
// -> game_logic gets events
// -> game_render gets buffer, rendering is done afterwards
//
