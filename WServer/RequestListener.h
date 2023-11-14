/**
 * RequestListener.h
 *
 * A class which subclasses SocketListener to represent a class that
 * listens for requests coming from Sockets and uses the passed in
 * RequestProcessorFactory to create  RequestProcessor to process
 * the request.
 *
 * Author: Eric Wistrand 11/12/2023
 */

#ifndef RequestListenerH
#define RequestListenerH

#include "SocketListener.h"
#include "RequestProcessorFactory.h"

/**
 * RequestListener.h
 *
 * A class which subclasses SocketListener to represent a class that
 * listens for requests coming from Sockets and uses the passed in
 * RequestProcessorFactory to create  RequestProcessor to process
 * the request.
 */
class WSERVER_API RequestListener : public SocketListener
{
public:
	RequestListener(const char * address, int nPort, RequestProcessorFactory & requestProcessorFactory, bool isUDP = false);
    ~RequestListener();

    virtual int		listen4Request();

#if	0	// [ Now in SocketListener
	SOCKET	setUpListener();
	virtual void	acceptConnections(SOCKET ListeningSocket);
	void	stop();
#endif

protected:
#if	0	// [ Now in SocketListener
	const char	* pcAddress;
    const int	  port;
	BOOL		  quit; // flag that stops the listen4Request.
#endif

    RequestProcessorFactory & requestProcessorFactory;

	/**
	 * SocketListener interface. Called by acceptConnections to handle the accepted connection.
	 *
	 * @ret int, 0 upon success, non zero error code upon errors.
	 */
	virtual int handleConnection(SOCKET sd, sockaddr_in& sinRemote);

	/**
	 * Called by handleConnection with a appSocket subclass to handle the accepted connection.
	 * This method has the responsibility of deleting the AppSocket.
	 *
	 * @ret int, 0 upon success, non zero error code upon errors.
	 */
	virtual int handleConnection(AppSocket& appSocket);
};


//---------------------------------------------------------------------------
#endif
