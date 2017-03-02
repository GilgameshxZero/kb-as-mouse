#include "HookEventManager.h"

namespace KeyAsMouse {
	namespace HookEventManager {
		int screen_width, screen_height;

		LRESULT CALLBACK LLKBProc (_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
			if (nCode < 0)
				return CallNextHookEx (NULL, nCode, wParam, lParam);

			static KBDLLHOOKSTRUCT *hs;
			hs = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

			switch (wParam) {
				case WM_KEYDOWN: {
					if (KeyDown (static_cast<int>(hs->vkCode)))
						return -1;
					break;
				}

				case WM_KEYUP: {
					if (KeyUp (static_cast<int>(hs->vkCode)))
						return -1;
					break;
				}

				default:
					break;
			}

			return CallNextHookEx (NULL, nCode, wParam, lParam);
		}

		LRESULT CALLBACK LLMouseProc (_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
			if (nCode < 0)
				return CallNextHookEx (NULL, nCode, wParam, lParam);

			static MSLLHOOKSTRUCT *hs;
			static POINT mouse_pos;

			hs = reinterpret_cast<MSLLHOOKSTRUCT *>(lParam);
			GetCursorPos (&mouse_pos);

			switch (wParam) {
				case WM_MOUSEMOVE: {
					if (!(hs->flags & LLMHF_INJECTED))
						MouseMove (mouse_pos);
					break;
				}

				default:
					break;
			}

			return CallNextHookEx (NULL, nCode, wParam, lParam);
		}

		bool KeyUp (int vkCode) {
			if (Settings::crit_keys.find (vkCode) != Settings::crit_keys.end ()) {
				if (vkCode == Settings::lclick || vkCode == Settings::rclick || vkCode == Settings::mclick) {
					static INPUT iev;
					iev = PrepMouseEvent ();
					if (vkCode == Settings::lclick)
						iev.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					else if (vkCode == Settings::rclick)
						iev.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
					else if (vkCode == Settings::mclick)
						iev.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
					SendInput (1, &iev, sizeof (INPUT));
				} else
					MKBState::vkkey_down[vkCode] = false;

				return true;
			}
			return false;
		}
		bool KeyDown (int vkCode) {
			if (vkCode == Settings::terminate_key) {
				PostQuitMessage (0);
				return true;
			}

			if (Settings::crit_keys.find (vkCode) != Settings::crit_keys.end ()) {
				if (vkCode == Settings::lclick || vkCode == Settings::rclick || vkCode == Settings::mclick) {
					static INPUT iev;
					iev = PrepMouseEvent ();
					if (vkCode == Settings::lclick)
						iev.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					else if (vkCode == Settings::rclick)
						iev.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
					else if (vkCode == Settings::mclick)
						iev.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
					SendInput (1, &iev, sizeof (INPUT));
				} else {
					MKBState::vkkey_down[vkCode] = true;

					//if timer has stopped (velocity is null), then restart it
					if (!TimerManager::timer_running) {
						TimerManager::timer_running = true;
						SetTimer (NULL, 0, 0, TimerManager::TimerProc);
					}
				}

				return true;
			}
			return false;
		}
		void MouseMove (POINT mouse_pos) {
			MKBState::exact_pos = std::make_pair (static_cast<double>(mouse_pos.x), static_cast<double>(mouse_pos.y));
		}

		INPUT PrepMouseEvent () {
			static INPUT iev;
			iev.type = INPUT_MOUSE;
			iev.mi.dx = static_cast<LONG>(MKBState::exact_pos.first * 65535 / Start::screen_width);
			iev.mi.dy = static_cast<LONG>(MKBState::exact_pos.second * 65535 / Start::screen_height);
			iev.mi.mouseData = 0;
			return iev;
		}
	}
}