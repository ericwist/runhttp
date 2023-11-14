//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
//#pragma hdrstop

#ifndef	_WIN32_WCE	// [
#include <time.h>

#else				// ][
#include "utils.h"
/*
_CRTIMP char *  __cdecl _strrchr(const char *, int);
_CRTIMP int     __cdecl _stricmp(const char *, const char *);
#define	stricmp _stricmp
#define strrchr _strrchr
*/
#endif				// ]

#include "HttpResponse.h"

const int HttpResponse::status_CONTINUE = 100;
const int HttpResponse::status_SWITCHING_PROTOCOLS = 101;
const int HttpResponse::status_OK = 200;
const int HttpResponse::status_CREATED = 201;
const int HttpResponse::status_ACCEPTED = 202;
const int HttpResponse::status_NON_AUTHORITATIVE_INFORMATION = 203;
const int HttpResponse::status_NO_CONTENT = 204;
const int HttpResponse::status_RESET_CONTENT = 205;
const int HttpResponse::status_PARTIAL_CONTENT = 206;
const int HttpResponse::status_MULTIPLE_CHOICES = 300;
const int HttpResponse::status_MOVED_PERMANENTLY = 301;
const int HttpResponse::status_MOVED_TEMPORARILY = 302;
const int HttpResponse::status_SEE_OTHER = 303;
const int HttpResponse::status_NOT_MODIFIED = 304;
const int HttpResponse::status_USE_PROXY = 305;
const int HttpResponse::status_TEMPORARY_REDIRECT = 307;
const int HttpResponse::status_BAD_REQUEST = 400;
const int HttpResponse::status_UNAUTHORIZED = 401;
const int HttpResponse::status_PAYMENT_REQUIRED = 402;
const int HttpResponse::status_FORBIDDEN = 403;
const int HttpResponse::status_NOT_FOUND = 404;
const int HttpResponse::status_METHOD_NOT_ALLOWED = 405;
const int HttpResponse::status_NOT_ACCEPTABLE = 406;
const int HttpResponse::status_PROXY_AUTHENTICATION_REQUIRED = 407;
const int HttpResponse::status_REQUEST_TIMEOUT = 408;
const int HttpResponse::status_CONFLICT = 409;
const int HttpResponse::status_GONE = 410;
const int HttpResponse::status_LENGTH_REQUIRED = 411;
const int HttpResponse::status_PRECONDITION_FAILED = 412;
const int HttpResponse::status_REQUEST_ENTITY_TOO_LARGE = 413;
const int HttpResponse::status_REQUEST_URI_TOO_LONG = 414;
const int HttpResponse::status_UNSUPPORTED_MEDIA_TYPE = 415;
const int HttpResponse::status_REQUESTED_RANGE_NOT_SATISFIABLE = 416;
const int HttpResponse::status_EXPECTATION_FAILED = 417;
const int HttpResponse::status_INTERNAL_SERVER_ERROR = 500;
const int HttpResponse::status_NOT_IMPLEMENTED = 501;
const int HttpResponse::status_BAD_GATEWAY = 502;
const int HttpResponse::status_SERVICE_UNAVAILABLE = 503;
const int HttpResponse::status_GATEWAY_TIMEOUT = 504;
const int HttpResponse::status_HTTP_VERSION_NOT_SUPPORTED = 505;

/**
 * Create an HttpResponse that uses the supplied AppSocket to output the response
 * data.
 *
 * @param appSocket AppSocket, the AppSocket to use to output the response data.
 */
/*
HttpResponse::HttpResponse(SOCKET socket) : appSocket(socket),
*/
HttpResponse::HttpResponse(AppSocket& appSocket) : appSocket(appSocket),
	contentType(NULL), request(NULL),
	contentLanguage(NULL), characterEncoding(NULL),
	contentLength(-1), status(HttpResponse::status_OK), committed(FALSE), session(NULL)
{
	setContentType(*DEFAULT_CONTENT_TYPE);
}

HttpResponse::~HttpResponse()
{
	if ( contentType )
		delete[] contentType;

	if ( contentLanguage )
		delete[] contentLanguage;

	if ( characterEncoding )
		delete[] characterEncoding;

	if ( session )
	{
		removeReference(*session);
		// If the session is new, then the client hasn't joined the session
		// nor has the session been added to the SessionManager's internal list.
		// We need to delete it here.
		if ( session->isNew() )
			delete session;
	}
}

/**
 * Sets a response header with the given name and value. If the header
 * had already been set, the new value overwrites the previous one, otherwise
 * a new header is created. The getHeader method can be used to test for
 * the presence of a header before setting its value. 
 * Over ridden from HeaderManager so that checkSpecialHeader() can be called first.
 *
 * @param name - const char&, a reference to the name of the header. A copy of this value is made.
 * @param value - const char&, a reference to the value of the header. A copy of this value is made.
 */
void HttpResponse::setHeader(const char& name, const char& value)
{
	if( !checkSpecialHeader(name, value) )
    {
		HeaderManager::setHeader(name,value);


    }
}

/**
 * Adds a response header with the given name and value. This method allows
 * response headers to have multiple values.
 * Over ridden from HeaderManager so that checkSpecialHeader() can be called first.
 *
 * @param name - const char&, a reference to the name of the header. A copy of this value is made.
 * @param value - const char&, a reference to the value of the header. A copy of this value is made.
 */
void HttpResponse::addHeader(const char& name, const char& value)
{
	if( !checkSpecialHeader(name, value) )
	{
		HeaderManager::addHeader(name,value);


	}
}

/**
 * Set internal fields for special header names. Called from set/addHeader.
 * Return true if the header is special, no need to set the header.
 *
 * @param name - const char&, a reference to the name of the header.
 * @param value - const char&, a reference to the value of the header.
 * @ret bool - FALSE if errors are encountered.
 */
bool HttpResponse::checkSpecialHeader(const char& name, const char& value)
{
	if( !_stricmp(&name,ContentType_Header) )
	{
		setContentType(value);
		return true;
	}

	if( !_stricmp(&name,ContentLength_Header) )
	{
		int len = atoi(&value);

		setContentLength(len);
        return true;
	}

	return false;
}

/**
 * Returns a boolean indicating if the response has been committed. A commited
 * response has already had its status code and headers written.
 *
 * @ret bool - TRUE/FASE indicator if the response has been committed
 */
bool HttpResponse::isCommitted()
{
	return committed;
}

/**
 * If the response has not been committed clears any data that exists in the buffer as well as the status code and headers.
 * If the response has been committed, this method returns false.
 *
 * @ret bool - FALSE if the response has already been committed, TRUE otherwise.
 */
bool HttpResponse::reset()
{
	if ( committed )
    	return FALSE;

	//headers.erase(headers.begin(),headers.end());
	deleteHeaders();

	//contentType = DEFAULT_CONTENT_TYPE;
	setContentType(*DEFAULT_CONTENT_TYPE);
	//characterEncoding = DEFAULT_CHAR_ENCODING;
	contentLength = -1;
	status = HttpResponse::status_OK;

	return TRUE;
}

/**
 * Set content type - this might also set encoding, if specified
 *
 * @param contentType - const char&, a reference to the content type.
 */
void HttpResponse::setContentType(const char& contentType)
{
	if ( this->contentType )
		delete this->contentType;

	int size = strlen(&contentType)+1;

	this->contentType = new char[size];

	if ( contentType )
		strcpy(this->contentType, & contentType);
}

#ifdef	_WIN32_WCE	// [
/**
 * Set content type - this might also set encoding, if specified
 *
 * @param contentType - const char&, a reference to the content type.
 */
void HttpResponse::setContentType(const TCHAR& contentType)
{
	if ( this->contentType )
		delete this->contentType;

	this->contentType = cvt(&contentType, &this->contentType);
}
#endif				// ]

/**
 * Get the content type.
 *
 * @ret const char * - a pointer to the content type, or NULL. Callers
 *		must not delete or alter this value.
 */
const char * HttpResponse::getContentType()
{
	return contentType;
}


/**
 * Set the contentLength.
 *
 * @param contentLength - int, the content length;
 */
void HttpResponse::setContentLength(int contentLength)
{
	this->contentLength = contentLength;
}

/**
 * Get the contentLength.
 *
 * @ret int - The connt length.
 */
int HttpResponse::getContentLength()
{
	return contentLength;
}

/**
 * Set the status.
 *
 * @param status - int, the status.
 */
void HttpResponse::setStatus(int status)
{
	this->status = status;
}

/**
 * Get the status.
 *
 * @ret int - The status.
 */
int HttpResponse::getStatus()
{
	return status;
}

/**
 * Write the HTTP headers.
 *
 * @ret bool - FALSE if errors are encountered.
 */
bool HttpResponse::writeHeaders()
{
	if ( committed )
    	return FALSE;

	bool	ret;

    committed = true;

	// If a session exists, write the ID as a Cookie.
	if ( session && session->isValid() )
	{
		HttpCookie cookie(*Session_Cookie_Name, session->getSessionID());

		cookie.setMaxAge(-1);
		cookie.setVersion(0);
		addCookie(cookie);
	}

	// First write standard 'built-in' headers'
	//ret = appSocket.printf("%s: %d\r\n",Status_Header,status);
	// ZZZ The Opera browser doesn't recognize the headers if this Status_Header is here...
	ret = TRUE;

    if ( ret )
    {
	    // ZZZ Fix to use #defines for version and status codes etc. and handle all status codes.
    	char	* httpHeader;
	    switch (status)
    	{
			case HttpResponse::status_OK:
	        	httpHeader = "OK";
	            break;

			case HttpResponse::status_NOT_FOUND:
        		httpHeader = "File Not Found";
            	break;

	        case HttpResponse::status_NOT_IMPLEMENTED:
    	    	httpHeader = "Not Implemented";
        	    break;

			case HttpResponse::status_MOVED_PERMANENTLY:
	        case HttpResponse::status_MOVED_TEMPORARILY:
    	    	httpHeader = "Redirect";
        	    break;

			default:
    	    	httpHeader = "";
        	    break;
	    }

    	ret = appSocket.printf("%s %d %s\r\n",HTTPVersion,status,httpHeader);

		if ( ret )
			ret = appSocket.printf("%s: %d\r\n",Status_Header,status);

		if ( ret )
        {
			char	timebuf[SMALL_LENGTH];

#ifdef	_WIN32_WCE	// [
		SYSTEMTIME	systemTime;
		GetSystemTime( &systemTime ); 


		sprintf(timebuf,"%s, %2d %s %d %2d:%2d:%2d GMT",daysAbbrev[systemTime.wDayOfWeek],
			systemTime.wDay,monthsAbbrev[systemTime.wMonth-1],systemTime.wYear,
			systemTime.wHour,systemTime.wMinute,systemTime.wSecond);
#else				// ][
			time_t	now;

			time(&now);
		    strftime(timebuf, sizeof(timebuf),
        		     "%a, %d %b %Y %H:%M:%S GMT",
		             gmtime(&now));

#endif				// ]

			ret = appSocket.printf("%s: %s\r\n",Date_Header,timebuf);

			// ZZZ Test!!!
			// setContentLength(512);

            if ( ret )
            {
				ret = appSocket.printf("%s: %s %s\r\n",Server_Header,Server,ServerVersion);

                if ( ret )
                {
					if ( contentType )
						ret = appSocket.printf("%s: %s\r\n",ContentType_Header,contentType);

				    if ( ret && (contentLength > 0) )
	   					ret = appSocket.printf("%s: %d\r\n",ContentLength_Header,contentLength);
                }
            }
        }

	    // Now print out the user headers...
	    if ( ret )
		{
			for ( KeyValue * keyValue = headers; keyValue; keyValue = keyValue->next )
			{
				ret = appSocket.printf("%s: %s\r\n",keyValue->key,keyValue->value);
			}

        }

        if ( ret )
	        ret = appSocket.writeChars("\r\n");
	}

    return ret;
}

/**
 * Write the specified length of bytes to the socket.
 * If not committed, will write the headers and set committed to true
 * before writing the bytes.
 *
 * @param bytes - byte *, the bytes to write.
 * @param len - int, the length of the bytes to write.
 * @ret bool - FALSE if errors are encountered.
 */
bool HttpResponse::writeBytes(const byte * bytes, int len)
{
	bool	ret;

	if ( !committed )
    	ret = writeHeaders();
    else
    	ret = true;

    if ( ret )
    	ret = appSocket.writeBytes(bytes,len);

	return ret;
}

/**
 * Write the specified characters to the socket.
 * If not committed, will write the headers and set committed to true
 * before writing the characters.
 *
 * @param str - char *, the string of characters to write.
 * @ret bool - FALSE if errors are encountered.
 */
bool HttpResponse::writeChars(const char * str)
{
	bool	ret;
/**/
	if ( !committed )
    	ret = writeHeaders();
    else
/**/
    	ret = true;

    if ( ret )
    	ret = appSocket.writeChars(str);

	return ret;
}

/**
 * Formatted print to the socket.
 * If not committed, will write the headers and set committed to true
 * before printing.
 *
 * @ret param fmt - char *, the format string followed by the comma delimited
 *                      print parameter list.
 * @ret bool - FALSE if errors are encountered.
 */
bool HttpResponse::printf(const char *fmt, ...)
{
	bool	ret;

	if ( !committed )
    	ret = writeHeaders();
    else
    	ret = true;

    if ( ret )
	{
		va_list list;

		va_start(list, fmt);
    	//ret = appSocket.printf(fmt);

		char buf[256];
		/*len =*/ _vsnprintf(buf, sizeof(buf), fmt, list);

		buf[sizeof(buf)-1] = 0;
		ret = writeChars(buf);

		va_end(list);
	}

	return ret;
}

/**
 * Stream the string out the socket.
 * If not committed, will write the headers and set committed to true
 * before writing the characters.
 *
 * @param str - char *, the string of characters to write.
 * @ret HttpResponse& - a reference to this HttpResponse so streaming
 *	statments can be concatenated as in:
 *	response << "Welcome " << name << ".";
 */
Response& HttpResponse::operator << (const char * str)
{
	writeChars(str);

	return *this;
}


/**
 * Stream the character out the socket.
 * If not committed, will write the headers and set committed to true
 * before writing the characters.
 *
 * @param c - const char, the character to write.
 * @ret HttpResponse& - a reference to this HttpResponse so streaming
 *	statments can be concatenated as in:
 *	appSocket << "Welcome " << name << '.' << endl;
 */
Response& HttpResponse::operator << (const char c)
{
	appSocket << c;

	return *this;
}

/**
 * Stream the int out the socket as a string.
 * If not committed, will write the headers and set committed to true
 * before writing the characters.
 *
 * @param val - int, the int to write.
 * @ret HttpResponse& - a reference to this HttpResponse.
 */
Response& HttpResponse::operator << (const int val)
{
	//writeBytes((const byte *)&val, sizeof (val));

	// The _itoa function converts the digits of the given value argument
	// to a null-terminated character string and stores the result
	// (up to 33 bytes) in string. 
	char buf[34] = {0};

	writeChars(_itoa(val,buf,10));

	return *this;
}

/**
 * Sends a temporary redirect response to the client using the specified redirect
 * location URL. This method can accept relative URLs; the servlet container will
 * convert the relative URL to an absolute URL before sending the response to the client.
 * If the response has already been committed, this method returns FALSE and does nothing.
 * After using this method, the response should be considered to be committed and should
 * not be written to.
 *
 * @param location char& the URL to redirect to.
 * @ret bool TRUE if the response was not previously commited and could be redirected,
 *		FALSE otherwise.
 */
bool HttpResponse::sendRedirect(const char& location)
{
	bool	ret;

	if ( !committed )
	{
		setStatus(status_MOVED_TEMPORARILY);
		addHeader(*Location_Header,location);
		ret = writeHeaders();
		//ret = writeChars("\r\n");
	}
	else
		ret = FALSE;

	return ret;

}

/**
 * Rewrites the specified URL by including the session ID in it if the URL is a relative
 * URL or an absolute URL that returns to the specified host name. If URL rewriting
 * is disabled or unnecessary, returns the URL unchanged. The implementation of this method includes
 * the logic to determine whether the session ID needs to be encoded in the URL. For
 * example, if the browser supports cookies, or session tracking is turned off, URL
 * encoding is unnecessary.
 *
 * @param url const char&, a reference to the URL string to rewrite with the session ID.
 * @ret char * a pointer to a newly allocated string which contains the rewriten URL
 *		or if rewriting is unneccary or disabled, the original URL string.
 */
char * HttpResponse::rewriteURL(const char& urlRef)
{
	const char	* url = &urlRef;
	char		* retURL = NULL;

	if ( session && session->isValid() && !session->isIDFromCookie() )
	{
		//String sch=req.getScheme();
		// ZZZ FIX to handle Scheme... Scheme could be http: or https: or ftp: ect
		// Encode all relative URLs unless they start with a hash
		//if (!url.startsWith(sch+":"))

		// ZZZ Since we don't have the scheme (need to implement this?), we look
		// for :// to see if this is an absolute URL. Might not work always. Z.Z.Z 020202
		const char * match = strstr(url,"://");
		if ( !match )
		{
			if (*url != '#')
				retURL = rewriteURL(session->getSessionID(), *url);
		}
		else
		{
			if ( request )
			{
				// Encode all absolute URLs that return to this hostname
				const char * serverName = request->getServerName();

				if ( serverName )
				{
					if ( !_stricmp(&match[3],serverName ) )
						retURL = rewriteURL(session->getSessionID(), *url);
				}
			}
		}
	}

	if ( !retURL )
	{
		retURL = new char[strlen(url)+1];
		strcpy(retURL,url);
	}

	return retURL;
}

/**
 * Rewrite the URL by encoding the specified session id into the specified URL,
 * and return the resulting string.
 *
 * @param id const char&, a reference to the session id to be encoded.
 * @param url const char& a reference to the URL to be encoded with the session id.
 * @ret char * a pointer to a newly allocated string which contains the rewriten URL.
 *		The caller has the responsibilit of deleting this char pointer.
 */
char * HttpResponse::rewriteURL(const char& id, const char& inURL)
{
	const char	* url = &inURL;
	int			  len = strlen(url) + 1 + strlen(&id) + 1 +
 					strlen(Session_Cookie_Name) + 1 + 2;
	char		* retURL = new char[len];

	D(cout << "HttpResponse::rewriteURL() len= " << len << endl)

	// Locate the beginning of the query string (if any)
	const char	* question = strrchr(url,'?');

#ifdef	Embed_ID_In_URI	// [
	// Embed in URI - 'http://www.google.com/test;XSESSIONID=a4pk0uru61?param1=123'
	if ( question )
	{
		len = (question-url);
		strncpy(retURL,url,len);
		retURL[len] = 0;
	}
	else
	{
		strcpy(retURL,url);
	}

	strcat(retURL,";");
	strcat(retURL,Session_Cookie_Name);
	strcat(retURL,"=");
	strcat(retURL,&id);

	if ( question )
		strcat(retURL,question);
#else					// ][
	// Embed as a parameter - 'http://www.google.com/test?param1=123&XSESSIONID=a4pk0uru61'

	strcpy(retURL,url);

	strcat(retURL, (question ? "&" : "?"));
	strcat(retURL, Session_Cookie_Name);
	strcat(retURL,  "=");
	strcat(retURL, &id);

#endif					// ]

	D(cout << "HttpResponse::rewriteURL() strlen= " << strlen(retURL) << " retURL= '" << retURL << "'" << endl)

	return retURL;
}

/**
 * Add the specified cookie to the response. This method can be called multiple
 * times to set more than one cookie.
 *
 * @param cookie const HttpCookie&, a referene to the HttpCookie that contains the cookie info.
 */
void HttpResponse::addCookie(HttpCookie& cookie)
{
	if( cookie.getVersion() == 1 )
	{
		// add a version 0 header too.
		// XXX what if the user set both headers??
		HttpCookie cookie0(cookie);
		cookie0.setVersion(0);
		addHeader(cookie0.getHeaderName(), cookie0.getHeaderValue());
	}
	addHeader(cookie.getHeaderName(), cookie.getHeaderValue());
}


/**
 * Set the HttpRequest associated with this response. If NON NULL, it will be
 * used if URL rewriting is requested.
 * @param request HttpRequest * the HttpRequest object to associate with this HttpResponse.
 */
void HttpResponse::setRequest(HttpRequest * request)
{
	this->request = request;
}

/**
 * Set the Session associated with this response. If NON NULL, response data
 * will be written to the socket connection in the form of a Cookie, or URL
 * rewriting will be used to store the session ID.
 */
void HttpResponse::setSession(Session * session)
{
	this->session = session;
	addReference(*session);
}


//---------------------------------------------------------------------------
//#pragma package(smart_init)
