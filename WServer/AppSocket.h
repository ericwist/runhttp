//---------------------------------------------------------------------------
// author: Eric Wistrand 11/12/2023
#ifndef AppSocketH
#define AppSocketH

#ifndef	_WIN32_WCE	// [
#include <iostream>
#endif				// ]
#include "ws-util.h"
#include "WServer.h"

class WSERVER_API AppSocket
{
public:
	//AppSocket();
	AppSocket(SOCKET sd, int port, bool isUDP = false);
	AppSocket(SOCKET sd, int port, sockaddr_in& sinRemote, bool isUDP = false);
    ~AppSocket();

	void close();

	void setSocket(SOCKET sd, int port, bool isUDP = false);
    void setSocket(SOCKET sd, int port, sockaddr_in& sinRemote, bool isUDP = false);
    SOCKET getSocket();

	sockaddr_in& getSockaddr_in();

	int getPort();

	/**
	 * Is the socket an UPD type or TCP. If UPD it is a Datagram and was created
	 * with the SOCK_DGRAM parameter as opposed to SOCK_STREAM.
	 *
	 * @ret true if the socket is UDP, false otherwise.
	 */
	bool isSocketUDP();

	virtual bool readByte(char * buf);

	/**
	 * Attempt to read up to the specified length of bytes from the socket.
	 *
	 * @param buf - char *, the buffer to read the bytes into.
	 * @param len - int, the length of the buffer.
	 * @ret int - the number of bytes read.
	 */
    virtual int readBytes(char * buf, int len);

	/**
	 * Write the specified length of bytes to the socket.
	 *
	 * @param bytes - BYTE *, the bytes to write.
	 * @param len - int, the length of the bytes to write.
	 * @ret bool - FALSE if errors are encountered.
	 */
    virtual bool writeBytes(const BYTE * bytes, int len);

	/**
	 * Write the specified characters to the socket.
	 *
	 * @param str - char *, the string of characters to write.
	 * @ret bool - FALSE if errors are encountered.
	 */
	bool writeChars(const char * str);

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
	bool setPushBackSize(unsigned int size);

	/**
	 * Get the current push back buffer size.
	 *
	 * @ret unsigned int, the buffer size;
	 */
	unsigned int getPushBackSize();

    /**
     * Get the current the position within the push back buffer that the next byte will
     * be read. If pushBackBuffer is NULL or empty, <code>pushBackBufferPosition</code>
	 * is equal to <code>pushBackBufferSize</code>; when the pushBackBuffer is NON NULL
	 * and full, <code>pushBackBufferPosition</code> is zero.
	 *
	 * @ret unsigned int, the pushBackBufferPosition
	 */
	unsigned int getpushBackBufferPosition();

    /**
     * Push a byte back into the front of the pushBackBuffer.
     * When this method returns, the next byte to be read will have the value
     * <code>(BYTE)b</code>.
     *
     * @param   b - BYTE the byte to be pushed back
	 * @ret bool, true if the byte could be pushed back, false otherwise.
     */
    bool pushBack(BYTE b);

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
    bool pushBack(BYTE * b, unsigned int off, unsigned int len);

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
	char	readLine(char ** lineBuf, int len, int maxRead);

    /**
     * Formatted print to the socket.
     *
	 * @ret param fmt - char *, the format string followed by the comma delimited
     *                      print parameter list.
	 * @ret bool - FALSE if errors are encountered.
     */
    bool printf(const char *fmt, ...);

	/**
	 * Stream the string out the socket.
	 * @param str - char *, the string of characters to write.
	 * @ret AppSocket& - a reference to this AppSocket so streaming
	 *	statments can be concatenated as in:
	 *	appSocket << "Welcome " << name << ".";
	 */
	AppSocket& operator << (const char * str);

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
	AppSocket& operator << (const int val);

	/**
	 * Stream the character out the socket.
	 * @param c - const char, the character to write.
	 * @ret AppSocket& - a reference to this AppSocket so streaming
	 *	statments can be concatenated as in:
	 *	appSocket << "Welcome " << name << '.' << endl;
	 */
	AppSocket& operator << (const char c);

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

	/**
	 * A function similar in spirit to Unix's perror() that tacks a canned
	 * interpretation of the value of WSAGetLastError() onto the end of a
	 * passed string, separated by a ": ".  Generally, you should implement
	 * smarter error handling than this, but for default cases and simple
	 * programs, this function is sufficient.
	 *
	 * @ret const char *, a pointer to the passed in buffer.
	 */
	const char * getLastSocketErrorMessage(const char* pcMessagePrefix, char acErrorBuffer[], int acErrorBufferSize, int nErrorID = 0);

	/**
	 * Is the socket currently connected.
	 *
	 * @return bool, true if the socket is currently connected, false if the socket
	 *		was never connected, or if the connection was dropped.
	 */
	bool	isConnected();

protected:
	SOCKET socket;
	BYTE * pushBackBuffer;
	unsigned int pushBackBufferSize;
    /**
     * The position within the push back buffer that the next byte will
     * be read. If pushBackBuffer is NULL or empty, <code>pushBackBufferPosition</code>
	 * is equal to <code>pushBackBufferSize</code>; when the pushBackBuffer is NON NULL
	 * and full, <code>pushBackBufferPosition</code> is zero.
     */
	unsigned int pushBackBufferPosition;

private:
	/**
	 * Is the socket an UPD type or TCP. If UPD it is a Datagram and was created
	 * with the SOCK_DGRAM parameter as opposed to SOCK_STREAM.
	 */
	bool	isUDP;

	sockaddr_in sinRemote;

	int port;

	bool	connected;
};


//---------------------------------------------------------------------------
#endif
