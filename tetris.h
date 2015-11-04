#ifndef TETRIS_H_
#define TETRIS_H_

void tetris_load();
void tetris_update(int getkey(int));
void tetris_render(unsigned int* pixelbuffer,int zoom);
void tetris_suspend();
void reset_player(int nr);

#endif
