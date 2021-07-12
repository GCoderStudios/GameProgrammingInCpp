#include "Game.h"

int main(int argc, char** argv) {
	Game pong;
	bool successfullyInititlized = pong.Initialize();
	if (successfullyInititlized) {
		pong.RunGameLoop();
	}
	pong.Shutdown();
	return 0;
}