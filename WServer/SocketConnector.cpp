/**
 * SocketConnector.h
 *
 * A base class which has static methods to connect to a socket at a specified port.
 *
 * Author: Eric Wistrand 11/12/2023
 */

#include "SocketConnector.h"

// #include "debugson.h"

/**
 * Static class method that takes an address string, determines if it's a
 * dotted-quad IP address or a domain address. If the latter, ask DNS to
 * resolve it. In either case, return resolved IP address. If we fail,
 * we return INADDR_NONE.
 *
 * @param pcHost - const char&, the host whose address is to be looked up.
 * @ret u_long, the resolved IP address or INADDR_NONE upon error.
 */
u_long SocketConnector::lookupAddress(const char& pcHost)
{
    u_long nRemoteAddr = inet_addr(&pcHost);

    // D(std::cout << __LINE__ << " of " << __FILE__ << std::endl;)

    if (nRemoteAddr == INADDR_NONE)
    {
        // D(std::cout << __LINE__ << " of " << __FILE__ << std::endl;)

        // pcHost isn't a dotted IP, so resolve it through DNS
        hostent* pHE = gethostbyname(&pcHost);

        if (pHE != 0)
        {
            // D(std::cout << __LINE__ << " of " << __FILE__ << std::endl;)
            nRemoteAddr = *((u_long*)pHE->h_addr_list[0]);
        }
    }

    return nRemoteAddr;
}

/**
 * Static class method that establishes the socket connection on the given interface
 * and port, returning the socket if successful; if not, returns INVALID_SOCKET.
 *
 * @param nRemoteAddr, the remote addres to connect to.
 * @param nPort - u_short, the port to connect to.
 * @param isUDP - bool, true of this connection is UPD, false otherwise. Defualts to false.
 * @ret SOCKET, the SOCKET or INVALID_SOCKET upon error.
 */
SOCKET SocketConnector::establishConnection(u_long nRemoteAddr, u_short nPort, bool isUDP)
{
    // Create a socket
    SOCKET sd = socket(AF_INET, isUDP ? SOCK_DGRAM : SOCK_STREAM, 0);

    // D(std::cout << __LINE__ << " of " << __FILE__ << " nRemoteAddr= " << nRemoteAddr << " nPort= " << nPort << std::endl;)

    if (sd != INVALID_SOCKET)
    {
        // D(std::cout << __LINE__ << " of " << __FILE__ << std::endl;)

        sockaddr_in sinRemote;
        sinRemote.sin_family = AF_INET;
        sinRemote.sin_addr.s_addr = nRemoteAddr;
        sinRemote.sin_port = htons(nPort);

        if (connect(sd, (sockaddr*)&sinRemote, sizeof(sockaddr_in)) == SOCKET_ERROR)
        {
            // D(std::cout << __LINE__ << " of " << __FILE__ << std::endl;)
            sd = INVALID_SOCKET;
        }
    }

    return sd;
}

/**
 * Static class method that establishes the socket connection on the given interface
 * and port, returning the socket if successful; if not, returns INVALID_SOCKET.
 *
 * @param pcHost - const char&, the host to connect to.
 * @param nPort - u_short, the port to connect to.
 * @param isUDP - bool, true of this connection is UPD, false otherwise. Defualts to false.
 * @ret SOCKET, the SOCKET or INVALID_SOCKET upon error.
 */
SOCKET SocketConnector::establishConnection(const char& pcHost, u_short nPort, bool isUDP)
{
    // D(std::cout << __LINE__ << " of " << __FILE__ << " pcHost= '" << &pcHost << "' nPort= " << nPort << std::endl;)

    SOCKET sd;
    u_long nRemoteAddress = lookupAddress(pcHost);

    if (nRemoteAddress == INADDR_NONE)
    {
        D(std::cerr << __LINE__ << " of " << __FILE__ << WSAGetLastErrorMessage("lookup address") << std::endl;)
        sd = INVALID_SOCKET;
    }
    else
    {
        sd = establishConnection(nRemoteAddress, nPort, isUDP);
    }

    return sd;
}

//---------------------------------------------------------------------------
// #pragma package(smart_init)
