#include "Start.h"

namespace KeyAsMouse {
	namespace Start {
		int screen_width, screen_height;

		int Start (HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR lpCmdLine,
			int nCmdShow) {
			const int NUM_SETTINGS = 19;

			std::pair<std::streambuf *, std::ofstream *> cerr_filedata;
			HANDLE mem_leak;
			std::ifstream config;
			int csettings = 0;
			POINT mouse_pos;
			RECT wrect;
			HHOOK llkb_hook, llmouse_hook;
			BOOL bRet;
			MSG msg;

			//report errors to file
			cerr_filedata = Rain::RedirectCerrFile ("keyasmouse_errors.txt");

			//catch memory leaks
			mem_leak = Rain::LogMemoryLeaks ("memory_leaks.txt");

			//read config file
			config.open ("config.txt");
			if (!config.good ())
				return Rain::ReportError (-1, "No configuration file found.");

			for (;;csettings++) {
				std::string setting;

				std::getline (config, setting, config.widen (':'));
				if (!config.good ())
					break;

				setting.erase (std::remove_if (setting.begin (), setting.end (), ::isspace), setting.end ());

				if (setting == "LeftKey")
					config >> Settings::left;
				else if (setting == "RightKey")
					config >> Settings::right;
				else if (setting == "UpKey")
					config >> Settings::up;
				else if (setting == "DownKey")
					config >> Settings::down;
				else if (setting == "LeftClick")
					config >> Settings::lclick;
				else if (setting == "RightClick")
					config >> Settings::rclick;
				else if (setting == "MiddleClick")
					config >> Settings::mclick;
				else if (setting == "WheelUp")
					config >> Settings::wheelup;
				else if (setting == "WheelDown")
					config >> Settings::wheeldown;
				else if (setting == "FramesPerSecond")
					config >> Settings::fps;
				else if (setting == "Acceleration")
					config >> Settings::mouse_acc;
				else if (setting == "Resistance")
					config >> Settings::resist;
				else if (setting == "ResistPower")
					config >> Settings::resist_power;
				else if (setting == "ScrollAcc")
					config >> Settings::scroll_acc;
				else if (setting == "ScrollResist")
					config >> Settings::scroll_resist;
				else if (setting == "ScrollResistPower")
					config >> Settings::scroll_resist_power;
				else if (setting == "MinVelocityThreshold")
					config >> Settings::min_vel_thresh;
				else if (setting == "MinScrollVelThresh")
					config >> Settings::min_scroll_vel_thresh;
				else if (setting == "TerminateKey")
					config >> Settings::terminate_key;
				else
					return Rain::ReportError (-2, "Configuration file contained unrecognised setting.");
			}

			if (csettings != NUM_SETTINGS)
				return Rain::ReportError (-3, "Configuration file contained too many or too little settings.");

			config.close ();

			//init other variables
			Settings::crit_keys.insert (Settings::left);
			Settings::crit_keys.insert (Settings::right);
			Settings::crit_keys.insert (Settings::up);
			Settings::crit_keys.insert (Settings::down);
			Settings::crit_keys.insert (Settings::lclick);
			Settings::crit_keys.insert (Settings::rclick);
			Settings::crit_keys.insert (Settings::mclick);
			Settings::crit_keys.insert (Settings::wheelup);
			Settings::crit_keys.insert (Settings::wheeldown);

			GetCursorPos (&mouse_pos);
			MKBState::exact_pos = std::make_pair (static_cast<double>(mouse_pos.x), static_cast<double>(mouse_pos.y));
			MKBState::mouse_vel = std::make_pair (0, 0);
			MKBState::wheel_pos = 0;
			MKBState::wheel_vel = 0;

			GetWindowRect (GetDesktopWindow (), &wrect);
			screen_width = wrect.right - wrect.left;
			screen_height = wrect.bottom - wrect.top;

			TimerManager::mspf = 1000 / Settings::fps;

			//install hooks and message loop
			llkb_hook = SetWindowsHookEx (WH_KEYBOARD_LL, KeyAsMouse::HookEventManager::LLKBProc, NULL, NULL);
			llmouse_hook = SetWindowsHookEx (WH_MOUSE_LL, KeyAsMouse::HookEventManager::LLMouseProc, NULL, NULL);

			while ((bRet = GetMessage (&msg, NULL, 0, 0)) != 0) {
				if (bRet == -1)
					return Rain::ReportError (-1, "Message loop returned -1.");
				else {
					TranslateMessage (&msg);
					DispatchMessage (&msg);
				}
			}

			//clean up and exit
			UnhookWindowsHookEx (llkb_hook);
			UnhookWindowsHookEx (llmouse_hook);

			if (mem_leak != NULL)
				CloseHandle (mem_leak);

			Rain::ReportError (0, "No errors.");
			Rain::CloseCerrFile (cerr_filedata);

			return 0;
		}
	}
}