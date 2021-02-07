#include "kb-as-mouse.hpp"

int CALLBACK WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {
	// Limit so that we can only run one instance of this application.
	if (CreateMutex(NULL, TRUE, "kb-as-mouse") == NULL ||
		GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL,
			"Another instance of kb-as-mouse is already running.",
			"kb-as-mouse",
			MB_OK);
		return -1;
	}

	return KbAsMouse::start(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

namespace KbAsMouse {
	int start(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow) {
		// Create window for system tray icon events.
		static const std::string CLASS_NAME = "kb-as-mouse-wc";
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

		HWND mainWnd = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_TOPMOST |
				WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
			CLASS_NAME.c_str(),
			"",
			WS_POPUP,
			0,
			0,
			0,
			0,
			NULL,
			NULL,
			hInstance,
			NULL);

		// System tray icon.
		NOTIFYICONDATA nid;

		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.uID = 1;
		nid.uVersion = NOTIFYICON_VERSION_4;
		nid.uCallbackMessage = WM_CONTEXTMENU;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
		nid.hIcon = static_cast<HICON>(LoadImage(hInstance,
			MAKEINTRESOURCE(1),
			IMAGE_ICON,
			0,
			0,
			LR_DEFAULTSIZE));
		nid.hWnd = mainWnd;
		strcpy_s(nid.szTip, sizeof(nid.szTip), "kb-as-mouse | left-click: pause | right-click: terminate");
		if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
			return -1;
		}

		// read config file
		state.overwriteSettingsFromFile("kb-as-mouse.cfg");

		// install hooks and message loop
		HHOOK llkb_hook =
			SetWindowsHookEx(WH_KEYBOARD_LL, KbAsMouse::LLKBProc, NULL, NULL);

		BOOL bRet;
		MSG msg;
		while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
			if (bRet == -1)
				return -1;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		// clean up and exit
		DestroyIcon(nid.hIcon);
		UnhookWindowsHookEx(llkb_hook);
		DestroyWindow(mainWnd);
		return 0;
	}

	LRESULT CALLBACK mainWndProc(HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam) {
		switch (uMsg) {
			case WM_CONTEXTMENU:
				// Tray icon.
				switch (lParam) {
					case WM_LBUTTONUP:
						state.paused = !state.paused;
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
