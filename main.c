
#include <time.h>
#include <stdlib.h>


#include "bd_game.h"
#include "sdl_util.h"

int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) 
{
	srand(time(NULL));

	int zoom = 25;

	unsigned int* pixelbuffer = sdl_init(CAVE_WIDTH*zoom, (INFO_HEIGHT+CAVE_HEIGHT)*zoom,"Boudlerdash",60);

	struct bd_game_struct_t* bd_game = bd_game_initialize(0,0);

	int limiter=0;

	while(sdl_handle_events(pixelbuffer)) //limits loop to 60fps
	{
		while(sdl_limit_fps(&limiter,8))
		{
			bd_game_process(&bd_game,getkey);
			release_upped_keys();
		}

		bd_game_render(bd_game,pixelbuffer,zoom);

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
