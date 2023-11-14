//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
//#pragma hdrstop

#include "HttpRequestProcessor.h"
#include "debugson.h"

#ifndef	_WIN32_WCE	// [
#include <time.h>

using namespace std;
#else				// ][

#include "utils.h"

_CRTIMP char *  __cdecl strrchr(const char *, int);
_CRTIMP int     __cdecl _stricmp(const char *, const char *);
#define	stricmp _stricmp
#endif				// ]


DWORD WINAPI HttpHandler(void* rp);

HttpRequestProcessor::HttpRequestProcessor(const TCHAR * documentRoot, const TCHAR * defaultDocument, SessionManager& sessionManager)
: documentRoot(documentRoot ? documentRoot : TEXT(".")), defaultDocument(defaultDocument ? defaultDocument : TEXT("index.html")),
	  sessionManager(sessionManager)
{
	//D(cout << "HttpRequestProcessor::HttpRequestProcessor() " << endl)
}

/**
 * Process this request.
 */
void HttpRequestProcessor::processRequest()
{
	DWORD nThreadID;
    CreateThread(0, 0, HttpHandler, (void*)this, 0, &nThreadID);
}

/**
 * Get the SessionManager which will manage sessions.
 * @ret const SessionManager& a reference to the SessionManager which will manage sessions.
 */
SessionManager& HttpRequestProcessor::getSessionManager()
{
	return sessionManager;
}

/**
 * Get the default document if none is specified in the HTTP request.
 * This is set via the XML specified defaultDocumentAttrTag.
 *
 * @ret const TCHAR *, the default docment or NULL. If NON NULL, callers must NOT modify this value.
 */
const TCHAR * HttpRequestProcessor::getDefaultDocument()
{
	return defaultDocument;
}

/**
 * Get the document root.
 * This is set via the XML specified defaultDocumentAttrTag.
 *
 * @ret const TCHAR *, the docment root or NULL. If NON NULL, callers must NOT modify this value.
 */
const TCHAR * HttpRequestProcessor::getDocumentRoot()
{
	return documentRoot;
}

/**
 * Dispatch this request.
 *
// * @param request HttpRequest&, a reference to the HttpRequest that generated this dispatch.
// * @param response HttpResponse&, a reference to the HttpResponse to use while dispatching the request.
 * @param request Request&, a reference to the HttpRequest that generated this dispatch.
 * @param response Response&, a reference to the HttpResponse to use while dispatching the request.
 * @ret bool TRUE if the request was successfully dispatched, FALSE otherwise.
 */
bool HttpRequestProcessor::dispatch(Request& request, Response& response)
{
	bool			  ret;
    //D(cout << "HttpRequestProcessor::dispatch() started" << endl;)
    HttpRequest		* httpRequest = (HttpRequest *)&request;
	HttpResponse	* httpResponse = (HttpResponse *)&response;
	const char		* method = httpRequest->getMethod();

    if ( method )
    {
    	if ( !_stricmp(method,GetMethod) )
        {
            //D(cout << "HttpRequestProcessor::dispatch() get" << endl;)
            ret = get(*httpRequest, *httpResponse);
        }
        else if ( !_stricmp(method,PostMethod) )
        {
            //D(cout << "HttpRequestProcessor::dispatch() post" << endl;)
            ret = post(*httpRequest, *httpResponse);
        }
        else if ( !_stricmp(method,PutMethod) )
        {
            //D(cout << "HttpRequestProcessor::dispatch() put" << endl;)
            ret = put(*httpRequest, *httpResponse);
        }
        else
        {
            //D(cout << "HttpRequestProcessor::dispatch() notImplemented" << endl;)
            notImplemented(*httpResponse);
			ret = FALSE;
        }
	}
    else
	{
    	notImplemented(*httpResponse);
		ret = FALSE;
	}

	//D(cout << "HttpRequestProcessor::dispatch() END" << endl)

	return ret;
}


int HttpRequestProcessor::run()
{
  	//HttpResponse	httpResponse(socket);
	HttpResponse	httpResponse(*appSocket);
	HttpRequest		httpRequest(getSessionManager(),*this,httpResponse);

	httpResponse.setRequest(&httpRequest);
    //httpRequest.build(socket,&sinRemote);

	httpRequest.build(*appSocket);

	return dispatch(httpRequest, httpResponse);
}


/**
 * Send 501 'Not Implemented' headers and HTML back to the client.
 */
void HttpRequestProcessor::notImplemented(HttpResponse& response)
{
	response.setStatus(HttpResponse::status_NOT_IMPLEMENTED);
    response.setContentType(*TextHtml_CONTENT_TYPE);

	if ( response.writeChars("<HTML>\r\n") )
		if ( response.writeChars("<HEAD><TITLE>Not Implemented</TITLE>\r\n") )
			if ( response.writeChars("</HEAD>\r\n") )
				if ( response.writeChars("<BODY>") )
					if ( response.printf("<H2>HTTP Error %d: Not Implemented</H2>\r\n",HttpResponse::status_NOT_IMPLEMENTED) )
						response.writeChars("</BODY></HTML>\r\n");
}

/**
 * Send 404 'Not Found' headers and HTML back to the client.
 */
void HttpRequestProcessor::notFound(HttpResponse& response)
{
	response.setStatus(HttpResponse::status_NOT_FOUND);
    response.setContentType(*TextHtml_CONTENT_TYPE);

	if ( response.writeChars("<HTML>\r\n") )
		if ( response.writeChars("<HEAD><TITLE>File Not Found</TITLE>\r\n") )
			if ( response.writeChars("</HEAD>\r\n") )
				if ( response.writeChars("<BODY>") )
					if ( response.printf("<H2>HTTP Error %d: File Not Found</H2>\r\n",HttpResponse::status_NOT_FOUND) )
						response.writeChars("</BODY></HTML>\r\n");
}

/**
 * Send the file back to the client.
 *
 * @ret TRUE if the file was successfully sent, FALSE otherwise.
 */
bool HttpRequestProcessor::sendFile(HttpResponse& response, TCHAR const * fileName)
{
	bool	ret;
    cout << "HttpRequestProcessor::sendFile() started" << flush;

	// We check for '.' so that paths can't be formed that reference files
	// outside of the docroot (like ..\file.txt).
	while ( (*fileName == '/') || (*fileName == '\\') || (*fileName == '.') )
    	fileName++;

	if ( *fileName == 0 )
		fileName = defaultDocument;

	int		docRootLen,fileNameLen;

#ifdef	_WIN32_WCE
	docRootLen = wcslen(documentRoot);
	fileNameLen = wcslen(fileName) + 1;
#else
	docRootLen = strlen(documentRoot);
	fileNameLen = strlen(fileName) + 1;
#endif

	HANDLE	handle;
	TCHAR	filePath[MAX_PATH];
	if ( (docRootLen + fileNameLen + 1) >= (sizeof (filePath) / sizeof (TCHAR)) ) // add 1 for the \ if it is missing
		handle = NULL;
	else
	{
		// Build the path to the file from the documentRoot and the fileName.
		#ifdef	_WIN32_WCE
			wcscpy(filePath,documentRoot);
		#else
			strcpy(filePath,documentRoot);
		#endif

		// If the \ is missing, add it.
		if ( filePath[docRootLen-1] != '\\' )
			filePath[docRootLen] = '\\';

		#ifdef	_WIN32_WCE
			wcscat(filePath,fileName);
		#else
			strcat(filePath, fileName);
			SetErrorMode(SEM_NOOPENFILEERRORBOX);
		#endif

		//printf("filePath= '%s'\n",filePath);
		handle = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	}

	printf("%d of %s, handle= %p\n",__LINE__,__FILE__,handle);

	if ( handle == INVALID_HANDLE_VALUE )
	{
		notFound(response);
		D(cout << "HttpRequestProcessor::sendFile() Cound NOT open fileName: '" << fileName << "'" << endl)
		ret = FALSE;
	}
	else
	{
		DWORD 		size = GetFileSize(handle,NULL);
		DWORD		read;
		DWORD		sent = 0;
		const long	len = 32768;
		byte		buf[len];

		ret = TRUE;
		response.setContentLength(size);
		response.setContentType(guessContentTypeFromName(fileName));

		//D(cout << endl << "Content Type: '" << response.getContentType() << "'" << endl << endl)

		do
		{
			ReadFile( handle, buf, len, &read, FALSE );

			if ( read )
			{
				ret = response.writeBytes(buf,read);
				sent += read;

				if ( sent >= size )
					break;
			}
		} while ( ret && (read > 0) );

		CloseHandle(handle);
	}

	return ret;
}

#ifdef	_WIN32_WCE	// [
/**
 * Send the file back to the client.
 *
 * @ret TRUE if the file was successfully sent, FALSE otherwise.
 */
bool HttpRequestProcessor::sendFile(HttpResponse& response, const char * fileName)
{
	bool	ret;
	TCHAR * fName;

	fName = cvt(fileName, &fName);

	if ( fName )
	{
		ret = sendFile(response, fName);
		delete[] fName;
	}
	else
		ret = false;

	return ret;
}
#endif				// ]

/**
 * Handle a HTTP GET method.
 *
 * @ret bool TRUE/FALSE success indicator
 */
#include <string>
bool HttpRequestProcessor::get(HttpRequest& request, HttpResponse& response)
{
	bool		 ret;
    D(cout << "HttpRequestProcessor::get() started" << endl;)
    const char* fileName = request.getUri();
    D(cout << "HttpRequestProcessor::get() filename: " << fileName << endl;)
    char fullfile[32000];
    strcpy(fullfile, fileName);
    D(cout << "HttpRequestProcessor::get() fullfile: " << fullfile << endl;)
        if (fullfile) {
        D(cout << "Sending response..." << endl;)
        ret = sendFile(response, fullfile);
    }
    else {
        D(cout << "WARNING: No file found!!!" << endl;)
            ret = FALSE;
    }
	return ret;
}

/**
 * Handle a HTTP POST method.
 *
 * @ret bool TRUE/FALSE success indicator
 */
bool HttpRequestProcessor::post(HttpRequest& request, HttpResponse& response)
{
	bool		 ret;
    D(cout << "HttpRequestProcessor::post() started" << endl;)
    const char* fileName = request.getUri();
    D(cout << "HttpRequestProcessor::post() filename: " << fileName << endl;)
    char fullfile[32000];
    strcpy(fullfile, fileName);
    D(cout << "HttpRequestProcessor::post() fullfile: " << fullfile << endl;)
    
	if (fullfile)
		ret = sendFile(response, fullfile);
	else
		ret = FALSE;

	return ret;
}

/**
 * Handle a HTTP PUT method.
 *
 * @ret bool TRUE/FALSE success indicator
 */
bool HttpRequestProcessor::put(HttpRequest& request, HttpResponse& response)
{
	(void)request;
	notImplemented(response);

	return FALSE;
}

bool HttpRequestProcessor::hasExtension(const TCHAR * name, const TCHAR * ext)
{
	const TCHAR * cp;

#ifdef	_WIN32_WCE	// [

	if ( (cp = wcsrchr( name, '.' )) == NULL )
		return ( FALSE );

	if ( *ext == '.' )		// skip over the period, if present
		ext++;

    return !_wcsicmp( ext, cp+1 );
#else				// ][

	if ( (cp = strrchr( name, '.' )) == NULL )
		return ( FALSE );

	if ( *ext == '.' )		/* skip over the period, if present */
		ext++;

    return !_stricmp( ext, cp+1 );
#endif				// ]
}

const TCHAR& HttpRequestProcessor::guessContentTypeFromName(const TCHAR * name)
{
	if( hasExtension(name,TEXT(".html")) || hasExtension(name,TEXT(".htm")) )
	{
		return *TEXT("text/html");
	}
	else if ( hasExtension(name,TEXT(".txt")) || hasExtension(name,TEXT(".java")) )
	{
		return *TEXT("text/plain");
	}
	else if ( hasExtension(name,TEXT(".gif")) )
	{
		return *TEXT("image/gif");
	}
	else if ( hasExtension(name,TEXT(".class")) )
	{
		return *TEXT("application/octet-stream");
	}
	else if ( hasExtension(name,TEXT(".jpg")) || hasExtension(name,TEXT(".jpeg")) )
	{
		return *TEXT("image/jpeg");
	}
	else if ( hasExtension(name,TEXT(".png")) )
	{
		return *TEXT("image/png");
	}
	else if ( hasExtension(name,TEXT(".bmp")) )
	{
		return *TEXT("image/bmp");
	}
	else return *TEXT("text/html"); //return "text/plain";
}

DWORD WINAPI HttpHandler(void * rp)
{
	//D(cout << "HttpHandler() ENTERED with rp= " << rp << endl;)

    RequestProcessor	* requestProcessor = (RequestProcessor *)rp;
    int					  nRetval = requestProcessor->run(); //0;

	//D(cout << "HttpHandler() requestProcessor->run() returned: " << nRetval << endl;)
    delete requestProcessor;

	//D(cout << "after delete" << endl;)

    return nRetval;
}


//---------------------------------------------------------------------------
//#pragma package(smart_init)
