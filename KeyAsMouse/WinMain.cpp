#include "WinMain.h"

int CALLBACK WinMain (_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	//get width and height
	RECT wrect;
	GetWindowRect (GetDesktopWindow (), &wrect);

	//open settings file
	std::ifstream settings ("settings.txt");
	if (!settings.good ())
	{
		MessageBox (NULL, "No settings.txt file found.", "KeyAsMouse", MB_OK);
		return 0;
	}

	//initialize mkbstate
	KeyAsMouse::MKBState::keydown.resize (256, false);
	POINT p;
	GetCursorPos (&p);
	KeyAsMouse::MKBState::mousex = static_cast<double>(p.x);
	KeyAsMouse::MKBState::mousey = static_cast<double>(p.y);
	KeyAsMouse::MKBState::lbuttondown = KeyAsMouse::MouseEvent::LButtonDown;
	KeyAsMouse::MKBState::lbuttonup = KeyAsMouse::MouseEvent::LButtonUp;
	KeyAsMouse::MKBState::rbuttondown = KeyAsMouse::MouseEvent::RButtonDown;
	KeyAsMouse::MKBState::rbuttonup = KeyAsMouse::MouseEvent::RButtonUp;
	KeyAsMouse::MKBState::mbuttondown = KeyAsMouse::MouseEvent::MButtonDown;
	KeyAsMouse::MKBState::mbuttonup = KeyAsMouse::MouseEvent::MButtonUp;
	settings >> KeyAsMouse::MKBState::left >> KeyAsMouse::MKBState::right >> KeyAsMouse::MKBState::up >> KeyAsMouse::MKBState::down
		>> KeyAsMouse::MKBState::lclick >> KeyAsMouse::MKBState::rclick >> KeyAsMouse::MKBState::mclick
		>> KeyAsMouse::MKBState::wheelup >> KeyAsMouse::MKBState::wheeldown;

	//initialize stateprocess
	KeyAsMouse::StateProcess::velx = KeyAsMouse::StateProcess::vely = 0;
	settings >> KeyAsMouse::StateProcess::axmul >> KeyAsMouse::StateProcess::friction >> KeyAsMouse::StateProcess::minvel >> KeyAsMouse::StateProcess::maxvel >> KeyAsMouse::StateProcess::thresh;

	//initialize mouseevent and timer
	KeyAsMouse::MouseEvent::lasttime = GetTickCount ();
	KeyAsMouse::MouseEvent::wheelsum = 0;
	KeyAsMouse::MouseEvent::width = wrect.right;
	KeyAsMouse::MouseEvent::height = wrect.bottom;
	settings >> KeyAsMouse::MouseEvent::fps;
	KeyAsMouse::MouseEvent::spf = 1000.0 / KeyAsMouse::MouseEvent::fps;
	SetTimer (NULL, KeyAsMouse::MouseEvent::timerid, USER_TIMER_MINIMUM, KeyAsMouse::MouseEvent::TimerProc);

	settings.close ();

	//install hooks and message loop
	HHOOK llkbhook = SetWindowsHookEx (WH_KEYBOARD_LL, KeyAsMouse::LLHook::LLKBProc, NULL, NULL),
		llmousehook = SetWindowsHookEx (WH_MOUSE_LL, KeyAsMouse::LLHook::LLMouseProc, NULL, NULL);

	BOOL msgret;
	MSG msg;

	while ((msgret = GetMessage (&msg, NULL, 0, 0)) != 0)
	{
		if (msgret == -1) //Something is terribly wrong.
			return -1;
		else
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}

	UnhookWindowsHookEx (llkbhook);
	UnhookWindowsHookEx (llmousehook);

	return 0;
}