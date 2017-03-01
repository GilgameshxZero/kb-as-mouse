#pragma once

#include "MKBState.h"
#include "MouseEvent.h"

namespace KeyAsMouse
{
	//process mouse and keyboard states//some messages surpass this stage, directly to mouseevent stage (if no processing is required)
	namespace StateProcess
	{
		//update mouse pos (called rapidly)//use double for precision physics movement
		void UpdateXY (double *x, double *y, int difftime);

		//velocity of mouse
		extern double velx, vely;

		//modifier between axis input and acceleration//friction//minimum start acc
		extern double axmul, friction, minvel, maxvel, thresh;
	};
}