//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
//#pragma hdrstop

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "SessionIdGenerator.h"
#include "HttpRequestProcessor.h"
#include "StdRedirect.h"

//#include "debugson.h"


/**
 * Create an HttpRequest that has its session mananged by the supplied
 * SessionManager.
 * @param sessionManager SessionManager&, a reference to the SessionManager which
 *	will manage this request's sessions.
 * @param requestProcessor HttpRequestProcessor&, a reference to the HttpRequestProcessor
 *	which is processing this request.
 * @param response HttpResponse&, a reference to the HttpResponse object used
 *	to respond to this request.
 */
HttpRequest::HttpRequest(SessionManager& sessionManager, HttpRequestProcessor& requestProcessor, HttpResponse& response) : serverName(NULL),
	method(NULL), uri(NULL), query(NULL), protocol(NULL),
	serverPort(-1), remoteAddr(NULL), cookieList(NULL), session(NULL),
	sessionManager(sessionManager), requestProcessor(requestProcessor),
	response(response), appSock(NULL)
{
	// ZZZ this seems to cause problems when this object is deleted while the std is being
	// restored if some other code tries to write to cout or cerr (because of an error or something).
	// This is happening inside ws-util.cpp in ShutdownConnection();


#ifndef	_WIN32_WCE	// [
	 stdRedirect = NULL;
#endif				// ]
}

HttpRequest::~HttpRequest()
{
	if ( serverName )
		delete[] serverName;

	if ( remoteAddr )
		delete[] remoteAddr;

	if ( method )
		delete[] method;

	if ( uri )
		delete[] uri;

	if ( query )
		delete[] query;

	if ( protocol )
		delete[] protocol;

    HttpCookie * cookie = cookieList;

    while ( cookie )
    {
		cookieList = cookie->next;
        delete cookie;
        cookie = cookieList;
    }

	if ( session )
		session->removeReference();

#ifndef	_WIN32_WCE	// [
	if ( stdRedirect )
		delete stdRedirect;
#endif				// ]
}

/**
 * Get the request method.
 * @return const char *, a character pointer to the method string, or NULL. Callers must NOT modify or delete this constant value.
 */
const char * HttpRequest::getMethod()
{
	return method;
}

/**
 * Set the method.
 * @param const char&, a character reference to the method string.
 */
void HttpRequest::setMethod(const char& method)
{
	if ( this->method )
		delete[] this->method;

	this->method = new char[strlen(&method)+1];
	strcpy(this->method, &method);
}

/**
 * Get the request URI.
 * @return const char *, a character pointer to the URI string, or NULL. Callers must NOT modify or delete this constant value.
 */
const char * HttpRequest::getUri()
{
	return uri;
}

/**
 * Set the request URI.
 * @param const char&, a character refernece to the URI string.
 */
void HttpRequest::setUri(const char& uri)
{
	if ( this->uri )
		delete[] this->uri;

	this->uri = new char[strlen(&uri)+1];
	strcpy(this->uri, &uri);
}

/**
 * Get the request query.
 * @return const char *, a character pointer to the query string, or NULL. Callers must NOT modify or delete this constant value.
 */
const char * HttpRequest::getQuery()
{
	return query;
}

/**
 * Concatenate a query string to the existing query string and add
 * any new parameters to the parameter set.
 *
 * @param newQuery const char&, a reference to the query string to add to the existing query.
 */
void HttpRequest::addQueryString(const char& newQuery)
{
	int			  len = query ? strlen(query)+1 : 0;
	const char	* tquery = &newQuery;

	if ( *tquery == '?' )
		tquery++;

	len += strlen(tquery) + 1;

	char * oldQuery = query;

	query = new char[len];

	if ( oldQuery )
	{
		strcpy(query, oldQuery);
		delete[] oldQuery;
	}
	else
		query[0] = 0;

	strcat(query, tquery);
	parseQueryString(query,&parameters);
}

/**
 * Get the protocol.
 * @return const char *, a character pointer to the protocol string, or NULL. Callers must NOT modify or delete this constant value.
 */
const char * HttpRequest::getProtocol()
{
	return protocol;
}

/**
 * Set the protocol.
 * @param const char&, a character reference to the protocol string.
 */
void HttpRequest::setProtocol(const char& protocol)
{
	if ( this->protocol )
		delete[] this->protocol;

	this->protocol = new char[strlen(&protocol)+1];
	strcpy(this->protocol, &protocol);
}

/**
 * Get the host name of the server that received the request. This is same as the value
 * of the CGI variable SERVER_NAME.
 * @return const char *, a character pointer to the string containing the name of the
 *	server to which the request was sent, or NULL. Callers must NOT modify or delete this constant value.
 */
const char * HttpRequest::getServerName()
{
	if ( !serverName )
	{
		const char	* server = getHeader(*Host_Header);

		if ( server )
		{
			const char	* cp = strrchr(server,':');
			int			  len;

			if ( cp != NULL )
			{
				serverPort = strtol(cp+1,NULL,10);
				len = (cp-server);
				//cout << "len= " << len << " diff= " << (cp-server) << endl;
			}
			else
			{
				len = strlen(server);
				serverPort = DefaultServerPort;
			}

			serverName = new char[len+1];
			strncpy(serverName, server, len);
			serverName[len] = 0;
		}
	}

	return serverName;
}

/**
 * Get the port number on which this request was received. This is the same as the
 * value of the CGI variable SERVER_PORT.
 * @ret int - an integer specifying the port number.
 */
int HttpRequest::getServerPort()
{
	if ( serverPort == -1 )
		getServerName();

	return serverPort;
}

/**
 * Get the Internet Protocol (IP) address of the client that sent the request. This is the
 * same as the value of the CGI variable REMOTE_ADDR.
 * @return const char *, a character pointer to the string containing the IP address of the
 * client that sent the request, or NULL. Callers must NOT modify or delete this constant value.
 */
const char * HttpRequest::getRemoteAddr()
{
	return remoteAddr;
}

/**
 * Get a list containing all of the HttpCookie objects the client sent with this request.
 * This method will return NULL if no cookies were sent
 * @ret const HttpCookie * - A linked list of HttpCookie objects representing the cookies
 *	that the client sent, or NULL if no cookies were sent. Callers must NOT modify or delete this
 *	constant value, or the contents of this constant list.
 */
HttpCookie * HttpRequest::getCookieList()
{
	return cookieList;
}

/**
 * Return a reference to the HttpRequestProcessor that is processing this request.
 *
 * @ret HttpRequestProcessor&, a reference to the HttpRequestProcessor
 *	that is processing this request.
 */
HttpRequestProcessor& HttpRequest::getRequestProcessor()
{
	return requestProcessor;
}

/**
 * Get the current Session associated with this request or, if if there is no
 * current session and create is true, create and return a new session.
 * If create is false and the request has no valid Session, this method returns null.
 * To make sure the session is properly maintained, you must call this method before
 * the response is committed.
 * @param create bool, TRUE - to create a new session for this request if necessary;
 *	FALSE to return NULL if there's no current session.
 * @ret const Session * the Session associated with this request or NULL if create is false
 *	and the request has no valid session
 */
Session * HttpRequest::getSession(bool create)
{
	D(cout << "HttpRequest::getSession() ENTERED with session= " << session << endl)

	if ( !session )
	{
		HttpCookie	* cookie_List = getCookieList();
		const char	* sessionID = NULL;
		bool		  idFromCookie = FALSE;

		// First look for the sessionID in the cookies.
		while ( cookie_List )
		{
			if ( !strcmp(cookie_List->getName(),Session_Cookie_Name) )
			{
				sessionID = cookieList->getValue();
				idFromCookie = TRUE;
				break;
			}
			cookie_List = cookie_List->next;
		}

		D(cout << "HttpRequest::getSession() 1 sessionID= '" << (sessionID ? sessionID : "Null") << "'" << endl)

		// If the sessionID couldn't be foundin the cookies, look for in the URL params.
		if ( !sessionID )
		{
			sessionID = getParameter(*Session_Parameter_Name);
			D(cout << "HttpRequest::getSession() 2 sessionID= '" << (sessionID ? sessionID : "Null") << "'" << endl)
		}

		if ( sessionID )
		{
			session = sessionManager.findSession(*sessionID /*, TRUE*/);
			// If we were able to find a session, then it's not a new session.
			/* ZZZ sessionIsNew would have already been set in addSession() the last time around,
				otherwise the Response object would have deleted it.
			if ( session )
				session.sessionIsNew = FALSE;
			*/
		}

		D(cout << "HttpRequest::getSession() 3 with session= " << session << endl)
		if ( !session && create )
		{
			session = sessionManager.createSession();

/*			// ZZZ Changed to below... 
			if ( session && sessionID )
			{
				session->setSessionID(*sessionID);
				sessionManager.addSession(*session);
			}
*/
			if ( session )
			{
				if ( sessionID )
					session->setSessionID(*sessionID);
				sessionManager.addSession(*session);
			}
		}

		if ( session )
		{
			session->addReference();
			session->setIDFromCookie(idFromCookie);
			response.setSession(session);
		}
	}

	D(cout << "HttpRequest::getSession() END with session= " << session << "ID= '" << (session ? &session->getSessionID() : "undefined") << "'" << endl)

	return session;
}

/**
 * Forward this request to the specified path. The path must be a path
 * to an addressable resource on this server. It can be a static file
 * or a servlet. Example "/doc/index.html" or "/servlets/myServlet?parm1=value@param2=value2".
 * All request headers and parameters are reserved and if the path itself contains
 * any parameters, they are added to the paramater list.
 *
 * @param path const char&, a reference to the path to foward this request onto.
 * @ret bool TRUE if the request could be successfully forwarded, false otherwise.
 */
bool HttpRequest::forward(const char& path)
{
	char * p = (char *)&path;
	char * question = (char *)strchr(p,'?');
    D(cout << "HttpRequest::forward() started" << endl;)
	if ( question )
	{
		addQueryString(*(question+1));
		*question = 0;
	}

	setUri(path);
	if ( question )
		*question = '?';

	return requestProcessor.dispatch(*this,response);
}

/**
 * Include the content of a resource. The path must be a path to an addressable resource
 * on this server. It can be a static file or a servlet. Example "/doc/index.html" or
 * "/servlets/myServlet?parm1=value@param2=value2". All request headers and parameters
 * are reserved and if the path itself contains any parameters, they are added to the
 * paramater list. If a servlet is included it cannot change the response status code
 * or set headers; any attempt to make a change is ignored.
 *
 * @param path const char&, a reference to the path of the resource to include.
 * @ret bool TRUE if the resource could be successfully included, false otherwise.
 */
bool HttpRequest::include(const char& path)
{
	// Force the writting of the headers if they've not been written yet. This will set
	// the response to committed so that any further attempt to writeHeaders will be ignored.
    D(cout << "HttpRequest::include() started" << endl;)

	response.writeHeaders();
	return forward(path);
}

/**
 * Get the AppSocket used to build this request, or NULL.
 *
 * @ret AppSocket *, the AppSocket used to build this request, or NULL. Callers
 *		must NOT try to delete this.
 */
AppSocket * HttpRequest::getAppSocket()
{
	return appSock;
}

bool HttpRequest::scanRequest(AppSocket& appSocket)
{
	char	method[50];
    bool	ret;

    // get the method.

    // skip spaces
    do
    {
		//printf("%d of %s\n",__LINE__,__FILE__);
        ret = appSocket.readByte(method);
		//printf("%d of %s\n",__LINE__,__FILE__);
    } while( ret && isspace(*method) );

    if ( !ret )
    	return false;

    int	i, ch = *method;

	for ( i = 0; ret && (i < sizeof (method)); ret = appSocket.readByte(&method[i]) )
    {
	    ch = method[i];

		if ( isspace(ch) )
        	break;
        else
        	i++;
  	}

	if ( !ret )
    	return false;

    method[i] = 0;

    // get the uri & query.
	char	uri[SMALL_LENGTH];

    // skip spaces
    do
    {
		ret = appSocket.readByte(uri);
    } while( ret && isspace(*uri) );

    if ( !ret )
    	return false;

    char * query = NULL;

    ch = *uri;
	for ( i = 0; ret && (i < SMALL_LENGTH); ret = appSocket.readByte(&uri[i]) )
    {
	    ch = uri[i];
    	if ( (ch == '?') && !query )
        {
      		uri[i++] = 0;
      		query = &uri[i];
    	}
        else if ( isspace(ch) )
        {
        	break;
        }
        else
        	i++;
  	}

	if ( !ret )
		return false;

    uri[i] = 0;

    // get the protocol.
	char	protocol[50];

    // skip
    do
    {
		ret = appSocket.readByte(protocol);
    } while( ret && *protocol == ' ' || *protocol == '\t' );

    if ( !ret )
    	return false;

	ch = *protocol;

	for ( i = 0; ret && (i < sizeof (protocol)); ret = appSocket.readByte(&protocol[i]) )
    {
	    ch = protocol[i];

		if ( isspace(ch) )
        	break;
        else
        	i++;
  	}

	if ( !ret )
    	return false;

    protocol[i] = 0;

    char	tmp[1];
    // skip to end of line
    do
    {
		ret = appSocket.readByte(tmp);
    } while( ret && *tmp != '\n' );


	setMethod(*method);

	setUri(*uri);

	setProtocol(*protocol);

	if ( query )
		addQueryString(*query);



	parseQueryString(query,&parameters);


    return ret;
}

bool HttpRequest::scanHeaders(AppSocket& appSocket)
{
	char	buf[SMALL_LENGTH];
    int		i, ch;
    bool	ret;

	while ( (ret = appSocket.readByte(buf)) == TRUE )
    {
	    char		* key = buf;
    	char		* value = NULL;

        ch = *buf;

	    if (ch == '\n')
        {
        	return true;
	    }
    	else if (ch == '\r')
        {
        	/* should be '\n' */
    		appSocket.readByte(buf);
        	return true;
    	}

		for ( i = 0; ret && (i < SMALL_LENGTH); ret = appSocket.readByte(&buf[i]) )
    	{
	    	ch = buf[i];

			if ( isspace(ch) || (ch == ':') )
    	    	break;
        	else
        		i++;
	  	}

		if ( !ret || (ch != ':') || (i == SMALL_LENGTH) )
        	return false;

		buf[i++] = 0;
        value = &buf[i];

	    // skip
    	do
	    {
			ret = appSocket.readByte(&buf[i]);
	    } while( ret && buf[i] == ' ' || buf[i] == '\t' );

    	if ( !ret )
    		return false;

		for ( i++ ; (i < SMALL_LENGTH) && ((ret = appSocket.readByte(&buf[i])) == TRUE); )
    	{
	    	ch = buf[i];

			if ( ch == '\n' )
    	    	break;
			else if ( ch == '\r' )
            	buf[i] = 0;
        	else
        		i++;
	  	}
		buf[i] = 0;

		addHeader(*key, *value);

		
	}

	return ret;
}

/**
 * Process all cookies in the request headers and build the cookieList.
 */
void HttpRequest::processCookies()
{
	char ** values = getHeaderValues(*Cookie_Header);

	if ( values )
    {
    	for ( int i = 0; values[i]; i++ )
        {
			StringTokenizer tok(*values[i], *";", FALSE);
			while (tok.hasMoreTokens())
			{
				char * cookieStr = tok.nextToken();

				if ( !cookieStr )
					break;

				char * value = strchr(cookieStr, '=');    // find the equals sign

				if ( value )
				{
					char * name = cookieStr;
					*value++ = 0;

					// Trime leading and trailing spaces.
                    // These trims here are a *hack* -- this should
                    // be more properly fixed to be spec compliant
					while ( *name == ' ' )
						*name++ = 0;

					while ( *value == ' ' )
						*value++ = 0;

					int len = strlen(name);
					while ( len )
					{
						char * t = &name[len-1];
						if (*t == ' ')
							*t = 0;
						else
							break;

						len = strlen(name);
					}

					len = strlen(value);
					while ( len )
					{
						char * t = &value[len-1];
						if (*t == ' ')
							*t = 0;
						else
							break;

						len = strlen(value);
					}

					// RFC 2109 and bug, remove begining an trailing quotes
					//
					// Remove beginning.
					if ( (*value == '"') || (*value == '\'') )
						*value++ = 0;

					// Remove trailing.
					len = strlen(value);
					if ( len )
					{
						char * t = &value[len-1];
						if ( (*t == '"') || (*t == '\'') )
							*t = 0;
					}

					HttpCookie * cookie = new HttpCookie(*name, *value);

					cookie->next = cookieList;
					cookieList = cookie;
				}
				else
				{
					// we have a bad cookie.... just let it go
				}

				delete[] cookieStr;
			}

        }
		delete values;
    }
}

//bool HttpRequest::build(SOCKET socket, sockaddr_in * sinRemote)
bool HttpRequest::build(AppSocket& appSocket)
{

	appSock = &appSocket;

	//printf("%d of %s\n",__LINE__,__FILE__);
	bool ret = scanRequest(appSocket);
	//printf("%d of %s\n",__LINE__,__FILE__);

    if ( ret )
    	ret = scanHeaders(appSocket);

	processCookies();

	const char * contentType = getHeader(*ContentType_Header);

    if ( contentType )
    {
		D(cout << endl << "contentType= '" << *contentType << "'" << endl;)

        if ( strstr(contentType,AppUrlEncodedStr) )
        {
	        ret = parsePostData(appSocket);
        }
    }

	// Build the remoteAddr.
	//const char * addr = inet_ntoa(sinRemote->sin_addr);
	const char * addr = inet_ntoa(appSocket.getSockaddr_in().sin_addr);

	if ( addr )
	{
		remoteAddr = new char[strlen(addr)+1];
		strcpy(remoteAddr, addr);
	}

D(
	const char * p = getParameter(*"two");

    if ( p )
		cout << endl << "p= '" << p << "'" << endl;

	cout << "1 " << endl;
	char ** values = getParameterValues(*"two");
	cout << "2 " << endl;

	if ( values )
    {
		cout << "3 " << endl;
    	for ( int i = 0; values[i]; i++ )
        {
			cout << "4 " << endl;
        	cout << i << ": '" << values[i] << "'" << endl;
        }
		cout << "5 " << endl;
		delete values;
    }

	values = getParameterNames();

	if ( values )
    {
    	for ( int i = 0; values[i]; i++ )
        {
        	cout << "name " << i << ": '" << values[i] << "'" << endl;
        }
		delete values;
    }

	values = getHeaderNames();

	if ( values )
    {
    	for ( int i = 0; values[i]; i++ )
        {
			const char * headerValue = getHeader(*values[i]);
			if ( !headerValue )
				headerValue = "Null";

        	cout << "header " << i << ": '" << values[i] << "'= '" << headerValue << "'" << endl;
        }
		delete values;
    }

	const char	* server_Name = getServerName();
	int			  server_Port = getServerPort();

	if ( !server_Name )
		server_Name = "Null";

	cout << "server_Name= '" << server_Name << "' server_Port= " << server_Port << endl;

	const char	* remote_Addr = getRemoteAddr();

	if ( !remote_Addr )
		remote_Addr = "Null";

	cout << "remote_Addr= '" << remote_Addr << "'" << endl;

	HttpCookie * cookie_List = getCookieList();

	while ( cookie_List )
	{
		cout << "Cookie Name: '" << cookie_List->getName() << "' Value: '" << cookie_List->getValue() << "'" << endl;
		cookie_List = cookie_List->next;
	}
)

	return ret;
}

/**
 * Parses data from an HTML form that the client sends to the server using the HTTP POST
 * method and the application/x-www-form-urlencoded MIME type. The data sent by the POST
 * method contains key-value pairs. A key can appear more than once in the POST data with
 * different values. The keys and values are stored in their decoded form, so any +
 * characters are converted to spaces, and characters sent in hexadecimal notation
 * (like %xx) are converted to ASCII characters
 */
bool HttpRequest::parsePostData(AppSocket& appSocket)
{
	//D(cout << endl << "parsePostData() ENTERED" << endl;)

	const char	* contentLength = getHeader(*ContentLength_Header);
    bool	   	  ret = false;

	//D(cout << endl << "parsePostData() contentLength= " << contentLength << " " << __LINE__ << endl;)

	if ( contentLength )
	{
    	int	len = atoi(contentLength);

        if ( (len > 0) && (len <= SMALL_LENGTH) )
        {
			char	buf[SMALL_LENGTH];
		    int		i = 0;

            while( (i < len) && ((ret = appSocket.readByte(&buf[i])) == TRUE) )
            {
				i++;
            }

	    	if ( ret )
            {
	       		buf[i] = 0;
				parseQueryString(buf,&parameters);
            }
        }
    }

    return ret;
}

/**
 * Parses a query string passed from the client to the server and builds a KeyValue
 * list with key-value pairs. The query string should be in the form of
 * a string packaged by the GET or POST method, that is, it should have key-value pairs
 * in the form key=value, with each pair separated from the next by a & character. A key
 * can appear more than once in the query string with different values. The keys and values
 * are stored in their decoded form, so any + characters are converted to spaces, and
 * characters sent in hexadecimal notation (like %xx) are converted to ASCII characters.
 *
 * @param s - const char *, a pointer to the char string containing the query to be parsed;
 * @param keyValueList - a pointer to the KeyValue list pointer to store the key-value pairs in.
 */
void HttpRequest::parseQueryString(const char * s, KeyValue **keyValueList)
{
	if ( s )
	{
		static const char& t = *"&";

		for( StringTokenizer st(*s, t); st.hasMoreTokens(); )
		{
			char * key = st.nextToken();

			if ( !key )
				break;

			D(cout << endl << "**********" << endl << "HttpRequest::parseQueryString() key= '" << key << "'" << endl)

			char * val = strchr(key, '=');    // find the equals sign

			if ( val )
			{
				*val++ = 0;
				URLCoder::decode(key);
				URLCoder::decode(val);
				addParameter(*key, *val);
			}

			delete[] key;
		}
	}
}

//---------------------------------------------------------------------------
//#pragma package(smart_init)
