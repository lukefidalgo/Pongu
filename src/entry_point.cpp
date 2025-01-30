#include <windows.h>
#include "game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Game pong(800, 600, "Pong");
	pong.run();
	return 0;
}
