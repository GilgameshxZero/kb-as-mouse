#include "RainUtilityFile.h"

namespace Rain
{
	void TrimBSR (std::string &s)
	{
		if (!s.empty () && s.back () == '\r')
			s.pop_back ();
	}

	void GetFiles (std::string directory, std::vector<std::string> &rtrn, std::string format)
	{
		char search_path[MAX_PATH], multibyte[MAX_PATH];
		wchar_t unicodesp[MAX_PATH];
		sprintf_s (search_path, ("%s/" + format).c_str (), directory.c_str ());
		WIN32_FIND_DATAW fd;

		MultiByteToWideChar (CP_UTF8, 0, search_path, -1, unicodesp, MAX_PATH);
		int k = GetLastError ();
		HANDLE hFind = ::FindFirstFileW (unicodesp, &fd);

		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					WideCharToMultiByte (CP_UTF8, 0, fd.cFileName, -1, multibyte, MAX_PATH, NULL, NULL);
					rtrn.push_back (multibyte);
				}
			} while (::FindNextFileW (hFind, &fd));
			::FindClose (hFind);
		}
	}

	void GetDirectories (std::string directory, std::vector<std::string> &rtrn, std::string format)
	{
		char search_path[MAX_PATH], multibyte[MAX_PATH];
		wchar_t unicodesp[MAX_PATH];
		sprintf_s (search_path, ("%s/" + format).c_str (), directory.c_str ());
		WIN32_FIND_DATAW fd;

		MultiByteToWideChar (CP_UTF8, 0, search_path, -1, unicodesp, MAX_PATH);
		int k = GetLastError ();
		HANDLE hFind = ::FindFirstFileW (unicodesp, &fd);

		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					WideCharToMultiByte (CP_UTF8, 0, fd.cFileName, -1, multibyte, MAX_PATH, NULL, NULL);
					rtrn.push_back (multibyte);
				}
			} while (::FindNextFileW (hFind, &fd));
			::FindClose (hFind);
		}
	}

	std::string GetExePath ()
	{
		char multibyte[MAX_PATH];
		wchar_t buffer[MAX_PATH];

		GetModuleFileNameW (NULL, buffer, MAX_PATH);
		WideCharToMultiByte (CP_UTF8, 0, buffer, -1, multibyte, MAX_PATH, NULL, NULL);

		std::string::size_type pos = std::string (multibyte).find_last_of ("\\/");

		return std::string (multibyte).substr (0, pos);
	}

	void GetRelFilePathRec (std::string directory, std::vector<std::string> &relpath, std::string format)
	{
		std::vector<std::string> dirs, files;
		Rain::GetDirectories (directory, dirs, "*");
		Rain::GetFiles (directory, files, format);

		for (std::size_t a = 0; a < files.size (); a++)
			relpath.push_back (files[a]);

		for (std::size_t a = 2; a < dirs.size (); a++)
		{
			std::vector<std::string> subrel;
			GetRelFilePathRec (directory + dirs[a] + "\\", subrel, format);

			for (std::size_t b = 0; b < subrel.size (); b++)
				relpath.push_back (dirs[a] + "\\" + subrel[b]);
		}
	}

	void GetRelDirPathRec (std::string directory, std::vector<std::string> &relpath, std::string format)
	{
		std::vector<std::string> dirs;
		Rain::GetDirectories (directory, dirs, "*");

		for (size_t a = 2; a < dirs.size (); a++)
		{
			relpath.push_back (dirs[a] + "\\");

			std::vector<std::string> subrel;
			GetRelDirPathRec (directory + dirs[a] + "\\", subrel, format);

			for (size_t b = 0; b < subrel.size (); b++)
				relpath.push_back (dirs[a] + "\\" + subrel[b]);
		}
	}

	void GetLastModTime (std::vector<std::string> &files, std::vector<FILETIME> &lastmod)
	{
		wchar_t unicode[MAX_PATH];

		for (size_t b = 0; b < files.size (); b++)
		{
			MultiByteToWideChar (CP_UTF8, 0, files[b].c_str (), -1, unicode, MAX_PATH);
			HANDLE hfile = CreateFileW (unicode, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			lastmod.push_back (FILETIME ());

			if (hfile == INVALID_HANDLE_VALUE)
				lastmod.back ().dwLowDateTime = lastmod.back ().dwHighDateTime = 0;
			else
			{
				GetFileTime (hfile, NULL, NULL, &lastmod.back ());
				CloseHandle (hfile);
			}
		}
	}

	void RecursiveRmDir (std::string path)
	{
		wchar_t unicode[MAX_PATH];
		std::vector<std::string> ldir, lfile;

		GetDirectories (path, ldir, "*");
		GetFiles (path, lfile, "*");

		for (std::size_t a = 0; a < lfile.size (); a++)
		{
			MultiByteToWideChar (CP_UTF8, 0, (path + lfile[a]).c_str (), -1, unicode, MAX_PATH);
			DeleteFileW (unicode);
		}
		for (std::size_t a = 2; a < ldir.size (); a++) //skip . and ..
		{
			MultiByteToWideChar (CP_UTF8, 0, (path + ldir[a] + '\\').c_str (), -1, unicode, MAX_PATH);
			RecursiveRmDir (path + ldir[a] + '\\');
			RemoveDirectoryW (unicode);
		}
	}

	std::string GetShortName (std::string directory)
	{
		std::string dirname;
		bool seechar = false;
		for (std::size_t a = directory.length () - 1; a >= 0; a--)
		{
			if (directory[a] == ':')
			{
				dirname = "C:\\";
				break;
			}
			else if (directory[a] == '\\' && seechar)
			{
				dirname = directory.substr (a + 1, directory.length () - a - 1);
				break;
			}
			else
				seechar = true;
		}

		return dirname;
	}

	bool FileExists (std::string file)
	{
		struct stat buffer;
		return (stat (file.c_str (), &buffer) == 0);
	}

	void FastOutputFile (std::string filename, std::string &output, bool append)
	{
		std::ofstream out;
		if (append)
			out.open (filename, std::ios_base::out | std::ios_base::binary | std::ios_base::app);
		else
			out.open (filename, std::ios_base::out | std::ios_base::binary);
		out << output;
		out.close ();
	}
}