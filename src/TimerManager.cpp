#include "TimerManager.hpp"

namespace NumpadAsMouse {
	int mspf;	 // milliseconds per frame
	DWORD last_frame = 0;
	std::pair<LONG, LONG> last_mouse_pos = std::make_pair(0,
		0);	 // position of the mouse, in dx and dy, at the time TimerProc last
				 // sent a mouse position update message; only used in non-DiffMode
	bool timer_running = false;
	ULONG_PTR event_extra_info;

	VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD time) {
		static INPUT iev;

		// incase off timing issues when the Timer is killed
		if (!timer_running)
			return VOID();

		iev.mi.dwFlags = NULL;
		iev.mi.mouseData = 0;
		iev.mi.time = 0;
		iev.mi.dx = 0;
		iev.mi.dy = 0;

		// scale acceleration calculations by the time between the last frame and
		// the current frame; if the timer was just started, assume that the last
		// frame offured according to fps specs
		static double tdiff_s;	// time difference in seconds between now and the
														// last time this timer was run
		if (last_frame == 0) {	// beginning timer; we need to replace elapse time
			iev.type = INPUT_MOUSE;
			iev.mi.dwExtraInfo = event_extra_info = static_cast<ULONG_PTR>(idEvent);
			last_frame = time - mspf;
			SetTimer(hwnd, idEvent, mspf, TimerProc);
		}
		tdiff_s = static_cast<double>(time - last_frame) / 1000.0;

		// slow acceleartion for mouse and wheel if slow_mode key is held
		static double acc_scale;
		if (vkkey_down[slow_mode])
			acc_scale = slow_ratio;
		else
			acc_scale = 1;

		// calculate mouse acceleration, velocity, and position on both axis;
		// limit acceleration such that at max, it can only cancel out the current
		// velocity
		static double acc_x, acc_y;

		acc_x = resist * std::pow(std::abs(mouse_vel.first), resist_power);
		acc_y = resist * std::pow(std::abs(mouse_vel.second), resist_power);

		// reverse acceleration based on velocity should not exceed -velocity
		if (mouse_vel.first > 0)
			acc_x = max(-mouse_vel.first / tdiff_s, -acc_x);
		else
			acc_x = min(-mouse_vel.first / tdiff_s, acc_x);
		if (mouse_vel.second > 0)
			acc_y = max(-mouse_vel.second / tdiff_s, -acc_y);
		else
			acc_y = min(-mouse_vel.second / tdiff_s, acc_y);

		if (vkkey_down[left])
			acc_x -= mouse_acc * acc_scale;
		if (vkkey_down[right])
			acc_x += mouse_acc * acc_scale;
		if (vkkey_down[up])
			acc_y -= mouse_acc * acc_scale;
		if (vkkey_down[down])
			acc_y += mouse_acc * acc_scale;

		acc_x *= tdiff_s;
		acc_y *= tdiff_s;
		mouse_vel.first += acc_x;
		mouse_vel.second += acc_y;

		if (diff_mode) {
			mouse_pos_diff.first += mouse_vel.first;
			mouse_pos_diff.second += mouse_vel.second;

			// send an update if mouse_pos_diff has an integer portion
			if (static_cast<int>(mouse_pos_diff.first) != 0 ||
					static_cast<int>(mouse_pos_diff.second) != 0) {
				iev.mi.dwFlags |= MOUSEEVENTF_MOVE;
				iev.mi.dx = static_cast<LONG>(mouse_pos_diff.first);
				iev.mi.dy = static_cast<LONG>(mouse_pos_diff.second);
				mouse_pos_diff.first -= static_cast<int>(mouse_pos_diff.first);
				mouse_pos_diff.second -= static_cast<int>(mouse_pos_diff.second);
			}
		} else {
			exact_pos.first += mouse_vel.first;
			exact_pos.second += mouse_vel.second;

			if (exact_pos.first < 0) {
				mouse_vel.first = max(0, mouse_vel.first);
				exact_pos.first = 0;
			}
			if (exact_pos.first > screen_width) {
				mouse_vel.first = min(0, mouse_vel.first);
				exact_pos.first = screen_width;
			}
			if (exact_pos.second < 0) {
				mouse_vel.second = max(0, mouse_vel.second);
				exact_pos.second = 0;
			}
			if (exact_pos.second > screen_height) {
				mouse_vel.second = min(0, mouse_vel.second);
				exact_pos.second = screen_height;
			}

			// update exact_pos and send message if the rounded value of exact_pos
			// is different from the previous rounded value
			static LONG mouse_x, mouse_y;
			mouse_x =
				static_cast<LONG>(std::round(exact_pos.first * 65536 / screen_width));
			mouse_y =
				static_cast<LONG>(std::round(exact_pos.second * 65536 / screen_height));
			if (mouse_x != last_mouse_pos.first || mouse_y != last_mouse_pos.second) {
				iev.mi.dwFlags |= MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
				iev.mi.dx = mouse_x;
				iev.mi.dy = mouse_y;
				last_mouse_pos = std::make_pair(mouse_x, mouse_y);
			}
		}

		// do a similar process for scroll wheel; like in DiffMode, send an update
		// whenever the wheel position is >1 or <-1 (i.e., send an update with an
		// integer if the wheel position has an integer portion)
		static double wheel_acc;
		wheel_acc =
			scroll_resist * std::pow(std::abs(wheel_vel), scroll_resist_power);
		if (wheel_vel > 0)
			wheel_acc = max(-wheel_vel / tdiff_s, -wheel_acc);
		else
			wheel_acc = min(-wheel_vel / tdiff_s, wheel_acc);
		if (vkkey_down[wheelup])
			wheel_acc -= scroll_acc * acc_scale;
		if (vkkey_down[wheeldown])
			wheel_acc += scroll_acc * acc_scale;
		wheel_acc *= tdiff_s;
		wheel_vel += wheel_acc;
		wheel_pos += wheel_vel;

		if (static_cast<int>(wheel_pos) != 0) {
			iev.mi.mouseData = static_cast<DWORD>(wheel_pos);
			iev.mi.dwFlags |= MOUSEEVENTF_WHEEL;
			wheel_pos -= static_cast<int>(wheel_pos);
		}

		// send input if we have something to send (either mousemove or
		// mousewheel)
		if (iev.mi.dwFlags != NULL)
			SendInput(1, &iev, sizeof(INPUT));

		// if velocity is not below threshold or if one of the wheel keys is down,
		// then continue the timer and try to maintain the fps; else, kill timer
		if (abs(mouse_vel.first) < min_vel_thresh)
			mouse_vel.first = 0;
		if (abs(mouse_vel.second) < min_vel_thresh)
			mouse_vel.second = 0;
		if (abs(wheel_vel) < min_scroll_vel_thresh)
			wheel_vel = 0;
		if (!(mouse_vel.first == 0 && mouse_vel.second == 0 && wheel_vel == 0)) {
			last_frame = time;
		} else {	// prepare variables for the next time timer is started
			last_frame = 0;
			timer_running = false;
			wheel_pos = 0;
			if (diff_mode)
				mouse_pos_diff = std::make_pair(0, 0);

			KillTimer(hwnd, idEvent);
		}

		return VOID();
	}
}
