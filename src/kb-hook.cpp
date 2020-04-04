#include "kb-hook.hpp"

namespace NumpadAsMouse {
	LRESULT CALLBACK LLKBProc(_In_ int nCode,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam) {

		if (nCode < 0)
			return CallNextHookEx(NULL, nCode, wParam, lParam);

		KBDLLHOOKSTRUCT *hs = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
		switch (wParam) {
			case WM_KEYDOWN:
				if (KeyDown(static_cast<int>(hs->vkCode)))
					return -1;
				break;

			case WM_KEYUP:
				if (KeyUp(static_cast<int>(hs->vkCode)))
					return -1;
				break;

			default:
				break;
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	bool KeyDown(int vkCode) {
		// this stores the keydown status prior to alteration; some keys we do not
		// want to process multiple keydown messages when held (such as pause and
		// the clicks
		static bool keyWasDown;

		keyWasDown = state.isVKKeyDown[vkCode];
		state.isVKKeyDown[vkCode] = true;

		if (state.paused)
			return false;

		if (state.shouldInterceptKey(vkCode)) {
			if ((!keyWasDown &&
				(vkCode == state.leftClickKey || vkCode == state.rightClickKey || vkCode == state.middleClickKey)) ||
				vkCode == state.scrollLeftSingleKey || vkCode == state.scrollRightSingleKey) {
				INPUT inputEv;
				setupMouseInputEvent(inputEv);
				if (vkCode == state.leftClickKey)
					inputEv.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				else if (vkCode == state.rightClickKey)
					inputEv.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
				else if (vkCode == state.middleClickKey)
					inputEv.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
				else if (vkCode == state.scrollLeftSingleKey || vkCode == state.scrollRightSingleKey) {
					inputEv.mi.dwFlags = MOUSEEVENTF_HWHEEL;
					if (vkCode == state.scrollLeftSingleKey)
						inputEv.mi.mouseData = -WHEEL_DELTA;
					else
						inputEv.mi.mouseData = WHEEL_DELTA;
				}
				SendInput(1, &inputEv, sizeof(INPUT));
			} else if (vkCode != state.terminateKey && !state.isTimerRunning) {
				// if timer is stopped (velocity is null), restart it
				state.isTimerRunning = true;
				SetTimer(NULL, 0, 0, TimerProc);
			}

			return true;
		}

		return false;
	}
	bool KeyUp(int vkCode) {
		state.isVKKeyDown[vkCode] = false;

		if (vkCode == state.terminateKey) {
			PostQuitMessage(0);
			return true;
		} else if (vkCode == state.pauseKey) {
			state.paused = !state.paused;
			MessageBox(NULL, ((state.paused ? std::string("Paused") : std::string("Resumed")) + " numpad-as-mouse.").c_str(), "numpad-as-mouse", MB_OK);
			return true;
		}

		if (state.paused)
			return false;

		if (state.shouldInterceptKey(vkCode)) {
			if (vkCode == state.leftClickKey || vkCode == state.rightClickKey || vkCode == state.middleClickKey) {
				INPUT inputEv;
				setupMouseInputEvent(inputEv);
				if (vkCode == state.leftClickKey)
					inputEv.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				else if (vkCode == state.rightClickKey)
					inputEv.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
				else if (vkCode == state.middleClickKey)
					inputEv.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
				SendInput(1, &inputEv, sizeof(INPUT));
			}
			return true;
		}
		return false;
	}

	void setupMouseInputEvent(INPUT &inputEv) {
		inputEv.type = INPUT_MOUSE;
		inputEv.mi.dx = 0;
		inputEv.mi.dy = 0;
		inputEv.mi.mouseData = 0;
		inputEv.mi.time = 0;
	}
}
