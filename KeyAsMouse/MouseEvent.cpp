#include "MouseEvent.h"

namespace KeyAsMouse
{
	namespace MouseEvent
	{
		const int timerid = 19993; //arbitrary
		int fps = 30;
		UINT spf;
		int width, height;
		DWORD lasttime;
		int wheelsum;
	}
}

VOID CALLBACK KeyAsMouse::MouseEvent::TimerProc (_In_ HWND hwnd, _In_ UINT uMsg, _In_ UINT_PTR idEvent, _In_ DWORD time)
{
	//update pos, if there is change
	static int origx, origy;
	origx = static_cast<int>(MKBState::mousex);
	origy = static_cast<int>(MKBState::mousey);
	StateProcess::UpdateXY (&MKBState::mousex, &MKBState::mousey, static_cast<int>(time - lasttime));

	if (static_cast<int>(MKBState::mousex) != origx || static_cast<int>(MKBState::mousey) != origy)
	{
		static INPUT iev;
		iev.type = INPUT_MOUSE;
		iev.mi.dx = static_cast<LONG>(MKBState::mousex * 65535 / width);
		iev.mi.dy = static_cast<LONG>(MKBState::mousey * 65535 / height);
		iev.mi.mouseData = 0;
		iev.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
		SendInput (1, &iev, sizeof (INPUT));
	}

	wheelsum += -MKBState::GetScrollAxis () * (time - lasttime);

	//send scroll message if axis is not 0
	if (wheelsum >= 100 || wheelsum <= -100)
	{
		static INPUT iev;
		iev.type = INPUT_MOUSE;
		iev.mi.dx = static_cast<LONG>(MKBState::mousex * 65535 / width);
		iev.mi.dy = static_cast<LONG>(MKBState::mousey * 65535 / height);
		iev.mi.dwFlags = MOUSEEVENTF_WHEEL;

		if (wheelsum >= 100)
		{
			iev.mi.mouseData = WHEEL_DELTA;
			wheelsum -= 100;
		}
		else
		{
			iev.mi.mouseData = -WHEEL_DELTA;
			wheelsum += 100;
		}
		SendInput (1, &iev, sizeof (INPUT));
	}

	lasttime = time;

	//set new timer based on fps
	SetTimer (NULL, timerid, spf, TimerProc);

	return VOID ();
}

void KeyAsMouse::MouseEvent::LButtonUp ()
{
	static INPUT iev;
	iev.type = INPUT_MOUSE;
	iev.mi.dx = static_cast<LONG>(MKBState::mousex * 65535 / width);
	iev.mi.dy = static_cast<LONG>(MKBState::mousey * 65535 / height);
	iev.mi.mouseData = 0;
	iev.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput (1, &iev, sizeof (INPUT));
}

void KeyAsMouse::MouseEvent::LButtonDown ()
{
	static INPUT iev;
	iev.type = INPUT_MOUSE;
	iev.mi.dx = static_cast<LONG>(MKBState::mousex * 65535 / width);
	iev.mi.dy = static_cast<LONG>(MKBState::mousey * 65535 / height);
	iev.mi.mouseData = 0;
	iev.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput (1, &iev, sizeof (INPUT));
}

void KeyAsMouse::MouseEvent::RButtonUp ()
{
	static INPUT iev;
	iev.type = INPUT_MOUSE;
	iev.mi.dx = static_cast<LONG>(MKBState::mousex * 65535 / width);
	iev.mi.dy = static_cast<LONG>(MKBState::mousey * 65535 / height);
	iev.mi.mouseData = 0;
	iev.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput (1, &iev, sizeof (INPUT));
}

void KeyAsMouse::MouseEvent::RButtonDown ()
{
	static INPUT iev;
	iev.type = INPUT_MOUSE;
	iev.mi.dx = static_cast<LONG>(MKBState::mousex * 65535 / width);
	iev.mi.dy = static_cast<LONG>(MKBState::mousey * 65535 / height);
	iev.mi.mouseData = 0;
	iev.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput (1, &iev, sizeof (INPUT));
}

void KeyAsMouse::MouseEvent::MButtonUp ()
{
	static INPUT iev;
	iev.type = INPUT_MOUSE;
	iev.mi.dx = static_cast<LONG>(MKBState::mousex * 65535 / width);
	iev.mi.dy = static_cast<LONG>(MKBState::mousey * 65535 / height);
	iev.mi.mouseData = 0;
	iev.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
	SendInput (1, &iev, sizeof (INPUT));
}

void KeyAsMouse::MouseEvent::MButtonDown ()
{
	static INPUT iev;
	iev.type = INPUT_MOUSE;
	iev.mi.dx = static_cast<LONG>(MKBState::mousex * 65535 / width);
	iev.mi.dy = static_cast<LONG>(MKBState::mousey * 65535 / height);
	iev.mi.mouseData = 0;
	iev.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
	SendInput (1, &iev, sizeof (INPUT));
}
