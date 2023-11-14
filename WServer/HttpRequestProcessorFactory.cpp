//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
//#pragma hdrstop

#include "HttpRequestProcessorFactory.h"

HttpRequestProcessorFactory::HttpRequestProcessorFactory(const TCHAR * docRootPath, const TCHAR * indexFileName) : docRootPath(docRootPath), indexFileName(indexFileName)
{
}

RequestProcessor * HttpRequestProcessorFactory::createRequestProcessor()
{
	return new HttpRequestProcessor(docRootPath,indexFileName,sessionManager);
}


//---------------------------------------------------------------------------
//#pragma package(smart_init)
