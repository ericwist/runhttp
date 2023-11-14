// Author: Eric Wistrand 11/12/2023
#ifndef SessionManagerH
#define SessionManagerH


#include "WServer.h"
#include "Session.h"
#include "SessionIdGenerator.h"

/**
 * Basic implementation of a Session Manager that provides
 * session persistence and an optional, configurable, maximum
 * number of active sessions allowed.
 */
class WSERVER_API SessionManager
{
public:
	friend class Session;
	//friend class HttpRequest;

	SessionManager();
	~SessionManager();

	/**
	 * Return the default maximum inactive interval (in milliseconds)
	 * for Sessions created by this Manager.
	 * @ret int - the default maximum inactive interval (in milliseconds).
	 */
	int getMaxInactiveInterval();

	/**
	 * Set the default maximum inactive interval (in milliseconds)
	 * for Sessions created by this Manager.
	 *
	 * @param interval The new default value
	 */
	void setMaxInactiveInterval(int interval);

	/**
	 * Return the check interval (in seconds) for this Manager.
	 * @ret int the check interval (in seconds) for this Manager.
	 */
	int getCheckInterval();

	/**
	 * Set the check interval (in seconds) for this Manager.
	 *
	 * @param checkInterval The new check interval
	 */
	void setCheckInterval(int checkInterval);

	/**
	 * Return the maximum number of active Sessions allowed, or -1 for
	 * no limit.
	 * @ret int the maximum number of active Sessions allowed.
	 */
	int getMaxActiveSessions();

	/**
	 * Set the maximum number of actives Sessions allowed, or -1 for
	 * no limit.
	 *
	 * @param max The new maximum number of sessions
	 */
	void setMaxActiveSessions(int max);

    /**
     * Invalidate all sessions that have expired.
	 * @ret bool FALSE to terminate session checking, TRUE to continue.
     */
    bool invalidateSessions();

	/**
	 * Return the active Session, associated with this SessionManager, with the
	 * specified session id. If no Session can be found with the specified id,
	 * and create is TRUE, create a new session using the id, otherwise if return
	 * <code>null</code>.
	 *
	 * @param id const char&, a reference to the session id for the session to be returned.
	 * @param updateAccessTime - bool, if true and a session is found, the sessions last
	 *		access time is updated (for session timeout purposes). Defaults to true.
	 * @ret Session * the HttpSession with this id, no NULL if none could be found.
	 */
	Session * findSession(const char& id, bool updateAccessTime = true);

	/**
	 * Create a new Session object and assign it a unique Session ID.
	 * The Session is NOT added to the internal Session list.
	 *
	 * @ret Session * a newly created Session.
	 */
	Session * createSession();

	/**
	 * Add a Session object to the session list. This should only be done
	 * once the client has joined the session (session.isNew() == FALSE).
	 * Once a session is added to the session list, the SessionManager has
	 * the responsiblity of managing and deleting it.
	 *
	 * @param Session& a reference to the Session to add to the list.
	 */
	void addSession(Session& session);

protected:
	/**
	 * The default maximum inactive interval (in milliseconds) for Sessions created by
	 * this Manager.
	 */
	int maxInactiveInterval; // = 60 * 20 * 1000;

	/**
	 * The interval (in seconds) between checks for expired sessions.
	 */
	int checkInterval;	// = 60;

	/**
	 * The maximum number of active Sessions allowed, or -1 for no limit.
	 */
	int maxActiveSessions;	// = -1;

private:
	/**
	 * The background thread completion semaphore.
	 */
	bool threadDone;	// = false;

	/**
	 * The SessionIdGenerator object used to generate unique session IDs.
	 */
	SessionIdGenerator sessionIdGenerator;

	/**
	 * A list containing all of the Session objects that this SessionManager manages.
	 * This will be NULL if no sessions are present.
	 */
	Session	* sessionList;

	/**
	 * A list containing all of the Session objects that this SessionManager manages
	 * and that have been invalidated, waiting to be freed. Session objects that are
	 * invalid but have their referenceCount greater than 0 are placed here to hold
	 * until their referenceCount becomes 0, at which time they will be freed.
	 * This will be NULL if no invalid sessions are present.
	 */
	Session	* invalidSessionList;

	/**
	 * Platform specific Mutex object.
	 */
	void * sessionMutex;

	/**
	 * The HANDLE to the thread which manages the session expiration.
	 */
	HANDLE threadHandle;

	/**
	 * The time that an invalid session whose referenceCount > 0 will
	 * remain in the invalidSessionList before it is forcefully deleted.
	 */
	/*const*/ int forceInvalidSessionDeleteTime;
};

#endif