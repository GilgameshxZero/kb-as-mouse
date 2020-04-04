#pragma once
#include "MKBState.hpp"
#include "Settings.hpp"
#include "main.hpp"

#include <Windows.h>
#include <algorithm>
#include <cmath>

namespace NumpadAsMouse {
	extern int mspf;	// milliseconds per frame
	extern DWORD last_frame;
	extern std::pair<LONG, LONG>
		last_mouse_pos;	 // position of the mouse, in dx and dy, at the time
										 // TimerProc last sent a mouse position update message;
										 // only used in non-DiffMode
	extern bool timer_running;
	extern ULONG_PTR event_extra_info;

	VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD time);
}
