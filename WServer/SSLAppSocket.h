//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023

#ifndef	_WIN32_WCE	// [
#ifndef SSLAppSocketH
#define SSLAppSocketH


/* SSLeay stuff */
#include <openssl/ssl.h>
#include <openssl/rsa.h>       
#include <openssl/err.h>

#ifdef SSL_ENGINE
#include <openssl/engine.h>
#endif

#include "AppSocket.h"


/* Threading */
/*
#ifdef __SOLARIS__
#include <thread.h>
#include <synch.h>

#define pthread_mutex_t mutex_t
#define pthread_lock(x) mutext_lock(x)
#define pthread_unlock(x) mutext_unlock(x)

#define pthread_cond_t cond_t
#define pthread_cond_wait(cond,mutex) cond_wait(cond,mutex)
#define pthread_cond_signal(x) cond_signal(x)

#define pthread_t thread_t
#define pthread_create(thread_id, foo, start, arg) \
        thr_create(0, 0, start, arg, 0, thread_id)
#else
#ifdef WIN32
#define pthread_mutex_t int
#define pthread_mutex_init(x,y)
#define pthread_mutex_lock(x)
#define pthread_mutex_unlock(x)
#define ECONNRESET EPIPE
#else
#include <pthread.h>
#define closesocket(x) close(x)
#endif
#endif
*/

#ifdef	WIN32

#define	MUTEX_TYPE			HANDLE
#define	MUTEX_SETUP(x)		(x) = CreateMutex(NULL, FALSE, NULL)
#define	MUTEX_CLEANUP(x)	CloseHandle(x)
#define	MUTEX_LOCK(x)		WaitForSingleObject((x), INFINITE)
#define	MUTEX_UNLOCK(x)		ReleaseMutex(x)
#define	THREAD_ID			GetCurrentThreadId()

#elif _POSIX_THREADS
// _POSIX_THREADS is normally defined in unistd.h if pthreads are availabe on the platfomr

#define	MUTEX_TYPE			pthread_mutex_
#define	MUTEX_SETUP(x)		pthread_mutex_init(&(x), NULL)
#define	MUTEX_CLEANUP(x)	pthread_mutex_destroy(&(x))
#define	MUTEX_LOCK(x)		pthread_mutex_lock(&(x))
#define	MUTEX_UNLOCK(x)		pthread_mutex_unlock(&(x))
#define	THREAD_ID			pthread_self()

#else
#error You must define mutex operations appropriate for your platform!
#endif



#define INTERRUPT_EXN -2
#define DISCONNECT_EXN -3
#define TIMEOUT_EXN -4

#ifdef	WIN32
#define ECONNRESET EPIPE
#endif

class WSERVER_API SSLAppSocket : public AppSocket
{
public:
	//SSLAppSocket();
	SSLAppSocket(SOCKET sd, SSL * ssl, int port, sockaddr_in& sinRemote, bool isUDP = false);
    ~SSLAppSocket();

	bool readByte(char * buf);

	/**
	 * Write the specified length of bytes to the socket.
	 *
	 * @param bytes - BYTE *, the bytes to write.
	 * @param len - int, the length of the bytes to write.
	 * @ret bool - FALSE if errors are encountered.
	 */
    bool writeBytes(const BYTE * bytes, int len);

#if	0	// [
	/**
	 * Stream the string out the socket.
	 * @param str - char *, the string of characters to write.
	 * @ret SSLAppSocket& - a reference to this SSLAppSocket so streaming
	 *	statments can be concatenated as in:
	 *	SSLAppSocket << "Welcome " << name << ".";
	 */
	SSLAppSocket& operator << (const char * str);

	/**
	 * Stream the int out the socket.
     * If not committed, will write the headers and set committed to true
     * before writing the characters.
	 *
	 * @param val - int, the int to write.
	 * @ret SSLAppSocket& - a reference to this SSLAppSocket so streaming
	 *	statments can be concatenated as in:
	 *	SSLAppSocket << "Your total is " << (price + tax) << ".";
	 */
	SSLAppSocket& operator << (const int val);

	/**
	 * Stream the character out the socket.
	 * @param c - const char, the character to write.
	 * @ret SSLAppSocket& - a reference to this SSLAppSocket so streaming
	 *	statments can be concatenated as in:
	 *	SSLAppSocket << "Welcome " << name << '.' << endl;
	 */
	SSLAppSocket& operator << (const char c);
#endif	// ]

/*	// Add the following operator << overrides:
	bool
	short
	unsigned short
	int
	unsigned int
	long
	unsigned long
	float
	double
	long double
*/
protected:
	void ssl_safe_free();
	int ssl_close();
	int exception_status(int error);
	int ssl_read(char *buf, int len);
	int ssl_write(char *buf, int len);

	SSL		* ssl;
};


//---------------------------------------------------------------------------
#endif

#endif				// ]