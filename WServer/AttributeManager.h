/*
 * AttributeManager.h
 *
 * Basic implementation of the an Attribute manager that allows objects to be bound
 * as attributes which can be accessed, removed and deleted, unbound, and automatically
 * deleted when the AttributeManager gets deleted.
 *
 *  Author: Eric Wistrand 11/12/2023
 */

#ifndef AttributeManagerH
#define AttributeManagerH

#include "KeyValue.h"
#include "ObjectDestructor.h"
#include "WServer.h"

///**
// * DeleteableKeyValue struct used to store and reference key named values
// * which also has a pointer to a delete function.
// */
/*
typedef struct DeletableKeyValue : KeyValue
{
    void (*deleteFunc)(void *);

} DeletableKeyValue;
*/

/**
 * DeleteableKeyValue struct used to store and reference key named values
 * which also has an optional pointer to an ObjectDestructor object, or an
 * optional pointer to a delete function used to destruct the value.
 */
typedef struct DeletableKeyValue : KeyValue
{
    ObjectDestructor* objectDestructor;
    void (*deleteFunc)(void*);

} DeletableKeyValue;

/**
 * Basic implementation of the an Attribute manager that allows objects to be bound
 * as attributes which can be accessed, removed and deleted, unbound, and automatically
 * deleted when the AttributeManager gets deleted.
 */
class WSERVER_API AttributeManager
{
public:
    /**
     * Create an AttributeManager object.
     */
    AttributeManager();

    /**
     * Delete all attributes that are bound to this AttributeManager.
     */
    ~AttributeManager();

    /**
     * Binds an object to this AttributeManager, using the name specified. If an object of
     * the same name is already bound to the AttributeManager, the object is deleted and
     * replaced.
     *
     * @param name - const char&, a reference to the attribute name.
     * @param value - const void *, a pointer to the object to bind to the AttributeManager.
     *		This object must be deletable with the C++ delete operator.
     */
    void setAttribute(const char& name, const void* value);

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
    void setAttribute(const char& name, const void* value, ObjectDestructor* objectDestructor);

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
    void setAttribute(const char& name, const void* value, void (*deleteFunc)(void* value));

    /**
     * Returns the named attribute value from the AttributeManager if it exists.
     *
     *
     * @param name - const char&, a reference to the attribute name.
     * @ret const void *, the object with the specified name bound to the AttributeManager..
     */
    const void* getAttribute(const char& name);

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
    char** getAttributeNames();

    /**
     * Removes and deletes all bound attributes and their values from the
     * AttributeManager.
     */
    void removeAllAttributes();

    /**
     * Removes and deletes the named attribute and its value from the
     * AttributeManager if it exists.
     *
     *
     * @param name - const char&, a reference to the attribute name.
     * @ret bool - TRUE/FALSE indicating whether the named response attribute
     * existed.
     */
    bool removeAttribute(const char& name);

    /**
     * Unbinds and returns the named attribute value from the AttributeManager if it
     * exists. The AttributeManager no longer has the responsibility of deleting the object
     *
     * @param name - const char&, a reference to the attribute name.
     * @ret void *, the object with the specified name bound to the AttributeManager, or NULL.
     */
    void* unbindAttribute(const char& name);

protected:
    /**
     * The KeyValue list of attributes
     */
    KeyValue* attributeList;

    /**
     * Platform specific Mutex object.
     */
    void* mutex;
};

#endif