/*
Standard
*/

#pragma once

#include "RainUtilityFile.h"
#include "RainWindowsLAM.h"

#include <cstdarg>
#include <iomanip> //EncodeURL and DecodeURL
#include <sstream>
#include <string>

namespace Rain
{
	template <typename T>
	std::string TToStr (T x)
	{
		std::stringstream ss;
		ss << x;
		return ss.str ();
	}

	template <typename T>
	T StrToT (std::string s)
	{
		T r;
		std::stringstream ss (s);
		ss >> r;
		return r;
	}

	//PackPointers packages a set of pointers into one variable, which the caller must free
	template <typename T>
	void PushToPtrVectorPtr (std::vector<void *> *p_vector_p, T *x)
	{
		p_vector_p->push_back (reinterpret_cast<void *>(x));
		return;
	}

	template <typename T, typename... Args>
	void PushToPtrVectorPtr (std::vector<void *> *p_vector_p, T *x, Args... args)
	{
		p_vector_p->push_back (reinterpret_cast<void *>(x));
		PushToPtrVectorPtr (p_vector_p, args...);
	}

	template <typename... Args>
	std::vector<void *> *PackPointers (Args... args)
	{
		std::vector<void *> *p_vector_p = new std::vector<void *> ();
		PushToPtrVectorPtr (p_vector_p, args...);
		return p_vector_p;
	}

	//can use in conjunction with PackPointers after done with the pack
	void FreePtrVectorPtr (std::vector<void *> *p_vector_p);

	char IntToBase64 (int x);
	int Base64ToInt (char c);
	void EncodeBase64 (const std::string &str, std::string &rtrn);
	void DecodeBase64 (const std::string &str, std::string &rtrn);

	std::string EncodeURL (const std::string &value);
	std::string DecodeURL (const std::string &value);

	//length of x, interpreted as a base10 string
	int IntLogLen (int x);

	HANDLE SimpleCreateThread (LPTHREAD_START_ROUTINE threadfunc, LPVOID threadparam);

	//dumps memory leaks to a file if on debug mode; application must CloseHandle the return HANDLE, unless it's debug mode and the return is NULL
	HANDLE LogMemoryLeaks (std::string out_file);
}