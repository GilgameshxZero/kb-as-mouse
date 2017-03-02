/*
Standard
*/

#pragma once

#include <fstream>
#include <iostream>
#include <string>

namespace Rain
{
	int ReportError (int code, std::string desc = "");
	std::pair<std::streambuf *, std::ofstream *> RedirectCerrFile (std::string filename);
	void CloseCerrFile (std::pair<std::streambuf *, std::ofstream *> cerr_filedata);
}