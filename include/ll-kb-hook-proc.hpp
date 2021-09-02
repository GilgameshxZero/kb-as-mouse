#pragma once

#include <rain.hpp>

namespace KbAsMouse {
	LRESULT CALLBACK
	llKbHookProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
	void keyEventsHandler();
	void physicsHandler();
}
