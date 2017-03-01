#pragma once

#include <vector>

namespace KeyAsMouse
{
	//keep track of the state of relevant keys on the keyboard and mouse position
	namespace MKBState
	{
		//update state
		void UpdKeyDown (int vkcode);
		void UpdKeyUp (int vkcode);

		void UpdMousePos (double x, double y);

		//retreive state
		bool GetKey (int vkcode);

		//returns values between -1 and 1 (usually -1, 0, or 1)
		int GetHoroAxis ();
		int GetVertAxis ();
		int GetScrollAxis ();

		//test if key should be intercepted by hook and not passed down the chain
		bool ShouldIntercept (int vkcode);

		//keystate
		extern std::vector<bool> keydown;

		//mousestate
		extern double mousex, mousey;

		//keys from settings
		extern int left, down, right, up, lclick, rclick, mclick, wheeldown, wheelup;

		//function pointers to call when button clicked
		typedef void (*NotifyFunc)(void);
		extern NotifyFunc lbuttonup, lbuttondown, rbuttonup, rbuttondown, mbuttonup, mbuttondown;
	};
}