#pragma once
#include "../rain/src/rain.hpp"

#include "state.hpp"
#include "main.hpp"

namespace NumpadAsMouse {
	VOID CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD time);
}
