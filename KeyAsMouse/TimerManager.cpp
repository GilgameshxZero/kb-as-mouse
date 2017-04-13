#include "TimerManager.h"

namespace KeyAsMouse {
	namespace TimerManager {
		int mspf; //milliseconds per frame
		DWORD last_frame = 0;
		std::pair<LONG, LONG> last_mouse_pos = std::make_pair (0, 0); //position of the mouse, in dx and dy, at the time TimerProc last sent a mouse position update message; only used in non-DiffMode
		bool timer_running = false;
		ULONG_PTR event_extra_info;

		VOID CALLBACK TimerProc (HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD time) {
			static INPUT iev;

			//incase off timing issues when the Timer is killed
			if (!timer_running)
				return VOID ();

			iev.mi.dwFlags = NULL;
			iev.mi.mouseData = 0;
			iev.mi.time = 0;
			iev.mi.dx = 0;
			iev.mi.dy = 0;

			//scale acceleration calculations by the time between the last frame and the current frame; if the timer was just started, assume that the last frame offured according to fps specs
			static double tdiff_s; //time difference in seconds between now and the last time this timer was run
			if (last_frame == 0) { //beginning timer; we need to replace elapse time
				iev.type = INPUT_MOUSE;
				iev.mi.dwExtraInfo = TimerManager::event_extra_info = static_cast<ULONG_PTR>(idEvent);
				last_frame = time - mspf;
				SetTimer (hwnd, idEvent, mspf, TimerProc);
			}
			tdiff_s = static_cast<double>(time - last_frame) / 1000.0;

			//slow acceleartion for mouse and wheel if slow_mode key is held
			static double acc_scale;
			if (MKBState::vkkey_down[Settings::slow_mode])
				acc_scale = Settings::slow_ratio;
			else
				acc_scale = 1;

			//calculate mouse acceleration, velocity, and position on both axis; limit acceleration such that at max, it can only cancel out the current velocity
			static double acc_x, acc_y;

			acc_x = Settings::resist * std::pow (std::abs (MKBState::mouse_vel.first), Settings::resist_power);
			acc_y = Settings::resist * std::pow (std::abs (MKBState::mouse_vel.second), Settings::resist_power);
			
			//reverse acceleration based on velocity should not exceed -velocity
			if (MKBState::mouse_vel.first > 0)
				acc_x = max (-MKBState::mouse_vel.first / tdiff_s, -acc_x);
			else
				acc_x = min (-MKBState::mouse_vel.first / tdiff_s, acc_x);
			if (MKBState::mouse_vel.second > 0)
				acc_y = max (-MKBState::mouse_vel.second / tdiff_s, -acc_y);
			else
				acc_y = min (-MKBState::mouse_vel.second / tdiff_s, acc_y);

			if (MKBState::vkkey_down[Settings::left])
				acc_x -= Settings::mouse_acc * acc_scale;
			if (MKBState::vkkey_down[Settings::right])
				acc_x += Settings::mouse_acc * acc_scale;
			if (MKBState::vkkey_down[Settings::up])
				acc_y -= Settings::mouse_acc * acc_scale;
			if (MKBState::vkkey_down[Settings::down])
				acc_y += Settings::mouse_acc * acc_scale;

			acc_x *= tdiff_s;
			acc_y *= tdiff_s;
			MKBState::mouse_vel.first += acc_x;
			MKBState::mouse_vel.second += acc_y;

			if (Settings::diff_mode) {
				MKBState::mouse_pos_diff.first += MKBState::mouse_vel.first;
				MKBState::mouse_pos_diff.second += MKBState::mouse_vel.second;

				//send an update if mouse_pos_diff has an integer portion
				if (static_cast<int>(MKBState::mouse_pos_diff.first) != 0 || 
					static_cast<int>(MKBState::mouse_pos_diff.second) != 0) {
					iev.mi.dwFlags |= MOUSEEVENTF_MOVE;
					iev.mi.dx = static_cast<LONG>(MKBState::mouse_pos_diff.first);
					iev.mi.dy = static_cast<LONG>(MKBState::mouse_pos_diff.second);
					MKBState::mouse_pos_diff.first -= static_cast<int>(MKBState::mouse_pos_diff.first);
					MKBState::mouse_pos_diff.second -= static_cast<int>(MKBState::mouse_pos_diff.second);
				}
			} else {
				MKBState::exact_pos.first += MKBState::mouse_vel.first;
				MKBState::exact_pos.second += MKBState::mouse_vel.second;

				if (MKBState::exact_pos.first < 0) {
					MKBState::mouse_vel.first = max (0, MKBState::mouse_vel.first);
					MKBState::exact_pos.first = 0;
				}
				if (MKBState::exact_pos.first > Start::screen_width) {
					MKBState::mouse_vel.first = min (0, MKBState::mouse_vel.first);
					MKBState::exact_pos.first = Start::screen_width;
				}
				if (MKBState::exact_pos.second < 0) {
					MKBState::mouse_vel.second = max (0, MKBState::mouse_vel.second);
					MKBState::exact_pos.second = 0;
				}
				if (MKBState::exact_pos.second > Start::screen_height) {
					MKBState::mouse_vel.second = min (0, MKBState::mouse_vel.second);
					MKBState::exact_pos.second = Start::screen_height;
				}

				//update exact_pos and send message if the rounded value of exact_pos is different from the previous rounded value
				static LONG mouse_x, mouse_y;
				mouse_x = static_cast<LONG>(std::round (MKBState::exact_pos.first * 65536 / Start::screen_width));
				mouse_y = static_cast<LONG>(std::round (MKBState::exact_pos.second * 65536 / Start::screen_height));
				if (mouse_x != last_mouse_pos.first || mouse_y != last_mouse_pos.second) {
					iev.mi.dwFlags |= MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
					iev.mi.dx = mouse_x;
					iev.mi.dy = mouse_y;
					last_mouse_pos = std::make_pair (mouse_x, mouse_y);
				}
			}

			//do a similar process for scroll wheel; like in DiffMode, send an update whenever the wheel position is >1 or <-1 (i.e., send an update with an integer if the wheel position has an integer portion)
			static double wheel_acc;
			wheel_acc = Settings::scroll_resist * std::pow (std::abs (MKBState::wheel_vel), Settings::scroll_resist_power);
			if (MKBState::wheel_vel > 0)
				wheel_acc = max (-MKBState::wheel_vel / tdiff_s, -wheel_acc);
			else
				wheel_acc = min (-MKBState::wheel_vel / tdiff_s, wheel_acc);
			if (MKBState::vkkey_down[Settings::wheelup])
				wheel_acc -= Settings::scroll_acc * acc_scale;
			if (MKBState::vkkey_down[Settings::wheeldown])
				wheel_acc += Settings::scroll_acc * acc_scale;
			wheel_acc *= tdiff_s;
			MKBState::wheel_vel += wheel_acc;
			MKBState::wheel_pos += MKBState::wheel_vel;

			if (static_cast<int>(MKBState::wheel_pos) != 0) {
				iev.mi.mouseData = static_cast<DWORD>(MKBState::wheel_pos);
				iev.mi.dwFlags |= MOUSEEVENTF_WHEEL;
				MKBState::wheel_pos -= static_cast<int>(MKBState::wheel_pos);
			}

			//send input if we have something to send (either mousemove or mousewheel)
			if (iev.mi.dwFlags != NULL)
				SendInput (1, &iev, sizeof (INPUT));

			//if velocity is not below threshold or if one of the wheel keys is down, then continue the timer and try to maintain the fps; else, kill timer
			if (abs (MKBState::mouse_vel.first) < Settings::min_vel_thresh)
				MKBState::mouse_vel.first = 0;
			if (abs (MKBState::mouse_vel.second) < Settings::min_vel_thresh)
				MKBState::mouse_vel.second = 0;
			if (abs (MKBState::wheel_vel) < Settings::min_scroll_vel_thresh)
				MKBState::wheel_vel = 0;
			if (!(MKBState::mouse_vel.first == 0 && MKBState::mouse_vel.second == 0 && MKBState::wheel_vel == 0)) {
				last_frame = time;
			} else {//prepare variables for the next time timer is started
				last_frame = 0;
 				timer_running = false;
				MKBState::wheel_pos = 0;
				if (Settings::diff_mode)
					MKBState::mouse_pos_diff = std::make_pair (0, 0);

				KillTimer (hwnd, idEvent);
			}

			return VOID ();
		}
	}
}