#pragma once
#include "../rain/src/rain.hpp"

#include "kb-hook.hpp"
#include "state.hpp"
#include "timer-proc.hpp"

#include <Shellapi.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

namespace NumpadAsMouse {
	extern State state;

	int start(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	LRESULT CALLBACK mainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}
