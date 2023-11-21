//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
// #pragma hdrstop

#ifdef _WIN32_WCE // [
#include <windows.h>

_CRTIMP int __cdecl _isctype(int, int);
#define isxdigit(_c) (_isctype(_c, _HEX))

_CRTIMP long __cdecl strtol(const char*, char**, int);
#endif // ]

#include "URLCoder.h"

/**
 * Decodes a x-www-form-urlencoded to a char String.
 *
 * @param s - const char&, a reference to the char String to decode
 * @return char& - a reference to the newly decoded char String which must be deleted by the caller.
 */
char& URLCoder::decode(const char& s)
{
    int len = strlen(&s);
    char* retStr = new char[len + 1];

    strcpy(retStr, &s);
    decode(retStr);

    return *retStr;
}

/**
 * Decodes a x-www-form-urlencoded in place.
 *
 * @param encoded - char *, a pointer the char string which will be decoded in place.
 */
void URLCoder::decode(char* encoded)
{
    char* decoded; // generic pointer

    // Now, loop through looking for escapes
    decoded = encoded;
    while (*encoded)
    {
        if (*encoded == '%')
        {
            // A percent sign followed by two hex digits means
            // that the digits represent an escaped character.
            // We must decode it.
            encoded++;
            if (isxdigit(encoded[0]) && isxdigit(encoded[1]))
            {
                char buf[3];

                buf[0] = encoded[0];
                buf[1] = encoded[1];
                buf[2] = 0;

                *decoded++ = (char)strtol(buf, NULL, 16); //(char)IntFromHex(encoded);
                encoded += 2;
            }
        }
        else
        {
            if (*encoded == '+')
                *decoded = ' ';

            *decoded++ = *encoded++;
        }
    }
    *decoded = 0;
}

/**
 * Determine if the passed in character needs to be encoded.
 *
 * @param c char, the character to check.
 * @param extended bool, TRUE for extended encoding, FALSE otherwise.
 * @ret bool TRUE if encoding is necessary, false otherwise.
 */
bool URLCoder::shouldEncode(char c, bool extended)
{
    if (extended)
    {
        if (c < 32) return true;
        if (c == '\"') return true;
        if (c == '\'') return true;
        if (c == ',') return true;
        if (c > 'z') return true;
    }
    else
    {
        if (c == '.') return false;
        if (c < '0') return true;
        if (c > '9' && c < 'A') return true;
        if (c > 'z') return true;
    }

    return false;
}

/**
 * Translates a string into x-www-form-urlencoded format.
 *
 * @param s - const char&, a reference to the char String to be translated.
 * @return char& - a reference to the newly translated char String which must be deleted by the caller.
 */
char& URLCoder::encode(const char& s)
{

    char* str = (char*)&s;
    int len = strlen(str);
    int dstLen = len;

    // First count to see how big to allocate the encoded return string.
    while (*str)
    {
        if (shouldEncode(*str++, false))
            dstLen += 2;
    }

    char* encoded = new char[dstLen + 1];

    str = (char*)&s;
    if (dstLen == len)
    {
        // No encoding is needed, just return a copy of the original string.
        strcpy(encoded, str);
    }
    else
    {
        char* ptr = encoded;

        while (*str)
        {
            char c = *str++;

            if (shouldEncode(c, false))
            {
                *ptr++ = '%';
                *ptr++ = ((c >> 4) > 9 ? ('A' + (c >> 4) - 10) : (c >> 4) + '0');
                *ptr++ = ((c & 0xF) > 9 ? ('A' + (c & 0xF) - 10) : (c & 0xF) + '0');
            }
            else
                *ptr++ = c;
        }
        *ptr = 0;
    }

    return *encoded;
}

/* // From http://www.bjnet.edu.cn/tech/book/seucgi/ch22.htm

// This code fragment shows how to un-URL-encode a given string.

// First, a subroutine that substitutes any instance of cBad with
// cGood in a string.  This is used to replace the plus sign with
// a space character.

void SwapChar(char * pOriginal, char cBad, char cGood) {
    int i;    // generic counter variable

    // Loop through the input string (cOriginal), character by
    // character, replacing each instance of cBad with cGood

    i = 0;
    while (pOriginal[i]) {
        if (pOriginal[i] == cBad) pOriginal[i] = cGood;
        i++;
    }
}

// Now, a subroutine that unescapes escaped characters.
static int IntFromHex(char *pChars) {
    int Hi;        // holds high byte
    int Lo;        // holds low byte
    int Result;    // holds result

    // Get the value of the first byte to Hi

    Hi = pChars[0];
    if ('0' <= Hi && Hi <= '9') {
        Hi -= '0';
    } else
    if ('a' <= Hi && Hi <= 'f') {
        Hi -= ('a'-10);
    } else
    if ('A' <= Hi && Hi <= 'F') {
        Hi -= ('A'-10);
    }

    // Get the value of the second byte to Lo

    Lo = pChars[1];
    if ('0' <= Lo && Lo <= '9') {
        Lo -= '0';
    } else
    if ('a' <= Lo && Lo <= 'f') {
        Lo -= ('a'-10);
    } else
    if ('A' <= Lo && Lo <= 'F') {
        Lo -= ('A'-10);
    }
    Result = Lo + (16 * Hi);
    return (Result);
}

// And now, the main URLDecode() routine.  The routine loops
// through the string pEncoded, and decodes it in place. It checks
// for escaped values, and changes all plus signs to spaces. The
// result is a normalized string.  It calls the two subroutines
// directly above in this listing.

void URLDecode(unsigned char *pEncoded) {
    char *pDecoded;          // generic pointer

    // First, change those pesky plusses to spaces
    SwapChar (pEncoded, '+', ' ');

    // Now, loop through looking for escapes
    pDecoded = pEncoded;
    while (*pEncoded) {
        if (*pEncoded=='%') {
            // A percent sign followed by two hex digits means
            // that the digits represent an escaped character.
            // We must decode it.

            pEncoded++;
            if (isxdigit(pEncoded[0]) && isxdigit(pEncoded[1])) {
                *pDecoded++ = (char) IntFromHex(pEncoded);
                pEncoded += 2;
            }
        } else {
            *pDecoded ++ = *pEncoded++;
        }
    }
    *pDecoded = '\0';
}
*/

//---------------------------------------------------------------------------
// #pragma package(smart_init)
