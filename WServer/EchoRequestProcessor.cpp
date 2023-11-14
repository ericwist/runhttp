//---------------------------------------------------------------------------
#pragma hdrstop

#ifndef	_WIN32_WCE	// [
#include "EchoRequestProcessor.h"

using namespace std;


DWORD WINAPI EchoHandler(void* rp);

EchoRequestProcessor::EchoRequestProcessor()
{
	//cout << "EchoRequestProcessor::EchoRequestProcessor() " << endl;
}

void EchoRequestProcessor::processRequest()
{
	DWORD nThreadID;
    CreateThread(0, 0, EchoHandler, (void*)this, 0, &nThreadID);
}

int EchoRequestProcessor::run()
{
   int					  nRetval = 0;

    if (!EchoIncomingPackets())
    {
        //cerr << endl << WSAGetLastErrorMessage("Echo incoming packets failed") << endl;
        nRetval = 3;
    }

    return nRetval;
}

//// EchoIncomingPackets ///////////////////////////////////////////////
// Bounces any incoming packets back to the client.  We return false
// on errors, or true if the client closed the socket normally.
bool EchoRequestProcessor::EchoIncomingPackets()
{
    // Read data from client
    char	acReadBuffer[bufferSize];
    int		nReadBytes;

    do
    {
        nReadBytes = recv(socket, acReadBuffer, bufferSize, 0);

        if (nReadBytes > 0)
        {
            //cout << "Received " << nReadBytes << " bytes from client." << endl;
            acReadBuffer[nReadBytes] = 0;
            //cout << "'" << acReadBuffer << "'" << endl;

            int	nSentBytes = 0;

            while( nSentBytes < nReadBytes )
            {
                int nTemp = send(socket, acReadBuffer + nSentBytes, nReadBytes - nSentBytes, 0);

                if (nTemp > 0)
                {
                    //cout << "Sent " << nTemp << " bytes back to client." << endl;
                    nSentBytes += nTemp;
                }
                else if (nTemp == SOCKET_ERROR)
                {
                    return false;
                }
                else
                {
                    // Client closed connection before we could reply to
                    // all the data it sent, so bomb out early.
                    //cout << "Peer unexpectedly dropped connection!" << endl;
                    return true;
                }
            }
        }
        else if (nReadBytes == SOCKET_ERROR)
        {
            return false;
        }
    } while (nReadBytes != 0);

    //cout << "Connection closed by peer." << endl;
    return true;
}

//// EchoHandler ///////////////////////////////////////////////////////
// Handles the incoming data by reflecting it back to the sender.

DWORD WINAPI EchoHandler(void * rp)
{
    RequestProcessor	* requestProcessor = (RequestProcessor *)rp;
    //SOCKET 				  sd = requestProcessor->getSocket();
    int					  nRetval = requestProcessor->run(); //0;

/*
    if (!EchoIncomingPackets(sd))
    {
        cerr << endl << WSAGetLastErrorMessage("Echo incoming packets failed") << endl;
        nRetval = 3;
    }
*/
/*
    cout << "Shutting connection down..." << flush;

    if (ShutdownConnection(sd))
    {
        cout << "Connection is down." << endl;
    }
    else
    {
        cerr << endl << WSAGetLastErrorMessage("Connection shutdown failed") << endl;
        nRetval = 3;
    }
*/
    delete requestProcessor;

    return nRetval;
}

//---------------------------------------------------------------------------
//#pragma package(smart_init)

#endif				// ]