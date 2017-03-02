#include "RainError.h"

namespace Rain
{
	int ReportError (int code, std::string desc)
	{
		if (desc != "")
			std::cerr << desc << "\n";
		std::cerr << "Error code: " << code << std::endl;
		return code;
	}

	std::pair<std::streambuf *, std::ofstream *> RedirectCerrFile (std::string filename)
	{
		std::ofstream *cerrfile = new std::ofstream;
		cerrfile->open (filename);
		return std::make_pair (std::cerr.rdbuf (cerrfile->rdbuf ()), cerrfile);
	}

	void CloseCerrFile (std::pair<std::streambuf *, std::ofstream *> cerr_filedata)
	{
		std::cerr.rdbuf (cerr_filedata.first);
		cerr_filedata.second->close ();
		delete cerr_filedata.second;
	}
}