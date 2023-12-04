// runhttp.cpp : Defines the entry point for the console application.
// author: Eric Wistrand 11/12/2023

#include "Chttp.h"
#include "stdafx.h"
#include <iostream>
const int defaultServerPort = 8080;
using namespace std;

int gethostipaddress(std::string& ip_address)
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        std::cout << "Failed to get hostname" << std::endl;
        return 1;
    }

    struct hostent* host;
    if ((host = gethostbyname(hostname)) == NULL)
    {
        std::cout << "Failed to get host by name" << std::endl;
        return 1;
    }

    struct in_addr** addr_list = reinterpret_cast<struct in_addr**>(host->h_addr_list);
    for (int i = 0; addr_list[i] != NULL; i++)
    {
        ip_address = inet_ntoa(*addr_list[i]);
        std::cout << "IPv4 Address: " << ip_address << std::endl;
    }

    WSACleanup();
    return 0;
}

int main(int argc, char* argv[])
{
    // Do we have enough command line arguments?
    if (argc < 3)
    {
        cerr << "usage: " << argv[0] << " <server-address> "
             << "<docroot> "
                "[server-port] [indexFileName]"
             << endl
             << endl;
        cerr << "\tIf you don't pass server-port, it defaults to " << defaultServerPort << ", indexFileName defaults to index.html." << endl;
        return 1;
    }

    // Get host and (optionally) port from the command line
    std::string ip = "";
    char pcHost[20] = {};
    int retvalerr = gethostipaddress(ip);
    
    if (retvalerr == 1 || ip == "")
    {
        std::cout << "Failed to get host ip address using cmd arguement" << std::endl;
        strcpy(pcHost, argv[1]);
    }
    else
    {
        strcpy(pcHost, ip.c_str());
    }

    const char* indexFileName;
    int nPort;

    LPCTSTR dirName = argv[2];

    DWORD fileAttrs = GetFileAttributes(dirName);

    cout << "fileAttrs= " << fileAttrs << " FILE_ATTRIBUTE_DIRECTORY= " << FILE_ATTRIBUTE_DIRECTORY << " (fileAttrs & FILE_ATTRIBUTE_DIRECTORY)= " << (fileAttrs & FILE_ATTRIBUTE_DIRECTORY) << endl;

    if (fileAttrs == 0xFFFFFFFF)
    {
        cerr << "GetFileAttributes() FAILED! GetLastError()= " << GetLastError() << endl
             << endl;
    }

    if ((fileAttrs == 0xFFFFFFFF) || !(fileAttrs & FILE_ATTRIBUTE_DIRECTORY))
    {
        cerr << "Error: " << dirName << " does not exist as a directory" << endl
             << endl;
        return 1;
    }

    if (argc >= 4)
        nPort = atoi(argv[3]);
    else
        nPort = defaultServerPort;

    if (argc >= 5)
        indexFileName = argv[4];
    else
        indexFileName = NULL;

    // Do a little sanity checking because we're anal.
    int nNumArgsIgnored = (argc - 5);

    if (nNumArgsIgnored > 0)
    {
        cerr << nNumArgsIgnored << " extra argument" << (nNumArgsIgnored == 1 ? "" : "s") << " ignored.  FYI." << endl;
    }

    cout << "Data DIRECTORY:" << dirName << " PORT:" << nPort << " DEFAULT PAGE:" << indexFileName << endl;

    // Start Winsock up
    WSAData wsaData;
    int nCode;

    if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0)
    {
        cerr << "WSAStartup() returned error code " << nCode << "." << endl;
        return 255;
    }

    Chttp chttp(pcHost, nPort, dirName, indexFileName);

    int retval = chttp.run();

    // Shut Winsock back down and take off.
    WSACleanup();
    return retval;
}
