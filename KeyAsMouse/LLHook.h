#pragma once

#include "MKBState.h"
#include <Windows.h>

namespace KeyAsMouse
{
	namespace LLHook
	{
		LRESULT CALLBACK LLKBProc (_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);

		//keep track of mousemove messages, and pass them to 
		LRESULT CALLBACK LLMouseProc (_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);

		//screen dpi scaling
		extern double dpiscale;
	};
}