//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023

#ifndef ResponseH
#define ResponseH

#include "Request.h"
#include "WServer.h"

class WSERVER_API Response
{
    friend class Request;

public:
    /**
     * Returns a boolean indicating if the response has been committed. A commited
     * response has already had its status code and headers written.
     *
     * @ret bool - TRUE/FASE indicator if the response has been committed
     */
    virtual bool isCommitted() = 0;

    /**
     * If the response has not been committed clears any data that exists in the buffer as well as the status code and headers.
     * If the response has been committed, this method returns false.
     *
     * @ret bool - FALSE if the response has already been committed, TRUE otherwise.
     */
    virtual bool reset() = 0;

    /**
     * Set the status.
     *
     * @param status - int, the status.
     */
    virtual void setStatus(int status) = 0;

    /**
     * Get the status.
     *
     * @ret int - The status.
     */
    virtual int getStatus() = 0;

    /**
     * Write the specified length of bytes to the socket.
     * If not committed, will write the headers and set committed to true
     * before writing the bytes.
     *
     * @param bytes - byte *, the bytes to write.
     * @param len - int, the length of the bytes to write.
     * @ret bool - FALSE if errors are encountered.
     */
    virtual bool writeBytes(const BYTE* bytes, int len) = 0;

    /**
     * Write the specified characters to the socket.
     * If not committed, will write the headers and set committed to true
     * before writing the characters.
     *
     * @param str - char *, the string of characters to write.
     * @ret bool - FALSE if errors are encountered.
     */
    virtual bool writeChars(const char* str) = 0;

    /**
     * Formatted print to the socket.
     * If not committed, will write the headers and set committed to true
     * before printing.
     *
     * @ret param fmt - char *, the format string followed by the comma delimited
     *                      print parameter list.
     * @ret bool - FALSE if errors are encountered.
     */
    virtual bool printf(const char* fmt, ...) = 0;

    /**
     * Stream the string out the socket.
     * If not committed, will write the headers and set committed to true
     * before writing the characters.
     *
     * @param str - char *, the string of characters to write.
     * @ret Response& - a reference to this Response so streaming
     *	statments can be concatenated as in:
     *	response << "Welcome " << name << ".";
     */
    virtual Response& operator<<(const char* str) = 0;

    /**
     * Stream the int out the socket.
     * If not committed, will write the headers and set committed to true
     * before writing the characters.
     *
     * @param val - int, the int to write.
     * @ret Response& - a reference to this Response.
     */
    virtual Response& operator<<(const int val) = 0;

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
     * Set the Request associated with this response.
     * @param request Request * the Request object to associate with this Response.
     */
    // virtual void setRequest(Request * request) = 0;

protected:
    /**
     * Set the Session associated with this response. If NON NULL, response data
     * will be written to the socket connection in the form of a Cookie, or URL
     * rewriting will be used to store the session ID.
     *
     * @param session Session * the Session object to associate with this Response.
     */
    virtual void setSession(Session* session) = 0;

    /**
     * Increase the reference count of the passed in session by 1. This is the reference count of objects that
     * reference this Session object (like HttpRequest & HttpResponse). Objects which
     * reference this Session object need to call addReference(), and releaseReference()
     * when finished. This is VERY important, as the SessionManager will not free any
     * invalidated Session objects with a reference count > 0.
     *
     * @param session - const Session&, the Session object to manipulate.
     * @ret int the new reference count. This value is meant to be used for diagnostic
     *	and testing purposes only.
     */
    int addReference(Session& session)
    {
        return session.addReference();
    }

    /**
     * Decrease the reference count of the passed in session by 1. This is the reference count of objects that
     * reference this Session object (like HttpRequest & HttpResponse). Objects which
     * reference this Session object need to call addReference(), and releaseReference()
     * when finished. This is VERY important, as the SessionManager will not free any
     * invalidated Session objects with a reference count > 0.
     *
     * @param session - const Session&, the Session object to manipulate.
     * @ret int the new reference count. This value is meant to be used for diagnostic
     *	and testing purposes only.
     */
    int removeReference(Session& session)
    {
        return session.removeReference();
    }
};

//---------------------------------------------------------------------------
#endif
