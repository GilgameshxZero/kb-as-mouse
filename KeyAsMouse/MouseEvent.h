#pragma once

#include "StateProcess.h"
#include <Windows.h>

namespace KeyAsMouse
{
	//call functions to process mouse state, and send messages to system
	namespace MouseEvent
	{
		//timerproc, called rapidly to update position of mouse and scroll//speed is not based on fps (the time parameter ensures this)
		VOID CALLBACK TimerProc (_In_ HWND hwnd, _In_ UINT uMsg, _In_ UINT_PTR idEvent, _In_ DWORD time);

		//button press
		void LButtonUp ();
		void LButtonDown ();
		void RButtonUp ();
		void RButtonDown ();
		void MButtonUp ();
		void MButtonDown ();

		//screen width and height
		extern int width, height;

		//last time the timer popped
		extern DWORD lasttime;

		//send a wheel message when wheelsum > WHEEL_DELTA
		extern int wheelsum;

		//fps
		extern int fps;
		extern UINT spf;

		//timer var
		extern const int timerid;
	};
}