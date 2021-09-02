#include <ll-kb-hook-proc.hpp>
#include <state.hpp>

#include <rain.hpp>

namespace KbAsMouse {
	// This procedure must not block for any significant amount of time, or else
	// the hook may be improperly unhooked and necessitate a system restart to
	// kill the zombie process.
	LRESULT llKbHookProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
		if (nCode < 0) {
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}

		// Determine whether or not we should intercept the key; if so, return -1.
		// Otherwise, CallNextHookEx.

		// This static set is initialized by the initial VK codes and shall not be
		// changed throughout application runtime.
		static std::unordered_set<unsigned long> const vkIntercepts{
			state.vkUp,
			state.vkRight,
			state.vkDown,
			state.vkLeft,
			state.vkMouseLeft,
			state.vkMouseRight,
			state.vkMouseMiddle,
			state.vkScrollUp,
			state.vkScrollDown,
			state.vkScrollLeft,
			state.vkScrollRight,
			state.vkSlow,
			state.vkPause};

		DWORD vkCode = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam)->vkCode;
		switch (wParam) {
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYUP:
				if (
					(state.paused && vkCode == state.vkPause) ||
					(!state.paused && vkIntercepts.find(vkCode) != vkIntercepts.end())) {
					// Intercept the key and queue it.
					std::unique_lock lck(state.keyEventsMtx);
					state.keyEvents.push(
						{wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN, vkCode});
					state.keyEventsEv.notify_one();
					return -1;
				}
				break;

			default:
				break;
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	// Separate thread is responsible for handling all key events.
	void keyEventsHandler() {
		while (true) {
			std::unique_lock lck(state.keyEventsMtx);
			state.keyEventsEv.wait(lck);
			if (state.terminating) {
				break;
			}

			// Consume all key events possible, but don't hog the mutex.
			while (!state.keyEvents.empty()) {
				auto keyEvent = state.keyEvents.front();
				state.keyEvents.pop();
				lck.unlock();

				// Process the key event.
				if (keyEvent.isDown) {
					// Pause key is ignored when pressed down and processed on release.

					if (
						keyEvent.vkCode == state.vkUp || keyEvent.vkCode == state.vkRight ||
						keyEvent.vkCode == state.vkDown ||
						keyEvent.vkCode == state.vkLeft ||

						// Regardless of slow key state, track scroll up/down pressed state
						// in case slow key is pressed/unpressed during a scroll up/down
						// press sequence.
						keyEvent.vkCode == state.vkScrollUp ||
						keyEvent.vkCode == state.vkScrollDown ||

						keyEvent.vkCode == state.vkSlow) {
						// Mark the key as pressed for continuous movement keys.
						state.pressed[keyEvent.vkCode] = true;

						// Unsleep the physics engine only if necessary; extra calls to
						// notify_one may cause performance issues.
						if (state.physicsSleeping) {
							state.physicsEv.notify_one();
						}
					}

					// Click keys are processed only the first time they are pressed down,
					// except for the scroll single keys.
					INPUT mouseInput{INPUT_MOUSE, {}};
					// Track pressed state of the mouse buttons so that we dont click
					// multiple times while holding.
					if (
						!state.pressed[keyEvent.vkCode] &&
						keyEvent.vkCode == state.vkMouseLeft) {
						mouseInput.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
						state.pressed[keyEvent.vkCode] = true;
					} else if (
						!state.pressed[keyEvent.vkCode] &&
						keyEvent.vkCode == state.vkMouseRight) {
						mouseInput.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
						state.pressed[keyEvent.vkCode] = true;
					} else if (
						!state.pressed[keyEvent.vkCode] &&
						keyEvent.vkCode == state.vkMouseMiddle) {
						mouseInput.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
						state.pressed[keyEvent.vkCode] = true;
					} else if (
						state.pressed[state.vkSlow] &&
						keyEvent.vkCode == state.vkScrollUp) {
						mouseInput.mi.dwFlags = MOUSEEVENTF_WHEEL;
						mouseInput.mi.mouseData = -WHEEL_DELTA;
					} else if (
						state.pressed[state.vkSlow] &&
						keyEvent.vkCode == state.vkScrollDown) {
						mouseInput.mi.dwFlags = MOUSEEVENTF_WHEEL;
						mouseInput.mi.mouseData = WHEEL_DELTA;
					} else if (keyEvent.vkCode == state.vkScrollLeft) {
						mouseInput.mi.dwFlags = MOUSEEVENTF_HWHEEL;
						mouseInput.mi.mouseData = -WHEEL_DELTA;
					} else if (keyEvent.vkCode == state.vkScrollRight) {
						mouseInput.mi.dwFlags = MOUSEEVENTF_HWHEEL;
						mouseInput.mi.mouseData = WHEEL_DELTA;
					}

					if (mouseInput.mi.dwFlags != 0) {
						SendInput(1, &mouseInput, sizeof(INPUT));
					}

				} else {
					// On pause, immediately sets state.pause, which halts the physics
					// engine and is only triggered by the hook on unpause. Additionally,
					// pausing resets the pressed bitset.
					if (keyEvent.vkCode == state.vkPause) {
						state.togglePause();
					}

					if (
						keyEvent.vkCode == state.vkUp || keyEvent.vkCode == state.vkRight ||
						keyEvent.vkCode == state.vkDown ||
						keyEvent.vkCode == state.vkLeft ||
						keyEvent.vkCode == state.vkScrollUp ||
						keyEvent.vkCode == state.vkScrollDown ||
						keyEvent.vkCode == state.vkSlow) {
						// Mark the key as unpressed for continuous movement keys.
						state.pressed[keyEvent.vkCode] = false;

						if (state.physicsSleeping) {
							state.physicsEv.notify_one();
						}
					}

					INPUT mouseInput{INPUT_MOUSE, {}};
					if (keyEvent.vkCode == state.vkMouseLeft) {
						mouseInput.mi.dwFlags = MOUSEEVENTF_LEFTUP;
						state.pressed[keyEvent.vkCode] = false;
					} else if (keyEvent.vkCode == state.vkMouseRight) {
						mouseInput.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
						state.pressed[keyEvent.vkCode] = false;
					} else if (keyEvent.vkCode == state.vkMouseMiddle) {
						mouseInput.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
						state.pressed[keyEvent.vkCode] = false;
					}

					if (mouseInput.mi.dwFlags != 0) {
						SendInput(1, &mouseInput, sizeof(INPUT));
					}
				}

				lck.lock();
			}
			lck.unlock();
		}
	}

	void physicsHandler() {
		using namespace Rain::Literal;

		// Duration to wait for internal FPS timer, and beginning of last timestep.
		auto const internalWait =
			std::chrono::duration_cast<std::chrono::steady_clock::duration>(
				1000000000ns / state.fps);
		auto prevTimestep = std::chrono::steady_clock::time_point::min();

		// Waiting on a condition variable causes some lag while waking up. The
		// previous wake-up lag is stored so that we can account for this before
		// entering wait. We assume that this doesn't change much over nearby
		// timesteps.
		std::chrono::steady_clock::duration waitLag = 0ns;

		// Used internally for the condition variable, since others don't need
		// access.
		std::mutex physicsEvMtx;

		// Tracking for physics simulations.
		std::pair<double, double> mouseVel{0, 0};
		double scrollVel = 0;

		// Track mouse position and its fractional remainder after sending mouse
		// events.
		std::pair<double, double> mousePosRem{0, 0};
		double scrollPosRem = 0;

		// Positions are scaled by this based on DPI before being passed as INPUT.
		double dpiMod;

		// Physics engine sleeps if all velocities are under this.
		double const minVel = 1;

		// These local variables inside the timestep scope are moved out to avoid
		// redefinition each time.
		std::unique_lock lck(physicsEvMtx);
		std::chrono::steady_clock::duration timestepDelta;
		std::chrono::steady_clock::time_point timeNow;
		INPUT mouseInput{INPUT_MOUSE, {}};
		mouseInput.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_WHEEL;
		double timestepMod, slowMod;

		while (true) {
			// If sleeping, wait indefinitly. Otherwise, wait until next assigned
			// frame by FPS.
			if (state.physicsSleeping) {
				state.physicsEv.wait(lck);

				// If previously sleeping, adjust the timestepDelta to default.
				timestepDelta = internalWait;
				prevTimestep = std::chrono::steady_clock::now();

				// Re-retrieve the system DPI when waking from sleep, so that latest
				// changes are taken into account.
				dpiMod =
					static_cast<double>(GetDpiForSystem()) / USER_DEFAULT_SCREEN_DPI;
			} else {
				state.physicsEv.wait_until(lck, prevTimestep + internalWait - waitLag);
				timeNow = std::chrono::steady_clock::now();
				timestepDelta = timeNow - prevTimestep;
				waitLag = timestepDelta - internalWait + waitLag;
				prevTimestep = timeNow;
			}
			if (state.terminating) {
				break;
			}

			// Run a single timestep of physics simulations.
			mouseInput.mi.dx = mouseInput.mi.dy = 0;
			mouseInput.mi.mouseData = 0;

			// Compute accelerations for mouse movement and v-scroll and update
			// velocity and positions.
			timestepMod =
				std::chrono::duration_cast<std::chrono::nanoseconds>(timestepDelta)
					.count() /
				1e9;
			slowMod = state.pressed[state.vkSlow] ? state.slowMod : 1;

			// Acceleration component from resistance cannot be more than the original
			// velocity in any given timestep.
			mouseVel.first +=
				(-mouseVel.first * min(state.mouseDrag / slowMod, 1 / timestepMod) -
				 state.pressed[state.vkLeft] * state.mouseAcc * slowMod +
				 state.pressed[state.vkRight] * state.mouseAcc * slowMod) *
				timestepMod;
			mouseVel.second +=
				(-mouseVel.second * min(state.mouseDrag / slowMod, 1 / timestepMod) -
				 state.pressed[state.vkUp] * state.mouseAcc * slowMod +
				 state.pressed[state.vkDown] * state.mouseAcc * slowMod) *
				timestepMod;

			// Scroll continuous movement is only processed if slow key is not held.
			scrollVel +=
				(-scrollVel * min(state.scrollDrag / slowMod, 1 / timestepMod) -
				 (state.pressed[state.vkScrollUp] && !state.pressed[state.vkSlow]) *
					 state.scrollAcc * WHEEL_DELTA * slowMod +
				 (state.pressed[state.vkScrollDown] && !state.pressed[state.vkSlow]) *
					 state.scrollAcc * WHEEL_DELTA * slowMod) *
				timestepMod;
			mousePosRem.first += mouseVel.first * timestepMod * dpiMod;
			mousePosRem.second += mouseVel.second * timestepMod * dpiMod;
			scrollPosRem += scrollVel * timestepMod;

			// Send and remove integral position portions as events.
			mouseInput.mi.dx = static_cast<LONG>(mousePosRem.first);
			mouseInput.mi.dy = static_cast<LONG>(mousePosRem.second);
			mouseInput.mi.mouseData = static_cast<DWORD>(scrollPosRem);
			mousePosRem.first -= mouseInput.mi.dx;
			mousePosRem.second -= mouseInput.mi.dy;
			scrollPosRem -= mouseInput.mi.mouseData;
			if (
				mouseInput.mi.dx != 0 || mouseInput.mi.dy != 0 ||
				mouseInput.mi.mouseData != 0) {
				SendInput(1, &mouseInput, sizeof(INPUT));
			}

			// Determine whether or not to sleep; i.e. whether velocities are low
			// enough to not warrant further timesteps for now. If velocities are too
			// low, nullify them as well.
			state.physicsSleeping = abs(mouseVel.first) < minVel &&
				abs(mouseVel.second) < minVel && abs(scrollVel) < minVel;
		}
	}
}