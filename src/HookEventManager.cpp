#include "HookEventManager.hpp"

namespace NumpadAsMouse {
	LRESULT CALLBACK LLKBProc(_In_ int nCode,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam) {
		if (nCode < 0)
			return CallNextHookEx(NULL, nCode, wParam, lParam);

		static KBDLLHOOKSTRUCT *hs;
		hs = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

		switch (wParam) {
			case WM_KEYDOWN: {
				if (KeyDown(static_cast<int>(hs->vkCode)))
					return -1;
				break;
			}

			case WM_KEYUP: {
				if (KeyUp(static_cast<int>(hs->vkCode)))
					return -1;
				break;
			}

			default:
				break;
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	LRESULT CALLBACK LLMouseProc(_In_ int nCode,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam) {
		if (nCode < 0)
			return CallNextHookEx(NULL, nCode, wParam, lParam);

		static MSLLHOOKSTRUCT *hs;

		hs = reinterpret_cast<MSLLHOOKSTRUCT *>(lParam);
		POINT temp;
		GetCursorPos(&temp);

		switch (wParam) {
			case WM_MOUSEMOVE: {
				if (!(hs->dwExtraInfo == event_extra_info))	 // process message if not
																										 // sent from this program
					MouseMove(hs->pt);
				break;
			}

			default:
				break;
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	bool KeyUp(int vkCode) {
		vkkey_down[vkCode] = false;

		if (vkCode == terminate_key) {
			PostQuitMessage(0);
			return true;
		}

		if (paused)
			return false;

		if (crit_keys.find(vkCode) != crit_keys.end()) {
			if (vkCode == lclick || vkCode == rclick || vkCode == mclick) {
				static INPUT iev;
				iev = PrepMouseEvent();
				if (vkCode == lclick)
					iev.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				else if (vkCode == rclick)
					iev.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
				else if (vkCode == mclick)
					iev.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
				SendInput(1, &iev, sizeof(INPUT));
			} else if (vkCode == pause_key || vkCode == wheeldownsingle ||
								 vkCode == wheelupsingle || vkCode == wheelleft ||
								 vkCode == wheelright || vkCode == terminate_key)
				;	 // do nothing
			else
				;	 // do nothing

			return true;
		}
		return false;
	}
	bool KeyDown(int vkCode) {
		// this stores the keydown status prior to alteration; some keys we do not
		// want to process multiple keydown messages when held (such as pause and
		// the clicks
		static bool prev_key_down;

		prev_key_down = vkkey_down[vkCode];
		vkkey_down[vkCode] = true;

		if (!prev_key_down && vkCode == pause_key)
			paused = !paused;

		if (paused)
			return false;

		if (crit_keys.find(vkCode) != crit_keys.end()) {
			if ((!prev_key_down &&
						(vkCode == lclick || vkCode == rclick || vkCode == mclick)) ||
					vkCode == wheeldownsingle || vkCode == wheelupsingle ||
					vkCode == wheelleft || vkCode == wheelright) {
				static INPUT iev;
				iev = PrepMouseEvent();
				if (vkCode == lclick)
					iev.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				else if (vkCode == rclick)
					iev.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
				else if (vkCode == mclick)
					iev.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
				else if (vkCode == wheeldownsingle || vkCode == wheelupsingle ||
								 vkCode == wheelleft || vkCode == wheelright) {
					if (vkCode == wheeldownsingle || vkCode == wheelupsingle)
						iev.mi.dwFlags = MOUSEEVENTF_WHEEL;
					else
						iev.mi.dwFlags = MOUSEEVENTF_HWHEEL;
					if (vkCode == wheeldownsingle)
						iev.mi.mouseData = -WHEEL_DELTA;
					else if (vkCode == wheelupsingle)
						iev.mi.mouseData = WHEEL_DELTA;
					else if (vkCode == wheelleft)
						iev.mi.mouseData = -WHEEL_DELTA;
					else if (vkCode == wheelright)
						iev.mi.mouseData = WHEEL_DELTA;
				}
				SendInput(1, &iev, sizeof(INPUT));
			} else if (vkCode == pause_key || vkCode == terminate_key)
				;	 // do nothing
			else {
				// if timer is stopped (velocity is null), then restart it
				if (!timer_running) {
					timer_running = true;
					SetTimer(NULL, 0, 0, TimerProc);
				}
			}

			return true;
		}

		return false;
	}
	void MouseMove(POINT mouse_pos) {
		if (!paused && !diff_mode)
			exact_pos = std::make_pair(
				static_cast<double>(mouse_pos.x), static_cast<double>(mouse_pos.y));
	}

	INPUT PrepMouseEvent() {
		static INPUT iev;
		iev.type = INPUT_MOUSE;
		iev.mi.dx = 0;
		iev.mi.dy = 0;
		iev.mi.mouseData = 0;
		iev.mi.time = 0;
		return iev;
	}
}
