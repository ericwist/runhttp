/**
 * KeyValue.h
 * Author: Eric Wistrand 11/12/2023
 */

#ifndef _KeyValue_H_
#define _KeyValue_H_

#include "WServer.h"

/**
 * KeyValue struct used to store and reference key named values.
 */
typedef struct KeyValue
{
    struct KeyValue* next;
    char* key;
    void* value;

} KeyValue;

/**
 * A class for managing KeyValues
 */
class WSERVER_API KeyValueUtils
{
    friend class HttpRequestProcessor;

public:
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
    static void setKeyValue(const char& keyName, const char& value, KeyValue** keyValueList);

    /**
     * Adds a KeyValue with the given name and value to a keyList. This method allows
     * KeyValues to have multiple values.
     *
     * @param keyName - const char&, a reference to the name of the KeyValue.
     * @param inValue - const char&, a reference to the value of the KeyValue.
     * @param keyValueList - KeyValue **, a pointer to the pointer of the KeyValue
     *		list to add the KeyValue to.
     */
    static void addKeyValue(const char& keyName, const char& inValue, KeyValue** keyValueList);

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
    char** getValues(const char& keyname, const KeyValue& keyValueList);

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
    static const char* getValue(const char& keyname, const KeyValue& keyValueList);

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
    static char** getKeyNames(const KeyValue& keyValueList);

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
    static bool removeKeyValue(const char& keyname, KeyValue** keyValueList);

    /**
     * Delete all KeyValues, from the supplied KeyValue and set the list pointer to NULL.
     *
     * @param keyValueList - KeyValue **, a pointer to the pointer of the KeyValue
     *		list to delete the KeyValues from.
     */
    static void deleteKeyValues(KeyValue** keyValueList);
};

#endif