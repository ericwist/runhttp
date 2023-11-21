//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
#ifndef HttpCookieH
#define HttpCookieH

#include "WServer.h"

class WSERVER_API HttpCookie
{
public:
    HttpCookie(const char& name, const char& value);
    HttpCookie(HttpCookie& cookie);
    ~HttpCookie();

    HttpCookie* next;

    const char* getComment();
    const char* getDomain();
    const char* getValue();
    const char* getName();
    const char* getPath();
    int getMaxAge();
    int getVersion();
    bool getSecure();
    void setComment(const char* comment);
    void setDomain(const char* pattern);
    void setMaxAge(int age);
    void setPath(const char* path);
    void setSecure(bool flag);
    void setValue(const char& value);
    void setVersion(int version);

    /**
     * Return the HTTP header name to set the cookie, based on cookie version.
     * @ret char& - a reference to the char header value.
     */
    const char& getHeaderName();

    /**
     * Return the header value used to set this cookie
     */
    const char& getHeaderValue();

private:
    bool isToken(const char& value);
    void maybeQuote(const char& value);

    char* name;
    char* value;
    char* comment;
    char* domain;
    char* path;
    char header[256] = {};
    bool secure;
    int maxAge;
    int version;
    //
    // from RFC 2068, token special case characters
    //
    static const char& tspecials;
};

//---------------------------------------------------------------------------
#endif
