/*
 * Base64Converter.h
 *
 * Convert to base 64
 *
 * Author: Eric Wistrand 11/12/2023
 */

#ifndef _Base64Converter_H
#define _Base64Converter_H

#include "WServer.h"

/**
 * Convert to base 64
 */
class WSERVER_API Base64Converter
{
public:
    /**
     * Convert the input string to base 64.
     *
     * @param str - char&, a reference to the character string to convert to base 64.
     * @ret	char *, a pointer to the base 64 converted string, or NULL. If NON-NULL,
     *		the caller has the the responsiblity of delete[]-ing this.
     */
    static char* encode(char& str);

private:
    static const char alphabet[];
};

#endif
