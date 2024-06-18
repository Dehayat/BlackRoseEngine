#if _WIN64
#define SDL_MAIN_HANDLED
#endif

#include "Game.h"

int main(int argc, char* argv[]) {
	Game game;
	game.Run();
	return 0;
}