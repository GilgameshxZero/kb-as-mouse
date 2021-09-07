#pragma once

#include <rain.hpp>

#include <bitset>

namespace KbAsMouse {
	// Shared state and settings for the application.
	class State {
		public:
		unsigned long vkUp = 'P',	 // P
			vkRight = VK_OEM_7,	 // ' or "
			vkDown = VK_OEM_1,	// ; or :
			vkLeft = 'L',	 // L
			vkMouseLeft = VK_RMENU,	 // Right ALT
			vkMouseRight = VK_OEM_COMMA,	// ,
			vkMouseMiddle = VK_OEM_PERIOD,	// .
			vkScrollUp = VK_OEM_4,	// [ or {
			vkScrollDown = 'O',	 // O

			// While scroll left/right seem similar to scroll up/down, these are
			// single keys by default and don't have continuous (held) actions.
			vkScrollLeft = VK_OEM_6,	// ] or }
			vkScrollRight = VK_OEM_5,	 // \ or |

			vkSlow = VK_RETURN,	 // ENTER
			vkPause = VK_APPS;	// CONTEXT MENU
		double fps;

		// Set to true iff the virtual key is pressed. Only valid for the continuous
		// keys processed by the physics engine: up, right, down, left, scroll up,
		// scroll down, slow.
		std::bitset<256> pressed;

		double mouseDrag = 12, mouseAcc = 7000, scrollDrag = 12, scrollAcc = 200,
					 slowMod = 0.45;

		// Notification icon-related data.
		HICON notifyIconOpen, notifyIconClosed;
		NOTIFYICONDATA notifyIconData;

		// Intercepted keys are stored in a queue to be processed periodically,
		// protected by a mutex.
		class KeyEvent {
			public:
			bool isDown;
			unsigned long vkCode;
		};
		std::queue<KeyEvent> keyEvents;
		std::mutex keyEventsMtx;
		std::condition_variable keyEventsEv;

		// The physics handler does not access shared memory outside of keys, and
		// thus does not use a mutex for the condition variable. The condition
		// variable is used only for waking the physics handler from sleep.
		std::condition_variable physicsEv;
		std::atomic_bool physicsSleeping = true;

		// paused only affects the intercepting of keys; terminating affects both
		// the key handler and the physics handler.
		std::atomic_bool terminating = false, paused;

		void togglePause();

		// Based on pause state, sets the notify icon tip and icon.
		void updateNotifyIconData();
	};

	// State must be stored globally to allow access from low level hook proc.
	extern State state;
}
