//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
// #pragma hdrstop

#ifndef _WIN32_WCE // [

#include "SSLAppSocket.h"

using namespace std;

SSLAppSocket::SSLAppSocket(SOCKET sd, SSL* ssl, int port, sockaddr_in& sinRemote, bool isUDP)
: AppSocket(sd, port, sinRemote, isUDP)
, ssl(ssl)
{
}

SSLAppSocket::~SSLAppSocket()
{
    ssl_close();
}

bool SSLAppSocket::readByte(char* buf)
{

    return (ssl_read(buf, 1) > 0);
}

/**
 * Write the specified length of bytes to the socket.
 *
 * @param bytes - BYTE *, the bytes to write.
 * @param len - int, the length of the bytes to write.
 * @ret bool - FALSE if errors are encountered.
 */
bool SSLAppSocket::writeBytes(const BYTE* bytes, int len)
{
    bool ret = TRUE;

    if ((len > 0) && bytes)
    {
        int sent = 0;

        int l = ssl_write((char*)bytes, len);

        if (l <= 0) // if (l == SOCKET_ERROR)
            ret = FALSE;
    }
    else
        ret = FALSE;

    return ret;
}

#if 0 // [
/**
 * Stream the string out the socket.
 * @param str - const char *, the string of characters to write.
 * @ret SSLAppSocket& - a reference to this SSLAppSocket so streaming
 *	statments can be concatenated as in:
 *	SSLAppSocket << "Welcome " << name << ".";
 */
SSLAppSocket& SSLAppSocket::operator << (const char * str)
{
	writeChars(str);

	return *this;
}

/**
 * Stream the character out the socket.
 * @param c - const char, the character to write.
 * @ret SSLAppSocket& - a reference to this SSLAppSocket so streaming
 *	statments can be concatenated as in:
 *	SSLAppSocket << "Welcome " << name << '.' << endl;
 */
SSLAppSocket& SSLAppSocket::operator << (const char c)
{
	writeBytes((const BYTE *)&c, 1);

	return *this;
}

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
SSLAppSocket& SSLAppSocket::operator << (const int val)
{
	writeBytes((const BYTE *)&val, sizeof (val));

	return *this;
}

#endif // ]

void SSLAppSocket::ssl_safe_free()
{
    if (ssl)
    {
        int count;

        /* clear non-blocking i/o */
#ifndef WIN32
        {
            int flags;
            flags = fcntl(fd, F_GETFL);
            fcntl(socket, F_SETFL, ~O_NONBLOCK & flags);
        }
#endif

        /* SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN|SSL_RECEIVED_SHUTDOWN); */
        SSL_set_shutdown(ssl, SSL_RECEIVED_SHUTDOWN);
        for (count = 4; count > 0; count--)
        {
            int result = SSL_shutdown(ssl);
        }

        SSL_free(ssl);
        ssl = NULL;
    }
}

int SSLAppSocket::ssl_close()
{
    ssl_safe_free();

    if (socket != INVALID_SOCKET)
    {
        if (ShutdownConnection(socket))
        {
            D(cout << "Connection is down." << endl;)
        }
        else
        {
            cerr << endl
                 << WSAGetLastErrorMessage("Connection shutdown failed") << endl;
        }
        socket = INVALID_SOCKET;
    }

    return 0;
}

int SSLAppSocket::exception_status(int error)
{
    if (error == EINTR || error == EAGAIN)
        return INTERRUPT_EXN;
    else if (error == EPIPE || errno == ECONNRESET)
    {
        ssl_close();
        return DISCONNECT_EXN;
    }
    else
    {
        ssl_close();
        return -1;
    }
}

int SSLAppSocket::ssl_read(char* buf, int len)
{
    fd_set read_mask;
    struct timeval timeout;
    int result;
    int retry = 100;
    int ssl_error = 0;

    if (!ssl || (socket == INVALID_SOCKET))
        return -1;

    result = SSL_read(ssl, buf, len);

    if (result > 0)
        return result;

    FD_ZERO(&read_mask);

    timeout.tv_sec = 30;
    timeout.tv_usec = 0;

    while (retry-- > 0)
    {
        ssl_error = 0;

        FD_SET(socket, &read_mask);
        result = select(socket + 1, &read_mask, 0, 0, &timeout);

        if (result > 0)
        {
        }
        else if (result == 0)
        {
            return TIMEOUT_EXN;
        }
        else if (errno == EINTR || errno == EAGAIN)
            continue;
        else
            return exception_status(errno);

        result = SSL_read(ssl, buf, len);

        if (result >= 0)
            return result;

        ssl_error = SSL_get_error(ssl, result);

        if (ssl_error == SSL_ERROR_WANT_READ)
            continue;
        else
        {
            return DISCONNECT_EXN;
        }
    }

    return exception_status(errno);
}

int SSLAppSocket::ssl_write(char* buf, int len)
{

    fd_set write_mask;
    struct timeval timeout;
    int result;
    int retry = 100;

    result = SSL_write(ssl, buf, len);

    if (result > 0)
        return result;

    while (retry-- > 0)
    {
        FD_ZERO(&write_mask);
        FD_SET(socket, &write_mask);

        timeout.tv_sec = 30;
        timeout.tv_usec = 0;

        result = select(socket + 1, 0, &write_mask, 0, &timeout);

        if (result > 0)
        {
        }
        else if (result == 0)
        {
            ssl_close();
            return TIMEOUT_EXN;
        }
        else if (errno == EINTR || errno == EAGAIN)
            continue;
        else
            return exception_status(errno);

        result = SSL_write(ssl, buf, len);

        if (result >= 0)
            return result;
        else
            continue;
    }

    return exception_status(errno);
}

//---------------------------------------------------------------------------
// #pragma package(smart_init)
#endif // ]