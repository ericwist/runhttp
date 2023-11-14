/*
 * ODBC_Connection.h
 *
 * Class to handle ODBC Database connections.
 *
 * W.D.L 031022
 */

#ifndef	_WIN32_WCE	// [

#ifndef _ODBC_Connection_H
#define _ODBC_Connection_H

#include "WServer.h"

#include <windows.h>

#include <mbstring.h>

// ODBC HEADERS
#include <sql.h>
#include <sqlext.h>

/**
 * Class to handle ODBC Database connections.
 */
class WSERVER_API ODBC_Connection
{
public:
	ODBC_Connection(SQLCHAR * inDSN, SQLCHAR * inUser, SQLCHAR * inPassword);

	~ODBC_Connection();

	static SQLRETURN initODBC(SQLUINTEGER pooling = SQL_CP_ONE_PER_HENV, SQLUINTEGER ODBC_Version = SQL_OV_ODBC3);
	static SQLRETURN termODBC();
	static SQLRETURN getErrorText(SQLRETURN rc, SQLSMALLINT htype, SQLHANDLE handle, SQLCHAR retErrorMsg[], int retErrorMsgSize);

protected:
	static SQLHENV		henv;
	SQLHDBC		hdbc;
	//SQLHSTMT	hstmt;
	SQLCHAR		* dsn, * user, * password;
	BOOL		connected;

	SQLRETURN connect();
	SQLRETURN disConnect();
};

#endif

#endif				// ]