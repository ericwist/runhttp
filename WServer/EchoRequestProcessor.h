//---------------------------------------------------------------------------
#ifndef EchoRequestProcessorH
#define EchoRequestProcessorH

#include "RequestProcessor.h"

class EchoRequestProcessor : public RequestProcessor
{
public:
	EchoRequestProcessor();

    void processRequest();

   	int run();

private:    
    bool EchoRequestProcessor::EchoIncomingPackets();
};


//---------------------------------------------------------------------------
#endif
