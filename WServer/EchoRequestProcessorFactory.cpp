//---------------------------------------------------------------------------

#pragma hdrstop

#ifndef	_WIN32_WCE	// [
#include "EchoRequestProcessorFactory.h"

EchoRequestProcessorFactory::EchoRequestProcessorFactory()
{
}

RequestProcessor & EchoRequestProcessorFactory::createRequestProcessor()
{
	EchoRequestProcessor * echoRequestProcessor = new EchoRequestProcessor();

    //echoRequestProcessor->setSocket(sd);

    return *echoRequestProcessor;
}
#endif				// ]

//---------------------------------------------------------------------------
//#pragma package(smart_init)
