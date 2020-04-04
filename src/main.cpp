#include "main.hpp"

int CALLBACK WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) {
	return NumpadAsMouse::start(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

namespace NumpadAsMouse {
	int screen_width, screen_height;

	int start(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow) {

		int csettings = 0;
		POINT mouse_pos;
		RECT wrect;
		BOOL bRet;
		MSG msg;

		// catch memory leaks and errors
		HANDLE hMemoryLeaks = Rain::logMemoryLeaks("memory-leaks.txt");

		// read config file
		Rain::Configuration configuration(".cfg");
		std::ifstream config;
		config.open(".cfg");
		if (!config.good())
			return Rain::reportError(-1, "No configuration file found.");
		
		const int NUM_SETTINGS = 27;
		for (;; csettings++) {
			std::string setting;

			std::getline(config, setting, config.widen(':'));
			if (!config.good())
				break;

			// remove whitespace from setting name
			setting.erase(std::remove_if(setting.begin(), setting.end(), ::isspace),
				setting.end());

			if (setting == "LeftKey")
				config >> left;
			else if (setting == "RightKey")
				config >> right;
			else if (setting == "UpKey")
				config >> up;
			else if (setting == "DownKey")
				config >> down;
			else if (setting == "LeftClick")
				config >> lclick;
			else if (setting == "RightClick")
				config >> rclick;
			else if (setting == "MiddleClick")
				config >> mclick;
			else if (setting == "WheelUp")
				config >> wheelup;
			else if (setting == "WheelDown")
				config >> wheeldown;
			else if (setting == "WheelUpSingle")
				config >> wheelupsingle;
			else if (setting == "WheelDownSingle")
				config >> wheeldownsingle;
			else if (setting == "WheelLeft")
				config >> wheelleft;
			else if (setting == "WheelRight")
				config >> wheelright;
			else if (setting == "SlowMode")
				config >> slow_mode;
			else if (setting == "PauseKey")
				config >> pause_key;
			else if (setting == "TerminateKey")
				config >> terminate_key;
			else if (setting == "FramesPerSecond")
				config >> fps;
			else if (setting == "Acceleration")
				config >> mouse_acc;
			else if (setting == "Resistance")
				config >> resist;
			else if (setting == "ResistPower")
				config >> resist_power;
			else if (setting == "ScrollAcc")
				config >> scroll_acc;
			else if (setting == "ScrollResist")
				config >> scroll_resist;
			else if (setting == "ScrollResistPower")
				config >> scroll_resist_power;
			else if (setting == "MinVelocityThreshold")
				config >> min_vel_thresh;
			else if (setting == "MinScrollVelThresh")
				config >> min_scroll_vel_thresh;
			else if (setting == "SlowRatio")
				config >> slow_ratio;
			else if (setting == "DiffMode")
				config >> diff_mode;
			else
				return Rain::reportError(
					-2, "Configuration file contained unrecognised setting.");
		}

		if (csettings != NUM_SETTINGS)
			return Rain::reportError(
				-3, "Configuration file contained too many or too little settings.");

		config.close();

		// init other variables
		crit_keys.insert(left);
		crit_keys.insert(right);
		crit_keys.insert(up);
		crit_keys.insert(down);
		crit_keys.insert(lclick);
		crit_keys.insert(rclick);
		crit_keys.insert(mclick);
		crit_keys.insert(wheelup);
		crit_keys.insert(wheeldown);
		crit_keys.insert(wheelupsingle);
		crit_keys.insert(wheeldownsingle);
		crit_keys.insert(wheelleft);
		crit_keys.insert(wheelright);
		crit_keys.insert(slow_mode);
		crit_keys.insert(pause_key);

		GetCursorPos(&mouse_pos);
		exact_pos = std::make_pair(
			static_cast<double>(mouse_pos.x), static_cast<double>(mouse_pos.y));
		mouse_vel = mouse_pos_diff = std::make_pair(0, 0);
		wheel_pos = 0;
		wheel_vel = 0;
		paused = false;

		GetWindowRect(GetDesktopWindow(), &wrect);
		screen_width = wrect.right - wrect.left;
		screen_height = wrect.bottom - wrect.top;

		mspf = 1000 / fps;

		// install hooks and message loop
		HHOOK llkb_hook = SetWindowsHookEx(
						WH_KEYBOARD_LL, NumpadAsMouse::LLKBProc, NULL, NULL),
					llmouse_hook = SetWindowsHookEx(
						WH_MOUSE_LL, NumpadAsMouse::LLMouseProc, NULL, NULL);

		while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
			if (bRet == -1)
				return Rain::reportError(-1, "Message loop returned -1.");
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		// clean up and exit
		UnhookWindowsHookEx(llkb_hook);
		UnhookWindowsHookEx(llmouse_hook);

		if (hMemoryLeaks != NULL)
			CloseHandle(hMemoryLeaks);

		return 0;
	}
}
