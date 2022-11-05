all:
	gcc -Isrc/include -L/src/lib -Wall -o main.exe main.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf