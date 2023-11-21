//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023

#ifndef HttpRequestProcessorH
#define HttpRequestProcessorH

#include "WServer.h"

#include "RequestProcessor.h"
// #include "HttpRequest.h"
#include "HttpResponse.h"

class WSERVER_API HttpRequestProcessor : public RequestProcessor
{
public:
    HttpRequestProcessor(const TCHAR* documentRoot, const TCHAR* defaultDocument, SessionManager& sessionManager);

    /**
     * Process this request.
     */
    void processRequest();

    /**
     * Run this RequestProcessor
     *
     * @ret int, 0 for success, non zero error code upon failure.
     */
    int run();

    /**
     * Dispatch this request.
     *
     //* @param request HttpRequest&, a reference to the HttpRequest that generated this dispatch.
     //* @param response HttpResponse&, a reference to the HttpResponse to use while dispatching the request.
     * @param request Request&, a reference to the HttpRequest that generated this dispatch.
     * @param response Response&, a reference to the HttpResponse to use while dispatching the request.
     * @ret bool TRUE if the request was successfully dispatched, FALSE otherwise.
     */
    virtual bool dispatch(/*Http*/ Request& request, /*Http*/ Response& response);

    /**
     * Get the default document if none is specified in the HTTP request.
     * This is set via the XML specified defaultDocumentAttrTag.
     *
     * @ret const TCHAR *, the default docment or NULL. If NON NULL, callers must NOT modify this value.
     */
    const TCHAR* getDefaultDocument();

    /**
     * Get the document root.
     * This is set via the XML specified defaultDocumentAttrTag.
     *
     * @ret const TCHAR *, the docment root or NULL. If NON NULL, callers must NOT modify this value.
     */
    const TCHAR* getDocumentRoot();

protected:
    const TCHAR& guessContentTypeFromName(const TCHAR* name);
    bool hasExtension(const TCHAR* name, const TCHAR* ext);

    /**
     * Handle a HTTP GET method.
     * @ret bool TRUE/FALSE success indicator
     */
    bool get(HttpRequest& request, HttpResponse& response);

    /**
     * Handle a HTTP POST method.
     * @ret bool TRUE/FALSE success indicator
     */
    bool post(HttpRequest& request, HttpResponse& response);

    /**
     * Handle a HTTP PUT method.
     * @ret bool TRUE/FALSE success indicator
     */
    bool put(HttpRequest& request, HttpResponse& response);

    /**
     * Send 501 'Not Implemented' headers and HTML back to the client.
     */
    void notImplemented(HttpResponse& response);

    /**
     * Send 404 'Not Found' headers and HTML back to the client.
     */
    void notFound(HttpResponse& response);

#ifdef _WIN32_WCE // [
    /**
     * Send the file back to the client.
     * @ret TRUE if the file was successfully sent, FALSE otherwise.
     */
    bool sendFile(HttpResponse& response, char const* fileName);
#endif // ]

    /**
     * Send the file back to the client.
     * @ret TRUE if the file was successfully sent, FALSE otherwise.
     */
    bool sendFile(HttpResponse& response, TCHAR const* fileName);

    /**
     * Get the SessionManager which will manage sessions.
     * @ret const SessionManager& a reference to the SessionManager which will manage sessions.
     */
    SessionManager& getSessionManager();

    // private:
    const TCHAR* documentRoot;
    const TCHAR* defaultDocument;

    /**
     * The SessionManager which will manage this request's sessions.
     */
    SessionManager& sessionManager;
};

//---------------------------------------------------------------------------
#endif
