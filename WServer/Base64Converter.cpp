/*
 * Base64Converter.cpp
 *
 * Convert to base 64
 *
 * Author: Eric Wistrand 11/12/2023
 */

#include <string.h>

#include "Base64Converter.h"

const char Base64Converter::alphabet[] =
    {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
        'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
        'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
        'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', '+', '/'};

/**
 * Convert the input string to base 64.
 *
 * @param str - char&, a reference to the charretStrter string to convert to base 64.
 * @ret	char *, a pointer to the base 64 converted string, or NULL. If NON-NULL,
 *		the caller has the the responsiblity of delete[]-ing this.
 */
char* Base64Converter::encode(char& str)
{
    int len = strlen(&str);
    char* retStr = NULL;

    if (len)
    {
        int k1 = 0;
        int l1;

        retStr = new char[(len / 3 + 1) * 4];

        if (retStr)
        {
            char* inStr = &str;

            for (l1 = 0; l1 + 3 <= len;)
            {
                int i = (inStr[l1++] & 0xff) << 16;

                i |= (inStr[l1++] & 0xff) << 8;
                i |= inStr[l1++] & 0xff;

                int l = (i & 0xfc0000) >> 18;

                retStr[k1++] = alphabet[l];
                l = (i & 0x3f000) >> 12;

                retStr[k1++] = alphabet[l];
                l = (i & 0xfc0) >> 6;

                retStr[k1++] = alphabet[l];
                l = i & 0x3f;

                retStr[k1++] = alphabet[l];
            }

            if (len - l1 == 2)
            {
                int j = (inStr[l1] & 0xff) << 16;

                j |= (inStr[l1 + 1] & 0xff) << 8;

                int i1 = (j & 0xfc0000) >> 18;

                retStr[k1++] = alphabet[i1];
                i1 = (j & 0x3f000) >> 12;

                retStr[k1++] = alphabet[i1];
                i1 = (j & 0xfc0) >> 6;

                retStr[k1++] = alphabet[i1];
                retStr[k1++] = '=';
            }
            else if (len - l1 == 1)
            {
                int k = (inStr[l1] & 0xff) << 16;
                int j1 = (k & 0xfc0000) >> 18;

                retStr[k1++] = alphabet[j1];
                j1 = (k & 0x3f000) >> 12;

                retStr[k1++] = alphabet[j1];
                retStr[k1++] = '=';
                retStr[k1++] = '=';
            }
        }
    }
    else
        retStr = NULL;

    return retStr;
}
