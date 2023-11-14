/**
 * Basic implementation of a Session Manager that provides
 * session persistence and an optional, configurable, maximum
 * number of active sessions allowed.
 * Author: Eric Wistrand 11/12/2023
 */

#include <windows.h>

#ifndef	_WIN32_WCE	// [
#include <iostream>

using namespace std;
#endif				// ]

#include "SessionManager.h"
#include "SessionIdGenerator.h"
#include "Platform.h"

DWORD WINAPI sessionHandler(void * sm);

#define	forceInvalidSessionDeleteTimeCoefficient	5

SessionManager::SessionManager() : maxInactiveInterval( 60 * 20 * 1000), checkInterval(60),
	maxActiveSessions(-1), threadDone(FALSE), sessionList(NULL), invalidSessionList(NULL),
	forceInvalidSessionDeleteTime( /*maxInactiveInterval + (forceInvalidSessionDeleteTimeCoefficient*60)*/ 60 * 60 * 1000)
{
	sessionMutex = Platform::makeMutex();
	//D(cout << "SessionManager::SessionManager() ENTERED " << endl;)

	DWORD nThreadID;
    threadHandle = CreateThread(0, 0, sessionHandler, (void*)this, 0, &nThreadID);
}

SessionManager::~SessionManager()
{
	threadDone = TRUE;

	//D(cout << "SessionManager::~SessionManager() ENTERED " << endl;)

	Platform::lockMutex(sessionMutex);

	TerminateThread(threadHandle,0);

	Session * session = sessionList;
	while ( session )
	{
		sessionList = session->next;
		delete session;
		session = sessionList;
	}

	session = invalidSessionList;
	while ( session )
	{
		sessionList = session->next;
		delete session;
		session = sessionList;
	}

	Platform::unlockMutex(sessionMutex);
	Platform::closeMutex(sessionMutex);
}

/**
 * Return the default maximum inactive interval (in milliseconds)
 * for Sessions created by this Manager.
 * @ret int - the default maximum inactive interval (in milliseconds).
 */
int SessionManager::getMaxInactiveInterval()
{
	return maxInactiveInterval;
}

/**
 * Set the default maximum inactive interval (in milliseconds)
 * for Sessions created by this Manager.
 *
 * @param interval The new default value
 */
void SessionManager::setMaxInactiveInterval(int interval)
{
	maxInactiveInterval = interval;
	//forceInvalidSessionDeleteTime = maxInactiveInterval + (forceInvalidSessionDeleteTimeCoefficient*60);
}

/**
 * Return the check interval (in seconds) for this Manager.
 * @ret int the check interval (in seconds) for this Manager.
 */
int SessionManager::getCheckInterval()
{
	return checkInterval;
}

/**
 * Set the check interval (in seconds) for this Manager.
 *
 * @param checkInterval The new check interval
 */
void SessionManager::setCheckInterval(int checkInterval)
{
	this->checkInterval = checkInterval;
}

/**
 * Return the maximum number of active Sessions allowed, or -1 for
 * no limit.
 * @ret int the maximum number of active Sessions allowed.
 */
int SessionManager::getMaxActiveSessions()
{
	return maxActiveSessions;
}

/**
 * Set the maximum number of actives Sessions allowed, or -1 for
 * no limit.
 *
 * @param max The new maximum number of sessions
 */
void SessionManager::setMaxActiveSessions(int max)
{
	maxActiveSessions = max;
}

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
Session * SessionManager::findSession(const char& id, bool updateAccessTime)
{
	//D(cout << "SessionManager::findSession() locking sessionMutex= " << sessionMutex << endl;)
	Platform::lockMutex(sessionMutex);

	Session * session = sessionList;

	//D(cout << "SessionManager::findSession() 0 with session= " << session << " id= '" << &id << "'" << endl;)

	while ( session )
	{
	//	D(cout << "SessionManager::findSession() 1 session= " << session << " session->getSessionID()= '" << &session->getSessionID() << "'" << endl;)

		if ( !strcmp(&session->getSessionID(),&id) )
		{
			//D(cout << "SessionManager::findSession() Found id" << endl;)
			break;
		}
		session = session->next;
	}

	//D(cout << "SessionManager::findSession() 2 with session= " << session << endl;)

	if ( !session )
	{
/*
		if ( create )
		{
			session = new Session(*this);

			//D(cout << "SessionManager::findSession() created new session= " << session << endl;)

			session->setSessionID(id);
			session->next = sessionList;
			sessionList = session;
		}
*/
	}
	else if ( updateAccessTime )
	{
		session->access();
	}

	Platform::unlockMutex(sessionMutex);
	//D(cout << "SessionManager::findSession() unlocking sessionMutex= " << sessionMutex << endl;)
	//D(cout << "SessionManager::findSession() END with session= " << session << endl;)

	return session;
}

/**
 * Create a new Session object and assign it a unique Session ID.
 * The Session is NOT added to the internal Session list.
 *
 * @ret Session * a newly created Session.
 */
Session * SessionManager::createSession()
{
	Session * session = new Session(*this);

	session->setSessionID(*sessionIdGenerator.generateId(NULL));
/*
	Platform::lockMutex(sessionMutex);
	session->next = sessionList;
	sessionList = session;

	//cout << "*** Created new session: " << session << " id= '" << &session->getSessionID() << "' ***" << endl;

	Platform::unlockMutex(sessionMutex);
*/
	return session;
}

/**
 * Add a Session object to the session list. This should only be done
 * once the client has joined the session (session.isNew() == FALSE).
 * Once a session is added to the session list, the SessionManager has
 * the responsiblity of managing and deleting it.
 *
 * @param Session& a reference to the Session to add to the list.
 */
void SessionManager::addSession(Session& session)
{
	Platform::lockMutex(sessionMutex);
	session.next = sessionList;
	sessionList = &session;
	session.sessionIsNew = FALSE;
	Platform::unlockMutex(sessionMutex);
}

/**
 * Invalidate all sessions that have expired.
 * @ret bool FALSE to terminate session checking, TRUE to continue.
 */
bool SessionManager::invalidateSessions()
{
	//printf("%d of %s, sessionHandler(), maxInctiveInterval= %d\n",__LINE__,__FILE__,maxInactiveInterval);
	if ( maxInactiveInterval >= 0 )
	{
		//D(cout << endl << "SessionManager::invalidateSessions() locking sessionMutex= " << sessionMutex << endl;)
		//printf("%d of %s, sessionHandler()\n",__LINE__,__FILE__);
		Platform::lockMutex(sessionMutex);

		long curTime = Platform::getCurrentMillis();
		Session * session = sessionList;
		Session * prev = NULL;

		printf("%d of %s, sessionHandler(), session= %p\n",__LINE__,__FILE__,session);
		while ( session )
		{
			//D(cout << "SessionManager::invalidateSessions() 1 session= " << session << " session->getSessionID()= '" << &session->getSessionID() << "'" << endl;)
			printf("%d of %s, sessionHandler()\n",__LINE__,__FILE__);

			if ( session->isValid() )
			{
				long idleTime = curTime - session->getLastAccessedTime();
				/*
				D(cout << "SessionManager::invalidateSessions() idleTime= " << idleTime <<
					" maxInactiveInterval= " << maxInactiveInterval << endl;)
				*/
				printf("%d of %s, sessionHandler()\n",__LINE__,__FILE__);

				if ( idleTime >= maxInactiveInterval )
				{
					//D(cout << "SessionManager::invalidateSessions() invalidateing session= " << session << " session->getSessionID()= '" << &session->getSessionID() << "'" << endl;)
					printf("%d of %s, sessionHandler()\n",__LINE__,__FILE__);
					session->invalidate();
				}
			}

			// if the session is invalid, remove it from the sessionList and place it on
			// the invalidSessionList to hold for deletion later.
			if ( !session->isValid() )
			{
				Session * next = session->next;

				if ( session == sessionList )
					sessionList = session->next;
				else
				{
					prev->next = session->next;
				}

				session->next = invalidSessionList;
				invalidSessionList = session;

				session = next;
/*
				if ( prev )
					prev->next = session->next;
				else
					sessionList = session->next;

				prev = session->next;
				session->next = invalidSessionList;
				invalidSessionList = session;
				session = prev;
*/
			}
			else
			{
				prev = session;
				session = session->next;
			}
		}
		Platform::unlockMutex(sessionMutex);
		//D(cout << "SessionManager::invalidateSessions() unlocked sessionMutex= " << sessionMutex << endl;)
		//printf("%d of %s, sessionHandler()\n",__LINE__,__FILE__);
	}

	// Now delete any invalid sessions that have their reference count <= 0
	Session	* session = invalidSessionList;
	Session	* prev = NULL;
	long curTime = Platform::getCurrentMillis();

	while ( session )
	{
		//D(cout << "SessionManager::invalidateSessions() invalidSessionList 1 session= " << session << " session->getSessionID()= '" << &session->getSessionID() << "'" << endl;)
		printf("%d of %s, sessionHandler(), session= %p\n",__LINE__,__FILE__,session);

		if ( (session->referenceCount <= 0) || ((curTime - session->getLastAccessedTime()) > forceInvalidSessionDeleteTime) )
		{
			printf("%d of %s, sessionHandler(), session= %p\n",__LINE__,__FILE__,session);

			Session * next = session->next;

			printf("%d of %s, sessionHandler(), next= %p\n",__LINE__,__FILE__,next);

			if ( session == invalidSessionList )
				invalidSessionList = session->next;
			else
			{
				prev->next = session->next;
			}

			printf("%d of %s, sessionHandler()\n",__LINE__,__FILE__);

			delete session;
			printf("%d of %s, sessionHandler()\n",__LINE__,__FILE__);

			session = next;
/*
			if ( prev )
				prev->next = session->next;
			else
				invalidSessionList = session->next;

			prev = session->next;
			//D(cout << "SessionManager::invalidateSessions() invalidSessionList 2 deleting session= " << session << " session->getSessionID()= '" << &session->getSessionID() << "'" << endl;)
			delete session;
			session = prev;
*/
		}
		else
		{
			prev = session;
			session = session->next;
		}
	}

	//printf("%d of %s, sessionHandler()\n",__LINE__,__FILE__);

	return !threadDone;
}


DWORD WINAPI sessionHandler(void * sm)
{
    SessionManager	* sessionManager = (SessionManager *)sm;
	bool			  threadContinue = true;

	printf("\n\n\n!!!!!!\n%d of %s, sessionHandler()\n",__LINE__,__FILE__);
    printf("version 2b\n");
	while ( threadContinue )
	{
		printf("%d of %s, sessionHandler() sessionManager->getCheckInterval()= %d\n",__LINE__,__FILE__,sessionManager->getCheckInterval());
		Sleep(sessionManager->getCheckInterval()*1000L);
		//printf("%d of %s, sessionHandler()\n",__LINE__,__FILE__);
	    threadContinue = sessionManager->invalidateSessions();
		printf("%d of %s, sessionHandler(), threadContinue= %d\n",__LINE__,__FILE__,threadContinue);
	}

	return 0;
}

