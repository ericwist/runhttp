// Author: Eric Wistrand 11/12/2023
// #pragma hdrstop

#ifndef _WIN32_WCE // [
#include <windows.h>
#endif // ]

#include "Chttp.h"
#include "HttpRequestProcessorFactory.h"

Chttp::Chttp(const char* address, int port,
             const TCHAR* docRootPath,
             const TCHAR* indexFileName)
: address(address)
, port(port)
, docRootPath(docRootPath)
, indexFileName(indexFileName)
{
    DWORD fileAttrs = GetFileAttributes((LPCTSTR)docRootPath);

    if ((fileAttrs == 0xFFFFFFFF) || !(fileAttrs & FILE_ATTRIBUTE_DIRECTORY))
    {
#ifndef _WIN32_WCE // [
        throw new exception("The specified document root does not exist as a directory.");
#endif // ]
    }
}

Chttp::~Chttp()
{
}

int Chttp::run()
{
    D(cout << "Chttp::run() 0 : " << docRootPath << endl;)
    if (indexFileName != NULL)
    {
        D(cout << "Chttp::run() 0a1 : " << indexFileName << endl;)
    }
    else
    {
        D(cout << "Chttp::run() 0a2 : null" << endl;)
    }

    D(cout << "Chttp::run() ob : " << address << ":" << port << endl;)
    HttpRequestProcessorFactory requestProcessorFactory(docRootPath, indexFileName);

    D(cout << "Chttp::run() 1" << endl;)

    RequestListener requestListener(address, port, requestProcessorFactory);

    D(cout << "Chttp::run() 2" << address << endl;)

    int ret = requestListener.listen4Request();

    D(cout << "Chttp::run() end ret= " << ret << endl;)

    return ret;
}
