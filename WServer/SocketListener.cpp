/**
 * SocketListener.cpp
 *
 * An abstract class which listens on a socket at a specified port.
 * abstract because it doesn't implement the handleConnection() method.
 *
 * Author: Eric Wistrand 11/12/2023
 */

#include "SocketListener.h"

#ifndef	_WIN32_WCE	// [
using namespace std;
#endif				// ]

SocketListener::SocketListener(const char * address, int nPort, bool isUDP) : pcAddress(address), port(nPort), isUDP(isUDP), quit(FALSE)
{

}

SocketListener::~SocketListener()
{
//	WSACleanup();
}


/**
 * Is the socket an UPD type or TCP. If UPD it is a Datagram and was created
 * with the SOCK_DGRAM parameter as opposed to SOCK_STREAM.
 *
 * @ret true if the socket is UDP, false otherwise.
 */
bool SocketListener::isSocketUDP()
{
	return isUDP;
}

/**
 * Set up the socketlistener on the given interface and port, returning the
 * listening socket if successful; if not, returns INVALID_SOCKET.
 *
 * @ret SOCKET, the SOCKET or INVALID_SOCKET upon error.
 */
SOCKET SocketListener::setUpListener()
{
	short		nPort = htons((short)port);
    u_long	nInterfaceAddr = inet_addr(pcAddress);

    if (nInterfaceAddr != INADDR_NONE)
    {
        SOCKET sd = socket(AF_INET, isUDP ? SOCK_DGRAM : SOCK_STREAM, 0);
        if (sd != INVALID_SOCKET)
        {
            sockaddr_in sinInterface;
            sinInterface.sin_family = AF_INET;
            sinInterface.sin_addr.s_addr = nInterfaceAddr;
            sinInterface.sin_port = nPort;

            if (bind(sd, (sockaddr*)&sinInterface, sizeof(sockaddr_in)) != SOCKET_ERROR)
            {
				// We cannot listen() on a UDP socket.
				if ( !isUDP )
	                listen(sd, SOMAXCONN);
                return sd;
            }
            else
            {
                //cerr << WSAGetLastErrorMessage("bind() failed") << endl;
            }
        }
    }

    return INVALID_SOCKET;
}

/**
 * Spins until the stop() method is called waiting for connections.  For
 * each one that comes in, we handleConnection() to handle it and go back
 * to waiting for connections. The sublass which implements handleConnection()
 * might spawn a thread to handle the connection.  If an error occurs, we return.
 */
void SocketListener::acceptConnections(SOCKET listeningSocket)
{
    sockaddr_in	sinRemote;
    int			nAddrSize = sizeof(sinRemote);
	struct		timeval tv;

	tv.tv_sec = 2;
	tv.tv_usec = 0;

	//D(cout << "acceptConnections() ENTERED" << endl;)

    while (!quit)
    {
		fd_set readFDs, exceptFDs;

		FD_ZERO(&readFDs);
		FD_ZERO(&exceptFDs);

		// Add the listener socket to the read and except FD sets
		FD_SET(listeningSocket, &readFDs);
		FD_SET(listeningSocket, &exceptFDs);

		//D(cout << "acceptConnections() before select()" << endl;)
		if (select(0, &readFDs, NULL, &exceptFDs, &tv) > 0)
		{
			//D(cout << "acceptConnections() after select()" << endl;)
			// Something happened on the socket.
			if (FD_ISSET(listeningSocket, &readFDs))
			{
				//D(cout << "acceptConnections() readFDs ISSET" << endl;)
				SOCKET sd;

				//
				// accept() doesn't make sense on UDP, since we do not listen().
				//
				if ( !isUDP )
					sd = accept(listeningSocket, (sockaddr*)&sinRemote, &nAddrSize);
				else
					sd = listeningSocket;

				//
				// In the case of SOCK_STREAM, the server can do recv() and
				// send() on the accepted socket and then close it.

				// However, for SOCK_DGRAM (UDP), the server will do
				// recvfrom() and sendto()  in a loop.

				if (sd != INVALID_SOCKET)
				{
					/*
					D(cout << "Accepted connection from " <<
							inet_ntoa(sinRemote.sin_addr) << ":" <<
							ntohs(sinRemote.sin_port) << "." <<
							endl;)
					*/

					handleConnection(sd, sinRemote);
				}
				else
				{
					//cerr << WSAGetLastErrorMessage("accept() failed") << endl;
					//return;
				}
			}
			else if (FD_ISSET(listeningSocket, &exceptFDs))
			{
				int err;
				int errlen = sizeof(err);
				getsockopt(listeningSocket, SOL_SOCKET, SO_ERROR,
						(char*)&err, &errlen);
				/*
				cerr << WSAGetLastErrorMessage(
						"Exception on listening socket: ", err) << endl;
				*/
				//return;
			}
			else
			{
				//cerr << "??? Neither readFS nor exceptFDs ISSET ???" << endl;
			}
		}


    }
}

void SocketListener::stop()
{
	quit = true;
/*
	while ( quit )
	{
		Sleep(2000L);
	}
*/
}

//---------------------------------------------------------------------------
//#pragma package(smart_init)
