/*
Standard
*/

#pragma once

#include "RainWindowsLAM.h"

#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <vector>

namespace Rain
{
	//trim possible /r at end of string
	void TrimBSR (std::string &s);

	bool FileExists (std::string file);

	//Get all files in a directory, in a certain format. NOTE: Takes and returns UTF8 multibyte strings - but works with unicode directories.
	void GetFiles (std::string directory, std::vector<std::string> &rtrn, std::string format);

	//Get all subdirectories in a directory, in a certain format. NOTE: Takes and returns UTF8 multibyte strings - but works with unicode directories.
	void GetDirectories (std::string directory, std::vector<std::string> &rtrn, std::string format);

	//returns UTF8
	std::string GetExePath ();

	//works with both Unicode and UTF8 (untested)
	std::string GetShortName (std::string directory);

	//takes and returns UTF8 strings, but uses unicode
	void GetRelFilePathRec (std::string directory, std::vector<std::string> &relpath, std::string format = "*");

	//takes and returns UTF8 strings, but uses unicode
	void GetRelDirPathRec (std::string directory, std::vector<std::string> &relpath, std::string format = "*");

	//takes and returns UTF8 strings, but uses unicode
	void GetLastModTime (std::vector<std::string> &files, std::vector<FILETIME> &lastmod);

	//takes and returns UTF8 strings, but uses unicode
	void RecursiveRmDir (std::string path);

	void FastOutputFile (std::string filename, std::string &output, bool append = false);
}