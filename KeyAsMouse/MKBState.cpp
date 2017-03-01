#include "MKBState.h"

namespace KeyAsMouse
{
	namespace MKBState
	{
		std::vector<bool> keydown;
		double mousex, mousey;
		int left, down, right, up, lclick, rclick, mclick, wheeldown, wheelup;

		typedef void (*NotifyFunc)(void);
		NotifyFunc lbuttonup, lbuttondown, rbuttonup, rbuttondown, mbuttonup, mbuttondown;
	}
}

void KeyAsMouse::MKBState::UpdKeyDown (int vkcode)
{
	if (vkcode == lclick && !keydown[vkcode])
		lbuttondown ();
	if (vkcode == rclick && !keydown[vkcode])
		rbuttondown ();
	if (vkcode == mclick && !keydown[vkcode])
		mbuttondown ();

	keydown[vkcode] = true;
}

void KeyAsMouse::MKBState::UpdKeyUp (int vkcode)
{
	if (vkcode == lclick && keydown[vkcode])
		lbuttonup ();
	if (vkcode == rclick && keydown[vkcode])
		rbuttonup ();
	if (vkcode == mclick && keydown[vkcode])
		mbuttonup ();

	keydown[vkcode] = false;
}

void KeyAsMouse::MKBState::UpdMousePos (double x, double y)
{
	mousex = x;
	mousey = y;
}

bool KeyAsMouse::MKBState::GetKey (int vkcode)
{
	return keydown[vkcode];
}

int KeyAsMouse::MKBState::GetHoroAxis ()
{
	return -static_cast<int>(keydown[left]) + static_cast<int>(keydown[right]);
}

int KeyAsMouse::MKBState::GetVertAxis ()
{
	return -static_cast<int>(keydown[up]) + static_cast<int>(keydown[down]);
}

int KeyAsMouse::MKBState::GetScrollAxis ()
{
	return -static_cast<int>(keydown[wheelup]) + static_cast<int>(keydown[wheeldown]);
}

bool KeyAsMouse::MKBState::ShouldIntercept (int vkcode)
{
	return (vkcode == left) || (vkcode == down) || (vkcode == right) || (vkcode == up) ||
		(vkcode == lclick) || (vkcode == rclick) || (vkcode == mclick) ||
		(vkcode == wheeldown) || (vkcode == wheelup);
}
