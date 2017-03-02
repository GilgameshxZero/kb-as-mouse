#pragma once
#include "MKBState.h"
#include "Settings.h"
#include "Start.h"
#include <algorithm>
#include <cmath>
#include <Windows.h>

namespace KeyAsMouse {
	namespace TimerManager {
		extern int mspf; //milliseconds per frame
		extern DWORD last_frame;
		extern std::pair<LONG, LONG> last_mouse_pos; //position of the mouse, in dx and dy, at the time TimerProc last sent a mouse position update message
		extern bool timer_running;

		VOID CALLBACK TimerProc (HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD time);
	}
}