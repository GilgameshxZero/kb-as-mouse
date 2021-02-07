#pragma once

#include "state.hpp"
#include "timer-proc.hpp"

#include <rain.hpp>

namespace KbAsMouse {
	LRESULT CALLBACK LLKBProc(_In_ int nCode,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam);

	// hook procedures call these functions; KeyUp and KeyDown should return
	// true only if the vkkey is to be intercepted
	bool KeyDown(int vkCode);
	bool KeyUp(int vkCode);
}
