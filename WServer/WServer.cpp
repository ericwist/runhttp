// WServer.cpp : Defines the entry point for the DLL application.
//
// Author: Eric Wistrand 11/12/2023

// #include <windows.h>
#include "WServer.h"
#include "debugson.h"
#include "ws-util.h"

#ifndef _WIN32_WCE // [
#include <iostream>

using namespace std;
#endif

BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
    BOOL ret = true;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        // D(cout << endl << endl << "WServer DLLMain() DLL_PROCESS_ATTACH" << endl << endl;)
        /*
                    // Start Winsock up
                    int		nCode;
                    WSAData	wsaData;

                    if ( (nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0 )
                    {
                        //cerr << "WSAStartup() returned error code " << nCode << "." << endl;
                        ret = false;
                    }
        /**/
        break;
    }

    case DLL_THREAD_ATTACH:
    {
        // D(cout << endl << "*************" << "WServer DLLMain() DLL_THREAD_ATTACH" << endl;)
        /*
                    // Start Winsock up
                    int		nCode;
                    WSAData	wsaData;

                    if ( (nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0 )
                    {
                        cerr << "WSAStartup() returned error code " << nCode << "." << endl;
                        ret = false;
                    }
                    break;
        /**/
    }

    break;
    case DLL_THREAD_DETACH:
        // D(cout << "WServer DLLMain() DLL_THREAD_DETACH" << "*************" << endl;)
        // WSACleanup();
        break;
    case DLL_PROCESS_DETACH:
        // D(cout << endl << endl << "WServer DLLMain() DLL_PROCESS_DETACH" << endl << endl;)
        //  Shut Winsock back down and take off.
        // WSACleanup();
        break;
    }

    return ret;
}

/*
// This is an example of an exported variable
WSERVER_API int nWServer=0;

// This is an example of an exported function.
WSERVER_API int fnWServer(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see WServer.h for the class definition
CWServer::CWServer()
{
    return;
}
*/
