/*
 * AsciiParameterManager.h
 *
 * A class which sublasses KeyValueUtils to manage character based paramaters.
 *
 * author: Eric Wistrand 11 / 12 / 2023
 */

#ifndef	_AsciiParameterManager_H
#define _AsciiParameterManager_H

#include "KeyValue.h"

class WSERVER_API AsciiParameterManager : protected KeyValueUtils
{
public:

	AsciiParameterManager();
    ~AsciiParameterManager();


	/**
	 * Sets a parameter with the given name and value. If the parameter
	 * has already been set, the new value overwrites the previous one, otherwise
	 * a new parameter is created. The getParameter method can be used to test for
	 * the presence of a parameter before setting its value.
	 *
	 * @param const char& name - a reference to a character string specifying the name of the parameter.
	 * @param const char& value - a reference to a character string specifying the value of the named parameter.
	 */
	void setParameter(const char& name, const char& value);

	/**
	 * Adds a parameter with the given name and value. This method allows
	 * parameters to have multiple values.
	 *
	 * @param const char& name - a reference to a character string specifying the name of the parameter.
	 * @param const char& value - a reference to a character string specifying the value of the named parameter.
	 */
	void addParameter(const char& name, const char& value);

	/**
	 * Returns the value of a parameter as a char pointer, or NULL if the parameter
	 * does not exist. You should only use this method when you are sure the parameter
	 * has only one value. If the parameter might have more than one value, use
	 * getParameterValues(string). If you use this method with a multivalued parameter,
	 * the value returned is equal to the first value in the array returned by
	 * getParameterValues.
	 *
	 * @param const char& name - a reference to a character string specifying the name of the parameter
	 * &ret const char * - a pointer to a character string representing the single value of the parameter,
	 *                 or NULL of the named parameter does not exist. Callers should NOT
	 *                 modify or delete this value.
	 */
	const char * getParameter(const char& name);

	/**
	 * Returns an array of character pointers containing all of the values the given
	 * named parameter has, or null if the parameter does not exist. The last element
	 * of this array is NULL, so users can index into the array until NULL is found.
	 * If the parameter has a single value, the array has a length of 2, 1 for the
	 * value, 1 for the terminating NULL. If the return array is NON NULL, the caller
	 * has the responsibility of deleting it.
	 *
	 * &param const char& name - a reference to the character string containing the name of the parameter whose value is requested.
	 * @ret char **  - a pointer to a NULL terminated array of character pointers containing the parameter's
	 *                  values, or NULL if the parameter does not exist. If NON NULL, the caller
	 *                  has the responsibility of deleting it, but should not modify or
	 *                  delete the values contained in the array.
	 */
	char ** getParameterValues(const char& name);

	/**
	 * Returns an array of character pointers containing the names of the parameters
	 * contained in this AsciiParameterManager. If there no parameters, the method returns NULL.
	 * The last element of this array is NULL, so users can index into the array until NULL
	 * is found. If the parameter has a single value, the array has a length of 2, 1 for the
	 * value, 1 for the terminating NULL. If the return array is NON NULL, the caller
	 * has the responsibility of deleting it.
	 *
	 * @ret char ** a pointer to a NULL terminated array of string object pointers containing the
	 * names of the parameters; or NULL if there are no parameters.
	 */
	char ** getParameterNames();

	/**
	 * Removes the named parameter if it exists.
	 *
	 * @param name - const char&, a reference to the parameter name.
	 *
	 * @ret bool - TRUE/FALSE indicating whether the named parameter
	 * had already been set.
	 */
	bool removeParameter(const char& name);

	/**
	 * Delete all parameters, and set the parameters pointer to NULL.
	 */
	void deleteParameters();

protected:
	KeyValue		* parameters;

};

#endif
