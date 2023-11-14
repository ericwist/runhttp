/*
 * StdRedirect.cpp
 *
 * A class which redirects cout and cerr to a file.
 *
 * Author: Eric Wistrand 11/12/2023
 */

#ifndef	_WIN32_WCE	// [
#include "StdRedirect.h"

StdRedirect::StdRedirect(const char * coutPath, const char * cerrPath) : coutFile(coutPath), cerrFile(cerrPath)
{
	// save output buffers of the streams
	cout_buffer = std::cout.rdbuf();
	cerr_buffer = std::cout.rdbuf();

	// redirect ouput into the file
	std::cout.rdbuf(coutFile.rdbuf());
	std::cerr.rdbuf(cerrFile.rdbuf());

	std::cout << "StdRedirect() END" << std::endl;
}

StdRedirect::~StdRedirect()
{
	std::cout << "~StdRedirect() ENTERED" << std::endl;
	// restore old output buffers
	std::cerr.rdbuf(cerr_buffer);
	std::cout.rdbuf(cout_buffer);
} // closes file AND its buffer automatically

#endif				// ]