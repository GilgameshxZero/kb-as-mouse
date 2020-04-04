#include "state.hpp"

namespace NumpadAsMouse {
	State::State() {
		this->refreshInterceptKeys();
	}

	void State::overwriteSettingsFromFile(std::string file) {
		static const std::map<std::string, int &> INT_SETTINGS_MAP = {
			{"upKey", this->upKey},
			{"rightKey", this->rightKey},
			{"downKey", this->downKey},
			{"leftKey", this->leftKey},
			{"leftClickKey", this->leftClickKey},
			{"rightClickKey", this->rightClickKey},
			{"middleClickKey", this->middleClickKey},
			{"scrollUpKey", this->scrollUpKey},
			{"scrollDownKey", this->scrollDownKey},
			{"scrollLeftSingleKey", this->scrollLeftSingleKey},
			{"scrollRightSingleKey", this->scrollRightSingleKey},
			{"slowKey", this->slowKey},
			{"pauseKey", this->pauseKey},
			{"terminateKey", this->terminateKey},
			{"framesPerSecond", this->framesPerSecond},
		};
		static const std::map<std::string, double &> DOUBLE_SETTINGS_MAP = {
			{"mouseResistance", this->mouseResistance},
			{"mouseAcceleration", this->mouseAcceleration},
			{"scrollResistance", this->scrollResistance},
			{"scrollAcceleration", this->scrollAcceleration},
			{"slowRatio", this->slowRatio},
		};
		Rain::Configuration fileSettings(file);

		for (auto it = INT_SETTINGS_MAP.begin(); it != INT_SETTINGS_MAP.end(); it++) {
			if (!fileSettings.has(it->first)) continue;
			it->second = fileSettings[it->first].i();
		}
		for (auto it = DOUBLE_SETTINGS_MAP.begin(); it != DOUBLE_SETTINGS_MAP.end(); it++) {
			if (!fileSettings.has(it->first)) continue;
			it->second = fileSettings[it->first].d();
		}

		this->refreshInterceptKeys();
	}

	bool State::shouldInterceptKey(int vkCode) {
		return this->keysToIntercept.find(vkCode) != this->keysToIntercept.end();
	}

	void State::refreshInterceptKeys() {
		this->keysToIntercept = {
			this->upKey,
			this->rightKey,
			this->downKey,
			this->leftKey,
			this->leftClickKey,
			this->rightClickKey,
			this->middleClickKey,
			this->scrollUpKey,
			this->scrollDownKey,
			this->scrollLeftSingleKey,
			this->scrollRightSingleKey,
			this->slowKey,
			this->pauseKey,
			this->terminateKey,
		};
	}
}
