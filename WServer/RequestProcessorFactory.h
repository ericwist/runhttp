//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023

#ifndef RequestProcessorFactoryH
#define RequestProcessorFactoryH

#include "WServer.h"
#include "RequestProcessor.h"

class WSERVER_API RequestProcessorFactory
{
public:
	RequestProcessorFactory();
    virtual ~RequestProcessorFactory();

    virtual RequestProcessor * createRequestProcessor() = 0;
};


//---------------------------------------------------------------------------
#endif
