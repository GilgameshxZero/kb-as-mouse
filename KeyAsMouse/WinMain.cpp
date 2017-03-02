#include "WinMain.h"

int CALLBACK WinMain (HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow) {
	return KeyAsMouse::Start::Start (hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}