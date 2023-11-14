/*
 * Session.h
 *
 * Basic implementation of the a Session that provides
 * session persistence.
 * 
 * Author: Eric Wistrand 11/12/2023
 */

#ifndef SessionH
#define SessionH


#include "WServer.h"
//#include "SessionManager.h"
//#include "KeyValue.h"
#include "AttributeManager.h"

class SessionManager;

/**
 * Basic implementation of the a Session that provides
 * session persistence.
 */
class WSERVER_API Session : public AttributeManager
{
public:
	friend class SessionManager;
	friend class HttpRequest;
//	friend class HttpResponse;
	friend class Request;
	friend class Response;

	/**
	 * Create a Session object which will be managed by the supplied SessionManager.
	 * @param sessionManager SessionManager&, a reference to the SessionManager that
	 *	will manage this session.
	 */
	Session(SessionManager& sessionManager);
	~Session();

	/**
	 * Return the time when this session was created, in milliseconds since
	 * midnight, January 1, 1970 GMT.
	 *
	 * @ret long the creation time.
	 */
	long getCreationTime();

	/**
	 * Return the last time the client sent a request associated with this
	 * session, as the number of milliseconds since midnight, January 1, 1970
	 * GMT.  Actions that your application takes, such as getting or setting
	 * a value associated with the session, do not affect the access time.
	 */
	long getLastAccessedTime();

	/**
	 * Invalidates this session and free any objects bound to it.
	 */
	void invalidate();

	/**
	 * Return <code>TRUE</code> if the client does not yet know about the
	 * session, or if the client chooses not to join the session.  For
	 * example, if the server used only cookie-based sessions, and the client
	 * has disabled the use of cookies, then a session would be new on each
	 * request.
	 */
	bool isNew();

	/**
	 * Return the <code>valid</code> flag for this session.
	 */
	bool isValid();

	/**
	 * Get the session id that uniquely identifes this session. Callers should not
	 * modify or delete this constant value.
	 * @ret char& a reference to the session ID string.
	 */
	const char& getSessionID();

	/**
	 * Returns whether the session ID was found from a cookie (as opposed to form within the URL).
	 * @ret bool, TRUE if the session ID was found as a cookie, false otherwise.
	 */
	bool isIDFromCookie();

	/**
	 * Set the id that uniquely identifes this session. This function will
	 * make a copy of the supplied string. If the caller has allocated the supplied string,
	 * it has the responsibility of deleting it (since the code makes its own copy).
	 * @param id const char&, a reference to the session id to uniquely identify this session.
	 */
	void setSessionID(const char& id);

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
	int addReference();

	//void * operator new( size_t stAllocateBlock, void * args );
protected:
	Session		* next;

	/**
	 * List of KeyValue attributes that persist accross sessions.
	 */
	//KeyValue	* attributeList;

	/**
	 * The SessionManager that will manage this session.
	 */
	SessionManager& sessionManager;

	/**
	 * Update the accessed time information for this session.
	 */
	void access();

	/**
	 * Sets whether the session ID was found from a cookie (as opposed to form within the URL).
	 * @param idFromCookie bool, TRUE if the session ID was found as a cookie, false otherwise.
	 */
	void setIDFromCookie(bool idFromCookie);

#if	0	// [
	/**
	 * Set the id that uniquely identifes this session. This function will
	 * make a copy of the supplied string. If the caller has allocated the supplied string,
	 * it has the responsibility of deleting it (since the code makes its own copy).
	 * @param id const char&, a reference to the session id to uniquely identify this session.
	 */
	void setSessionID(const char& id);

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
	int addReference();
#endif	// ]

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
	int removeReference();

private:
	/**
	 * The string that serves as an ID that uniquely identifies this session.
	 */
	char * id;

	/**
	 * <code>TRUE</code> if the client does not yet know about the
	 * session, or if the client chooses not to join the session.  For
	 * example, if the server used only cookie-based sessions, and the client
	 * has disabled the use of cookies, then a session would be new on each
	 * request.
	 */
	bool sessionIsNew;

	/**
	 * The valid flag for this session.
	 */
	bool valid;

	/**
	 * The time this session was created, in milliseconds since midnight,
	 * January 1, 1970 GMT.
	 */
	long creationTime;

	/**
	 * The last accessed time for this Session.
	 */
	long lastAccessedTime;

	/**
	 * The current accessed time for this session.
	 */
	long thisAccessedTime;

	/**
	 * The reference count of objects that reference this Session object
	 * (like HttpRequest & HttpResponse). Objects which reference this Session
	 * object need to call addReference(), and releaseReference() when finished.
	 * This is VERY important, as the SessionManager will not free any invalidated
	 * Session objects with a reference count > 0.
	 */
	int referenceCount;

	// TRUE if the session ID was found as a cookie, false otherwise.
	bool idFromCookie;
};

#endif