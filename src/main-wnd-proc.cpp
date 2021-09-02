#include <main-wnd-proc.hpp>
#include <state.hpp>

namespace KbAsMouse {
	LRESULT CALLBACK mainWndProc(
		_In_ HWND hWnd,
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam) {
		switch (uMsg) {
				// L/R clicking the tray icon will terminate the application.
			case WM_CONTEXTMENU:
				switch (lParam) {
					case WM_LBUTTONUP:
						state.togglePause();
						break;

					case WM_RBUTTONUP:
						PostQuitMessage(0);
						break;
				}
				break;

			default:
				break;
		}

		// No context menu is displayed, so always return DefWindowProc.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}