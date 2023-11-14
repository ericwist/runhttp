//---------------------------------------------------------------------------

#ifndef EchoRequestProcessorFactoryH
#define EchoRequestProcessorFactoryH

#include "RequestProcessorFactory.h"
#include "EchoRequestProcessor.h"

class EchoRequestProcessorFactory : public RequestProcessorFactory
{
public:
	EchoRequestProcessorFactory();

    RequestProcessor & createRequestProcessor();
};

//---------------------------------------------------------------------------
#endif
