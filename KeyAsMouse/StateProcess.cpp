#include "StateProcess.h"

namespace KeyAsMouse
{
	//process mouse and keyboard states//some messages surpass this stage, directly to mouseevent stage (if no processing is required)
	namespace StateProcess
	{
		double velx, vely, axmul, friction, minvel, maxvel, thresh;
	}
}

void KeyAsMouse::StateProcess::UpdateXY (double * x, double * y, int difftime)
{
	if (abs(velx) < thresh && abs (vely) < thresh && MKBState::GetHoroAxis () == 0 && MKBState::GetVertAxis () == 0)
		return;

	velx += axmul * difftime * MKBState::GetHoroAxis () * min (maxvel, max (minvel, abs (velx)));
	vely += axmul * difftime * MKBState::GetVertAxis () * min (maxvel, max (minvel, abs (vely)));
	(*x) += velx;
	(*y) += vely;
	velx -= friction * velx * difftime;
	vely -= friction * vely * difftime;

	if  (*x > MouseEvent::width)
		(*x) = MouseEvent::width,
		velx = 0;
	if (*x < 0)
		(*x) = 0,
		velx = 0;
	if (*y > MouseEvent::height)
		(*y) = MouseEvent::height,
		vely = 0;
	if (*y < 0)
		(*y) = 0,
		vely = 0;
}
