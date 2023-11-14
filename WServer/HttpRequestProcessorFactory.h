//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023

#ifndef HttpRequestProcessorFactoryH
#define HttpRequestProcessorFactoryH

#include "WServer.h"

#include "RequestProcessorFactory.h"
#include "HttpRequestProcessor.h"

class WSERVER_API HttpRequestProcessorFactory : public RequestProcessorFactory
{
public:
	HttpRequestProcessorFactory(const TCHAR * docRootPath, const TCHAR * indexFileName);

    RequestProcessor * createRequestProcessor();

private:
	const TCHAR * docRootPath;
	const TCHAR * indexFileName;

	/**
	 * The SessionManager which will manage the request's sessions.
	 */
	SessionManager sessionManager;
};


//---------------------------------------------------------------------------
#endif
