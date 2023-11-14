/**
 * SocketListener.h
 *
 * An abstract class which listens on a socket at a specified port.
 * abstract because it doesn't implement the handleConnection() method.
 *
 * Author: Eric Wistrand 11/12/2023
 */


//---------------------------------------------------------------------------
#ifndef SocketListenerH
#define SocketListenerH

#include "WServer.h"
#include "ws-util.h"
#include <stdlib.h>

#ifndef	_WIN32_WCE	// [
#include <iostream>
#endif				// ]

class WSERVER_API SocketListener
{
public:
	SocketListener(const char * address, int nPort, bool isUDP = false);
    ~SocketListener();

	/**
	 * Stop this listener.
	 */
	void	stop();

	/**
	 * Is the socket an UPD type or TCP. If UPD it is a Datagram and was created
	 * with the SOCK_DGRAM parameter as opposed to SOCK_STREAM.
	 *
	 * @ret true if the socket is UDP, false otherwise.
	 */
	bool isSocketUDP();

protected:
	const char	* pcAddress;
    const int	  port;
	BOOL		  quit; // flag that stops the listen4Request.

	/**
	 * Called by acceptConnections to handle the accepted connection. This will only
	 * be called if a valid SOCKET is available.
	 *
	 * @param sd SOCKET, a valid socket. It will never be INVALID_SOCKET.
	 * @ret int, 0 upon success, non zero error code upon errors.
	 */
	virtual int handleConnection(SOCKET sd, sockaddr_in& sinRemote) = 0;

	/**
	 * Spins until the stop() method is called waiting for connections.  For
	 * each one that comes in, we handleConnection() to handle it and go back
	 * to waiting for connections. The sublass which implements handleConnection()
	 * might spawn a thread to handle the connection.  If an error occurs, we return.
	 */
	virtual void acceptConnections(SOCKET ListeningSocket);

	/**
	 * Set up the socketlistener on the given interface and port, returning the
	 * listening socket if successful; if not, returns INVALID_SOCKET.
	 *
	 * @ret SOCKET, the SOCKET or INVALID_SOCKET upon error.
	 */
	virtual SOCKET setUpListener();

private:
	/**
	 * Is the socket an UPD type or TCP. If UPD it is a Datagram and was created
	 * with the SOCK_DGRAM parameter as opposed to SOCK_STREAM.
	 */
	bool	isUDP;
};


//---------------------------------------------------------------------------
#endif
