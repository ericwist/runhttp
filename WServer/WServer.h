// Author: Eric Wistrand 11/12/2023
#ifndef WServerH
#define WServerH

// Insert your headers here
// #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// #include <windows.h>

#ifndef _WIN32_WCE // [
#include <iostream>
#endif // ]
#include "debugson.h"
// #include "debugsof.h"

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the WSERVER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// WSERVER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WSERVER_EXPORTS
#define WSERVER_API __declspec(dllexport)
#else
#define WSERVER_API __declspec(dllimport)

WSERVER_API void* globalNew(size_t nSize);
WSERVER_API void globalDelete(void* ptr);

#ifndef _WIN32_WCE // [
#if 0
inline void * __cdecl operator new(size_t nSize) //throw(std::bad_alloc)
{
	//printf("new operator function called at %d of %s\n",__LINE__,__FILE__);
	return globalNew(nSize);
	//printf("new operator function RETURNING at %d of %s\n",__LINE__,__FILE__);

	//return GlobalAlloc( GMEM_FIXED, nSize );
}

inline void __cdecl operator delete(void * ptr)
{
	//printf("***delete(void * ptr) operator function called at %d of %s\n",__LINE__,__FILE__);

	//GlobalFree(ptr);
	globalDelete(ptr);
	//printf("***delete(void * ptr) operator function RETURNING at %d of %s\n",__LINE__,__FILE__);
}
#endif
#endif // ]

/*
inline void  __cdecl operator delete(void * p, size_t s)    // Single object
{
    printf("!!!!delete(void * p, size_t s) operator function called at %d of %s\n",__LINE__,__FILE__);
    globalDelete(p);
}
*/
#endif

/*
// This class is exported from the WServer.dll
class WSERVER_API CWServer
{
public:
    CWServer(void);
    // TODO: add your methods here.
};

extern WSERVER_API int nWServer;

WSERVER_API int fnWServer(void);
*/

#endif