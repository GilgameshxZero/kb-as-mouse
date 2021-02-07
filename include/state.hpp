#pragma once

#include <rain.hpp>

#include <bitset>

namespace KbAsMouse {
	class State {
		public:
		State();

		// Read configuration settings from a file and overwrite current settings.
		void overwriteSettingsFromFile(std::string file);

		// Returns true if this VK key is part of the settings and should be
		// intercepted.
		bool shouldInterceptKey(int vkCode);

		// Is SHIFT key pressed—override Windows default numpad settings.
		bool isShiftDown();

		// Settings.
		int upKey = 80, rightKey = 222, downKey = 186, leftKey = 76,
				leftClickKey = 165, rightClickKey = 13, middleClickKey = 161,
				scrollUpKey = 219, scrollDownKey = 79, scrollLeftSingleKey = 190,
				scrollRightSingleKey = 191,
				slowKey = 221,	// when held, scale acceleration down
			pauseKey = 93,
			terminateKey = -1,
			// frames per second; higher = smoother actions
			framesPerSecond = 90;
		// similar to air resistance
		double mouseResistance = 10,
					 // pixels per second per second
			mouseAcceleration = 150, scrollResistance = 10, scrollAcceleration = 300,
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
		bool paused = false, isTimerRunning = false;
		std::bitset<256> isVKKeyDown;

		private:
		void refreshInterceptKeys();

		std::set<int> keysToIntercept;
	};

	extern State state;
}
