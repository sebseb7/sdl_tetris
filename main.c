
#include <time.h>
#include <stdlib.h>


#include "tetris.h"
#include "sdl_util.h"

int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) 
{
	srand(time(NULL));
	

	int zoom = 25;

	unsigned int* pixelbuffer = sdl_init(36*zoom, 27*zoom,"Tetris",60);

	tetris_load();

	int limiter=0;

	while(sdl_handle_events(pixelbuffer)) //limits loop to 60fps
	{
		while(sdl_limit_fps(&limiter,30))
		{
			tetris_update(getkey);
			release_upped_keys();
		}

		tetris_render(pixelbuffer,zoom);

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
