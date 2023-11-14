//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
//#pragma hdrstop

#include "RequestListener.h"

//#include "DebugsOn.h"

#ifndef	_WIN32_WCE	// [
using namespace std;
#endif				// ]


RequestListener::RequestListener(const char * address, int nPort, RequestProcessorFactory & requestProcessorFactory, bool isUDP) : SocketListener(address, nPort, isUDP), requestProcessorFactory(requestProcessorFactory)
{

}

RequestListener::~RequestListener()
{
//	WSACleanup();
}


int RequestListener::listen4Request()
{
    D(cout << "Establishing the listener..." << endl)
	quit = false;

    SOCKET listeningSocket = setUpListener();

    if (listeningSocket == INVALID_SOCKET)
    {
        D(cout << endl << WSAGetLastErrorMessage("establish listener") << endl;)
        return 3;
    }

    D(cout << "Waiting for connections..." << flush;)

    while (!quit)
    {
        acceptConnections(listeningSocket);
        D(cout << "RequestListener::listen4Request() restarting..." << endl;)
    }
	quit = false;

	if (ShutdownConnection(listeningSocket,isSocketUDP()))
	{
		//cout << __LINE__ << " of " << __FILE__  " connection is down." << endl;
	}
	else
	{
		//cerr << endl << __LINE__ << " of " << __FILE__ << " : " << WSAGetLastErrorMessage("Connection shutdown failed") << endl;
	}
	listeningSocket = INVALID_SOCKET;

#if defined(_MSC_VER)
    return 0;   // warning eater
#endif
}

/**
 * Called by handleConnection with a appSocket subclass to handle the accepted connection.
 * This method has the responsibility of deleting the AppSocket.
 *
 * @ret int, 0 upon success, non zero error code upon errors.
 */
int RequestListener::handleConnection(AppSocket& appSocket)
{
	int					  ret = -1;
	RequestProcessor	* requestProcessor = requestProcessorFactory.createRequestProcessor();

	if ( requestProcessor )
	{
		// The requestProcessor will delete the appSocket.
		requestProcessor->setAppSocket(&appSocket);

		requestProcessor->processRequest();

		ret = 0;
		// Since processRequest() spawns a thread, we can't delete
		// the requestProcessor here. It gets deleted in the thread
		// when the request is finished.
		//delete requestProcessor;
	}
	else
		delete &appSocket;

	return ret;
}

/**
 * SocketListener interface. Called by acceptConnections to handle the accepted connection.
 *
 * @ret int, 0 upon success, non zero error code upon errors.
 */
int RequestListener::handleConnection(SOCKET sd, sockaddr_in& sinRemote)
{
	int	ret;

	AppSocket * appSocket = new AppSocket(sd,port,sinRemote,isSocketUDP());

	if ( appSocket )
	{
		ret = handleConnection(*appSocket);
	}
	else
		ret = -1;

	return ret;
}

//---------------------------------------------------------------------------
//#pragma package(smart_init)
