//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
// #pragma hdrstop

#ifndef _WIN32_WCE // [

#include "SSLRequestListener.h"

using namespace std;

MUTEX_TYPE* SSLRequestListener::mutex_buf = NULL;

SSLRequestListener::SSLRequestListener(const char* address, int nPort, RequestProcessorFactory& requestProcessorFactory, int verifyClientFlags)
: RequestListener(address, nPort, requestProcessorFactory)
, verifyClientFlags(verifyClientFlags)
,
/*ssl(NULL),*/ ctx(NULL)
, ssl_cipher(NULL)
, ssl_bits(0)
{
    certificateFile = keyFile = certificateChainFile = caCertificatePath = caCertificateFile =
        caRevocationPath = caRevocationFile = password = NULL;

    char* tpassword = "klaatu";
    char* tcertificateFile = "C:\\resin-2.1.4\\keys\\openssl\\newcert.pem";
    char* tkeyFile = "C:\\resin-2.1.4\\keys\\openssl\\CertReq.key";

    setupSSL(tpassword, tcertificateFile, tkeyFile, certificateChainFile, caCertificatePath, caCertificateFile, caRevocationPath, caRevocationFile);
}

SSLRequestListener::~SSLRequestListener()
{
    //	WSACleanup();
    freeSSLinfo();

    if (ctx)
        SSL_CTX_free(ctx);

    mutex_cleanup();
}

void SSLRequestListener::freeSSLinfo()
{
    if (certificateFile)
    {
        delete[] certificateFile;
        certificateFile = NULL;
    }

    if (keyFile)
    {
        delete[] keyFile;
        keyFile = NULL;
    }

    if (certificateChainFile)
    {
        delete[] certificateChainFile;
        certificateChainFile = NULL;
    }

    if (caCertificatePath)
    {
        delete[] caCertificatePath;
        caCertificatePath = NULL;
    }

    if (caCertificateFile)
    {
        delete[] caCertificateFile;
        caCertificateFile = NULL;
    }

    if (caRevocationPath)
    {
        delete[] caRevocationPath;
        caRevocationPath = NULL;
    }

    if (caRevocationFile)
    {
        delete[] caRevocationFile;
        caRevocationFile = NULL;
    }

    if (password)
    {
        delete[] password;
        password = NULL;
    }
}

// ZZZ Need to move this into a utility class or file.
char* newDupStr(const char* str)
{
    char* dup;

    if (str)
    {
        dup = new char[strlen(str) + 1];
        strcpy(dup, str);
    }

    return str ? dup : NULL;
}

void SSLRequestListener::mutex_lock_callback(int mode, int n, const char* file, int line)
{
    if (mode & CRYPTO_LOCK)
        MUTEX_LOCK(mutex_buf[n]);
    else
        MUTEX_UNLOCK(mutex_buf[n]);
}

unsigned long SSLRequestListener::mutex_thread_id_callback()
{
    return ((unsigned long)THREAD_ID);
}

void SSLRequestListener::mutex_cleanup()
{
    if (mutex_buf)
    {
        CRYPTO_set_locking_callback(NULL);
        CRYPTO_set_id_callback(NULL);

        int num = CRYPTO_num_locks();
        for (int i = 0; i < num; i++)
        {
            if (mutex_buf[i])
                MUTEX_CLEANUP(mutex_buf[i]);
        }

        delete[] mutex_buf;
        mutex_buf = NULL;
    }
}

BOOL SSLRequestListener::mutex_setup()
{
    BOOL ret;
    int num = CRYPTO_num_locks();

    mutex_cleanup();
    mutex_buf = new MUTEX_TYPE[num];

    if (mutex_buf)
    {
        for (int i = 0, ret = TRUE; i < num; i++)
        {
            mutex_buf[i] = NULL;
            MUTEX_SETUP(mutex_buf[i]);
            if (!mutex_buf[i])
            {
                ret = false;
                printf("%d of %s, error creating Mutex object!\n", __LINE__, __FILE__);
                break;
            }
        }

        // If we failed, NULL out all remaing in the array so we can handle them
        // correclt when cleaning up.
        for (int i = 0; i < num; i++)
            mutex_buf[i] = NULL;
    }
    else
    {
        ret = false;
        printf("%d of %s, memory allocation error!\n", __LINE__, __FILE__);
    }

    if (ret)
    {
        CRYPTO_set_id_callback(mutex_thread_id_callback);
        CRYPTO_set_locking_callback(mutex_lock_callback);
    }
    else
    {
        mutex_cleanup();
    }

    return ret;
}

void SSLRequestListener::setupSSL(const char* password, const char* certificateFile,
                                  const char* keyFile, const char* certificateChainFile,
                                  const char* caCertificatePath, const char* caCertificateFile,
                                  const char* caRevocationPath, const char* caRevocationFile)
{
    freeSSLinfo();

    if (password)
        this->password = newDupStr(password);

    if (certificateFile)
        this->certificateFile = newDupStr(certificateFile);

    if (keyFile)
        this->keyFile = newDupStr(keyFile);

    if (certificateChainFile)
        this->certificateChainFile = newDupStr(certificateChainFile);

    if (caCertificatePath)
        this->caCertificatePath = newDupStr(caCertificatePath);

    if (caCertificateFile)
        this->caCertificateFile = newDupStr(caCertificateFile);

    if (caRevocationPath)
        this->caRevocationPath = newDupStr(caRevocationPath);

    if (caRevocationFile)
        this->caRevocationFile = newDupStr(caRevocationFile);
}

int SSLRequestListener::password_callback(char* buf, int size, int rwflag, void* userdata)
{
    strcpy(buf, (const char*)userdata);
    printf("%d of %s, SSLRequestListener::password_callback()\n", __LINE__, __FILE__);

    return strlen(buf);
}

/*
 * This OpenSSL callback function is called when OpenSSL
 * does client authentication and verifies the certificate chain.
 */
int SSLRequestListener::ssl_verify_callback(int ok, X509_STORE_CTX* ctx)
{
    printf("%d of %s, SSLRequestListener::ssl_verify_callback() ok= %d\n", __LINE__, __FILE__, ok);

    /* If openssl's check was okay, then the verify is okay. */
    if (ok)
        return 1;

    int error_code;

    SSL* ssl = (SSL*)X509_STORE_CTX_get_app_data(ctx);

    // If the user data is missing, then it's a failure.
    if (!ssl)
        return 0;

    SSLRequestListener* listener = (SSLRequestListener*)SSL_get_app_data(ssl);

    // If the user data is missing, then it's a failure.
    if (!listener)
        return 0;

    error_code = X509_STORE_CTX_get_error(ctx);

    /* optional and required do require valid certificates */
    if (listener->verifyClientFlags != SSL_CLIENT_VERIFY_OPTIONAL_NO_CA)
        return 0;

    if (error_code == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT ||
        error_code == X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN ||
        error_code == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY ||
        error_code == X509_V_ERR_CERT_UNTRUSTED ||
        error_code == X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE)
        return 1;

    return 0;
}

void SSLRequestListener::ssl_safe_free(int fd, SSL* ssl)
{
    // printf("%d of %s\n",__LINE__,__FILE__);

    if (ssl)
    {
        int count;

        /* clear non-blocking i/o */
#ifndef WIN32
        {
            int flags;
            flags = fcntl(fd, F_GETFL);
            fcntl(fd, F_SETFL, ~O_NONBLOCK & flags);
        }
#endif

        /* SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN|SSL_RECEIVED_SHUTDOWN); */
        SSL_set_shutdown(ssl, SSL_RECEIVED_SHUTDOWN);
        for (count = 4; count > 0; count--)
        {
            int result = SSL_shutdown(ssl);
        }

        SSL_free(ssl);
    }
}

SSL* SSLRequestListener::ssl_open(SOCKET sd)
{
    SSL* ssl;
    SSL_CIPHER* cipher;
    int algbits, result;

    // printf("%d of %s\n",__LINE__,__FILE__);

    ssl = SSL_new(ctx);

    if (!ssl)
    {

        printf("%d of %s, can't allocate ssl\n", __LINE__, __FILE__);
        return NULL;
    }

    SSL_set_fd(ssl, sd);
    SSL_set_app_data(ssl, this);

    result = SSL_accept(ssl);

    if (result < 0)
    {

        ssl_safe_free(sd, ssl);

        printf("%d of %s, can't accept ssl\n", __LINE__, __FILE__);
        return NULL;
    }

    cipher = (SSL_CIPHER*)SSL_get_current_cipher(ssl);

    if (cipher)
    {
        ssl_cipher = (char*)SSL_CIPHER_get_name(cipher);
        ssl_bits = SSL_CIPHER_get_bits(cipher, &algbits);
    }

#ifndef WIN32
    {
        int flags;
        flags = fcntl(sd, F_GETFL);
        fcntl(fListeningSocket, F_SETFL, O_NONBLOCK | flags);
    }
#endif

    // printf("%d of %s\n",__LINE__,__FILE__);
    return ssl;
}

BOOL SSLRequestListener::ssl_create()
{
    // printf("%d of %s\n",__LINE__,__FILE__);

    if (!mutex_setup())
    {
        printf("SSLRequestListener::ssl_create() mutex_setup() FAILED!!!");
        return FALSE;
    }
    // pthread_mutex_init(&ssl_lock, 0);

    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();

    SSL_METHOD* meth = (SSL_METHOD*)SSLv23_server_method();

#ifdef SSL_ENGINE
    if (config->crypto_device && !strcmp(config->crypto_device, "builtin"))
    {
        ENGINE* e = ENGINE_by_id(config->crypto_device);

        if (!e)
        {
            printf("unknown crypto device `%s'\n", config->crypto_device);
            return FALSE;
        }

        if (!ENGINE_set_default(e, ENGINE_METHOD_ALL))
        {
            printf("Can't initialize crypto device `%s'\n", config->crypto_device);
            return FALSE;
        }

        printf("using crypto-device `%s'\n", config->crypto_device);

        ENGINE_free(e);
    }
#endif

    if (ctx)
        SSL_CTX_free(ctx);

    ctx = SSL_CTX_new(meth);

    if (!ctx)
    {
        /* ERR_print_errors_fp(stderr); */
        printf("can't allocate context\n");
        return FALSE;
    }

    // From OpenSSL Book code. Z.Z.Z 020817
    // SSL_OP_SINGLE_DH_USE causes the server to generate a new private key for
    // each new connection which provides better security at the cost of more
    // computational power.
#ifndef _WIN32_WCE
    SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2);
#else
    SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2 /*| SSL_OP_SINGLE_DH_USE*/);
#endif

    if (!keyFile)
    {
        fprintf(stderr, "Can't find certificate-key-file in SSL configuration\n");
        return FALSE;
    }

    if (!password)
    {
        fprintf(stderr, "Can't find key-store-password in SSL configuration\n");
        return FALSE;
    }

    SSL_CTX_set_default_passwd_cb(ctx, password_callback);
    SSL_CTX_set_default_passwd_cb_userdata(ctx, password);

    if (SSL_CTX_use_certificate_file(ctx, certificateFile, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        fprintf(stderr, "Can't open certificate file %s\n", certificateFile);
        return FALSE;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, keyFile, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        return FALSE;
    }

    if (!SSL_CTX_check_private_key(ctx))
    {
        fprintf(stderr, "Private key does not match the certificate public key\n");
        return FALSE;
    }

    if (certificateChainFile && SSL_CTX_use_certificate_chain_file(ctx, certificateChainFile))
    {
        ERR_print_errors_fp(stderr);
        fprintf(stderr, "Can't open certificate chain file %s\n", certificateChainFile);
        return FALSE;
    }

    if (verifyClientFlags != SSL_CLIENT_VERIFY_NONE)
    {
        int nVerify = SSL_VERIFY_NONE | SSL_VERIFY_PEER;

        if (verifyClientFlags == SSL_CLIENT_VERIFY_REQUIRE)
            nVerify |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;

        SSL_CTX_set_verify(ctx, nVerify, ssl_verify_callback);
    }

    return TRUE;
}

int SSLRequestListener::listen4Request()
{
    D(cout << "Establishing the listener..." << endl)

    SOCKET listeningSocket = setUpListener();

    if (listeningSocket == INVALID_SOCKET)
    {
        D(cout << endl
               << WSAGetLastErrorMessage("establish listener") << endl;)
        return 3;
    }

    D(cout << "Creating SSL..." << flush;)

    if (!ssl_create())
        return 4;

    D(cout << "Waiting for connections..." << flush;)

    while (!quit)
    {
        acceptConnections(listeningSocket);
        D(cout << "SSLRequestListener::listen4Request() restarting..." << endl;)
    }
    quit = false;

#if defined(_MSC_VER)
    return 0; // warning eater
#endif
}

/**
 * SocketListener interface. Called by acceptConnections to handle the accepted
 * connection. Overrides RequestListener::handleConnection() to setup SSL specific
 * mechanisms.
 *
 * @ret int, 0 upon success, non zero error code upon errors.
 */
int SSLRequestListener::handleConnection(SOCKET sd, sockaddr_in& sinRemote)
{
    int ret = -1;

    SSL* ssl = ssl_open(sd);

    if (ssl)
    {
        SSLAppSocket* appSocket = new SSLAppSocket(sd, ssl, port, sinRemote, isSocketUDP());

        if (appSocket)
        {
            ret = RequestListener::handleConnection(*appSocket);
        }
        else
        {
            ssl_safe_free(sd, ssl); //??? SSL_free(ssl);
        }
    }

    return ret;
}

#if 0  // [ Now in SocketListener
//// AcceptConnections /////////////////////////////////////////////////
// Spins forever waiting for connections.  For each one that comes in,
// we create a thread to handle it and go back to waiting for
// connections.  If an error occurs, we return.

void SSLRequestListener::acceptConnections(SOCKET listeningSocket)
{
    sockaddr_in	sinRemote;
    int			nAddrSize = sizeof(sinRemote);
	struct		timeval tv;

	tv.tv_sec = 2;
	tv.tv_usec = 0;

	D(cout << "acceptConnections() ENTERED" << endl;)

    while (!quit)
    {
		fd_set readFDs, exceptFDs;

		FD_ZERO(&readFDs);
		FD_ZERO(&exceptFDs);

		// Add the listener socket to the read and except FD sets
		FD_SET(listeningSocket, &readFDs);
		FD_SET(listeningSocket, &exceptFDs);

		D(cout << "acceptConnections() before select()" << endl;)
		if (select(0, &readFDs, NULL, &exceptFDs, &tv) > 0)
		{
			D(cout << "acceptConnections() after select()" << endl;)
			// Something happened on the socket.
			if (FD_ISSET(listeningSocket, &readFDs))
			{
				D(cout << "acceptConnections() readFDs ISSET" << endl;)

				SOCKET sd = accept(listeningSocket, (sockaddr*)&sinRemote, &nAddrSize);

				if (sd != INVALID_SOCKET)
				{
					D(cout << "Accepted connection from " <<
							inet_ntoa(sinRemote.sin_addr) << ":" <<
							ntohs(sinRemote.sin_port) << "." <<
							endl;)

					SSL * ssl = ssl_open(sd);

					if ( ssl )
					{
						//new EchoRequestProcessor(sd);
						RequestProcessor * requestProcessor = requestProcessorFactory.createRequestProcessor();

						if ( requestProcessor )
						{
							//requestProcessor.setSocket(sd);
							SSLAppSocket * appSocket = new SSLAppSocket(sd,ssl);

							if ( appSocket )
							{
								requestProcessor->setAppSocket(appSocket);
								requestProcessor->setSockaddr_in(&sinRemote);

								requestProcessor->processRequest();

								// Since processRequest() spawns a thread, we can't delete
								// the requestProcessor here. It gets deleted in the thread
								// when the request is finished.
								//delete requestProcessor;
							}
							else
								delete requestProcessor;
						}
					}
					else
					{
						cerr << WSAGetLastErrorMessage("ssl_open() failed") << endl;

						if (ShutdownConnection(sd))
						{
							D(cout << "Connection is down." << endl;)
						}
						else
						{
							cerr << endl << WSAGetLastErrorMessage("Connection shutdown failed") << endl;
						}

						
					}
				}
				else
				{
					cerr << WSAGetLastErrorMessage("accept() failed") << endl;
					
				}

			}
			else if (FD_ISSET(listeningSocket, &exceptFDs))
			{
				int err;
				int errlen = sizeof(err);
				getsockopt(listeningSocket, SOL_SOCKET, SO_ERROR,
						(char*)&err, &errlen);
				cerr << WSAGetLastErrorMessage(
						"Exception on listening socket: ", err) << endl;
				;
			}
			else
			{
				cerr << "??? Neither readFS nor exceptFDs ISSET ???" << endl;
			}
		}

		*/

    }
}
#endif // ]

/*
void SSLRequestListener::stop()
{
    quit = true;
}
*/

//---------------------------------------------------------------------------
// #pragma package(smart_init)

#endif // ]