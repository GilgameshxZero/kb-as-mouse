#include "RainUtility.h"

namespace Rain
{
	void FreePtrVectorPtr (std::vector<void *> *p_vector_p)
	{
		for (std::size_t a = 0;a < p_vector_p->size ();a++)
			delete p_vector_p->at (a);
		delete p_vector_p;
	}

	char IntToBase64 (int x)
	{
		if (x < 26)
			return x + 'A';
		if (x < 52)
			return x - 26 + 'a';
		if (x < 62)
			return x - 52 + '0';
		if (x == 62)
			return '+';
		else //if (x == 63)
			return '/';
	}
	int Base64ToInt (char c)
	{
		if (c == '/')
			return 63;
		if (c == '+')
			return 62;
		if (c < 58)
			return c - '0' + 52;
		if (c < 91)
			return c - 'A';
		else //if (c < 123)
			return c - 'a' + 26;
	}
	void EncodeBase64 (const std::string &str, std::string &rtrn)
	{
		int bitsinchar = 0;
		int curchar = 0;
		static const int MAXBITS = 6;
		static const unsigned char MASK = 0xFF;
		for (std::size_t a = 0; a < str.length (); a++)
		{
			curchar |= ((str[a] & (MASK << (bitsinchar + 2))) >> (bitsinchar + 2));
			rtrn.push_back (IntToBase64 (curchar));
			curchar = (str[a] & (MASK >> (MAXBITS - bitsinchar)));
			bitsinchar = bitsinchar + 2;
			curchar <<= MAXBITS - bitsinchar;

			if (bitsinchar == MAXBITS)
			{
				rtrn.push_back (IntToBase64 (curchar));
				curchar = 0;
				bitsinchar = 0;
			}
		}

		if (bitsinchar != 0)
			rtrn.push_back (IntToBase64 (curchar));
	}
	void DecodeBase64 (const std::string &str, std::string &rtrn)
	{
		int bitsinchar = 0;
		char curchar = 0, curint;
		static const int MAXBITS = 8;
		static const unsigned char MASK = 0xFF;
		for (std::size_t a = 0; a < str.length (); a++)
		{
			curint = Base64ToInt (str[a]);
			if (bitsinchar == 0)
			{
				curchar = (curint << 2);
				bitsinchar = 6;
				continue;
			}
			curchar |= (((curint << 2) & (MASK << bitsinchar)) >> bitsinchar);
			rtrn.push_back (curchar);
			curchar = (curint & (MASK >> (10 - bitsinchar)));
			bitsinchar = bitsinchar - 2;
			curchar <<= MAXBITS - bitsinchar;
		}
	}

	//EncodeURL taken from http://stackoverflow.com/questions/154536/encode-decode-urls-in-c
	std::string EncodeURL (const std::string &value)
	{
		std::ostringstream escaped;
		escaped.fill ('0');
		escaped << std::hex;

		for (std::string::const_iterator i = value.begin (), n = value.end (); i != n; ++i) {
			std::string::value_type c = (*i);

			// Keep alphanumeric and other accepted characters intact
			if (isalnum (c) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}

			// Any other characters are percent-encoded
			escaped << std::uppercase;
			escaped << '%' << std::setw (2) << int ((unsigned char)c);
			escaped << std::nouppercase;
		}

		return escaped.str ();
	}
	//DecodeURL taken from http://stackoverflow.com/questions/2673207/c-c-url-decode-library
	std::string DecodeURL (const std::string &value)
	{
		std::string rtrn;
		char a, b;
		const char *src = value.c_str ();
		while (*src) {
			if ((*src == '%') &&
				((a = src[1]) && (b = src[2])) &&
				(isxdigit (a) && isxdigit (b))) {
				if (a >= 'a')
					a -= 'a' - 'A';
				if (a >= 'A')
					a -= ('A' - 10);
				else
					a -= '0';
				if (b >= 'a')
					b -= 'a' - 'A';
				if (b >= 'A')
					b -= ('A' - 10);
				else
					b -= '0';
				rtrn += 16 * a + b;
				src += 3;
			}
			else if (*src == '+') {
				rtrn += ' ';
				src++;
			}
			else {
				rtrn += *src++;
			}
		}

		return rtrn;
	}

	int IntLogLen (int x)
	{
		int ilen = 1;
		for (int a = 10; a <= x; a *= 10)
			ilen++;
		return ilen;
	}

	HANDLE SimpleCreateThread (LPTHREAD_START_ROUTINE threadfunc, LPVOID threadparam)
	{
		return CreateThread (NULL, 0, threadfunc, threadparam, 0, NULL);
	}

	HANDLE LogMemoryLeaks (std::string out_file)
	{
		if (IsDebuggerPresent ())
		{
			//Redirect the error stream to a file, only if the program is debugging.
			HANDLE mem_leak = CreateFile (out_file.c_str (), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			//Turn on debugging for memory leaks. This is automatically turned off when the build is Release.
			_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
			_CrtSetReportMode (_CRT_WARN, _CRTDBG_MODE_FILE);
			_CrtSetReportFile (_CRT_WARN, mem_leak);
			_CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_FILE);
			_CrtSetReportFile (_CRT_ERROR, mem_leak);
			_CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_FILE);
			_CrtSetReportFile (_CRT_ASSERT, mem_leak);

			return mem_leak;
		}

		return NULL;
	}
}