//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
// #pragma hdrstop

#include "RequestProcessor.h"

#ifndef _WIN32_WCE // [
using namespace std;
#endif // ]

DWORD WINAPI EchoHandler(void* rp);

RequestProcessor::RequestProcessor()
: appSocket(NULL)
{
}

RequestProcessor::~RequestProcessor()
{
    D(cout << "~RequestProcessor() " << endl;)

    D(cout << "Shutting connection down..." << flush;)

    if (appSocket)
    {
        /*
        SOCKET socket = appSocket->getSocket();

        if ( socket )
        {
            if (ShutdownConnection(socket))
            {
                D(cout << "Connection is down." << endl;)
            }
            else
            {
                cerr << endl << WSAGetLastErrorMessage("Connection shutdown failed") << endl;
            }
        }
        */

        delete appSocket;
    }
}

void RequestProcessor::setAppSocket(AppSocket* appSocket)
{
    this->appSocket = appSocket;
}

AppSocket* RequestProcessor::getAppSocket()
{
    return appSocket;
}

#if 0 // [
/**
 * Dispatch this request.
 *
 * @param request Request&, a reference to the Request that generated this dispatch.
 * @param response Response&, a reference to the Response to use while dispatching the request.
 * @ret bool TRUE if the request was successfully dispatched, FALSE otherwise.
 */
bool RequestProcessor::dispatch(Request& request, Response& response)
{
	return FALSE;
}

#endif // ]

//---------------------------------------------------------------------------
// #pragma package(smart_init)
