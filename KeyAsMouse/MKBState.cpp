#include "MKBState.h"

namespace KeyAsMouse
{
	namespace MKBState
	{
		std::pair<double, double> exact_pos, //position of mouse in real numbers, to be converted into integers later
			mouse_vel; //velocity of mouse, in pixels per second
		std::bitset<256> vkkey_down;
		double wheel_pos, //real number; accumulated wheel units; whenever this exceeds 1, the timer should remove the whole number portion and send that as a message
			wheel_vel; //velocity of mouse wheel, in units per second
	}
}