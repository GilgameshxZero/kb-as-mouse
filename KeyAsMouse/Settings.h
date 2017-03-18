#pragma once
#include <set>

namespace KeyAsMouse {
	namespace Settings {
		extern double resist, //"air resistance", s.t. the force against user acceleration is any direction is resist * velocity ^ power, similar to actual air resistance (except without the v^2, thus creation a sensation similar to jerk and removing terminal velocity); units are seconds^(-1); mouse is by definition 1 mass (unitless)
			mouse_acc, //pixels per second per second; acceleration caused by user input
			min_vel_thresh, //minimum velocity; below this, velocity will be counted as 0 and the timer will not run
			resist_power, //the power to which v is raised in the calculation of resistance force
			scroll_resist, //similar concept, to the scrollwheel, except this time we use v^2
			scroll_acc,
			min_scroll_vel_thresh,
			scroll_resist_power,
			slow_ratio; //the amount by which acceleartion is multiplied under effects of the slow key
		extern int left, down, right, up, lclick, rclick, mclick, wheeldown, wheelup, wheeldownsingle, wheelupsingle,wheelleft, wheelright, 
			fps, //frames per second; the desired number of times that calculations should be applied per second; higher fps result in smoother actions
			slow_mode, //when held, scale user inputted acceleration down to 50% effectivity
			pause_key, //toggles program activity
			terminate_key; //when pressed, this should end the program
		extern bool diff_mode; //if true, the exact position of the mouse is not stored; instead, updates to the mouse are sent every time the mouse has traveled > 1 pixel in any direction; this is similar to how scroll is processed; the mouse will retain momentum even when hitting edges of screen
		extern std::set<int> crit_keys; //set containing the left, down, etc. keys
	}
}