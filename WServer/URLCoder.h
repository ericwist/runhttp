//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023

#ifndef URLCoderH
#define URLCoderH

#ifndef	_WIN32_WCE	// [
#include <iostream>

using namespace std;
#endif				// ]

#include "WServer.h"

//using namespace std;

class WSERVER_API URLCoder
{
public:
	/**
	 * Decodes a x-www-form-urlencoded to a char String.
	 *
	 * @param s - const char&, a reference to the char String to decode
	 * @return char& - a reference to the newly decoded char String which must be deleted by the caller.
	 */
	static char& decode(const char& s);

	/**
	 * Decodes a x-www-form-urlencoded in place.
	 *
	 * @param encoded - char *, a pointer the char string which will be decoded in place.
	 */
	static void decode(char * encoded);

	/**
	 * Determine if the passed in character needs to be encoded.
	 *
	 * @param c char, the character to check.
	 * @param extended bool, TRUE for extended encoding, FALSE otherwise.
	 * @ret bool TRUE if encoding is necessary, false otherwise.
	 */
	static bool shouldEncode(char c, bool extended);

    /**
     * Translates a string into x-www-form-urlencoded format.
     *
     * @param s - const char&, a reference to the char String to be translated.
     * @return char& - a reference to the newly translated char String which must be deleted by the caller.
     */
    static char& encode(const char& s);

};

//---------------------------------------------------------------------------
#endif
