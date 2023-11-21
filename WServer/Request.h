//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
#ifndef RequestH
#define RequestH

#include "AppSocket.h"
#include "Session.h"
#include "WServer.h"

class StdRedirect;

class WSERVER_API Request //: public AttributeManager
{
public:
    // virtual bool build(SOCKET socket, sockaddr_in * sinRemote) = 0;
    virtual bool build(AppSocket& appSocket) = 0;

    /**
     * Get the protocol.
     * @return const char *, a character pointer to the protocol string, or NULL. Callers must NOT modify or delete this constant value.
     */
    virtual const char* getProtocol() = 0;

    /**
     * Get the host name of the server that received the request. This is same as the value
     * of the CGI variable SERVER_NAME.
     * @return const char *, a character pointer to the string containing the name of the
     *	server to which the request was sent, or NULL. Callers must NOT modify or delete this constant value.
     */
    virtual const char* getServerName() = 0;

    /**
     * Returns the port number on which this request was received. This is the same as the
     * value of the CGI variable SERVER_PORT.
     * @ret int - an integer specifying the port number.
     */
    virtual int getServerPort() = 0;

    /**
     * Get the Internet Protocol (IP) address of the client that sent the request. This is the
     * same as the value of the CGI variable REMOTE_ADDR.
     * @return const char *, a character pointer to the string containing the IP address of the
     * client that sent the request, or NULL. Callers must NOT modify or delete this constant value.
     */
    virtual const char* getRemoteAddr() = 0;

    /**
     * Get the current Session associated with this request or, if if there is no
     * current session and create is true, create and return a new session.
     * If create is false and the request has no valid Session, this method returns null.
     * To make sure the session is properly maintained, you must call this method before
     * the response is committed.
     * @param create bool, TRUE - to create a new session for this request if necessary;
     *	FALSE to return NULL if there's no current session.
     * @ret Session * the Session associated with this request or NULL if create is false
     *	and the request has no valid session
     */
    virtual Session* getSession(bool create) = 0;

protected:
    /**
     * Set the id of the passed in session that uniquely identifes this session. This function will
     * make a copy of the supplied string. If the caller has allocated the supplied string,
     * it has the responsibility of deleting it (since the code makes its own copy).
     *
     * @param session - const Session&, the Session object to manipulate.
     * @param id - const char&, a reference to the session id to uniquely identify this session.
     */
    void setSessionID(Session& session, const char& id)
    {
        session.setSessionID(id);
    }

    /**
     * Increase the reference count of the passed in session by 1. This is the reference count of objects that
     * reference this Session object (like HttpRequest & HttpResponse). Objects which
     * reference this Session object need to call addReference(), and releaseReference()
     * when finished. This is VERY important, as the SessionManager will not free any
     * invalidated Session objects with a reference count > 0.
     *
     * @param session - const Session&, the Session object to manipulate.
     * @ret int the new reference count. This value is meant to be used for diagnostic
     *	and testing purposes only.
     */
    int addReference(Session& session)
    {
        return session.addReference();
    }

    /**
     * Decrease the reference count of the passed in session by 1. This is the reference count of objects that
     * reference this Session object (like HttpRequest & HttpResponse). Objects which
     * reference this Session object need to call addReference(), and releaseReference()
     * when finished. This is VERY important, as the SessionManager will not free any
     * invalidated Session objects with a reference count > 0.
     *
     * @param session - const Session&, the Session object to manipulate.
     * @ret int the new reference count. This value is meant to be used for diagnostic
     *	and testing purposes only.
     */
    int removeReference(Session& session)
    {
        return session.removeReference();
    }

private:
    StdRedirect* stdRedirect;
};

//---------------------------------------------------------------------------
#endif
