/**
 * KeyValue.cpp
 *
 * Author: Eric Wistrand 11/12/2023
 */

#include <windows.h>
#include "KeyValue.h"

#ifdef	_WIN32_WCE	// [

_CRTIMP int     __cdecl _stricmp(const char *, const char *);
#define	stricmp _stricmp

#endif				// ]

/**
 * Sets a KeyValue with the given name and value. If the value
 * has already been set, the new value overwrites the previous one, otherwise
 * a new KeyValue is created. The getValue method can be used to test for
 * the presence of a KeyValue before setting its value.
 *
 * @param keyName - const char&, a reference to the name of the KeyValue.
 * @param value - const char&, a reference to the value of the KeyValue.
 * @param keyValueList - KeyValue **, a pointer to the pointer of the KeyValue
 *		list to set the KeyValue on.
 */
void KeyValueUtils::setKeyValue(const char& keyName, const char& value, KeyValue **keyValueList)
{
	removeKeyValue(keyName,keyValueList);
	addKeyValue(keyName,value,keyValueList);
}

/**
 * Adds a KeyValue with the given name and value to a keyList. This method allows
 * KeyValues to have multiple values.
 *
 * @param keyName - const char&, a reference to the name of the KeyValue.
 * @param inValue - const char&, a reference to the value of the KeyValue.
 * @param keyValueList - KeyValue **, a pointer to the pointer of the KeyValue
 *		list to add the KeyValue to.
 */
void KeyValueUtils::addKeyValue(const char& keyName, const char& inValue, KeyValue **keyValueList)
{
	const char * key = &keyName;
	const char * value = &inValue;

    int keyLen = strlen(key) + 1;
    int valueLen = strlen(value) + 1;
	int	size = sizeof (KeyValue) + keyLen + valueLen;

	KeyValue * keyValue = (KeyValue *)new byte[size];

    keyValue->key = (char *)(keyValue + 1);
    keyValue->value = (char *)keyValue->key + keyLen;

	strcpy(keyValue->key, key);
	strcpy((char *)keyValue->value, value);

    keyValue->next = *keyValueList;
    *keyValueList = keyValue;
}


/**
 * Returns an array of character pointers containing all of the values the given
 * KeyValue contains associate with the given key name, or null if the key does not exist.
 * The last element of this array is NULL, so users can index into the array until NULL is found.
 * If the key has a single value, the array has a length of 2, 1 for the
 * value, 1 for the terminating NULL. If the return array is NON NULL, the caller
 * has the responsibility of deleting it.
 *
 * &param const char keyname - a reference to the character string containing the name of the key whose value is requested.
 * @param const keyValueList keyValue - the KeyValue list to search for the key in.
 * @ret char ** - a pointer to a NULL terminated array of character string pointers containing the parameter_types's
 *                  values, or NULL if the key does not exist. If NON NULL, the caller
 *                  has the responsibility of deleting it, but should not modify or
 *                  delete the values contained in the array.
 */
char ** KeyValueUtils::getValues(const char& keyname,const KeyValue& keyValueList)
{
	int			  cnt;
	KeyValue	* t = (KeyValue *)&keyValueList;

	// first count them.
	for ( cnt = 0; t; t = t->next )
	{
		if ( !_stricmp(&keyname,t->key) )
			cnt++;
	}

	char **params = new char*[cnt+1];

    params[cnt] = NULL;

	t = (KeyValue *)&keyValueList;
	for ( cnt = 0; t; t = t->next )
	{
		if ( !_stricmp(&keyname,t->key) )
			params[cnt++] = (char *)t->value;
	}

    return params;
}

/**
 * Returns the value associated with a keyname contained in a KeyValue list,
 * or NULL if the key value pair does not exist. You should only use this method
 * when you are sure the key has only one value. If the key might have more than
 * one value, use getValues(char& keyname,KeyValue&). If you use this method with a
 * multivalued key, the value returned is equal to the first value in the array
 * returned by getValues.
 *
 * &param keyname - a reference to the string containing the name of the key whose value is requested.
 * @param keyValueList - a reference to the KeyValue list to search for the key in.
 * &ret const char * - a pointer to a character string representing the single value of the key,
 *                 or NULL of the key value does not exist. Callers should NOT
 *                 modify or delete this value.
 */
const char * KeyValueUtils::getValue(const char& keyname, const KeyValue& keyValueList)
{
	char * value = NULL;

	for ( const KeyValue * keyValue = &keyValueList; keyValue && !value; keyValue = keyValue->next )
	{
		if ( !_stricmp(&keyname,keyValue->key) )
			value = (char *)keyValue->value;
	}

	return value;
}

/**
 * Returns an array of character pointers containing the unique (case insensitive)
 * names of the keys contained in a KeyValue list. The array will NOT contain duplicate
 * (case insensitive) key names.
 * The last element of this array is NULL, so users can index into the array until NULL
 * is found. If the KeyValue list contains a single key, the array has a length of 2, 1 for the
 * key, 1 for the terminating NULL. The caller has the responsibility of deleting the returnArray,
 * but should not modify or delete the values contained in the array.
 *
 * @param keyValueList - a reference to the KeyValue list to search for the key in.
 * @ret a pointer to a NULL terminated array of character pointers containing the
 * unique names of a key. The array will NOT contain duplicate (case insensitive)
 * key names. The caller has the responsibility of deleting this returnArray, but should
 * not modify or delete the values contained in the array.
 */
char ** KeyValueUtils::getKeyNames(const KeyValue& keyValueList)
{
	int			  size;
	KeyValue	* t = (KeyValue *)&keyValueList;


	// first count all unique (case insensitive) names.
	for ( size = 0; t; t = t->next )
	{
		bool	found = false;

		KeyValue	* t2 = (KeyValue *)&keyValueList;

		for ( found = false; !found && t2 && (t2 != t); t2 = t2->next )
		{
			found = !_stricmp(t->key,t2->key);
		}

		if ( !found )
			size++;
	}



	char **names = new char*[size+1];


	for ( int i = size; i >= 0; i-- )
		names[i] = NULL;

	names[size] = NULL;

	t = (KeyValue *)&keyValueList;

	for ( int i = 0; t && (i < size); t = t->next )
	{
		bool	found = false;

		KeyValue	* t2 = (KeyValue *)&keyValueList;

		for ( found = false; !found && t2 && (t2 != t); t2 = t2->next )
		{
			found = !_stricmp(t->key,t2->key);
		}

		if ( !found )
			names[i++] = t->key;
	}

    return names;
}

/**
 * Removes the named key if it exists.
 *
 * @param keyName - const char&, a reference to the key name.
 * @param keyValueList - KeyValue **, a pointer to the pointer of the KeyValue
 *		list to remove the KeyValue from.
 *
 * @ret bool - TRUE/FALSE indicating whether the named key
 * had already been set.
 */
bool KeyValueUtils::removeKeyValue(const char& keyName, KeyValue ** keyValueList)
{
	bool	found = false;

	if ( keyValueList )
	{
		KeyValue	* keyValue = *keyValueList, * prev = NULL;

		for (; keyValue && !found; prev = keyValue, keyValue = keyValue->next )
		{
			if ( !_stricmp(&keyName,keyValue->key) )
			{
				if ( prev )
					prev->next = keyValue->next;
				else
					*keyValueList = keyValue->next;

				delete[] keyValue;
				found = true;
			}
		}
	}

	return found;
}

/**
 * Delete all KeyValues, from the supplied KeyValue and set the list pointer to NULL.
 *
 * @param keyValueList - KeyValue **, a pointer to the pointer of the KeyValue
 *		list to delete the KeyValues from.
 */
void KeyValueUtils::deleteKeyValues(KeyValue ** keyValueList)
{
    while ( *keyValueList )
    {
		KeyValue * keyValue = (*keyValueList)->next;
		//D(cout << "deleting header with key= '" << headers->key << "' value= '" << headers->value << "'" << endl)

        delete[] *keyValueList;
        *keyValueList = keyValue;
    }
}
