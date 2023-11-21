// Author: Eric Wistrand 11/12/2023
#include <windows.h>

#ifndef _WIN32_WCE // [

#include <iostream>
#include <time.h>

using namespace std;

#else // ][

#include "utils.h"
#endif // ]

#include "HttpCookie.h"

const char& HttpCookie::tspecials = *"()<>@,;:\\\"/[]?={} \t";

HttpCookie::HttpCookie(const char& name, const char& value)
: comment(NULL)
, domain(NULL)
, path(NULL)
, maxAge(-1)
, version(0)
, secure(FALSE)
, next(NULL)
{
    this->name = new char[strlen(&name) + 1];
    strcpy(this->name, &name);

    this->value = new char[strlen(&value) + 1];
    strcpy(this->value, &value);
}

HttpCookie::HttpCookie(HttpCookie& cookie)
: comment(NULL)
, domain(NULL)
, path(NULL)
, value(NULL)
, maxAge(cookie.getMaxAge())
, version(cookie.getVersion())
, secure(cookie.getSecure())
, next(NULL)
{
    if (cookie.name)
    {
        this->name = new char[strlen(cookie.name) + 1];
        strcpy(this->name, cookie.name);
    }
    else
        this->name = NULL;

    setValue(*cookie.getValue());
    setComment(cookie.getComment());
    setDomain(cookie.getDomain());
    setPath(cookie.getPath());
}

HttpCookie::~HttpCookie()
{
    if (name)
        delete[] name;

    if (value)
        delete[] value;

    if (comment)
        delete[] comment;

    if (domain)
        delete[] domain;

    if (path)
        delete[] path;
}

const char* HttpCookie::getComment()
{
    return comment;
}

void HttpCookie::setComment(const char* comment)
{
    if (this->comment)
        delete[] this->comment;

    if (comment)
    {
        this->comment = new char[strlen(comment) + 1];
        strcpy(this->comment, comment);
    }
    else
        this->comment = NULL;
}

const char* HttpCookie::getDomain()
{
    return domain;
}

void HttpCookie::setDomain(const char* domain)
{
    if (this->domain)
        delete[] this->domain;

    if (domain)
    {
        this->domain = new char[strlen(domain) + 1];
        strcpy(this->domain, domain);
    }
    else
        this->domain = NULL;
}

const char* HttpCookie::getValue()
{
    return value;
}

void HttpCookie::setValue(const char& value)
{
    if (this->value)
        delete[] this->value;

    this->value = new char[strlen(&value) + 1];
    strcpy(this->value, &value);
}

const char* HttpCookie::getName()
{
    return name;
}

const char* HttpCookie::getPath()
{
    return path;
}

void HttpCookie::setPath(const char* path)
{
    if (this->path)
        delete[] this->path;

    if (path)
    {
        this->path = new char[strlen(path) + 1];
        strcpy(this->path, path);
    }
    else
        this->path = NULL;
}

int HttpCookie::getMaxAge()
{
    return maxAge;
}

void HttpCookie::setMaxAge(int age)
{
    maxAge = age;
}

int HttpCookie::getVersion()
{
    return version;
}

void HttpCookie::setVersion(int version)
{
    this->version = version;
}

bool HttpCookie::getSecure()
{
    return secure;
}

void HttpCookie::setSecure(bool secure)
{
    this->secure = secure;
}

bool HttpCookie::isToken(const char& value)
{
    bool ret;

    ret = strpbrk(&value, &tspecials) != NULL;

    if (!ret)
    {
        const char* ptr = &value;

        while (!ret && *ptr)
            ret = (*ptr > 0x20) && (*ptr < 0x7f);
    }

    return ret;
}

/**
 * Return the HTTP header name to set the cookie, based on cookie version.
 * @ret char& - a reference to the char header value.
 */
const char& HttpCookie::getHeaderName()
{
    if (version == 1)
    {
        return *"Set-Cookie2";
    }
    else
    {
        return *"Set-Cookie";
    }
}

/**
 * Return the header value used to set this cookie
 */
const char& HttpCookie::getHeaderValue()
{
    // this part is the same for all cookies
    strcpy(header, name);
    strcat(header, "=");

    maybeQuote(*value);

    // add version 1 specific information
    if (version == 1)
    {
        // Version=1 ... required
        strcat(header, ";Version=1");

        // Comment=comment
        if (comment)
        {
            strcat(header, ";Comment=");
            maybeQuote(*comment);
        }
    }

    // add domain information, if present
    if (domain)
    {
        strcat(header, ";Domain=");
        maybeQuote(*domain);
    }

    // Max-Age=secs/Discard ... or use old "Expires" format
    if (maxAge >= 0)
    {
        if (version == 0)
        {
#ifdef _WIN32_WCE // [
                  // ZZZ_WINCE !!!FIX!!! Need to set the expired time...

            SYSTEMTIME systemTime;
            GetSystemTime(&systemTime);

            // strcat(header,";Expires=");
            int len = strlen(header);

            if (maxAge == 0)
            {
                sprintf(&header[len], "Expires=%s, %2d-%s-%d %2d:%2d:%2d GMT", daysAbbrev[systemTime.wDayOfWeek],
                        systemTime.wDay, monthsAbbrev[systemTime.wMonth - 1], systemTime.wYear,
                        systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
                // cb.append("; Expires=Thu, 01-Dec-1994 16:00:00 GMT");
            }
            else
            {
                // ZZZ Will this work??? Z.Z.Z 030522
                systemTime.wSecond += (maxAge * 1000);
                sprintf(&header[len], "Expires=%s, %2d-%s-%d %2d:%2d:%2d GMT", daysAbbrev[systemTime.wDayOfWeek],
                        systemTime.wDay, monthsAbbrev[systemTime.wMonth - 1], systemTime.wYear,
                        systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
            }
#else  // ][
            time_t now;

            //"EEE, dd-MMM-yyyy HH:mm:ss z";

            strcat(header, ";Expires=");
            if (maxAge == 0)
            {
                now = 1000;
            }
            else
            {
                time(&now);

                now += (maxAge * 1000);
            }
            int len = strlen(header);
            strftime(&header[len], sizeof(header) - len - 1,
                     "%a, %d-%b-%Y %H:%M:%S GMT",
                     gmtime(&now));
#endif // ]
        }
        else
        {
            strcat(header, ";Max-Age=");
            int len = strlen(header);
            _itoa(maxAge, &header[len], 10);
        }
    }
    else if (version == 1)
        strcat(header, ";Discard");

    // Path=path
    if (path)
    {
        strcat(header, ";Path=");
        maybeQuote(*path);
    }

    // Secure
    if (secure)
    {
        strcat(header, ";Secure");
    }

    return *header;
}

void HttpCookie::maybeQuote(const char& value)
{
    if ((version == 0) || isToken(value))
    {
        strcat(header, &value);
    }
    else
    {
        strcat(header, "\"");
        strcat(header, &value);
        strcat(header, "\"");
    }
}
