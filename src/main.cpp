#include "main.hpp"

int CALLBACK WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) {
	// Limit so that we can only run one instance of this application.
	if (CreateMutex(NULL, TRUE, "numpad-as-mouse") == NULL ||
		GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, "Another instance of numpad-as-mouse is already running.", "numpad-as-mouse", MB_OK);
		return 1;
	}

	return NumpadAsMouse::start(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

namespace NumpadAsMouse {
	State state;

	int start(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow) {
		// catch memory leaks and errors
		HANDLE hMemoryLeaks = Rain::logMemoryLeaks("memory-leaks.txt");

		// Create window for system tray icon events.
		static const std::string CLASS_NAME = "numpad-as-mouse-wc";
		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = NULL;
		wndClass.lpfnWndProc = mainWndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = NULL;
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = NULL;
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = CLASS_NAME.c_str();
		wndClass.hIconSm = NULL;

		if (!RegisterClassEx(&wndClass)) {
			return -1;
		}

		HWND mainWnd = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, CLASS_NAME.c_str(), "", WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

		// System tray icon.
		NOTIFYICONDATA nid;

		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.uID = 1;
		nid.uFlags = NIF_ICON | NIF_MESSAGE;
		nid.hIcon = static_cast<HICON>(
			LoadImage(hInstance,
			MAKEINTRESOURCE(1),
			IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR));
		nid.hWnd = mainWnd;
		nid.uCallbackMessage = WM_APP;

		if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
			return -1;
		}

		DestroyIcon(nid.hIcon);

		// read config file
		state.overwriteSettingsFromFile(".cfg");

		// install hooks and message loop
		HHOOK llkb_hook = SetWindowsHookEx(
			WH_KEYBOARD_LL, NumpadAsMouse::LLKBProc, NULL, NULL);

		BOOL bRet;
		MSG msg;
		while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
			if (bRet == -1)
				return Rain::reportError(-1, "Message loop returned -1.");
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		MessageBox(NULL, "Terminated numpad-as-mouse.", "numpad-as-mouse", MB_OK);

		// clean up and exit
		UnhookWindowsHookEx(llkb_hook);

		if (hMemoryLeaks != NULL)
			CloseHandle(hMemoryLeaks);

		return 0;
	}

	LRESULT CALLBACK mainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			case WM_APP:
				// Tray message.
				switch (lParam) {
					case WM_LBUTTONUP:
						state.paused = !state.paused;
						MessageBox(NULL, ((state.paused ? std::string("Paused") : std::string("Resumed")) + " numpad-as-mouse.").c_str(), "numpad-as-mouse", MB_OK);
						break;
					case WM_RBUTTONUP:
						PostQuitMessage(0);
						break;
				}
				break;

			default:
				break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}
