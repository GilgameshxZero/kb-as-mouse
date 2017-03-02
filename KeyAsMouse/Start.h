#pragma once
#include "HookEventManager.h"
#include "Settings.h"
#include "TimerManager.h"
#include "RainError.h"
#include "RainUtility.h"
#include <algorithm>
#include <fstream>
#include <map>
#include <Windows.h>

namespace KeyAsMouse {
	namespace Start {
		extern int screen_width, screen_height;

		int Start (HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR lpCmdLine,
			int nCmdShow);
	}
}