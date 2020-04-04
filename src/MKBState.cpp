#include "MKBState.hpp"

namespace NumpadAsMouse {
	std::pair<double, double> exact_pos,	// position of mouse in real numbers,
																				// to be converted into integers later
		mouse_vel,			 // velocity of mouse, in pixels per second
		mouse_pos_diff;	 // the leftover non-integer portion of the mouse
										 // position, used in DiffMode
	std::bitset<256> vkkey_down;
	double wheel_pos,	 // real number; accumulated wheel units; whenever this
										 // exceeds 1, the timer should remove the whole number
										 // portion and send that as a message
		wheel_vel;			 // velocity of mouse wheel, in units per second
	bool paused;			 // toggled by pause_key
}
