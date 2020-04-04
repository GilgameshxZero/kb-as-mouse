#pragma once
#include "../rain/cpp/rain.hpp"

#include "HookEventManager.hpp"
#include "Settings.hpp"
#include "TimerManager.hpp"

#include <algorithm>
#include <fstream>
#include <map>

int CALLBACK WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow);

namespace NumpadAsMouse {
	extern int screen_width, screen_height;

	int start(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow);
}
