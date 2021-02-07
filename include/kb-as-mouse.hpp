#pragma once

#include "kb-hook.hpp"
#include "state.hpp"
#include "timer-proc.hpp"

#include <rain.hpp>

#include <Shellapi.h>

int CALLBACK WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow);

namespace KbAsMouse {
	int start(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow);
	LRESULT CALLBACK mainWndProc(HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);
}
