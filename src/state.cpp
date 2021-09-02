#include <state.hpp>

namespace KbAsMouse {
	State state;

	void State::togglePause() {
		this->paused = !this->paused;

		// Reset pressed in case pausing during keydown/keyup sequence. Update
		// notify icon to reflect status.
		if (this->paused) {
			this->pressed.reset();
		}

		this->updateNotifyIconData();
		Rain::Windows::validateSystemCall(
			Shell_NotifyIcon(NIM_MODIFY, &state.notifyIconData));
	}
	void State::updateNotifyIconData() {
		if (this->paused) {
			this->pressed.reset();

			strcpy(
				this->notifyIconData.szTip,
				"kb-as-mouse (paused)\nleft click = resume | right click = exit");
			this->notifyIconData.hIcon = this->notifyIconOpen;
		} else {
			strcpy(
				this->notifyIconData.szTip,
				"kb-as-mouse\nleft click = pause | right click = exit");
			this->notifyIconData.hIcon = this->notifyIconClosed;
		}
	}
}