/*
 * HeaderManager.cpp
 *
 * A class which sublasses KeyValueUtils to manage character based headers.
 *
 * Author: Eric Wistrand 11/12/2023
 */

#include "HeaderManager.h"

#ifdef _WIN32_WCE    // [
#include <windows.h> // For NULL
#endif               // ]

HeaderManager::HeaderManager()
: headers(NULL)
{
}

HeaderManager::~HeaderManager()
{
    deleteHeaders();
}

/**
 * Sets a header with the given name and value. If the header
 * has already been set, the new value overwrites the previous one, otherwise
 * a new header is created. The getHeader method can be used to test for
 * the presence of a header before setting its value.
 *
 * @param name - const char&, a reference to the name of the header.
 * @param value - const char&, a reference to the value of the header.
 */
void HeaderManager::setHeader(const char& name, const char& value)
{
    setKeyValue(name, value, &headers);
}

/**
 * Adds a header with the given name and value. This method allows
 * headers to have multiple values.
 *
 * @param name - const char&, a reference to the name of the header.
 * @param value - const char&, a reference to the value of the header.
 */
void HeaderManager::addHeader(const char& name, const char& value)
{
    addKeyValue(name, value, &headers);
}

/**
 * Returns the value of a header as a character pointer or NULL if the header does not exist.
 *
 * @param name - a reference to a string specifying the name of the header
 * @ret const char * - a constant pointer to a character string representing the value of the header,
 *                 or NULL of the named header does not exist. Callers should NOT
 *                 modify or delete this value.
 */
const char* HeaderManager::getHeader(const char& name)
{
    const char* value;

    if (headers)
        value = getValue(name, *headers);
    else
        value = NULL;

    return value;
}

/**
 * Returns an array of character pointers containing all of the values the given
 * named header has, or null if the header does not exist. The last element
 * of this array is NULL, so users can index into the array until NULL is found.
 * If the header has a single value, the array has a length of 2, 1 for the
 * value, 1 for the terminating NULL. If the return array is NON NULL, the caller
 * has the responsibility of deleting it.
 *
 * &param const char& name - a reference to the character string containing the name of the header whose value is requested.
 * @ret char **  - a pointer to a NULL terminated array of character pointers containing the header's
 *                  values, or NULL if the header does not exist. If NON NULL, the caller
 *                  has the responsibility of deleting it, but should not modify or
 *                  delete the values contained in the array.
 */
char** HeaderManager::getHeaderValues(const char& name)
{
    char** values;

    if (headers)
        values = getValues(name, *headers);
    else
        values = NULL;

    return values;
}

/**
 * Returns an array of character pointers containing the names of the headers
 * contained in this HeaderManager. If the there no headers, the method returns NULL.
 * The last element of this array is NULL, so users can index into the array until NULL
 * is found. If the header has a single key - value pair, the array has a length of 2, 1 for the
 * header name, 1 for the terminating NULL. If the return array is NON NULL, the caller
 * has the responsibility of deleting it.
 *
 * @ret char ** a pointer to a NULL terminated array of character pointers containing the
 * names of the headers; or NULL if the there are no headers.
 */
char** HeaderManager::getHeaderNames()
{
    char** names;

    if (headers)
        names = getKeyNames(*headers);
    else
        names = NULL;

    return names;
}

/**
 * Removes the named header if it exists.
 *
 * @param name - const char&, a reference to the header name.
 *
 * @ret bool - TRUE/FALSE indicating whether the named header
 * had already been set.
 *
 * @param name - string, a reference to the name of the header.
 */
bool HeaderManager::removeHeader(const char& name)
{
    return removeKeyValue(name, &headers);
}

/**
 * Delete all headers, and set the headers poiner to NULL.
 */
void HeaderManager::deleteHeaders()
{
    deleteKeyValues(&headers);
}