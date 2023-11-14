/*
 * AttributeManager.cpp
 *
 * Basic implementation of the an Attribute manager that allows objects to be bound
 * as attributes which can be accessed, removed and deleted, unbound, and automatically
 * deleted when the AttributeManager gets deleted.
 *
 * Author: Eric Wistrand 11/12/2023
 */



#ifndef	_WIN32_WCE	// [

#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <iostream>

#else				// ]

#include <windows.h>

_CRTIMP int     __cdecl _stricmp(const char *, const char *);
#define	stricmp _stricmp
#endif				// ]

#include "AttributeManager.h"
#include "Platform.h"


/**
 * Create an AttributeManager object.
 */
AttributeManager::AttributeManager() : attributeList(NULL)
{
	mutex = Platform::makeMutex();
}

/**
 * Delete all attributes that are bound to this AttributeManager.
 */
AttributeManager::~AttributeManager()
{
	removeAllAttributes();
	Platform::closeMutex(mutex);
}

/**
 * Binds an object to this AttributeManager, using the name specified. If an object of
 * the same name is already bound to the AttributeManager, the object is deleted and
 * replaced. 
 *
 * @param name - const char&, a reference to the attribute name.
 * @param value - const void *, a pointer to the object to bind to the AttributeManager.
 */
void AttributeManager::setAttribute(const char& name, const void * value)
{
	setAttribute(name, value, (ObjectDestructor *)NULL);
}

/**
 * Binds an object to this AttributeManager, using the name specified. If an object of
 * the same name is already bound to the AttributeManager, the object is deleted and
 * replaced. 
 *
 * @param name - const char&, a reference to the attribute name.
 * @param value - const void *, a pointer to the object to bind to the AttributeManager.
 * @param objectDestructor - ObjectDestructor *, a pointer to a ObjectDestructor
 *		which will be used to delete the value in removeAttribute() and in ~AttributeManager()
 *		if and attributes are still on the list. If this ObjectDestructor is NULL, the standard
 *		C++ delete operator will be used, otherwise ObjectDestructor::destructObject(void * value)
 *		will be	called. ObjectDestructor subclasses should delete the value within destructObject()
 *		and could optionally delete itself. This ObjectDestructor could really be a pointer
 *		to the value if the value is a subclass of ObjectDestructor.
 */
void AttributeManager::setAttribute(const char& name, const void * value, ObjectDestructor * objectDestructor )
{
	const char	* key = &name;
	int			  size = sizeof (DeletableKeyValue) + strlen(key) + 1;

	// Remove the attribute if it already exists.
	removeAttribute(name);

	DeletableKeyValue * keyValue = (DeletableKeyValue *)new byte[size];

    keyValue->key = (char *)(keyValue + 1);
    keyValue->value = (void *)value;
	keyValue->objectDestructor = objectDestructor;
	keyValue->deleteFunc = NULL;

	//strcpy(keyValue->key, key);
	strcpy(keyValue->key, key);

	Platform::lockMutex(mutex);
    keyValue->next = attributeList;
    attributeList = keyValue;
	Platform::unlockMutex(mutex);
	D(std::cout << "******** AttributeManager::setAttribute() keyValue->value= " << keyValue->value << " value= " << value << std::endl;)
}

/**
 * Binds an object to this AttributeManager, using the name specified. If an object of
 * the same name is already bound to the AttributeManager, the object is deleted and
 * replaced. 
 *
 * @param name - const char&, a reference to the attribute name.
 * @param value - const void *, a pointer to the object to bind to the AttributeManager.
 * @param deleteFunc - void (*deleteFunc)(void * value), a pointer to a callback function
 *		which will be used to delete the value in removeAttribute() and in ~AttributeManager()
 *		if and attributes are still on the list. If this deleteFunc is NULL, the standard
 *		C++ delete operator will be used.
 */
void AttributeManager::setAttribute(const char& name, const void * value, void (*deleteFunc)(void *) )
{
	const char	* key = &name;
	int			  size = sizeof (DeletableKeyValue) + strlen(key) + 1;

	// Remove the attribute if it already exists.
	removeAttribute(name);

	DeletableKeyValue * keyValue = (DeletableKeyValue *)new byte[size];

    keyValue->key = (char *)(keyValue + 1);
    keyValue->value = (void *)value;
	keyValue->deleteFunc = deleteFunc;
	keyValue->objectDestructor = NULL;

	//strcpy(keyValue->key, key);
	strcpy(keyValue->key, key);

	Platform::lockMutex(mutex);
    keyValue->next = attributeList;
    attributeList = keyValue;
	Platform::unlockMutex(mutex);
	D(std::cout << "******** AttributeManager::setAttribute() keyValue->value= " << keyValue->value << " value= " << value << std::endl;)
}


/**
 * Returns the named attribute value from the AttributeManager if it exists.
 * 
 *
 * @param name - const char&, a reference to the attribute name.
 * @ret const void *, the object with the specified name bound to the AttributeManager..
 */
const void * AttributeManager::getAttribute(const char& name)
{
	void * value = NULL;

	Platform::lockMutex(mutex);
	for ( const KeyValue * keyValue = attributeList; keyValue && !value; keyValue = keyValue->next )
	{
		if ( !_stricmp(&name,keyValue->key) )
		{
			value = keyValue->value;
			D(std::cout << "******** AttributeManager::getAttribute() matched name: '" << &name << "' to key '" << keyValue->key << "'" << std::endl;)
		}
	}
	Platform::unlockMutex(mutex);

	D(std::cout << "******** AttributeManager::getAttribute() returning value= " << (value ? value : "Null!!" ) << std::endl;)
	return value;
}

/**
 * Returns an array of character pointers containing the names of the attributes
 * bound to this AttributeManager.
 * The last element of this array is NULL, so users can index into the array until NULL
 * is found. If the AttributeManager has a bound single attribute, the array has a length of 2, 1 for the
 * attribute name, 1 for the terminating NULL. The caller
 * has the responsibility of deleting the return array.
 *
 * @ret char ** a pointer to a NULL terminated array of character pointers containing the
 *	names of the bound attributes.The caller has the responsibility of deleting this return array.
 */
char ** AttributeManager::getAttributeNames()
{
	int			  size;

	Platform::lockMutex(mutex);
	KeyValue	* t = attributeList;

	// first count them.
	for ( size = 0; t; t = t->next, size++ );


	char **names = new char*[size+1];

    names[size] = NULL;
	t = attributeList;

	for ( size = 0; t; t = t->next, size++ )
    {
		names[size] = t->key;
	}
	Platform::unlockMutex(mutex);

    return names;
}

/**
 * Removes and deletes all bound attributes and their values from the
 * AttributeManager.
 */
void AttributeManager::removeAllAttributes()
{
	Platform::lockMutex(mutex);

	DeletableKeyValue * t = (DeletableKeyValue *)attributeList;
	while ( t )
	{
		attributeList = t->next;
		if ( t->value )
		{
			D(std::cout << "******** AttributeManager::~AttributeManager() deleting t->value= " << t->value << std::endl;)

			if ( t->objectDestructor )
				t->objectDestructor->destructObject(t->value);
			else if ( t->deleteFunc )
				t->deleteFunc(t->value);
			else
				delete t->value;
		}
		delete[] t;
		t = (DeletableKeyValue *)attributeList;
	}

	Platform::unlockMutex(mutex);
}

/**
 * Removes and deletes the named attribute and its value from the
 * AttributeManager if it exists.
 * 
 *
 * @param name - const char&, a reference to the attribute name.
 * @ret bool - TRUE/FALSE indicating whether the named response attribute
 * existed.
 */
bool AttributeManager::removeAttribute(const char& name)
{
	bool	  found;


	found = FALSE;
	Platform::lockMutex(mutex);
	for (DeletableKeyValue * keyValue = (DeletableKeyValue *)attributeList, * prev = NULL; keyValue; prev = keyValue, keyValue = (DeletableKeyValue *)keyValue->next)
	{
		D(std::cout << "******** AttributeManager::removeAttribute() keyValue= " << keyValue << std::endl;)
		if ( !_stricmp(&name,keyValue->key) )
		{
			if ( prev )
				prev->next = keyValue->next;
			else
				attributeList = keyValue->next;

			D(std::cout << "**************** AttributeManager::removeAttribute() deleting keyValue= " << keyValue << " keyValue->value= " << (keyValue->value ? keyValue->value : "Null!!") << std::endl;)
			if ( keyValue->value )
			{
				found = TRUE;

				if ( keyValue->objectDestructor )
					keyValue->objectDestructor->destructObject(keyValue->value);
				else if ( keyValue->deleteFunc )
					keyValue->deleteFunc(keyValue->value);
				else
					delete keyValue->value;
			}

			delete[] keyValue;

			break;
		}
	}
	Platform::unlockMutex(mutex);

	return found;
}

/**
 * Unbinds and returns the named attribute value from the AttributeManager if it
 * exists. The AttributeManager no longer has the responsibility of deleting the object
 *
 * @param name - const char&, a reference to the attribute name.
 * @ret void *, the object with the specified name bound to the AttributeManager, or NULL.
 */
void * AttributeManager::unbindAttribute(const char& name)
{
	void * value = NULL;

	Platform::lockMutex(mutex);
	for (KeyValue * keyValue = attributeList, * prev = NULL; keyValue; prev = keyValue, keyValue = keyValue->next)
	{
		D(std::cout << "******** AttributeManager::unbindAttribute() keyValue= " << keyValue << std::endl;)
		if ( !_stricmp(&name,keyValue->key) )
		{
			if ( prev )
				prev->next = keyValue->next;
			else
				attributeList = keyValue->next;

			D(std::cout << "**************** AttributeManager::unbindAttribute() deleting keyValue= " << keyValue << " keyValue->value= " << (keyValue->value ? keyValue->value : "Null!!") << std::endl;)
			if ( keyValue->value )
			{
				value = keyValue->value;
			}

			delete[] keyValue;

			break;
		}
	}
	Platform::unlockMutex(mutex);

	return value;
}
