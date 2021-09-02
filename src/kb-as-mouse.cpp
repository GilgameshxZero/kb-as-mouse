#include <kb-as-mouse.hpp>
#include <ll-kb-hook-proc.hpp>
#include <main-wnd-proc.hpp>
#include <state.hpp>

#include <rain.hpp>

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {
	// Parse command line.
	KbAsMouse::State &state = KbAsMouse::state;
	Rain::String::CommandLineParser parser;

	bool showHelp = false;
	parser.addParser("help", showHelp);
	parser.addParser("h", showHelp);

	parser.addParser("up", state.vkUp);
	parser.addParser("right", state.vkRight);
	parser.addParser("down", state.vkDown);
	parser.addParser("left", state.vkLeft);
	parser.addParser("mouse-left", state.vkMouseLeft);
	parser.addParser("mouse-right", state.vkMouseRight);
	parser.addParser("mouse-middle", state.vkMouseMiddle);
	parser.addParser("scroll-up", state.vkScrollUp);
	parser.addParser("scroll-down", state.vkScrollDown);
	parser.addParser("scroll-left", state.vkScrollLeft);
	parser.addParser("scroll-right", state.vkScrollRight);
	parser.addParser("slow", state.vkSlow);
	parser.addParser("pause", state.vkPause);

	parser.addParser("fps", state.fps);
	parser.addParser("mouse-drag", state.mouseDrag);
	parser.addParser("mouse-acc", state.mouseAcc);
	parser.addParser("scroll-drag", state.scrollDrag);
	parser.addParser("scroll-acc", state.scrollAcc);
	parser.addParser("slow-mod", state.slowMod);

	// Default FPS is set to that of the current display.
	DEVMODE displayEnv{};
	Rain::Windows::validateSystemCall(
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &displayEnv));
	state.fps = displayEnv.dmDisplayFrequency;

	try {
		parser.parse(__argc - 1, __argv + 1);
	} catch (...) {
		MessageBox(
			NULL,
			"Failed to parse command-line options. Consider running with --help.",
			"kb-as-mouse",
			MB_OK);
		return -1;
	}

	if (showHelp) {
		MessageBox(
			NULL,
			"Command-line options (default specified in parenthesis):\n"
			"--help, -h (off): Display this help message and exit.\n"
			"--up (80 (P)): Virtual key code for mouse movement up.\n"
			"--right (222 (' or \")): Virtual key code for mouse movement right.\n"
			"--down (186 (; or :): Virtual key code for mouse movement down.\n"
			"--left (76 (L)): Virtual key code for mouse movement left.\n"
			"--mouse-left (165 (right ALT)): Virtual key code for left click.\n"
			"--mouse-right (188 (,)): Virtual key code for right click.\n"
			"--mouse-middle (190 (.)): Virtual key code for middle click.\n"
			"--scroll-up (219 ([ or {)): Virtual key code for continuous scrolling "
			"up.\n"
			"--scroll-down (79 (O)): Virtual key code for continuous scrolling "
			"down.\n"
			"--scroll-left (221 (] or })): Virtual key code for single scroll left.\n"
			"--scroll-right (220 (\\ or |)): Virtual key code for single scroll "
			"right.\n"
			"--slow (13 (ENTER)): Virtual key code to apply slowness modifier to "
			"movement keys.\n"
			"--pause (93 (CONTEXT MENU)): Virtual key code for pausing/unpausing "
			"kb-as-mouse.\n"
			"\n"
			"--fps (90): While simulating mouse keys, the frames-per-second of the "
			"physics engine.\n"
			"--mouse-drag (10): Unitless drag for mouse velocity. A drag of x will "
			"nullify 1/x of the velocity per second.\n"
			"--mouse-acc (120): Pixels/second/second acceleration while mouse "
			"movement keys are held. Additionally scaled by application DPI. An "
			"acceleration of x will increase the velocity by x per second.\n"
			"--scroll-drag (10): Unitless drag for scroll velocity. A drag of x will "
			"nullify 1/x of the velocity per second.\n"
			"--scroll-acc (400): WHEEL_DELTAs/second/second acceleration while "
			"scroll keys  are held. An acceleration of x will increase the velocity "
			"by x per second. A WHEEL_DELTA is one detund on the scroll wheel.\n"
			"--slow-mod (0.2): Slowness modifier applied to acceleration when slow "
			"key is held; scrolling is reduced to single scrolls instead of having "
			"the modifier applied. A modifier of x will also apply a modifier of 1/x "
			"to the corresponding drag. \n",
			"kb-as-mouse",
			MB_OK);
		return 0;
	}

	// Register HWND for the tray icon.
	WNDCLASSEX mainWndClass{
		sizeof(WNDCLASSEX),
		NULL,
		KbAsMouse::mainWndProc,
		0,
		0,
		hInstance,
		NULL,
		NULL,
		NULL,
		NULL,
		"kb-as-mouse-main-wnd",
		NULL};
	Rain::Windows::validateSystemCall(RegisterClassEx(&mainWndClass));
	HWND mainWnd = Rain::Windows::validateSystemCall(CreateWindowEx(
		WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT |
			WS_EX_TOOLWINDOW,
		mainWndClass.lpszClassName,
		"",
		WS_POPUP,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		hInstance,
		NULL));
	state.notifyIconOpen = static_cast<HICON>(
		LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
	state.notifyIconClosed = static_cast<HICON>(
		LoadImage(hInstance, MAKEINTRESOURCE(2), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
	state.notifyIconData = {
		sizeof(NOTIFYICONDATA),
		mainWnd,
		1,
		NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP,
		WM_CONTEXTMENU,
		NULL,
		"",
		0,
		0,
		{},
		// Initializes uVersion of the union; prefer using designated initializers
		// in C++20+; but this is fine since both members of the union are the same
		// size.
		{NOTIFYICON_VERSION_4},
		{},
		0,
		{},
		{}};
	state.updateNotifyIconData();
	Rain::Windows::validateSystemCall(
		Shell_NotifyIcon(NIM_ADD, &state.notifyIconData));

	// Low-level keyboard hook.
	HHOOK llKbHook = Rain::Windows::validateSystemCall(
		SetWindowsHookEx(WH_KEYBOARD_LL, KbAsMouse::llKbHookProc, hInstance, NULL));

	// Key/physics event handlers.
	auto keyEventsHandlerThread = std::thread(KbAsMouse::keyEventsHandler),
			 physicsHandlerThread = std::thread(KbAsMouse::physicsHandler);

	// Pump message loop for the tray icon.
	BOOL bRet;
	MSG msg;
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (bRet == -1) {
			MessageBox(NULL, "GetMessage returned -1.", "kb-as-mouse", MB_OK);
			break;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Trigger the events handlers and shut them down.
	state.terminating = true;
	state.keyEventsEv.notify_one();
	keyEventsHandlerThread.join();
	state.physicsEv.notify_one();
	physicsHandlerThread.join();

	Rain::Windows::validateSystemCall(
		Shell_NotifyIcon(NIM_DELETE, &state.notifyIconData));
	// For the LoadImage call earlier.
	Rain::Windows::validateSystemCall(DestroyIcon(state.notifyIconOpen));
	Rain::Windows::validateSystemCall(DestroyIcon(state.notifyIconClosed));
	Rain::Windows::validateSystemCall(UnhookWindowsHookEx(llKbHook));
	Rain::Windows::validateSystemCall(DestroyWindow(mainWnd));
	return 0;
}