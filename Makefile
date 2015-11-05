COMPILER = gcc
FLAGS= -O3 --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes 
LDFLAGS=

FLAGS+= `sdl2-config --cflags`
LDFLAGS+= `sdl2-config --libs`

all: tetris 

clean:
	rm -f tetris
	rm -f tetris.exe
	rm -f tetris_win.zip
	rm -f tetris.rc
	rm -f tetris.ico
	rm -rf Tetris.app/Contents/Frameworks/SDL2.framework
	rm -f Tetris.app/Contents/MacOS/tetris_osx
	rm -f Tetris.app/Contents/Resources/iconfile.icns
	rm -f tetrisrc.o

tetris: main.c sdl_util.c sdl_util.h grid.c grid.h tetris.h tetris.c Makefile 
	@$(COMPILER) $(FLAGS) main.c grid.c tetris.c sdl_util.c $(LDFLAGS) -o tetris 

tetris_osx: main.c sdl_util.c sdl_util.h grid.c grid.h tetris.h tetris.c Makefile 
	$(COMPILER) $(FLAGS) main.c grid.c tetris.c sdl_util.c $(LDFLAGS) -o tetris_osx
	@install_name_tool -change /usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib @executable_path/../Frameworks/SDL2.framework/libSDL2-2.0.0.dylib tetris_osx
	@makeicns -32 tetris_36x36x4.png -out Tetris.app/Contents/Resources/iconfile.icns 
	@mv tetris_osx Tetris.app/Contents/MacOS
	cp -f /usr/local/lib/libSDL2-2.0.0.dylib Tetris.app/Contents/Frameworks/SDL2.framework/libSDL2-2.0.0.dylib
	@touch Tetris.app
	@zip -r tetris_osx.zip Tetris.app 

tetris.ico: tetris_36x36x4.png Makefile
	icotool -c -o tetris.ico tetris_36x36x4.png

tetris.exe: tetris_36x36x4.png tetris.ico main.c sdl_util.c sdl_util.h grid.c grid.h tetris.h tetris.c Makefile SDL2-2.0.4
	echo "0 ICON tetris.ico" > tetris.rc
	i686-w64-mingw32-windres tetris.rc tetrisrc.o
	i686-w64-mingw32-gcc -static -std=gnu99 -ISDL2-2.0.4/i686-w64-mingw32/include/SDL2 -D_GNU_SOURCE=1 -Dmain=SDL_main -LSDL2-2.0.4/i686-w64-mingw32/lib  main.c grid.c tetris.c sdl_util.c -lmingw32 -lSDL2main -lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc  tetrisrc.o -o tetris.exe

tetris_win.zip: tetris.exe
	zip -j tetris_win.zip tetris.exe 
	

SDL2-2.0.4:
	wget https://www.libsdl.org/tmp/release/SDL2-devel-2.0.4-mingw.tar.gz
	tar -xzf SDL2-devel-2.0.4-mingw.tar.gz
	rm -f SDL2-devel-2.0.4-mingw.tar.gz


.PHONY : clean all


#@$(COMPILER) -framework SDL2 -I/Library/Frameworks/SDL2.framework/Headers --std=gnu99 main.c grid.c tetris.c sdl_util.c -o tetris_osx 

