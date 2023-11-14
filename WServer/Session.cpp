/*
 * Session.cpp
 *
 * Basic implementation of the a Session that provides
 * session persistence.
 * Author: Eric Wistrand 11/12/2023
 */

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#ifndef	_WIN32_WCE	// [
#include <iostream>
#endif				// ]

#include "Session.h"
#include "Platform.h"
#include "SessionManager.h"

#ifndef	_WIN32_WCE	// [
using namespace std;
#endif				// ]

/**
 * Create a Session object which will be managed by the supplied SessionManager.
 * @param sessionManager SessionManager&, a reference to the SessionManager that
 *	will manage this session.
 */
Session::Session(SessionManager& sessionManager) : sessionManager(sessionManager),
	id(NULL), next(NULL), sessionIsNew(TRUE), valid(TRUE), referenceCount(0),
	idFromCookie(FALSE)
{
	creationTime = lastAccessedTime = thisAccessedTime = Platform::getCurrentMillis();
}

Session::~Session()
{
	if ( id )
		delete[] id;
}

/*
void * Session::operator new( size_t stAllocateBlock, void * args )
{
	va_list list;

	va_start(list, args);

	void * ptr =  ::new void[stAllocateBlock,args,list];

	va_end(list);

	return ptr;
}
*/
/**
 * Set the id that uniquely identifes this session. This function will
 * make a copy of the supplied string. If the caller has allocated the supplied string,
 * it has the responsibility of deleting it (since the code makes its own copy).
 * @param id const char&, a reference to the session id to uniquely identify this session.
 */
void Session::setSessionID(const char& id)
{
	if ( this->id )
		delete[] this->id;

	this->id = new char[strlen(&id)+1];
	strcpy(this->id,&id);
}

/**
 * Get the session id that uniquely identifes this session. Callers should not
 * modify or delete this constant value.
 * @ret char& a reference to the session ID string.
 */
const char& Session::getSessionID()
{
	return *id;
}

/**
 * Return the time when this session was created, in milliseconds since
 * midnight, January 1, 1970 GMT.
 *
 * @ret long the creation time.
 */
long Session::getCreationTime()
{
	return creationTime;
}

/**
 * Invalidates this session and free any objects bound to it.
 */
void Session::invalidate()
{
	Platform::lockMutex(sessionManager.sessionMutex);

	valid = FALSE;
	removeAllAttributes();
	setSessionID(*"");

	Platform::unlockMutex(sessionManager.sessionMutex);
}


/**
 * Return the <code>valid</code> flag for this session.
 */
bool Session::isValid()
{
	//Platform::lockMutex(sessionManager.sessionMutex);
	return valid;
	//Platform::unlockMutex(sessionManager.sessionMutex);
}

/**
 * Return <code>TRUE</code> if the client does not yet know about the
 * session, or if the client chooses not to join the session.  For
 * example, if the server used only cookie-based sessions, and the client
 * has disabled the use of cookies, then a session would be new on each
 * request.
 *
 */
bool Session::isNew()
{
	return sessionIsNew;
}

/**
 * Update the accessed time information for this session.
 */
void Session::access()
{
	Platform::lockMutex(sessionManager.sessionMutex);
	lastAccessedTime = thisAccessedTime;
	thisAccessedTime = Platform::getCurrentMillis();
	sessionIsNew=false;
	Platform::unlockMutex(sessionManager.sessionMutex);
}

/**
 * Return the last time the client sent a request associated with this
 * session, as the number of milliseconds since midnight, January 1, 1970
 * GMT.  Actions that your application takes, such as getting or setting
 * a value associated with the session, do not affect the access time.
 */
long Session::getLastAccessedTime()
{
	return lastAccessedTime;
}

/**
 * Returns whether the session ID was found from a cookie (as opposed to form within the URL).
 * @ret bool, TRUE if the session ID was found as a cookie, false otherwise.
 */
bool Session::isIDFromCookie()
{
	return idFromCookie;
}

/**
 * Sets whether the session ID was found from a cookie (as opposed to form within the URL).
 * @param idFromCookie bool, TRUE if the session ID was found as a cookie, false otherwise.
 */
void Session::setIDFromCookie(bool idFromCookie)
{
	this->idFromCookie = idFromCookie;
}


/**
 * Increase the reference count by 1. This is the reference count of objects that
 * reference this Session object (like HttpRequest & HttpResponse). Objects which
 * reference this Session object need to call addReference(), and releaseReference()
 * when finished. This is VERY important, as the SessionManager will not free any
 * invalidated Session objects with a reference count > 0.
 *
 * @ret int the new reference count. This value is meant to be used for diagnostic
 *	and testing purposes only.
 */
int Session::addReference()
{
	return ++referenceCount;
}

/**
 * Decrease the reference count by 1. This is the reference count of objects that
 * reference this Session object (like HttpRequest & HttpResponse). Objects which
 * reference this Session object need to call addReference(), and releaseReference()
 * when finished. This is VERY important, as the SessionManager will not free any
 * invalidated Session objects with a reference count > 0.
 *
 * @ret int the new reference count. This value is meant to be used for diagnostic
 *	and testing purposes only.
 */
int Session::removeReference()
{
	return --referenceCount;
}
