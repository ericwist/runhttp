/*
 * ObjectDestructor.h
 *
 * An abstract class used to destruct (delete) another contained object.
 *
 * Author: Eric Wistrand 11/12/2023
 */

#ifndef ObjectDestructorH
#define ObjectDestructorH

/**
 * An abstract class used to destruct (delete) another contained object.
 */
class ObjectDestructor
{
public:
	/**
	 * Destruct the contained object.
	 *
	 * @param value - void *, the value to be deleted.
	 */
	virtual void destructObject(void * value) = 0;
};

#endif