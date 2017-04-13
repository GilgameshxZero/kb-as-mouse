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

			hs = reinterpret_cast<MSLLHOOKSTRUCT *>(lParam);
			POINT temp;
			GetCursorPos (&temp);

			switch (wParam) {
				case WM_MOUSEMOVE: {
					if (!(hs->dwExtraInfo == TimerManager::event_extra_info)) //process message if not sent from this program
						MouseMove (hs->pt);
					break;
				}

				default:
					break;
			}

			return CallNextHookEx (NULL, nCode, wParam, lParam);
		}

		bool KeyUp (int vkCode) {
			MKBState::vkkey_down[vkCode] = false;

			if (vkCode == Settings::terminate_key) {
				PostQuitMessage (0);
				return true;
			}

			if (MKBState::paused)
				return false;

			if (Settings::crit_keys.find (vkCode) != Settings::crit_keys.end ()) {
				if (vkCode == Settings::lclick ||
					vkCode == Settings::rclick ||
					vkCode == Settings::mclick) {
					static INPUT iev;
					iev = PrepMouseEvent ();
					if (vkCode == Settings::lclick)
						iev.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					else if (vkCode == Settings::rclick)
						iev.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
					else if (vkCode == Settings::mclick)
						iev.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
					SendInput (1, &iev, sizeof (INPUT));
				} else if (vkCode == Settings::pause_key ||
					vkCode == Settings::wheeldownsingle ||
					vkCode == Settings::wheelupsingle ||
					vkCode == Settings::wheelleft ||
					vkCode == Settings::wheelright ||
					vkCode == Settings::terminate_key)
					; //do nothing
				else
					; //do nothing

				return true;
			}
			return false;
		}
		bool KeyDown (int vkCode) {
			//this stores the keydown status prior to alteration; some keys we do not want to process multiple keydown messages when held (such as pause and the clicks
			static bool prev_key_down;

			prev_key_down = MKBState::vkkey_down[vkCode];
			MKBState::vkkey_down[vkCode] = true;

			if (!prev_key_down && vkCode == Settings::pause_key)
				MKBState::paused = !MKBState::paused;

			if (MKBState::paused)
				return false;

			if (Settings::crit_keys.find (vkCode) != Settings::crit_keys.end ()) {
				if ((!prev_key_down && 
						(vkCode == Settings::lclick ||
						vkCode == Settings::rclick || 
						vkCode == Settings::mclick)) || 
					vkCode == Settings::wheeldownsingle || 
					vkCode == Settings::wheelupsingle || 
					vkCode == Settings::wheelleft || 
					vkCode == Settings::wheelright) {
					static INPUT iev;
					iev = PrepMouseEvent ();
					if (vkCode == Settings::lclick)
						iev.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					else if (vkCode == Settings::rclick)
						iev.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
					else if (vkCode == Settings::mclick)
						iev.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
					else if (vkCode == Settings::wheeldownsingle ||
						vkCode == Settings::wheelupsingle ||
						vkCode == Settings::wheelleft ||
						vkCode == Settings::wheelright) {
						if (vkCode == Settings::wheeldownsingle || 
							vkCode == Settings::wheelupsingle)
							iev.mi.dwFlags = MOUSEEVENTF_WHEEL;
						else
							iev.mi.dwFlags = MOUSEEVENTF_HWHEEL;
						if (vkCode == Settings::wheeldownsingle)
							iev.mi.mouseData = -WHEEL_DELTA;
						else if (vkCode == Settings::wheelupsingle)
							iev.mi.mouseData = WHEEL_DELTA;
						else if (vkCode == Settings::wheelleft)
							iev.mi.mouseData = -WHEEL_DELTA;
						else if (vkCode == Settings::wheelright)
							iev.mi.mouseData = WHEEL_DELTA;
					}
					SendInput (1, &iev, sizeof (INPUT));
				} else if (vkCode == Settings::pause_key || 
					vkCode == Settings::terminate_key)
					; //do nothing
				else {
					//if timer is stopped (velocity is null), then restart it
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
			if (!MKBState::paused && !Settings::diff_mode)
				MKBState::exact_pos = std::make_pair (static_cast<double>(mouse_pos.x), static_cast<double>(mouse_pos.y));
		}

		INPUT PrepMouseEvent () {
			static INPUT iev;
			iev.type = INPUT_MOUSE;
			iev.mi.dx = 0;
			iev.mi.dy = 0;
			iev.mi.mouseData = 0;
			iev.mi.time = 0;
			return iev;
		}
	}
}