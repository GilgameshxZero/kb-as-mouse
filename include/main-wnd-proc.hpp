#pragma once

#include <rain.hpp>

namespace KbAsMouse {
	LRESULT CALLBACK mainWndProc(
		_In_ HWND hWnd,
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam);
}