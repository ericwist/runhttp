/***********************************************************************
 ws-util.h - Declarations for the Winsock utility functions module.
 Author: Eric Wistrand 11/12/2023
***********************************************************************/

#if !defined(WS_UTIL_H)
#define WS_UTIL_H

// Uncomment one.
#include <winsock.h>
//#include <winsock2.h>

extern const char* WSAGetLastErrorMessage(const char* pcMessagePrefix,
        int nErrorID = 0);

extern const char* WSAGetLastErrorMessage(const char* pcMessagePrefix, char acErrorBuffer[],
	int acErrorBufferSize, int nErrorID /* = 0 */);

extern bool ShutdownConnection(SOCKET sd, bool isUDP = false);

#endif // !defined (WS_UTIL_H)

