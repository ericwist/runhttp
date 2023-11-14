//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
#ifndef RequestProcessorH
#define RequestProcessorH

#include <stdlib.h>

#ifndef	_WIN32_WCE	// [
#include <iostream>
#endif				// ]

#include "WServer.h"
#include "Response.h"
#include "ws-util.h"

#define	bufferSize 1024

class WSERVER_API RequestProcessor
{
public:
	RequestProcessor();
    virtual ~RequestProcessor();

	/**
	 * Process this request.
	 */
    virtual void processRequest() = 0;

	/**
	 * Run this RequestProcessor
	 *
	 * @ret int, 0 for success, non zero error code upon failure.
	 */
    virtual int run() = 0;

	/**
	 * Dispatch this request.
	 *
	 * @param request Request&, a reference to the Request that generated this dispatch.
	 * @param response Response&, a reference to the Response to use while dispatching the request.
	 * @ret bool TRUE if the request was successfully dispatched, FALSE otherwise.
	 */
	virtual bool dispatch(Request& request, Response& response) = 0;

/*
    void setSocket(SOCKET sd);
    SOCKET getSocket();
*/

    void setAppSocket(AppSocket * appSocket);
    AppSocket * getAppSocket();

	//void setSockaddr_in(sockaddr_in	* sinRemote);


protected:
	//SOCKET		socket;
	AppSocket	* appSocket;
	//sockaddr_in	sinRemote;
};


//---------------------------------------------------------------------------
#endif
