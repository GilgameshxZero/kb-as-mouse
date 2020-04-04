#pragma once
#include "../rain/cpp/rain.hpp"

#include <bitset>

namespace NumpadAsMouse {
	class State {
		public:
		State();

		// Read configuration settings from a file and overwrite current settings.
		void overwriteSettingsFromFile(std::string file);
		
		// Returns true if this VK key is part of the settings and should be intercepted.
		bool shouldInterceptKey(int vkCode);

		// Settings.
		int upKey = 104,
			rightKey = 102,
			downKey = 101,
			leftKey = 100,
			leftClickKey = 96,
			rightClickKey = 110,
			middleClickKey = 98,
			scrollUpKey = 105,
			scrollDownKey = 103,
			scrollLeftSingleKey = 97,
			scrollRightSingleKey = 99,
			// when held, scale acceleration down
			slowKey = 107,
			pauseKey = -1, // 19
			terminateKey = -1, // 93
			// frames per second; higher = smoother actions
			framesPerSecond = 90;
		// similar to air resistance
		double mouseResistance = 10,
			// pixels per second per second
			mouseAcceleration = 80,
			scrollResistance = 10,
			scrollAcceleration = 500,
			// slows acceleration by this ratio
			slowRatio = 0.2;

		// State.
		// Non-integer portion of mouse movement.
		std::pair<double, double> mousePositionRemainder = std::make_pair(0, 0),
			// velocity of mouse, in pixels per second
			mouseVelocity = std::make_pair(0, 0);
		// Non-integer portion of scroll movement.
		double scrollPositionRemainder = 0,
			// velocity of mouse wheel, in units per second
			scrollVelocity = 0;
		// If the program is paused.
		bool paused = false,
			isTimerRunning = false;
		std::bitset<256> isVKKeyDown;

		private:
		void refreshInterceptKeys();

		std::set<int> keysToIntercept;
	};
}
