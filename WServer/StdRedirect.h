/*
 * StdRedirect.h
 *
 * A class which redirects cout and cerr to a file.
 *
 * Author: Eric Wistrand 11/12/2023
 */

#ifndef StdRedirectH
#define StdRedirectH

#ifndef	_WIN32_WCE	// [
#include "WServer.h"

#include <iostream>
#include <fstream>

class StdRedirect
{
public:

	StdRedirect(const char * coutPath, const char * cerrPath);
	~StdRedirect();

private:
	std::ofstream	  coutFile;
	std::ofstream	  cerrFile;
	std::streambuf	* cout_buffer, * cerr_buffer;

};
#endif				// ]

#endif