#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tetris.h"

static const char STONES[7][16] = {
	{ 0, 0, 0, 0, 0,15,15, 0, 0,15,15, 0, 0, 0, 0, 0 },
	{ 0, 0,10, 0, 5, 5,15, 5, 0, 0,10, 0, 0, 0,10, 0 },
	{ 0, 0, 5, 0, 0, 0,15,15, 0,10,10, 5, 0, 0, 0, 0 },
	{ 0, 0, 0, 5, 0,10,15, 5, 0, 0,15,10, 0, 0, 0, 0 },
	{ 0, 4, 5, 8, 0,10,15,10, 0, 2, 5, 1, 0, 0, 0, 0 },
	{ 0, 8, 5, 1, 0,10,15,10, 0, 4, 5, 2, 0, 0, 0, 0 },
	{ 0, 0,11, 0, 0,13,15, 7, 0, 0,14, 0, 0, 0, 0, 0 }
};

static const char PALETTE[] = {
	1, // background
	15, 9, 14, 10, 13, 11, 12, // stones
	0, 15, // blinking
};
enum { // indices in PALETTE
	COLOR_BACKGROUND = 0,
	COLOR_BLACK = 8,
	COLOR_WHITE = 9,
};

enum {
	ANIMATION_BLINK,
	ANIMATION_SHIFT,
	ANIMATION_VANISH,
	ANIMATION_COUNT
};


static void new_stone(Grid* grid) {
	grid->tick = 0;
	grid->x = GRID_WIDTH / 2 - 2;
	grid->y = -3;
	grid->rot = grid->next_rot;
	grid->stone = grid->next_stone;
	grid->next_rot = 1 << (rand()%3);
	grid->next_stone = rand()%7;
	grid->input_mov = 0;
	grid->input_rep = 0;
	grid->input_rot = 0;
	grid->stone_count++;
}


static int grid_collision(Grid* grid, int top_also) {
	int x, y;
	for(y = 0; y < 4; y++) {
		for(x = 0; x < 4; x++) {
			if(STONES[grid->stone][x * 4 + y] & grid->rot) {
				if(top_also) {
					if(	x + grid->x < 0 || x + grid->x >= GRID_WIDTH ||
						y + grid->y < 0 || y + grid->y >= GRID_HEIGHT ||
						grid->matrix[y + grid->y][x + grid->x]) {
						return 1;
					}
				}
				else {
					if(	x + grid->x < 0 || x + grid->x >= GRID_WIDTH ||
						y + grid->y >= GRID_HEIGHT || (
						y + grid->y >= 0 &&
						grid->matrix[y + grid->y][x + grid->x])) {
						return 1;
					}
				}
			}
		}
	}
	return 0;
}


static void stone_to_grid(Grid* grid, char color) {
	int x, y;
	for(y = 0; y < 4; y++) {
		for(x = 0; x < 4; x++) {
			if(STONES[grid->stone][x * 4 + y] & grid->rot &&
				y + grid->y >= 0) {
				grid->matrix[y + grid->y][x + grid->x] = color;
			}
		}
	}
}



static int rate_grid(Grid* grid) {

	int x, y, i;
	int magic = 0;

	for(y = 0; y < GRID_HEIGHT; y++) {
		for(x = 0; x < GRID_WIDTH; x++) {

			if(grid->matrix[y][x]) magic += y;
			else if(y > 0 && grid->matrix[y - 1][x]) magic -= 20;

			if(grid->matrix[y][x] == 127) {
				if(y > 0 && grid->matrix[y - 1][x] > 0) magic += 20;
				if(x == 0 || grid->matrix[y][x - 1] > 0) magic += 20;
				if(y == GRID_HEIGHT - 1 || grid->matrix[y + 1][x] > 0) magic += 20;
				if(x == GRID_WIDTH - 1  || grid->matrix[y][x + 1] > 0) magic += 20;
			}
		}
	}

	// remove complete lines
	for(y = 0; y < GRID_HEIGHT; y++) {
		for(x = 0; x < GRID_WIDTH; x++) {
			if(!grid->matrix[y][x]) break;
		}
		if(x == GRID_WIDTH) continue;

		for(i = y; i > 0; i--)
			for(x = 0; x < GRID_WIDTH; x++)
				grid->matrix[i][x] = grid->matrix[i - 1][x];
		for(x = 0; x < GRID_WIDTH; x++) grid->matrix[0][x] = 0;
	}

	int height = 0;
	for(y = GRID_HEIGHT - 1; y >= 0; y--) {
		for(x = 0; x < GRID_WIDTH; x++) {
			if(grid->matrix[y][x]) height = 19 - y;
		}
	}

	magic -= height * 4;
	return magic;
}


static void grid_bot(Grid* grid, int* mov, int* rot, int* drop) {

	static Grid bot_grid;
	Grid* bot = &bot_grid;

	*mov = 0;
	*rot = 0;
	*drop = 1;

	int magic = 0;
	int first = 1;

	int save_rot = grid->rot;
	int r;
	for(r = 0; r < 4; r++) {
		if(grid_collision(grid, 0)) break;

		int save_x = grid->x;
		int dir = grid->stone_count & 1	? -1 : 1;
		while(!grid_collision(grid, 0)) grid->x -= dir;
		grid->x += dir;

		while(!grid_collision(grid, 0)) {
			int save_y = grid->y;

			while(!grid_collision(grid, 0)) grid->y++;
			grid->y--;
			if(!grid_collision(grid, 1)) {
				memcpy(bot, grid, sizeof(Grid));
				stone_to_grid(bot, 127);

				int m = rate_grid(bot);
				if(first || m > magic) {
					magic = m;
					first = 0;

					*mov = (grid->x > save_x) - (grid->x < save_x);
					*rot = (rand()%2) ? 0 : save_rot != grid->rot;
					*drop = grid->x - save_x == 0 && save_rot == grid->rot;
				}
			}
			grid->y = save_y;
			grid->x += dir;
		}
		grid->x = save_x;
		grid->rot = grid->rot * 2 % 15;
	}
	grid->rot = save_rot;

}


static void get_grid_input(Grid* grid, int* mov, int* rot, int* drop,int getkey(int)) {
	if(grid->nr == 0) {
		*mov = getkey(1)-getkey(3);
		*rot = getkey(0);
		*drop = getkey(2);
	}
	else {
		grid_bot(grid, mov, rot, drop);
	}

	if(*mov != grid->input_mov) grid->input_rep = 0;
	grid->input_mov = *mov;
	if(grid->input_rep <= 0) grid->input_rep = 6;
	else {
		grid->input_rep--;
		*mov = 0;
	}
	if(*rot != grid->input_rot) grid->input_rot = *rot;
	else *rot = 0;
}



static void update_grid_normal(Grid* grid,int getkey(int)) {
	int i, x, y;

	int mov, rot, drop;
	get_grid_input(grid, &mov, &rot, &drop,getkey);

	// rotation
	if(rot) {
		i = grid->rot;
		grid->rot = (rot > 0)
			? grid->rot * 2 % 15
			: (grid->rot / 2 | grid->rot * 8) & 15;
		if(grid_collision(grid, 0)) grid->rot = i;
	}


	// horizontal movement
	i = grid->x;
	grid->x += mov;
	if(i != grid->x && grid_collision(grid, 0)) grid->x = i;


	// vertical movement
	grid->tick++;
	if(drop || grid->tick >= grid->ticks_per_drop) {
		grid->tick = 0;
		grid->y++;
		if(grid_collision(grid, 0)) {
			grid->y--;

			int over = 0;
			// check for game over
			if(grid_collision(grid, 1)) {
				grid->state_delay = 0;
				grid->state = STATE_GAMEOVER;
				over = 1;
			}

			stone_to_grid(grid, grid->stone + 1);

			if(over) return;


			// get a new stone
			new_stone(grid);

			// check for complete lines
			int lines = 0;
			for(y = 0; y < GRID_HEIGHT; y++) {

				for(x = 0; x < GRID_WIDTH; x++)
					if(!grid->matrix[y][x]) break;

				lines += (x == GRID_WIDTH);
				grid->highlight[y] = (x == GRID_WIDTH);
			}
			grid->state = lines ? STATE_CLEARLINES : STATE_WAIT;
			grid->state_delay = 0;
		}
	}
}


static void update_grid_clearlines(Grid* grid) {
	int i, x, y;

	// animations
	switch(grid->animation) {

	case ANIMATION_BLINK:
		i = (grid->state_delay & 2) ? COLOR_WHITE : COLOR_BLACK;
		for(y = 0; y < GRID_HEIGHT; y++) {
			if(!grid->highlight[y]) continue;
			for(x = 0; x < GRID_WIDTH; x++) grid->matrix[y][x] = i;
		}
		break;

	case ANIMATION_SHIFT:
		if(grid->state_delay & 1) {
			for(y = 0; y < GRID_HEIGHT; y++) {
				if(!grid->highlight[y]) continue;
				if(y & 1) {
					for(x = 1; x < GRID_WIDTH; x++)
						grid->matrix[y][x - 1] = grid->matrix[y][x];
					grid->matrix[y][GRID_WIDTH - 1] = 0;
				}
				else {
					for(x = GRID_WIDTH - 1; x > 0; x--)
						grid->matrix[y][x] = grid->matrix[y][x - 1];
					grid->matrix[y][0] = 0;
				}
			}
		}
		break;

	case ANIMATION_VANISH:
		if(grid->state_delay & 1) {
			for(y = 0; y < GRID_HEIGHT; y++) {
				if(!grid->highlight[y]) continue;

				x = rand() % GRID_WIDTH;
				for(i = 0; i < GRID_WIDTH; i++) {
					if(grid->matrix[y][x]) {
						grid->matrix[y][x] = 0;
						break;
					}
					if(++x == GRID_WIDTH) x = 0;
				}

			}
		}
		break;

	default: break;
	}

	// erase lines
	if(++grid->state_delay >= 24) {
		for(y = 0; y < GRID_HEIGHT; y++) {
			if(!grid->highlight[y]) continue;
			grid->highlight[y] = 0;

			grid->lines++;
			//push_lines(grid->nr, grid->lines);

			grid->level_progress++;
			if(grid->level_progress == 10) {
				grid->level_progress = 0;
				grid->ticks_per_drop--;
				if(grid->ticks_per_drop == 0) grid->ticks_per_drop = 1;
			}

			for(i = y; i > 0; i--)
				for(x = 0; x < GRID_WIDTH; x++)
					grid->matrix[i][x] = grid->matrix[i - 1][x];
			
			for(x = 0; x < GRID_WIDTH; x++) grid->matrix[0][x] = 0;
		}
		grid->state = STATE_NORMAL;

		// switch to next animation
		if(++grid->animation == ANIMATION_COUNT) grid->animation = 0;

	}
}

static void update_grid_wait(Grid* grid) {
	if(++grid->state_delay > 15) grid->state = STATE_NORMAL;
}

static void update_grid_free(Grid* grid) {
	++grid->state_delay;
}

static void update_grid_gameover(Grid* grid) {
	int x, y, i;
	i = (grid->state_delay & 2) ? COLOR_WHITE : COLOR_BLACK;

	for(x = 0; x < GRID_WIDTH; x++) {
		for(y = 0; y < GRID_HEIGHT; y++) {
			grid->matrix[y][x] = i;
		}
	}

	if(++grid->state_delay > 25) {
		//player_gameover(grid->nr,grid->lines);
		init_grid(grid, grid->nr);
	}
}


void suspend_grid(Grid* grid) {
	grid->state = STATE_FREE;
}
void init_grid(Grid* grid, int nr) {
	grid->nr = nr;
	grid->ticks_per_drop = 20;
	grid->level_progress = 0;
	grid->lines = 0;
	grid->animation = 0;
//	grid->state = STATE_FREE;
	grid->state = STATE_NORMAL;
	memset(grid->matrix, 0, sizeof(grid->matrix));
	memset(grid->highlight, 0, sizeof(grid->highlight));
	grid->stone_count = -1;
	new_stone(grid);
	new_stone(grid);
	int x, y;
	for(x = 0; x < 12; x++) {
		for(y = 0; y < 32; y++) {
//			pixel(grid->nr * 12 + x, y, 0);
//			set_frame_buffer(grid->nr * 12 + x, y, 0);
		}
	}
}


void update_grid(Grid* grid,int getkey(int)) {
	switch(grid->state) {
	case STATE_FREE:
		update_grid_free(grid);
		break;

	case STATE_NORMAL:
		update_grid_normal(grid,getkey);
		break;

	case STATE_WAIT:
		update_grid_wait(grid);
		break;

	case STATE_CLEARLINES:
		update_grid_clearlines(grid);
		break;

	case STATE_GAMEOVER:
		update_grid_gameover(grid);
		break;

	default: break;

	}
}


static void pixel2(int x, int y, int color,unsigned int* pixelbuffer,int zoom)
{
	uint8_t r[] = {30,100,255,0   ,255,255,255,  0,127,0  ,127,127,0  ,255,255,255};
	uint8_t g[] = {30,100 ,0  ,255,255,0  ,0  ,255,127,127,255,0  ,127,0  ,127,0  };
	uint8_t b[] = {30,0   ,0  ,0  ,0  ,255,255,255,255,255,127,255,127,127,0  ,255};
				
			
				
	unsigned int col = (r[color]<<16)+(g[color]<<8)+b[color];

				if(pixelbuffer[((y*(zoom/2))*36*zoom)+x*(zoom/2)] != col)
					for(int a = 0; a < (zoom/2)-1;a++)
					{
						for(int b = 0;b < (zoom/2)-1;b++)
						{
							pixelbuffer[((y*(zoom/2)+a)*36*zoom)+x*(zoom/2)+b] = col;
						}
					}

//	x+=3;
//	y+=3;

//	setLedXY(x*3,y*3,r[color],g[color],b[color]);
//	setLedXY(x*3+1,y*3+1,r[color],g[color],b[color]);
//	setLedXY(x*3,y*3+1,r[color],g[color],b[color]);
//	setLedXY(x*3+1,y*3,r[color],g[color],b[color]);

}
static void pixel(int x, int y, int color,unsigned int* pixelbuffer,int zoom)
{
	uint8_t r[] = {30,100,255,0   ,255,255,255,  0,127,0  ,127,127,0  ,255,255,255};
	uint8_t g[] = {30,100 ,0  ,255,255,0  ,0  ,255,127,127,255,0  ,127,0  ,127,0  };
	uint8_t b[] = {30,0   ,0  ,0  ,0  ,255,255,255,255,255,127,255,127,127,0  ,255};
				
				
				
	unsigned int col = (r[color]<<16)+(g[color]<<8)+b[color];

				if(pixelbuffer[((y*zoom)*36*zoom)+x*zoom] != col)
					for(int a = 0; a < zoom-1;a++)
					{
						for(int b = 0;b < zoom-1;b++)
						{
							pixelbuffer[((y*zoom+a)*36*zoom)+x*zoom+b] = col;
						}
					}

//	x+=3;
//	y+=3;

//	setLedXY(x*3,y*3,r[color],g[color],b[color]);
//	setLedXY(x*3+1,y*3+1,r[color],g[color],b[color]);
//	setLedXY(x*3,y*3+1,r[color],g[color],b[color]);
//	setLedXY(x*3+1,y*3,r[color],g[color],b[color]);

}

static unsigned int font5x3[] = {32319,17393,24253,32437,31879,30391,29343,31905,32447,31911};

static void render_digit_5x3(int x,int y, int digit,int color,unsigned int* pixelbuffer,int zoom)
{
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<5;j++)
		{
			if(digit == -1)
				pixel2(x+i,y+j,0,pixelbuffer,zoom);
			else
			{
				if(font5x3[digit] & (1<<(i*5+j)))
				{
					pixel2(x+i,y+j,color,pixelbuffer,zoom);
				}
				else
				{
					pixel2(x+i,y+j,0,pixelbuffer,zoom);
				}

			}
		}
	}
}

static void render_digits_5x3(int x, int y, const char *text, int color,unsigned int* pixelbuffer,int zoom)
{
	while (*text)
	{
		render_digit_5x3(x,y,(*text)-48,color,pixelbuffer,zoom);
		x+=4;
		text++;
	}

}

static void render_num(int number,int x,int y,int length,int pad, int color,unsigned int* pixelbuffer,int zoom)
{

	char s[10];
	snprintf(s,10, "%i", (int)number);

	int len = strlen(s);

	if (length < len) {
		int i;
		for (i = 0; i < length; i++) {
			render_digit_5x3(x, y, -1, color,pixelbuffer,zoom);
			x+=4;
		}
		return;
	}
	int i;
	for (i = 0; i < length - len; i++) {
		render_digit_5x3(x, y, pad, color,pixelbuffer,zoom);
		x+=4;
	}
	
	render_digits_5x3(x, y, (char*)s, color,pixelbuffer,zoom);

}




void draw_grid(Grid* grid,int nr,unsigned int* pixelbuffer,int zoom) {
	int x, y;

	switch(grid->state) {
	case STATE_FREE:

		// TODO: nice scroll text

		break;

	case STATE_NORMAL:
	case STATE_WAIT:
	case STATE_CLEARLINES:
	case STATE_GAMEOVER:

		// preview
		for(y = 0; y < 4; y++) {
			for(x = 0; x < 4; x++) {
				int color = COLOR_BLACK;
				if(STONES[grid->next_stone][x * 4 + y] & grid->next_rot) {
					color = grid->next_stone + 1;
				}
//				set_frame_buffer(grid->nr * 12 + x + 7, y + 6, PALETTE[color]);
				pixel(grid->nr * 12 + x + 7, y + 1, PALETTE[color],pixelbuffer,zoom);
			}
		}
		// matrix
		for(y = 0; y < GRID_HEIGHT; y++) {
			for(x = 0; x < GRID_WIDTH; x++) {
				int color = grid->matrix[y][x];
				if(	grid->state == STATE_NORMAL &&
					x >= grid->x && x < grid->x + 4 &&
					y >= grid->y && y < grid->y + 4 &&
					STONES[grid->stone][(x - grid->x) * 4 + y - grid->y] & grid->rot) {
					color = grid->stone + 1;
				}
//				set_frame_buffer(grid->nr * 12 + 1 + x, y + 11, PALETTE[color]);
				pixel(grid->nr * 12 + 1 + x, y + 6, PALETTE[color],pixelbuffer,zoom);
			}
		}
		// score
		render_num(grid->lines, grid->nr * 12 * 2 + 2, 5, 3, 0, 8,pixelbuffer,zoom);

		break;

	default: break;

	}


}


