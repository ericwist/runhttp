//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023

#ifndef _WIN32_WCE // [

#ifndef SSLRequestListenerH
#define SSLRequestListenerH

#include "RequestListener.h"
#include "SSLAppSocket.h"

#define SSL_CLIENT_VERIFY_NONE 0
#define SSL_CLIENT_VERIFY_OPTIONAL_NO_CA 1
#define SSL_CLIENT_VERIFY_OPTIONAL 2
#define SSL_CLIENT_VERIFY_REQUIRE 3

class WSERVER_API SSLRequestListener : public RequestListener
{
public:
    SSLRequestListener(const char* address, int nPort, RequestProcessorFactory& requestProcessorFactory, int verifyClientFlags = SSL_CLIENT_VERIFY_NONE);
    ~SSLRequestListener();

    void setupSSL(const char* password, const char* certificateFile, const char* keyFile, const char* certificateChainFile, const char* caCertificatePath, const char* caCertificateFile, const char* caRevocationPath, const char* caRevocationFile);

    int listen4Request();

#if 0  // [ Now in SocketListener
	void	acceptConnections(SOCKET ListeningSocket);
#endif // ]

protected:
    /**
     * SocketListener interface. Called by acceptConnections to handle the accepted
     * connection. Overrides RequestListener::handleConnection() to setup SSL specific
     * mechanisms.
     *
     * @ret int, 0 upon success, non zero error code upon errors.
     */
    virtual int handleConnection(SOCKET sd, sockaddr_in& sinRemote);

    void freeSSLinfo();
    BOOL ssl_create();
    SSL* ssl_open(SOCKET sd);
    void ssl_safe_free(int fd, SSL* ssl);
    void mutex_cleanup();
    BOOL mutex_setup();

    static int password_callback(char* buf, int size, int rwflag, void* userdata);
    static int ssl_verify_callback(int ok, X509_STORE_CTX* ctx);
    static void mutex_lock_callback(int mode, int n, const char* file, int line);
    static unsigned long SSLRequestListener::mutex_thread_id_callback();

    SSL_CTX* ctx;
    // SSL				* ssl;
    // pthread_mutex_t	* ssl_lock;
    static MUTEX_TYPE* mutex_buf;
    char* ssl_cipher;
    int ssl_bits;

    char* certificateFile;
    char* keyFile;
    char* certificateChainFile;
    char* caCertificatePath;
    char* caCertificateFile;
    char* caRevocationPath;
    char* caRevocationFile;
    char* password;

    int verifyClientFlags;
};

//---------------------------------------------------------------------------
#endif

#endif // ]