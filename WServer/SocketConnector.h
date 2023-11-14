/*
 * SocketConnector.h
 *
 * A base class which has static methods to connect to a socket at a specified port.
 *
 * Author: Eric Wistrand 11/12/2023
 */


//---------------------------------------------------------------------------
#ifndef _WSERVER_SocketConnectorH_
#define _WSERVER_SocketConnectorH_

#include "WServer.h"
#include "ws-util.h"
#include <stdlib.h>

#ifndef	_WIN32_WCE	// [
#include <iostream>
#endif				// ]



/**
 * SocketConnector.h
 *
 * A base class which connects to a socket at a specified port.
 *
 * Z.Z.Z 030705
 */
class WSERVER_API SocketConnector
{
public:
	/**
	 * Static class method that takes an address string, determines if it's a
	 * dotted-quad IP address or a domain address. If the latter, ask DNS to
	 * resolve it. In either case, return resolved IP address. If we fail,
	 * we return INADDR_NONE.
	 *
	 * @param pcHost - const char&, the host whose address is to be looked up.
	 * @ret u_long, the resolved IP address or INADDR_NONE upon error.
	 */
	static u_long lookupAddress(const char& pcHost);

	/**
	 * Static class method that establishes the socket connection on the given interface
	 * and port, returning the socket if successful; if not, returns INVALID_SOCKET.
	 *
	 * @param nRemoteAddr, the remote addres to connect to.
	 * @param nPort - u_short, the port to connect to.
	 * @param isUDP - bool, true of this connection is UPD, false otherwise. Defualts to false.
	 * @ret SOCKET, the SOCKET or INVALID_SOCKET upon error.
	 */
	static SOCKET establishConnection(u_long nRemoteAddr, u_short nPort, bool isUDP = false);

	/**
	 * Static class method that establishes the socket connection on the given interface
	 * and port, returning the socket if successful; if not, returns INVALID_SOCKET.
	 *
	 * @param pcHost - const char&, the host to connect to.
	 * @param nPort - u_short, the port to connect to.
	 * @param isUDP - bool, true of this connection is UPD, false otherwise. Defualts to false.
	 * @ret SOCKET, the SOCKET or INVALID_SOCKET upon error.
	 */
	static SOCKET establishConnection(const char& pcHost, u_short nPort, bool isUDP = false);

};


//---------------------------------------------------------------------------
#endif
