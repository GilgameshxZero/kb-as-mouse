#include "timer-proc.hpp"

namespace NumpadAsMouse {
	VOID CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD time) {
		static const double VELOCITY_ZERO_EPS = 1e-5;
		static int MS_PER_FRAME = 1000 / state.framesPerSecond;

		static DWORD timeOfPrevFrame = 0;

		// incase off timing issues when the Timer is killed
		if (!state.isTimerRunning)
			return VOID();

		// Static is important since we access the memory outside of this function from SendInput.
		static INPUT inputEv;
		inputEv.mi.dwFlags = NULL;
		inputEv.mi.mouseData = 0;
		inputEv.mi.time = 0;
		inputEv.mi.dx = 0;
		inputEv.mi.dy = 0;

		// scale acceleration calculations by the time between the last frame and
		// the current frame; if the timer was just started, assume that the last
		// frame offured according to framesPerSecond specs
		if (timeOfPrevFrame == 0) {	// beginning timer; we need to replace elapse time
			inputEv.type = INPUT_MOUSE;
			inputEv.mi.dwExtraInfo = static_cast<ULONG_PTR>(idEvent);
			timeOfPrevFrame = time - MS_PER_FRAME;
			SetTimer(hWnd, idEvent, MS_PER_FRAME, TimerProc);
		}
		// Seconds between now and the previous time this function was called.
		double timeDeltaS = static_cast<double>(time - timeOfPrevFrame) / 1000.0;

		// slow acceleartion for mouse and wheel if slow_mode key is held
		double accelerationModifier = state.isVKKeyDown[state.slowKey] ? state.slowRatio : 1;

		// calculate mouse acceleration, velocity, and position on both axis;
		// limit acceleration such that at max, it can only cancel out the current
		// velocity
		std::pair<double, double> mouseAcceleration = std::make_pair(
			-state.mouseVelocity.first * min(1 / timeDeltaS, state.mouseResistance),
			-state.mouseVelocity.second * min(1 / timeDeltaS, state.mouseResistance)
		);
		if (state.isVKKeyDown[state.leftKey])
			mouseAcceleration.first -= state.mouseAcceleration * accelerationModifier;
		if (state.isVKKeyDown[state.rightKey])
			mouseAcceleration.first += state.mouseAcceleration * accelerationModifier;
		if (state.isVKKeyDown[state.upKey])
			mouseAcceleration.second -= state.mouseAcceleration * accelerationModifier;
		if (state.isVKKeyDown[state.downKey])
			mouseAcceleration.second += state.mouseAcceleration * accelerationModifier;
		state.mouseVelocity.first += mouseAcceleration.first * timeDeltaS;
		state.mouseVelocity.second += mouseAcceleration.second * timeDeltaS;
		state.mousePositionRemainder.first += state.mouseVelocity.first;
		state.mousePositionRemainder.second += state.mouseVelocity.second;

		// send an update if state.mousePositionRemainder has an integer portion
		if (static_cast<int>(state.mousePositionRemainder.first) != 0 ||
			static_cast<int>(state.mousePositionRemainder.second) != 0) {
			inputEv.mi.dwFlags |= MOUSEEVENTF_MOVE;
			inputEv.mi.dx = static_cast<LONG>(state.mousePositionRemainder.first);
			inputEv.mi.dy = static_cast<LONG>(state.mousePositionRemainder.second);
			state.mousePositionRemainder.first -= static_cast<int>(state.mousePositionRemainder.first);
			state.mousePositionRemainder.second -= static_cast<int>(state.mousePositionRemainder.second);
		}

		// do a similar process for scroll wheel; like in DiffMode, send an update
		// whenever the wheel position is >1 or <-1 (i.e., send an update with an
		// integer if the wheel position has an integer portion)
		double scrollAcceleration = -state.scrollVelocity * min(1 / timeDeltaS, state.scrollResistance);
		if (state.isVKKeyDown[state.scrollUpKey])
			scrollAcceleration -= state.scrollAcceleration * accelerationModifier;
		if (state.isVKKeyDown[state.scrollDownKey])
			scrollAcceleration += state.scrollAcceleration * accelerationModifier;
		state.scrollVelocity += scrollAcceleration * timeDeltaS;
		state.scrollPositionRemainder += state.scrollVelocity;

		if (static_cast<int>(state.scrollPositionRemainder) != 0) {
			inputEv.mi.mouseData = static_cast<DWORD>(state.scrollPositionRemainder);
			inputEv.mi.dwFlags |= MOUSEEVENTF_WHEEL;
			state.scrollPositionRemainder -= static_cast<int>(state.scrollPositionRemainder);
		}

		// send input if we have something to send (either mousemove or
		// mousewheel)
		if (inputEv.mi.dwFlags != NULL)
			SendInput(1, &inputEv, sizeof(inputEv));

		// if velocity is not below threshold or if one of the wheel keys is down,
		// then continue the timer and try to maintain the framesPerSecond; else, kill timer
		if (abs(state.mouseVelocity.first) < VELOCITY_ZERO_EPS)
			state.mouseVelocity.first = 0;
		if (abs(state.mouseVelocity.second) < VELOCITY_ZERO_EPS)
			state.mouseVelocity.second = 0;
		if (abs(state.scrollVelocity) < VELOCITY_ZERO_EPS)
			state.scrollVelocity = 0;
		if (state.mouseVelocity.first == 0 && state.mouseVelocity.second == 0 && state.scrollVelocity == 0) {
			// Kill timer. Prepare variables for the next time timer is started.
			timeOfPrevFrame = 0;
			state.isTimerRunning = false;
			state.scrollPositionRemainder = 0;
			state.mousePositionRemainder = std::make_pair(0, 0);

			KillTimer(hWnd, idEvent);
		} else {
			timeOfPrevFrame = time;
		}
	}
}
