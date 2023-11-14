/*
 * ODBC_Connection.h
 *
 * Class to handle ODBC Database connections.
 *
 * W.D.L 031022
 */

#ifndef	_WIN32_WCE	// [
#include "debugson.h"

#include "ODBC_Connection.h"
//#include <mbstring.h>

SQLHENV		ODBC_Connection::henv = NULL;

ODBC_Connection::ODBC_Connection(SQLCHAR * inDSN, SQLCHAR * inUser, SQLCHAR * inPassword) : dsn(NULL), user(NULL), password(NULL), hdbc(NULL), /*hstmt(NULL),*/ connected(FALSE)
{
	if ( !henv )
		initODBC();

	if ( henv )
	{
		SQLRETURN rc = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc); 
		//if ( (rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO) )
		D(if ( rc != SQL_SUCCESS )
			getErrorText(rc, 1, henv, szErrorMsg, SQL_MAX_MESSAGE_LENGTH);)
	}

	if ( inDSN )
	{
		dsn = new SQLCHAR[_mbslen(inDSN)+1];
		if ( dsn )
			_mbscpy(dsn,inDSN);
	}

	if ( inUser )
	{
		user = new SQLCHAR[_mbslen(inUser)+1];
		if ( user )
			_mbscpy(user,inUser);
	}

	if ( inPassword )
	{
		password = new SQLCHAR[_mbslen(inPassword)+1];
		if ( password )
			_mbscpy(password,inPassword);
	}
}

ODBC_Connection::~ODBC_Connection()
{
	SQLRETURN rc;

	if ( password )
		delete[] password;

	if ( user )
		delete[] user;

	if ( dsn )
		delete[] dsn;
/*
	if ( hstmt )
	{
	    rc = SQLFreeStmt(hstmt, SQL_DROP);
		//if ( (rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO) )
	}
*/

	if ( hdbc )
	{
		disConnect();

		rc = SQLFreeConnect(hdbc);
		//if ( (rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO) )
		D(if ( rc != SQL_SUCCESS )
			getErrorText(rc, 2, hdbc, szErrorMsg, SQL_MAX_MESSAGE_LENGTH);)
	}

}

SQLRETURN ODBC_Connection::connect()
{
	disConnect();

	SQLRETURN rc = SQLConnect(hdbc, dsn, SQL_NTS, user, SQL_NTS, password, SQL_NTS);

	D(if ( rc != SQL_SUCCESS )
		getErrorText(rc, 2, hdbc, szErrorMsg, SQL_MAX_MESSAGE_LENGTH);)

	connected = ((rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO));

	return rc;
}

SQLRETURN ODBC_Connection::disConnect()
{
	SQLRETURN rc;

	if ( connected )
	{
		rc = SQLDisconnect(hdbc);
		D(if ( rc != SQL_SUCCESS )
			getErrorText(rc, 2, hdbc, szErrorMsg, SQL_MAX_MESSAGE_LENGTH);)

		connected = FALSE;
	}
	else
		rc = SQL_SUCCESS;

	return rc;
}

SQLRETURN ODBC_Connection::initODBC(SQLUINTEGER pooling, SQLUINTEGER ODBC_Version)
{
	SQLRETURN rc;

	if ( !henv )
		rc = SQLSetEnvAttr(NULL,SQL_ATTR_CONNECTION_POOLING, (SQLPOINTER)pooling, 0); 
	else
		rc = SQL_SUCCESS;

	if ( (rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO) )
	{
		D(SQLCHAR     szErrorMsg[SQL_MAX_MESSAGE_LENGTH];)

		rc = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&henv);

		D(if ( rc != SQL_SUCCESS )
			getErrorText(rc, 1, henv, szErrorMsg, SQL_MAX_MESSAGE_LENGTH);)

		if ( (rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO) )
			rc = SQLSetEnvAttr(henv,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)ODBC_Version,0);

		D(if ( rc != SQL_SUCCESS )
			getErrorText(rc, 1, henv, szErrorMsg, SQL_MAX_MESSAGE_LENGTH);)
	}

	return rc;
}

SQLRETURN ODBC_Connection::termODBC()
{
	SQLRETURN rc;

	if ( henv )
	{
		rc = SQLFreeEnv(henv);
		henv = NULL;
	}
	else
		rc = SQL_SUCCESS;

	return rc;
}

SQLRETURN ODBC_Connection::getErrorText(SQLRETURN rc, SQLSMALLINT htype,
										SQLHANDLE handle, SQLCHAR retErrorMsg[], int retErrorMsgSize)
{
	SQLRETURN lrc;

	//printf("%d of %s getErrorText()\n",__LINE__,__FILE__);
	if ( retErrorMsg && (retErrorMsgSize > 2) )
	{
		retErrorMsg[0] = 0;

		if( rc == SQL_ERROR || rc == SQL_SUCCESS_WITH_INFO ) 
		{
			SQLCHAR     szSqlState[6];
			SQLINTEGER  pfNativeError;
			SQLSMALLINT pcbErrorMsg;

			lrc = SQLGetDiagRec(htype, handle,1,    
								(SQLCHAR *)&szSqlState,
								(SQLINTEGER *)&pfNativeError,
								retErrorMsg,
								 retErrorMsgSize-1,
								(SQLSMALLINT *)&pcbErrorMsg);

			if(lrc == SQL_SUCCESS || lrc == SQL_SUCCESS_WITH_INFO)
				printf("\n [%s][%d:%s]\n",szSqlState,pfNativeError,retErrorMsg);
		}
		else
		  lrc = SQL_SUCCESS;
	}
	else
	{
		if ( retErrorMsg && (retErrorMsgSize >= 1) )
			retErrorMsg[0] = 0;

		lrc = SQL_ERROR;
	}

	//printf("%d of %s getErrorText() END with lrc= 0x%lx - %ld\n",__LINE__,__FILE__,lrc,lrc);
	return lrc;
}
#endif				// ]