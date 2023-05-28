#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

#include "Game.h"

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	Game game;
	
	game.Init();

	game.Run();

	game.Stop();

	return 0;
};