#include "state.hpp"

namespace KbAsMouse {
	State::State() { this->refreshInterceptKeys(); }

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
		std::map<std::string, std::string> fileSettings;
		std::ifstream in(file, std::ios::binary);
		std::string line;
		while (std::getline(in, line)) {
			if (line.back() == '\r') {
				line.pop_back();
			}
			if (line.empty()) {
				continue;
			}

			// scope to right configuration option
			std::size_t level;
			for (level = 0; level < line.size(); level++) {
				if (line[level] != '\t') {
					break;
				}
			}

			// this option
			Rain::String::trimWhitespaceStr(&line);
			std::size_t keyInd;
			for (keyInd = 0; keyInd < line.size(); keyInd++) {
				if (std::isspace(line[keyInd])) {
					break;
				}
			}
			fileSettings[line.substr(0, keyInd)] = line.substr(keyInd);
			Rain::String::trimWhitespaceStr(&fileSettings[line.substr(0, keyInd)]);
		}
		in.close();

		for (auto it = INT_SETTINGS_MAP.begin(); it != INT_SETTINGS_MAP.end();
				 it++) {
			if (fileSettings.find(it->first) == fileSettings.end()) continue;
			it->second = std::strtol(fileSettings[it->first].c_str(), NULL, 10);
		}
		for (auto it = DOUBLE_SETTINGS_MAP.begin(); it != DOUBLE_SETTINGS_MAP.end();
				 it++) {
			if (fileSettings.find(it->first) == fileSettings.end()) continue;
			it->second = std::strtold(fileSettings[it->first].c_str(), NULL);
		}

		this->refreshInterceptKeys();
	}

	bool State::shouldInterceptKey(int vkCode) {
		return this->keysToIntercept.find(vkCode) != this->keysToIntercept.end();
	}

	bool State::isShiftDown() {
		return this->isVKKeyDown[16] || this->isVKKeyDown[160] ||
			this->isVKKeyDown[161];
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

	State state;
}
