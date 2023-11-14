//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023

#ifndef HttpResponseH
#define HttpResponseH

#include "WServer.h"
#include "Response.h"
#include "HttpRequest.h"
#include "HttpCookie.h"
#include "HeaderManager.h"

#define TextPlain_CONTENT_TYPE	"text/plain"
#define TextHtml_CONTENT_TYPE	"text/html"
#define DEFAULT_CONTENT_TYPE	TextHtml_CONTENT_TYPE
#define DEFAULT_CHAR_ENCODING	"8859_1"


class WSERVER_API HttpResponse : public Response, public HeaderManager
{
	friend class HttpRequest;

public:
	/**
	 * Create an HttpResponse that uses the supplied AppSocket to output the response
	 * data.
	 *
	 * @param appSocket AppSocket, the AppSocket to use to output the response data.
	 */
	//HttpResponse(SOCKET socket);
	HttpResponse(AppSocket& appSocket);
	/*virtual*/ ~HttpResponse();

	/**
	 * Sets a response header with the given name and value. If the header
	 * had already been set, the new value overwrites the previous one, otherwise
	 * a new header is created. The getHeader method can be used to test for
	 * the presence of a header before setting its value.
	 * Over ridden from HeaderManager so that checkSpecialHeader() can be called first.
	 *
	 * @param name - const char&, a reference to the name of the header.
	 * @param value - const char&, a reference to the value of the header.
	 */
	void setHeader(const char& name, const char& value);

	/**
	 * Adds a response header with the given name and value. This method allows
	 * response headers to have multiple values.
	 * Over ridden from HeaderManager so that checkSpecialHeader() can be called first.
	 *
	 * @param name - const char&, a reference to the name of the header.
	 * @param value - const char&, a reference to the value of the header.
	 */
	void addHeader(const char& name, const char& value);

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
    bool sendRedirect(const char& location);

	/**
	 * Add the specified cookie to the response. This method can be called multiple
	 * times to set more than one cookie.
	 *
	 * @param cookie HttpCookie&, a referene to the HttpCookie that contains the cookie info.
	 *	This function copies the info. If the caller allocated the cookie, the caller has the
	 *	responsibility of deleting it.
	 */
	void addCookie(HttpCookie& cookie);

	/**
	 * Returns a boolean indicating if the response has been committed. A commited
	 * response has already had its status code and headers written.
	 *
	 * @ret bool - TRUE/FASE indicator if the response has been committed
	 */
	bool isCommitted();

	/**
	 * If the response has not been committed clears any data that exists in the buffer as well as the status code and headers.
	 * If the response has been committed, this method returns false.
	 *
	 * @ret bool - FALSE if the response has already been committed, TRUE otherwise.
	 */
	bool reset();

	/**
	 * Set content type - this might also set encoding, if specified
	 *
	 * @param contentType - const char&, a reference to the content type.
	 */
	void setContentType(const char& contentType);

#ifdef	_WIN32_WCE	// [
	/**
	 * Set content type - this might also set encoding, if specified
	 *
	 * @param contentType - const char&, a reference to the content type.
	 */
	void setContentType(const TCHAR& contentType);
#endif				// ]

	/**
	 * Get the content type.
	 *
	 * @ret const char * - a reference to the content type, or NULL. Callers
	 *		must not delete or alter this value.
     */
	const char * getContentType();

    /**
     * Set the contentLength.
     *
     * @param contentLength - int, the content length;
     */
	void setContentLength(int contentLength);

	/**
     * Get the contentLength.
     *
     * @ret int - The connt length.
     */
	int getContentLength() ;

    /**
     * Set the status.
     *
     * @param status - int, the status.
     */
	void setStatus(int status);

    /**
     * Get the status.
     *
     * @ret int - The status.
     */
	int getStatus();

    /**
     * Write the specified length of bytes to the socket.
     * If not committed, will write the headers and set committed to true
     * before writing the bytes.
     *
     * @param bytes - BYTE *, the bytes to write.
     * @param len - int, the length of the bytes to write.
	 * @ret bool - FALSE if errors are encountered.
     */
	bool writeBytes(const BYTE * bytes, int len);

    /**
     * Write the specified characters to the socket.
     * If not committed, will write the headers and set committed to true
     * before writing the characters.
     *
     * @param str - char *, the string of characters to write.
	 * @ret bool - FALSE if errors are encountered.
     */
	bool writeChars(const char * str);

    /**
     * Formatted print to the socket.
     * If not committed, will write the headers and set committed to true
     * before printing.
     *
	 * @ret param fmt - char *, the format string followed by the comma delimited
     *                      print parameter list.
	 * @ret bool - FALSE if errors are encountered.
     */
    bool printf(const char *fmt, ...);

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
	Response& operator << (const char * str);

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
	Response& operator << (const char c);

	/**
	 * Stream the int out the socket as a string.
     * If not committed, will write the headers and set committed to true
     * before writing the characters.
	 *
	 * @param val - int, the int to write.
	 * @ret HttpResponse& - a reference to this HttpResponse.
	 */
	Response& operator << (const int val);

/*	// Add the following operator << overrides:
	bool
	short
	unsigned short
	int
	unsigned int
	long
	unsigned long
	float
	double
	long double
*/
	/**
	 * Set the HttpRequest associated with this response. If NON NULL, it will be
	 * used if URL rewriting is requested.
	 * @param request HttpRequest * the HttpRequest object to associate with this HttpResponse.
	 */
	void setRequest(HttpRequest * request);

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
	 *		or if rewriting is unnecessary or disabled, the original URL string.
	 *		The caller has the responsibilit of deleting this char pointer.
	 */
	char * rewriteURL(const char& url);

	static const int status_CONTINUE;
	static const int status_SWITCHING_PROTOCOLS;
	static const int status_OK;
	static const int status_CREATED;
	static const int status_ACCEPTED;
	static const int status_NON_AUTHORITATIVE_INFORMATION;
	static const int status_NO_CONTENT;
	static const int status_RESET_CONTENT;
	static const int status_PARTIAL_CONTENT;
	static const int status_MULTIPLE_CHOICES;
	static const int status_MOVED_PERMANENTLY;
	static const int status_MOVED_TEMPORARILY;
	static const int status_SEE_OTHER;
	static const int status_NOT_MODIFIED;
	static const int status_USE_PROXY;
	static const int status_TEMPORARY_REDIRECT;
	static const int status_BAD_REQUEST;
	static const int status_UNAUTHORIZED;
	static const int status_PAYMENT_REQUIRED;
	static const int status_FORBIDDEN;
	static const int status_NOT_FOUND;
	static const int status_METHOD_NOT_ALLOWED;
	static const int status_NOT_ACCEPTABLE;
	static const int status_PROXY_AUTHENTICATION_REQUIRED;
	static const int status_REQUEST_TIMEOUT;
	static const int status_CONFLICT;
	static const int status_GONE;
	static const int status_LENGTH_REQUIRED;
	static const int status_PRECONDITION_FAILED;
	static const int status_REQUEST_ENTITY_TOO_LARGE;
	static const int status_REQUEST_URI_TOO_LONG;
	static const int status_UNSUPPORTED_MEDIA_TYPE;
	static const int status_REQUESTED_RANGE_NOT_SATISFIABLE;
	static const int status_EXPECTATION_FAILED;
	static const int status_INTERNAL_SERVER_ERROR;
	static const int status_NOT_IMPLEMENTED;
	static const int status_BAD_GATEWAY;
	static const int status_SERVICE_UNAVAILABLE;
	static const int status_GATEWAY_TIMEOUT;
	static const int status_HTTP_VERSION_NOT_SUPPORTED;

protected:
	char	* contentType;
    char	* contentLanguage;
    char	* characterEncoding;
    int		  contentLength;
    int		  status;
    bool	  committed;

	/**
	 * Set the Session associated with this response. If NON NULL, response data
	 * will be written to the socket connection in the form of a Cookie, or URL
	 * rewriting will be used to store the session ID.
	 * @param session Session * the Session object to associate with this HttpResponse.
	 */
	void setSession(Session * session);

	/**
	 * If not committed, will write the headers and set committed to true.
	 *
	 * @ret bool - FALSE if errors are encountered.
	 */
	bool writeHeaders();

//	/**
//	 * Set internal fields for special header names. Called from set/addHeader.
//	 * Return true if the header is special, no need to set the header.
//	 *
//	 * @param name - string, a reference to the name of the header.
//	 * @param value - string, a reference to the value of the header.
//	 * @ret bool - FALSE if errors are encountered.
//	 */
//	bool checkSpecialHeader(const string& name, const string& value);

	/**
	 * Set internal fields for special header names. Called from set/addHeader.
	 * Return true if the header is special, no need to set the header.
	 *
	 * @param name - const char&, a reference to the name of the header.
	 * @param value - const char&, a reference to the value of the header.
	 * @ret bool - FALSE if errors are encountered.
	 */
	bool checkSpecialHeader(const char& name, const char& value);

private:
	/**
	 * Used to write response data to the socket connection
	 */
	//AppSocket	appSocket;
	AppSocket&	appSocket;

	/**
	 * The HttpRequest object that initiated the response or NULL.
	 */
	HttpRequest * request;

	/**
	 * This Response's Session, or NULL if no Session exists.
	 */
	Session * session;

    /**
     * Rewrite the URL by encoding the specified session id into the specified URL,
     * and return the resulting string.
     *
	 * @param id const char&, a reference to the session id to be encoded.
	 * @param url const char& a reference to the URL to be encoded with the session id.
	 * @ret char * a pointer to a newly allocated string which contains the rewriten URL.
	 *		The caller has the responsibilit of deleting this char pointer.
     */
    char * rewriteURL(const char& id, const char& url);
};

//---------------------------------------------------------------------------
#endif
