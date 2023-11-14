//---------------------------------------------------------------------------
// Author: Eric Wistrand 11/12/2023
//#pragma hdrstop

#include "AppSocket.h"

#ifndef	_WIN32_WCE	// [
using namespace std;
#endif				// ]

AppSocket::AppSocket(SOCKET sd, int port, bool isUDP) : socket(sd), port(port), isUDP(isUDP), pushBackBuffer(NULL), pushBackBufferSize(0), pushBackBufferPosition(0)
{
	memset(&sinRemote,0,sizeof(sinRemote));

	connected = (socket != INVALID_SOCKET);
}

AppSocket::AppSocket(SOCKET sd, int port, sockaddr_in& sinRemote, bool isUDP) : socket(sd), port(port), isUDP(isUDP), pushBackBuffer(NULL), pushBackBufferSize(0), pushBackBufferPosition(0)
{
	// structure copy
	this->sinRemote = sinRemote;
}

AppSocket::~AppSocket()
{
	close();

	if ( pushBackBuffer )
		delete[] pushBackBuffer;
}

void AppSocket::close()
{
	if ( !isUDP && (socket != INVALID_SOCKET) )
	{
		if (ShutdownConnection(socket))
		{
			D(cout << "Connection is down." << endl;)
		}
		else
		{
#ifndef	_WIN32_WCE	// [
			cerr << endl << WSAGetLastErrorMessage("Connection shutdown failed") << endl;
#endif				// ]
		}
		socket = INVALID_SOCKET;
	}

	connected = (socket != INVALID_SOCKET);
}

void AppSocket::setSocket(SOCKET sd, int port, bool isUDP)
{
	socket = sd;
	this->port = port;
	this->isUDP = isUDP;

	connected = (socket != INVALID_SOCKET);
}

void AppSocket::setSocket(SOCKET sd, int port, sockaddr_in& sinRemote, bool isUDP)
{
	setSocket(sd, port, isUDP);

	// structure copy
	this->sinRemote = sinRemote;
}

SOCKET AppSocket::getSocket()
{
	return socket;
}

sockaddr_in& AppSocket::getSockaddr_in()
{
	return sinRemote;
}

int AppSocket::getPort()
{
	return port;
}

bool AppSocket::readByte(char * buf)
{
	bool ret;

	if ( pushBackBufferSize && (pushBackBufferPosition < pushBackBufferSize) )
	{
		buf[0] = pushBackBuffer[pushBackBufferPosition++];

		ret = true;
	}
	else
	{
		int		readBytes,i = 0;

		//
		// In the case of SOCK_STREAM, the server can do recv() and
		// send() on the accepted socket and then close it.

		// However, for SOCK_DGRAM (UDP), the server will do
		// recvfrom() and sendto()  in a loop.

		if ( !isUDP )
			while ( ((readBytes = recv(socket, buf, 1, 0)) == 0) && (i++ < 10) );
		else
		{
			int sinSize = sizeof (sinRemote);
			while ( ((readBytes = recvfrom(socket, buf, 1, 0, (struct sockaddr *)&sinRemote, &sinSize)) == 0) && (i++ < 10) );
		}


		ret = (readBytes > 0);
	}

	return ret;
}

/**
 * Attempt to read up to the specified length of bytes from the socket.
 *
 * @param buf - char *, the buffer to read the bytes into.
 * @param len - int, the length of the buffer.
 * @ret int - the number of bytes read.
 */
int AppSocket::readBytes(char * buf, int len)
{
	// ZZZ need to implement this in SSLAppSocket. Z.Z.Z 021122
	int		readBytes = 0;

	if ( len > 0 )
	{
		if ( pushBackBufferSize )
		{
			int avail = pushBackBufferSize - pushBackBufferPosition;

			if ( avail > 0 )
			{
				if ( len < avail )
					avail = len;

				memcpy(buf, pushBackBuffer+pushBackBufferPosition, avail);
				pushBackBufferPosition += avail;
				len -= avail;
				readBytes = avail;
			}
		}

		if ( len > 0 )
		{
			//
			// In the case of SOCK_STREAM, the server can do recv() and
			// send() on the accepted socket and then close it.

			// However, for SOCK_DGRAM (UDP), the server will do
			// recvfrom() and sendto()  in a loop.

			if ( !isUDP )
			{
				//readBytes = 0;
				for ( int i = 0; (readBytes < len) && (i < 10); i++ )
				{
					int ret = recv(socket, (char *)(buf+readBytes), len-readBytes, 0);

					if ( ret == SOCKET_ERROR )
						break;

					readBytes += ret;
				}
			}
			else
			{
				int sinSize = sizeof (sinRemote);
		/*
				for ( int i = 0; i < 10; i++ )
				{
					int ret = recvfrom(socket, (char *)(buf+readBytes), len-readBytes, 0, (struct sockaddr *)&sinRemote, &sinSize);

					if ( ret == SOCKET_ERROR )
						break;

					readBytes += ret;
				}
		*/
				int ret = recvfrom(socket, buf, len, 0, (struct sockaddr *)&sinRemote, &sinSize);

				if ( ret != SOCKET_ERROR )
					readBytes += ret;
		/*
				else
					readBytes = 0;
		*/
			}
		}
	}

    return readBytes;
}

/**
 * Write the specified length of bytes to the socket.
 *
 * @param bytes - BYTE *, the bytes to write.
 * @param len - int, the length of the bytes to write.
 * @ret bool - FALSE if errors are encountered.
 */
bool AppSocket::writeBytes(const BYTE * bytes, int len)
{
	bool	ret = TRUE;

	if ( (len > 0) && bytes )
    {
		int	sent = 0;

		while( ret && (sent < len) )
		{
			//
			// In the case of SOCK_STREAM, the server can do recv() and
			// send() on the accepted socket and then close it.

			// However, for SOCK_DGRAM (UDP), the server will do
			// recvfrom() and sendto()  in a loop.

			int l;

			if ( !isUDP )
				l = send(socket, (const char FAR *)(bytes + sent), len - sent, 0);
			else
				l = sendto(socket, (const char FAR *)(bytes + sent), len - sent, 0, (struct sockaddr *)&sinRemote, sizeof (sinRemote));

			if ( l )
			{
				if (l == SOCKET_ERROR)
					ret = FALSE;
				else
					sent += l;
			}
			else
			{
				// Client closed connection before we could send
				// all the data, so bomb out early.
#ifndef	_WIN32_WCE	// [
				cout << "Peer unexpectedly dropped connection!" << endl;
#endif				// ]
				ret = FALSE;
			}
		}
	}
	else
		ret = FALSE;

	return ret;
}

/**
 * Write the specified characters to the socket.
 *
 * @param str - char *, the string of characters to write.
 * @ret bool - FALSE if errors are encountered.
 */
bool AppSocket::writeChars(const char * str)
{
	int		len;

	if ( str != NULL )
		len = strlen(str);
	else
		len = 0;

	D(
	if ( len )
		cout << "AppSocket::writeChars() writing: '" << str << "'" << endl;
	)

	return writeBytes((const BYTE *)str,len);
}

/**
 * Set the push back buffer size. If greater than 0 this gives the
 * ability to "push back" or "unread" bytes that were read from the socket.
 * This is useful in situations where code needs to read ahead into the socket
 * to determine context, then push back bytes that it is not ready to handle.
 * For exanple readLine() uses this to see if a '\r' character follows a '\n'
 * or vice versa. If the following byte is not one of those, it will be pushed
 * back.
 *
 * @param size - unsigned int, if greater than 0 a new pushback buffer will be
 *		allocated, freeing any existing buffer. If 0, any existing buffer will
 *		be freed.
 * @ret bool - true if the buffer could be allocated, false otherwise.
 */
bool AppSocket::setPushBackSize(unsigned int size)
{
	pushBackBufferSize = pushBackBufferPosition = size;

	if ( pushBackBuffer )
		delete[] pushBackBuffer;

	if ( size )
		pushBackBuffer = new BYTE[size];
	else
		pushBackBuffer = NULL;

	if ( !pushBackBuffer )
		pushBackBufferSize = pushBackBufferPosition = 0;

	return (size && pushBackBuffer);
}


/**
 * Push a byte back into the front of the pushBackBuffer.
 * When this method returns, the next byte to be read will have the value
 * <code>(BYTE)b</code>.
 *
 * @param   b - BYTE the byte to be pushed back
 * @ret bool, true if the byte could be pushed back, false otherwise.
 */
bool AppSocket::pushBack(BYTE b)
{
	bool ret;

	if ( pushBackBufferPosition )
	{
		pushBackBuffer[--pushBackBufferPosition] = b;
		ret = true;
	}
	else
		ret = false;

	return ret;
}

/**
 * Push a section of a byte array back into the front of the pushBackBuffer.
 * When this method returns, the next byte to be read will have the value
 * <code>b[off]</code>, the byte after that will have the value
 * <code>b[off+1]</code>, etc.
 *
 * @param b - BYTE *, the byte array to push back.
 * @param off - unsigned int, the offset into the data.
 * @param len - unsigned int, the number of bytes to push back.
 * @ret bool, true if the byte could be pushed back, false otherwise.
 */
bool AppSocket::pushBack(BYTE * b, unsigned int off, unsigned int len)
{
	bool ret;

	if ( len <= pushBackBufferPosition )
	{
		pushBackBufferPosition -= len;
		memcpy(pushBackBuffer+pushBackBufferPosition, b+off, len);
		ret = true;
	}
	else
		ret = false;

	return ret;
}

/**
 * A recursive stack based method that will read a line of characters from the AppSocket.
 * This will recursively call successive readByte() calls until either a
 * '\r' or '\n' is encountered, or a carriage return, ('\r'), immediately followed by a
 * linefeed, ('\n'), or there is no more to be read, or maxRead has been reached, at
 * which point the lineBuf will be allocated to the size of len which is recursively
 * incremented. Then the stack will unwind from the recursion and the lineBuf will be
 * filled from the stack. The lineBuf contains all of the characters up to but not
 * including the '\r' or '\n'. The caller has the responsibility of freeing the lineBuf,
 * which MUST first be initialized to NULL upon the first call to this method. The
 * len variable SHOULD also be initialized to 0 in most cases. NOTE that for proper
 * operation, the pushBackBufferSize should be allocated to a size of at least 1. This
 * is needed when reading ahead to see if a linefeed follows a carriage return. If the
 * charater following a carriage return is NOT a linefeed, and the pushBackBufferSize
 * is > 0, (and the push back buffer is not full), the character will be pushed back.
 * If there is no push back buffer available, and a '\r' is encountred, this method
 * will not look for or pull off any following '\n'.
 *
 * @param lineBuf - char **, the secondary return value which will be allocated to
 *		the appropriate line size. The caller has the responsibility of freeing this
 *		lineBuf, which MUST be initialized to NULL upon the fist call to this method.
 * @param len - int, the length of the current line which is recursively incremented
 *		from within the method. This len variable SHOULD be initialized to 0 in
 *		most cases.
 * @param maxRead - int, the maximum size of the line to read, and thus represents the
 *		maximum amount of times this routine will recurse, and the max size that the
 *		lineBuf will be allocated to. Use this to avoid recursively blowing the stack.
 * @ret char, the char read from the socket, used recursively to store in the stack.
 */
char AppSocket::readLine(char ** lineBuf, int len, int maxRead)
{
	char ch[1] = {-1};


	if ( len < (maxRead-1) )
	{
		if ( readByte(ch) )
		{
			if ( (ch[0] != '\n') && (ch[0] != '\r') )
			{
                readLine(lineBuf,len+1,maxRead);
				if ( *lineBuf )
				{
					(*lineBuf)[len] = ch[0];
				}
				
			}
			else
			{
				if ( (ch[0] == '\r') && pushBackBufferSize && pushBackBufferPosition )
				{
					if ( readByte(ch) && (ch[0] != '\n') )
					{
						pushBack(ch[0]);
					}
				}

				

				*lineBuf = new char[(len)+1];
				ch[0] = -1;

				if ( *lineBuf )
				{
					// Set the first byte to NULL in case the the lineBuf only contained
					// a '\n'. NULL terminate also.
					(*lineBuf)[0] = (*lineBuf)[len] = 0;
				}
				else
				{
#ifndef	_WIN32_WCE
					std::cerr << __LINE__ << " of " << __FILE__ << ", Error Allocating memory of size %d" << len << std::endl;
#endif
					len = maxRead;
				}
			}
		}
		else
		{
			ch[0] = -1;
		}
	}

	return ch[0];

}

/**
 * Get the current push back buffer size.
 *
 * @ret unsigned int, the buffer size;
 */
unsigned int AppSocket::getPushBackSize()
{
	return pushBackBufferSize;
}

/**
 * Get the current the position within the push back buffer that the next byte will
 * be read. If pushBackBuffer is NULL or empty, <code>pushBackBufferPosition</code>
 * is equal to <code>pushBackBufferSize</code>; when the pushBackBuffer is NON NULL
 * and full, <code>pushBackBufferPosition</code> is zero.
 *
 * @ret unsigned int, the pushBackBufferPosition
 */
unsigned int AppSocket::getpushBackBufferPosition()
{
	return pushBackBufferPosition;
}

/**
 * Formatted print to the socket.
 *
 * @ret param fmt - char *, the format string followed by the comma delimited
 *                      print parameter list.
 * @ret bool - FALSE if errors are encountered.
 */
bool AppSocket::printf(const char *fmt, ...)
{
	char buf[8192];
    bool ret;

	va_list list;

	va_start(list, fmt);
    _vsnprintf(buf, sizeof(buf), fmt, list);
	 //_vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, list);

	buf[sizeof(buf)-1] = 0;
    ret = writeChars(buf);

	va_end(list);

    return ret;
}

/**
 * Stream the string out the socket.
 * @param str - const char *, the string of characters to write.
 * @ret AppSocket& - a reference to this AppSocket so streaming
 *	statments can be concatenated as in:
 *	appSocket << "Welcome " << name << ".";
 */
AppSocket& AppSocket::operator << (const char * str)
{
	writeChars(str);

	return *this;
}

/**
 * Stream the character out the socket.
 * @param c - const char, the character to write.
 * @ret AppSocket& - a reference to this AppSocket so streaming
 *	statments can be concatenated as in:
 *	appSocket << "Welcome " << name << '.' << endl;
 */
AppSocket& AppSocket::operator << (const char c)
{
	writeBytes((const BYTE *)&c, sizeof (c));

	return *this;
}

/**
 * Stream the int out the socket.
 * If not committed, will write the headers and set committed to true
 * before writing the characters.
 *
 * @param val - int, the int to write.
 * @ret AppSocket& - a reference to this AppSocket so streaming
 *	statments can be concatenated as in:
 *	appSocket << "Your total is " << (price + tax) << ".";
 */
AppSocket& AppSocket::operator << (const int val)
{
	writeBytes((const BYTE *)&val, sizeof (val));

	return *this;
}


/**
 * Is the socket currently connected.
 *
 * @return bool, true if the socket is currently connected, false if the socket
 *		was never connected, or if the connection was dropped.
 */
bool AppSocket::isConnected()
{
	if ( connected )
	{
		connected = (socket != INVALID_SOCKET);

		if ( connected )
		{
			int err = WSAGetLastError();

			switch (err)
			{
				case WSAECONNREFUSED:
				case WSAECONNRESET:
				case WSAEHOSTDOWN:
				case WSAEHOSTUNREACH:
				case WSAENETDOWN:
				case WSAENETRESET:
				case WSAENETUNREACH:
				case WSAENOTCONN:
				case WSAESHUTDOWN:
					connected = false;
			}
		}
	}

	return connected;
}

/**
 * A function similar in spirit to Unix's perror() that tacks a canned
 * interpretation of the value of WSAGetLastError() onto the end of a
 * passed string, separated by a ": ".  Generally, you should implement
 * smarter error handling than this, but for default cases and simple
 * programs, this function is sufficient.
 *
 * @ret const char *, a pointer to the passed in buffer.
 */
const char * AppSocket::getLastSocketErrorMessage(const char* pcMessagePrefix, char acErrorBuffer[], int acErrorBufferSize, int nErrorID)
{
	return WSAGetLastErrorMessage(pcMessagePrefix, acErrorBuffer, acErrorBufferSize, nErrorID);
}

//---------------------------------------------------------------------------
//#pragma package(smart_init)
