//---------------------------------------------------------------------------
// Author: Eric Wistrand 11 / 12 / 2023
#ifndef ChpptH
#define ChpptH

#include "WServer.h"
#include "RequestListener.h"



class WSERVER_API Chttp
{
public:
	//Chttp(const char * address, int nPort, HANDLE docRoot);
    Chttp(const char * address, int nPort, 
#ifdef	_WIN32_WCE	// [
		LPCTSTR docRootPath,
		LPCTSTR indexFileName
#else				// ][
		const char * docRootPath,
		const char * indexFileName
#endif				// ]
		);

    ~Chttp();

	int run();

protected:
	const char		* address;
    const int	 	  port;
#ifdef	_WIN32_WCE	// [
    const LPCTSTR	  docRootPath;
	const LPCTSTR	  indexFileName;
#else				// ][
	const char		* docRootPath;
	const char		* indexFileName;
#endif				// ]
    //HANDLE	  		  docRoot;
};


//---------------------------------------------------------------------------
#endif
