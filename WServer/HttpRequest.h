//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023

#ifndef HttpRequestH
#define HttpRequestH


#include "WServer.h"
#include "Request.h"
#include "HttpCookie.h"
#include "AppSocket.h"
#include "URLCoder.h"
#include "StringTokenizer.h"
#include "SessionManager.h"
//#include "KeyValue.h"

#include "HeaderManager.h"
#include "AsciiParameterManager.h"

#define SMALL_LENGTH 1024
#define BIG_LENGTH 8192

#define	AppUrlEncodedStr "application/x-www-form-urlencoded"

#define Accept_Header			"Accept"
#define AcceptEncoding_Header	"Accept-Encoding"
#define	AcceptLanguage_Header	"Accept-Language"
#define	Connection_Header		"Connection"
#define	ContentLength_Header	"Content-Length"
#define	ContentType_Header		"Content-Type"
#define	Host_Header				"Host"
#define	UserAgent_Header		"User-Agent"
#define	Status_Header			"Status"
#define	Date_Header				"Date"
#define	Server_Header			"Server"
#define	Cookie_Header			"Cookie"
#define	Location_Header			"Location"

#define	HTTPVersion				"HTTP/1.0"
#define	Server					"XServ"
#define	ServerVersion			"1.0"

#define	GetMethod				"GET"
#define	PostMethod				"POST"
#define	PutMethod				"PUT"

#define	Session_Cookie_Name		"XSESSIONID"
#define Session_Parameter_Name	"xsessionid"


#define	DefaultServerPort		80

#ifndef	_WIN32_WCE	// [
using namespace std;
#endif				// ]

//typedef multimap<string, string, less<string>,allocator<string> > parameter_type;

class HttpResponse;
class HttpRequestProcessor;

#ifndef	_WIN32_WCE	// [
class StdRedirect;
#endif				// ]

class WSERVER_API HttpRequest : public Request, public AsciiParameterManager, public HeaderManager
{
public:
	/**
	 * Create an HttpRequest that has its session mananged by the supplied
	 * SessionManager.
	 * @param sessionManager SessionManager&, a reference to the SessionManager which
	 *	will manage this request's sessions.
	 * @param requestProcessor HttpRequestProcessor&, a reference to the HttpRequestProcessor
	 *	which is processing this request.
	 * @param response HttpResponse&, a reference to the HttpResponse object used to
	 *	to respond to this request.
	 */
	HttpRequest(SessionManager& sessionManager, HttpRequestProcessor& requestProcessor, HttpResponse& response);
    /*virtual*/ ~HttpRequest();

    //bool build(SOCKET socket, sockaddr_in * sinRemote);
	bool build(AppSocket& appSocket);

	/**
	 * Get the request method.
	 * @return const char *, a character pointer to the method string, or NULL. Callers must NOT modify or delete this constant value.
	 */
	const char * getMethod();

	/**
	 * Get the request URI.
	 * @return const char *, a character pointer to the URI string, or NULL. Callers must NOT modify or delete this constant value.
	 */
	const char * getUri();

	/**
	 * Get the request query.
	 * @return const char *, a character pointer to the query string, or NULL. Callers must NOT modify or delete this constant value.
	 */
	const char * getQuery();

	/**
	 * Get the protocol.
	 * @return const char *, a character pointer to the protocol string, or NULL. Callers must NOT modify or delete this constant value.
	 */
	const char * getProtocol();

	/**
	 * Get the host name of the server that received the request. This is same as the value
	 * of the CGI variable SERVER_NAME.
	 * @return const char *, a character pointer to the string containing the name of the
	 *	server to which the request was sent, or NULL. Callers must NOT modify or delete this constant value.
	 */
	const char * getServerName();

	/**
	 * Returns the port number on which this request was received. This is the same as the
	 * value of the CGI variable SERVER_PORT.
	 * @ret int - an integer specifying the port number.
	 */
	int getServerPort();

	/**
	 * Get the Internet Protocol (IP) address of the client that sent the request. This is the
	 * same as the value of the CGI variable REMOTE_ADDR.
	 * @return const char *, a character pointer to the string containing the IP address of the
	 * client that sent the request, or NULL. Callers must NOT modify or delete this constant value.
	 */
	const char * getRemoteAddr();

	/**
	 * Get a list containing all of the HttpCookie objects the client sent with this request.
	 * This method will return NULL if no cookies were sent
	 * @ret const HttpCookie * - A linked list of HttpCookie objects representing the cookies
	 *	that the client sent, or NULL if no cookies were sent.
	 */
	HttpCookie * getCookieList();

	/**
	 * Return a reference to the HttpRequestProcessor that is processing this request.
	 *
	 * @ret HttpRequestProcessor&, a reference to the HttpRequestProcessor
	 *	that is processing this request.
	 */
	HttpRequestProcessor& getRequestProcessor();

	/**
	 * Get the current Session associated with this request or, if if there is no
	 * current session and create is true, create and return a new session.
	 * If create is false and the request has no valid Session, this method returns null.
	 * To make sure the session is properly maintained, you must call this method before
	 * the response is committed.
	 * @param create bool, TRUE - to create a new session for this request if necessary;
	 *	FALSE to return NULL if there's no current session.
	 * @ret Session * the Session associated with this request or NULL if create is false
	 *	and the request has no valid session
	 */
	Session * getSession(bool create);

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
	bool forward(const char& path);

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
	bool include(const char& path);

	/**
	 * Get the AppSocket used to build this request, or NULL.
	 *
	 * @ret AppSocket *, the AppSocket used to build this request, or NULL. Callers
	 *		must NOT try to delete this.
	 */
	AppSocket * getAppSocket();

protected:
    bool scanRequest(AppSocket& appSocket);
    bool scanHeaders(AppSocket& appSocket);

	/**
	 * Set the method.
	 * @param const char&, a character reference to the method string.
	 */
	void setMethod(const char& method);

	/**
	 * Set the protocol.
	 * @param const char&, a character reference to the protocol string.
	 */
	void setProtocol(const char& protocol);

	/**
	 * Set the request URI.
	 * @param const char&, a character refernece to the URI string.
	 */
	void setUri(const char& uri);

	/**
	 * Concatenate a query string to the existing query string and add
	 * any new parameters to the parameter set.
	 *
	 * @param newQuery const char&, a reference to the query string to add to the existing query.
	 */
	void addQueryString(const char& newQuery);

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
	void parseQueryString(const char * s, KeyValue **keyValueList);

	/**
	 * Process all cookies in the request headers and build the cookieList.
	 */
	void processCookies();

	/**
	 * Parses data from an HTML form that the client sends to the server using the HTTP POST
	 * method and the application/x-www-form-urlencoded MIME type. The data sent by the POST
	 * method contains key-value pairs. A key can appear more than once in the POST data with
	 * different values. The keys and values are stored in their decoded form, so any +
	 * characters are converted to spaces, and characters sent in hexadecimal notation
	 * (like %xx) are converted to ASCII characters
	 */
	bool parsePostData(AppSocket& appSocket);

	/**
	 * Sets whether the session ID of the passed in session was found from a cookie (as opposed to form within the URL).
	 *
	 * @param session - Session&, the Session object to manipulate.
	 * @param idFromCookie bool, TRUE if the session ID was found as a cookie, false otherwise.
	 */
	void setIDFromCookie(Session& session, bool idFromCookie)
	{
		session.setIDFromCookie(idFromCookie);
	}

private:
	/**
	 * The name of the server that received the request. This is same as the value
	 * of the CGI variable SERVER_NAME.
	 */
	char	* serverName;

	/**
	 * The port number on which this request was received. This is the same as the
	 * value of the CGI variable SERVER_PORT.
	 */
	int		  serverPort;

	/**
	 * The Internet Protocol (IP) address of the client that sent the request. This is the
	 * same as the value of the CGI variable REMOTE_ADDR.
	 */
	char	* remoteAddr;

	char * method, * uri, * query, * protocol;

	/**
	 * A list containing all of the HttpCookie objects the client sent with this request.
	 * This will NULL if no cookies were sent
	 */
	HttpCookie	* cookieList;

	/**
	 * The SessionManager which will manage this request's sessions.
	 */
	SessionManager& sessionManager;

	/**
	 * The HttpRequestProcessor	which is processing this request.
	 */
	HttpRequestProcessor& requestProcessor;

	/**
	 * This Request's Session, or NULL if no Session exists.
	 */
	Session * session;

	/**
	 * The HttpResponse object used to respond to this request.
	 */
	HttpResponse& response;

#ifndef	_WIN32_WCE	// [
	StdRedirect * stdRedirect;
#endif				// ]

	/**
	 * The AppSocket used to build this request, or NULL.
	 */
	AppSocket * appSock;
};


//---------------------------------------------------------------------------
#endif
