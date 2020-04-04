#pragma once
#include "MKBState.hpp"
#include "Settings.hpp"
#include "TimerManager.hpp"
#include "main.hpp"

#include <Windows.h>

namespace NumpadAsMouse {
	LRESULT CALLBACK LLKBProc(_In_ int nCode,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam);
	LRESULT CALLBACK LLMouseProc(_In_ int nCode,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam);

	// hook procedures call these functions; KeyUp and KeyDown should return
	// true only if the vkkey is to be intercepted
	bool KeyUp(int vkCode);
	bool KeyDown(int vkCode);
	void MouseMove(POINT mouse_pos);

	// sets up an INPUT structure for MOUSEEVENT
	INPUT PrepMouseEvent();
}
